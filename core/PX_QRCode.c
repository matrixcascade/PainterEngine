#include "PX_QRCode.h"

#define PX_QRCODE_LENGTH_OVERFLOW -1

typedef struct
{
	PX_QRCODE_MODE mode;
	px_int numChars;
	px_byte *data;
	px_int bitLength;
}PX_QRCode_Segment;


static const px_char *PX_QRCODE_ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

static const px_char PX_QRCODE_ECC_CODEWORDS_PER_BLOCK[4][41] = {
	{-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
	{-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},
	{-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
	{-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
};

#define PX_QRCODE_REED_SOLOMON_DEGREE_MAX 30

static const px_char PX_QRCODE_NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
	{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},
	{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},
	{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},
	{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},
};

static const px_int PX_QRCODE_PENALTY_N1 =  3;
static const px_int PX_QRCODE_PENALTY_N2 =  3;
static const px_int PX_QRCODE_PENALTY_N3 = 40;
static const px_int PX_QRCODE_PENALTY_N4 = 10;

static px_void PX_QRCode_AppendBitsToBuffer(px_uint val, px_int numBits, px_byte buffer[], px_int *bitLen);
static px_void PX_QRCode_AddEccAndInterleave(px_byte data[], px_int version, PX_QRCODE_ECC ecl, px_byte result[]);
static px_int PX_QRCode_GetNumDataCodewords(px_int version, PX_QRCODE_ECC ecl);
static px_int PX_QRCode_GetNumRawDataModules(px_int ver);
static px_void PX_QRCode_ReedSolomonComputeDivisor(px_int degree, px_byte result[]);
static px_void PX_QRCode_ReedSolomonComputeRemainder(const px_byte data[], px_int dataLen, const px_byte generator[], px_int degree, px_byte result[]);
static px_byte PX_QRCode_ReedSolomonMultiply(px_byte x, px_byte y);
static px_void PX_QRCode_InitializeFunctionModules(px_int version, px_byte qrcode[]);
static px_void PX_QRCode_DrawLightFunctionModules(px_byte qrcode[], px_int version);
static px_void PX_QRCode_DrawFormatBits(PX_QRCODE_ECC ecl, PX_QRCODE_MASK mask, px_byte qrcode[]);
static px_int PX_QRCode_GetAlignmentPatternPositions(px_int version, px_byte result[7]);
static px_void PX_QRCode_FillRectangle(px_int left, px_int top, px_int width, px_int height, px_byte qrcode[]);
static px_void PX_QRCode_DrawCodewords(const px_byte data[], px_int dataLen, px_byte qrcode[]);
static px_void PX_QRCode_ApplyMask(const px_byte functionModules[], px_byte qrcode[], PX_QRCODE_MASK mask);
static px_long PX_QRCode_GetPenaltyScore(const px_byte qrcode[]);
static px_int PX_QRCode_FinderPenaltyCountPatterns(const px_int runHistory[7], px_int qrsize);
static px_int PX_QRCode_FinderPenaltyTerminateAndCount(px_bool currentRunColor, px_int currentRunLength, px_int runHistory[7], px_int qrsize);
static px_void PX_QRCode_FinderPenaltyAddHistory(px_int currentRunLength, px_int runHistory[7], px_int qrsize);
static px_bool PX_QRCode_GetModuleBounded(const px_byte qrcode[], px_int x, px_int y);
static px_void PX_QRCode_SetModuleBounded(px_byte qrcode[], px_int x, px_int y, px_bool isDark);
static px_void PX_QRCode_SetModuleUnbounded(px_byte qrcode[], px_int x, px_int y, px_bool isDark);
static px_bool PX_QRCode_GetBit(px_int x, px_int i);
static px_int PX_QRCode_CalcSegmentBitLength(PX_QRCODE_MODE mode, px_int numChars);
static px_int PX_QRCode_GetTotalBits(const PX_QRCode_Segment segs[], px_int len, px_int version);
static px_int PX_QRCode_NumCharCountBits(PX_QRCODE_MODE mode, px_int version);
static px_int PX_QRCode_CalcSegmentBufferSize(PX_QRCODE_MODE mode, px_int numChars);
static PX_QRCode_Segment PX_QRCode_MakeBytes(const px_byte data[], px_int len, px_byte buf[]);
static PX_QRCode_Segment PX_QRCode_MakeNumeric(const px_char *digits, px_byte buf[]);
static PX_QRCode_Segment PX_QRCode_MakeAlphanumeric(const px_char *text, px_byte buf[]);
static PX_QRCode_Segment PX_QRCode_MakeEci(px_long assignVal, px_byte buf[]);
static px_bool PX_QRCode_EncodeSegments(const PX_QRCode_Segment segs[], px_int len,PX_QRCODE_ECC ecl, px_byte tempBuffer[], px_byte qrcode[]);
static px_bool PX_QRCode_EncodeSegmentsAdvanced(const PX_QRCode_Segment segs[], px_int len, PX_QRCODE_ECC ecl,	px_int minVersion, px_int maxVersion, PX_QRCODE_MASK mask, px_bool boostEcl, px_byte tempBuffer[], px_byte qrcode[]);


px_bool PX_QRCode_EncodeText(const px_char *text, px_byte tempBuffer[], px_byte qrcode[],
		PX_QRCODE_ECC ecl, px_int minVersion, px_int maxVersion, PX_QRCODE_MASK mask, px_bool boostEcl) {

	px_int textLen = PX_strlen(text);
	px_int bufLen;
	PX_QRCode_Segment seg;

	if (textLen == 0)
		return PX_QRCode_EncodeSegmentsAdvanced(PX_NULL, 0, ecl, minVersion, maxVersion, mask, boostEcl, tempBuffer, qrcode);
	bufLen = PX_QRCODE_BUFFER_LEN_FOR_VERSION(maxVersion);

	if (PX_QRCode_IsNumeric(text)) {
		if (PX_QRCode_CalcSegmentBufferSize(PX_QRCODE_MODE_NUMERIC, textLen) > bufLen)
			goto fail;
		seg = PX_QRCode_MakeNumeric(text, tempBuffer);
	} else if (PX_QRCode_IsAlphanumeric(text)) {
		if (PX_QRCode_CalcSegmentBufferSize(PX_QRCODE_MODE_ALPHANUMERIC, textLen) > bufLen)
			goto fail;
		seg = PX_QRCode_MakeAlphanumeric(text, tempBuffer);
	} else {
		px_int i;
		if (textLen > bufLen)
			goto fail;
		for (i = 0; i < textLen; i++)
			tempBuffer[i] = (px_byte)text[i];
		seg.mode = PX_QRCODE_MODE_BYTE;
		seg.bitLength = PX_QRCode_CalcSegmentBitLength(seg.mode, textLen);
		if (seg.bitLength == PX_QRCODE_LENGTH_OVERFLOW)
			goto fail;
		seg.numChars = textLen;
		seg.data = tempBuffer;
	}
	return PX_QRCode_EncodeSegmentsAdvanced(&seg, 1, ecl, minVersion, maxVersion, mask, boostEcl, tempBuffer, qrcode);

fail:
	qrcode[0] = 0;
	return PX_FALSE;
}


px_bool PX_QRCode_EncodeBinary(px_byte dataAndTemp[], px_int dataLen, px_byte qrcode[],
		PX_QRCODE_ECC ecl, px_int minVersion, px_int maxVersion, PX_QRCODE_MASK mask, px_bool boostEcl) {

	PX_QRCode_Segment seg;
	seg.mode = PX_QRCODE_MODE_BYTE;
	seg.bitLength = PX_QRCode_CalcSegmentBitLength(seg.mode, dataLen);
	if (seg.bitLength == PX_QRCODE_LENGTH_OVERFLOW) {
		qrcode[0] = 0;
		return PX_FALSE;
	}
	seg.numChars = dataLen;
	seg.data = dataAndTemp;
	return PX_QRCode_EncodeSegmentsAdvanced(&seg, 1, ecl, minVersion, maxVersion, mask, boostEcl, dataAndTemp, qrcode);
}

px_void PX_QRCode_RenderText(px_surface *psurface, px_int lefttop_x, px_int lefttop_y, const px_char *text, PX_QRCODE_ECC ecl, px_int render_rect_size)
{
	px_byte temp_qrcode[PX_QRCODE_BUFFER_LEN_MAX];
	px_byte qrcode[PX_QRCODE_BUFFER_LEN_MAX];
	if (!PX_QRCode_EncodeText(text, temp_qrcode, qrcode, ecl, PX_QRCODE_VERSION_MIN, PX_QRCODE_VERSION_MAX, PX_QRCODE_MASK_AUTO, PX_TRUE))
		return;
	 px_int size = PX_QRCode_GetSize(qrcode);
     px_int x, y;
      for (y = 0; y < size; y++)
      {
          for (x = 0; x < size; x++)
          {
              if (PX_QRCode_GetModule(qrcode, x, y))
              {
                  PX_GeoDrawRect(psurface, lefttop_x + x * render_rect_size, lefttop_y + y * render_rect_size, lefttop_x + x * render_rect_size+render_rect_size, lefttop_y + y * render_rect_size+render_rect_size, PX_COLOR(255,0, 0, 0));
              }
              else
              {
                  PX_GeoDrawRect(psurface, lefttop_x + x * render_rect_size, lefttop_y + y * render_rect_size, lefttop_x + x * render_rect_size+render_rect_size, lefttop_y + y * render_rect_size+render_rect_size, PX_COLOR(255,255, 255, 255));
              }
          }
      }
}

px_void PX_QRCode_RenderBinary(px_surface *psurface, px_int lefttop_x, px_int lefttop_y, const px_byte *data, px_int dataLen, PX_QRCODE_ECC ecl, px_int render_rect_size)
{
	px_byte qrcode[PX_QRCODE_BUFFER_LEN_MAX];
	if (!PX_QRCode_EncodeBinary((px_byte *)data, dataLen, qrcode, ecl, PX_QRCODE_VERSION_MIN, PX_QRCODE_VERSION_MAX, PX_QRCODE_MASK_AUTO, PX_TRUE))
		return;
	 px_int size = PX_QRCode_GetSize(qrcode);
	 px_int x, y;
	  for (y = 0; y < size; y++)
	  {
		  for (x = 0; x < size; x++)
		  {
			  if (PX_QRCode_GetModule(qrcode, x, y))
			  {
				  PX_GeoDrawRect(psurface, lefttop_x + x * render_rect_size, lefttop_y + y * render_rect_size, render_rect_size, render_rect_size, PX_COLOR(255,0, 0, 0));
			  }
			  else
			  {
				  PX_GeoDrawRect(psurface, lefttop_x + x * render_rect_size, lefttop_y + y * render_rect_size, render_rect_size, render_rect_size, PX_COLOR(255,255, 255, 255));
			  }
		  }
	  }
}


static px_void PX_QRCode_AppendBitsToBuffer(px_uint val, px_int numBits, px_byte buffer[], px_int *bitLen) {
	px_int i;
	for (i = numBits - 1; i >= 0; i--, (*bitLen)++)
		buffer[*bitLen >> 3] |= ((val >> i) & 1) << (7 - (*bitLen & 7));
}


px_bool PX_QRCode_EncodeSegments(const PX_QRCode_Segment segs[], px_int len,
		PX_QRCODE_ECC ecl, px_byte tempBuffer[], px_byte qrcode[]) {
	return PX_QRCode_EncodeSegmentsAdvanced(segs, len, ecl,
		PX_QRCODE_VERSION_MIN, PX_QRCODE_VERSION_MAX, PX_QRCODE_MASK_AUTO, PX_TRUE, tempBuffer, qrcode);
}


px_bool PX_QRCode_EncodeSegmentsAdvanced(const PX_QRCode_Segment segs[], px_int len, PX_QRCODE_ECC ecl,
		px_int minVersion, px_int maxVersion, PX_QRCODE_MASK mask, px_bool boostEcl, px_byte tempBuffer[], px_byte qrcode[]) {

	px_int version, dataUsedBits;
	px_int i;
	px_int bitLen;
	px_int dataCapacityBits;
	px_int terminatorBits;
	px_byte padByte;
	px_long minPenalty;

	for (version = minVersion; ; version++) {
		px_int capacityBits = PX_QRCode_GetNumDataCodewords(version, ecl) * 8;
		dataUsedBits = PX_QRCode_GetTotalBits(segs, len, version);
		if (dataUsedBits != PX_QRCODE_LENGTH_OVERFLOW && dataUsedBits <= capacityBits)
			break;
		if (version >= maxVersion) {
			qrcode[0] = 0;
			return PX_FALSE;
		}
	}

	for (i = (px_int)PX_QRCODE_ECC_MEDIUM; i <= (px_int)PX_QRCODE_ECC_HIGH; i++) {
		if (boostEcl && dataUsedBits <= PX_QRCode_GetNumDataCodewords(version, (PX_QRCODE_ECC)i) * 8)
			ecl = (PX_QRCODE_ECC)i;
	}

	PX_memset(qrcode, 0, PX_QRCODE_BUFFER_LEN_FOR_VERSION(version));
	bitLen = 0;
	for (i = 0; i < len; i++) {
		const PX_QRCode_Segment *seg = &segs[i];
		px_int j;
		PX_QRCode_AppendBitsToBuffer((px_uint)seg->mode, 4, qrcode, &bitLen);
		PX_QRCode_AppendBitsToBuffer((px_uint)seg->numChars, PX_QRCode_NumCharCountBits(seg->mode, version), qrcode, &bitLen);
		for (j = 0; j < seg->bitLength; j++) {
			px_int bit = (seg->data[j >> 3] >> (7 - (j & 7))) & 1;
			PX_QRCode_AppendBitsToBuffer((px_uint)bit, 1, qrcode, &bitLen);
		}
	}

	dataCapacityBits = PX_QRCode_GetNumDataCodewords(version, ecl) * 8;
	terminatorBits = dataCapacityBits - bitLen;
	if (terminatorBits > 4)
		terminatorBits = 4;
	PX_QRCode_AppendBitsToBuffer(0, terminatorBits, qrcode, &bitLen);
	PX_QRCode_AppendBitsToBuffer(0, (8 - bitLen % 8) % 8, qrcode, &bitLen);

	for (padByte = 0xEC; bitLen < dataCapacityBits; padByte ^= 0xEC ^ 0x11)
		PX_QRCode_AppendBitsToBuffer(padByte, 8, qrcode, &bitLen);

	PX_QRCode_AddEccAndInterleave(qrcode, version, ecl, tempBuffer);
	PX_QRCode_InitializeFunctionModules(version, qrcode);
	PX_QRCode_DrawCodewords(tempBuffer, PX_QRCode_GetNumRawDataModules(version) / 8, qrcode);
	PX_QRCode_DrawLightFunctionModules(qrcode, version);
	PX_QRCode_InitializeFunctionModules(version, tempBuffer);

	if (mask == PX_QRCODE_MASK_AUTO) {
		minPenalty = 0x7FFFFFFFL;
		for (i = 0; i < 8; i++) {
			PX_QRCODE_MASK msk = (PX_QRCODE_MASK)i;
			px_long penalty;
			PX_QRCode_ApplyMask(tempBuffer, qrcode, msk);
			PX_QRCode_DrawFormatBits(ecl, msk, qrcode);
			penalty = PX_QRCode_GetPenaltyScore(qrcode);
			if (penalty < minPenalty) {
				mask = msk;
				minPenalty = penalty;
			}
			PX_QRCode_ApplyMask(tempBuffer, qrcode, msk);
		}
	}
	PX_QRCode_ApplyMask(tempBuffer, qrcode, mask);
	PX_QRCode_DrawFormatBits(ecl, mask, qrcode);
	return PX_TRUE;
}


static px_void PX_QRCode_AddEccAndInterleave(px_byte data[], px_int version, PX_QRCODE_ECC ecl, px_byte result[]) {
	px_int numBlocks = PX_QRCODE_NUM_ERROR_CORRECTION_BLOCKS[(px_int)ecl][version];
	px_int blockEccLen = PX_QRCODE_ECC_CODEWORDS_PER_BLOCK[(px_int)ecl][version];
	px_int rawCodewords = PX_QRCode_GetNumRawDataModules(version) / 8;
	px_int dataLen = PX_QRCode_GetNumDataCodewords(version, ecl);
	px_int numShortBlocks = numBlocks - rawCodewords % numBlocks;
	px_int shortBlockDataLen = rawCodewords / numBlocks - blockEccLen;
	px_byte rsdiv[PX_QRCODE_REED_SOLOMON_DEGREE_MAX];
	const px_byte *dat = data;
	px_int i;

	PX_QRCode_ReedSolomonComputeDivisor(blockEccLen, rsdiv);
	for (i = 0; i < numBlocks; i++) {
		px_int datLen = shortBlockDataLen + (i < numShortBlocks ? 0 : 1);
		px_byte *ecc = &data[dataLen];
		px_int j, k;
		PX_QRCode_ReedSolomonComputeRemainder(dat, datLen, rsdiv, blockEccLen, ecc);
		for (j = 0, k = i; j < datLen; j++, k += numBlocks) {
			if (j == shortBlockDataLen)
				k -= numShortBlocks;
			result[k] = dat[j];
		}
		for (j = 0, k = dataLen + i; j < blockEccLen; j++, k += numBlocks)
			result[k] = ecc[j];
		dat += datLen;
	}
}


static px_int PX_QRCode_GetNumDataCodewords(px_int version, PX_QRCODE_ECC ecl) {
	px_int v = version, e = (px_int)ecl;
	return PX_QRCode_GetNumRawDataModules(v) / 8
		- PX_QRCODE_ECC_CODEWORDS_PER_BLOCK    [e][v]
		* PX_QRCODE_NUM_ERROR_CORRECTION_BLOCKS[e][v];
}


static px_int PX_QRCode_GetNumRawDataModules(px_int ver) {
	px_int result = (16 * ver + 128) * ver + 64;
	if (ver >= 2) {
		px_int numAlign = ver / 7 + 2;
		result -= (25 * numAlign - 10) * numAlign - 55;
		if (ver >= 7)
			result -= 36;
	}
	return result;
}


static px_void PX_QRCode_ReedSolomonComputeDivisor(px_int degree, px_byte result[]) {
	px_byte root;
	px_int i;
	PX_memset(result, 0, degree);
	result[degree - 1] = 1;

	root = 1;
	for (i = 0; i < degree; i++) {
		px_int j;
		for (j = 0; j < degree; j++) {
			result[j] = PX_QRCode_ReedSolomonMultiply(result[j], root);
			if (j + 1 < degree)
				result[j] ^= result[j + 1];
		}
		root = PX_QRCode_ReedSolomonMultiply(root, 0x02);
	}
}


static px_void PX_QRCode_ReedSolomonComputeRemainder(const px_byte data[], px_int dataLen,
		const px_byte generator[], px_int degree, px_byte result[]) {
	px_int i;
	PX_memset(result, 0, degree);
	for (i = 0; i < dataLen; i++) {
		px_byte factor = data[i] ^ result[0];
		px_int j;
		PX_memmove(&result[0], &result[1], (degree - 1) * sizeof(result[0]));
		result[degree - 1] = 0;
		for (j = 0; j < degree; j++)
			result[j] ^= PX_QRCode_ReedSolomonMultiply(generator[j], factor);
	}
}


static px_byte PX_QRCode_ReedSolomonMultiply(px_byte x, px_byte y) {
	px_byte z = 0;
	px_int i;
	for (i = 7; i >= 0; i--) {
		z = (px_byte)((z << 1) ^ ((z >> 7) * 0x11D));
		z ^= ((y >> i) & 1) * x;
	}
	return z;
}


static px_void PX_QRCode_InitializeFunctionModules(px_int version, px_byte qrcode[]) {
	px_int qrsize = version * 4 + 17;
	px_byte alignPatPos[7];
	px_int numAlign;
	px_int i, j;

	PX_memset(qrcode, 0, (qrsize * qrsize + 7) / 8 + 1);
	qrcode[0] = (px_byte)qrsize;

	PX_QRCode_FillRectangle(6, 0, 1, qrsize, qrcode);
	PX_QRCode_FillRectangle(0, 6, qrsize, 1, qrcode);

	PX_QRCode_FillRectangle(0, 0, 9, 9, qrcode);
	PX_QRCode_FillRectangle(qrsize - 8, 0, 8, 9, qrcode);
	PX_QRCode_FillRectangle(0, qrsize - 8, 9, 8, qrcode);

	numAlign = PX_QRCode_GetAlignmentPatternPositions(version, alignPatPos);
	for (i = 0; i < numAlign; i++) {
		for (j = 0; j < numAlign; j++) {
			if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
				PX_QRCode_FillRectangle(alignPatPos[i] - 2, alignPatPos[j] - 2, 5, 5, qrcode);
		}
	}

	if (version >= 7) {
		PX_QRCode_FillRectangle(qrsize - 11, 0, 3, 6, qrcode);
		PX_QRCode_FillRectangle(0, qrsize - 11, 6, 3, qrcode);
	}
}


static px_void PX_QRCode_DrawLightFunctionModules(px_byte qrcode[], px_int version) {
	px_int qrsize = PX_QRCode_GetSize(qrcode);
	px_byte alignPatPos[7];
	px_int numAlign;
	px_int i, j;
	px_int dy, dx;

	for (i = 7; i < qrsize - 7; i += 2) {
		PX_QRCode_SetModuleBounded(qrcode, 6, i, PX_FALSE);
		PX_QRCode_SetModuleBounded(qrcode, i, 6, PX_FALSE);
	}

	for (dy = -4; dy <= 4; dy++) {
		for (dx = -4; dx <= 4; dx++) {
			px_int dist = PX_ABS(dx);
			if (PX_ABS(dy) > dist)
				dist = PX_ABS(dy);
			if (dist == 2 || dist == 4) {
				PX_QRCode_SetModuleUnbounded(qrcode, 3 + dx, 3 + dy, PX_FALSE);
				PX_QRCode_SetModuleUnbounded(qrcode, qrsize - 4 + dx, 3 + dy, PX_FALSE);
				PX_QRCode_SetModuleUnbounded(qrcode, 3 + dx, qrsize - 4 + dy, PX_FALSE);
			}
		}
	}

	numAlign = PX_QRCode_GetAlignmentPatternPositions(version, alignPatPos);
	for (i = 0; i < numAlign; i++) {
		for (j = 0; j < numAlign; j++) {
			if ((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0))
				continue;
			for (dy = -1; dy <= 1; dy++) {
				for (dx = -1; dx <= 1; dx++)
					PX_QRCode_SetModuleBounded(qrcode, alignPatPos[i] + dx, alignPatPos[j] + dy, dx == 0 && dy == 0);
			}
		}
	}

	if (version >= 7) {
		px_int rem = version;
		px_long bits;
		for (i = 0; i < 12; i++)
			rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
		bits = (px_long)version << 12 | rem;

		for (i = 0; i < 6; i++) {
			for (j = 0; j < 3; j++) {
				px_int k = qrsize - 11 + j;
				PX_QRCode_SetModuleBounded(qrcode, k, i, (bits & 1) != 0);
				PX_QRCode_SetModuleBounded(qrcode, i, k, (bits & 1) != 0);
				bits >>= 1;
			}
		}
	}
}


static px_void PX_QRCode_DrawFormatBits(PX_QRCODE_ECC ecl, PX_QRCODE_MASK mask, px_byte qrcode[]) {
	static const px_int table[] = {1, 0, 3, 2};
	px_int data = table[(px_int)ecl] << 3 | (px_int)mask;
	px_int rem = data;
	px_int bits;
	px_int qrsize;
	px_int i;

	for (i = 0; i < 10; i++)
		rem = (rem << 1) ^ ((rem >> 9) * 0x537);
	bits = (data << 10 | rem) ^ 0x5412;

	for (i = 0; i <= 5; i++)
		PX_QRCode_SetModuleBounded(qrcode, 8, i, PX_QRCode_GetBit(bits, i));
	PX_QRCode_SetModuleBounded(qrcode, 8, 7, PX_QRCode_GetBit(bits, 6));
	PX_QRCode_SetModuleBounded(qrcode, 8, 8, PX_QRCode_GetBit(bits, 7));
	PX_QRCode_SetModuleBounded(qrcode, 7, 8, PX_QRCode_GetBit(bits, 8));
	for (i = 9; i < 15; i++)
		PX_QRCode_SetModuleBounded(qrcode, 14 - i, 8, PX_QRCode_GetBit(bits, i));

	qrsize = PX_QRCode_GetSize(qrcode);
	for (i = 0; i < 8; i++)
		PX_QRCode_SetModuleBounded(qrcode, qrsize - 1 - i, 8, PX_QRCode_GetBit(bits, i));
	for (i = 8; i < 15; i++)
		PX_QRCode_SetModuleBounded(qrcode, 8, qrsize - 15 + i, PX_QRCode_GetBit(bits, i));
	PX_QRCode_SetModuleBounded(qrcode, 8, qrsize - 8, PX_TRUE);
}


static px_int PX_QRCode_GetAlignmentPatternPositions(px_int version, px_byte result[7]) {
	px_int numAlign, step;
	px_int i, pos;
	if (version == 1)
		return 0;
	numAlign = version / 7 + 2;
	step = (version * 8 + numAlign * 3 + 5) / (numAlign * 4 - 4) * 2;
	for (i = numAlign - 1, pos = version * 4 + 10; i >= 1; i--, pos -= step)
		result[i] = (px_byte)pos;
	result[0] = 6;
	return numAlign;
}


static px_void PX_QRCode_FillRectangle(px_int left, px_int top, px_int width, px_int height, px_byte qrcode[]) {
	px_int dy, dx;
	for (dy = 0; dy < height; dy++) {
		for (dx = 0; dx < width; dx++)
			PX_QRCode_SetModuleBounded(qrcode, left + dx, top + dy, PX_TRUE);
	}
}


static px_void PX_QRCode_DrawCodewords(const px_byte data[], px_int dataLen, px_byte qrcode[]) {
	px_int qrsize = PX_QRCode_GetSize(qrcode);
	px_int i = 0;
	px_int right;
	for (right = qrsize - 1; right >= 1; right -= 2) {
		px_int vert;
		if (right == 6)
			right = 5;
		for (vert = 0; vert < qrsize; vert++) {
			px_int j;
			for (j = 0; j < 2; j++) {
				px_int x = right - j;
				px_bool upward = ((right + 1) & 2) == 0;
				px_int y = upward ? qrsize - 1 - vert : vert;
				if (!PX_QRCode_GetModuleBounded(qrcode, x, y) && i < dataLen * 8) {
					px_bool dark = PX_QRCode_GetBit(data[i >> 3], 7 - (i & 7));
					PX_QRCode_SetModuleBounded(qrcode, x, y, dark);
					i++;
				}
			}
		}
	}
}


static px_void PX_QRCode_ApplyMask(const px_byte functionModules[], px_byte qrcode[], PX_QRCODE_MASK mask) {
	px_int qrsize = PX_QRCode_GetSize(qrcode);
	px_int y, x;
	for (y = 0; y < qrsize; y++) {
		for (x = 0; x < qrsize; x++) {
			px_bool invert;
			px_bool val;
			if (PX_QRCode_GetModuleBounded(functionModules, x, y))
				continue;
			switch ((px_int)mask) {
				case 0:  invert = (x + y) % 2 == 0;                    break;
				case 1:  invert = y % 2 == 0;                          break;
				case 2:  invert = x % 3 == 0;                          break;
				case 3:  invert = (x + y) % 3 == 0;                    break;
				case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
				case 5:  invert = x * y % 2 + x * y % 3 == 0;          break;
				case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
				case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
				default: return;
			}
			val = PX_QRCode_GetModuleBounded(qrcode, x, y);
			PX_QRCode_SetModuleBounded(qrcode, x, y, val ^ invert);
		}
	}
}


static px_long PX_QRCode_GetPenaltyScore(const px_byte qrcode[]) {
	px_int qrsize = PX_QRCode_GetSize(qrcode);
	px_long result = 0;
	px_int y, x;
	px_int dark;
	px_int total;
	px_int k;

	for (y = 0; y < qrsize; y++) {
		px_bool runColor = PX_FALSE;
		px_int runX = 0;
		px_int runHistory[7] = {0};
		for (x = 0; x < qrsize; x++) {
			if (PX_QRCode_GetModuleBounded(qrcode, x, y) == runColor) {
				runX++;
				if (runX == 5)
					result += PX_QRCODE_PENALTY_N1;
				else if (runX > 5)
					result++;
			} else {
				PX_QRCode_FinderPenaltyAddHistory(runX, runHistory, qrsize);
				if (!runColor)
					result += PX_QRCode_FinderPenaltyCountPatterns(runHistory, qrsize) * PX_QRCODE_PENALTY_N3;
				runColor = PX_QRCode_GetModuleBounded(qrcode, x, y);
				runX = 1;
			}
		}
		result += PX_QRCode_FinderPenaltyTerminateAndCount(runColor, runX, runHistory, qrsize) * PX_QRCODE_PENALTY_N3;
	}

	for (x = 0; x < qrsize; x++) {
		px_bool runColor = PX_FALSE;
		px_int runY = 0;
		px_int runHistory[7] = {0};
		for (y = 0; y < qrsize; y++) {
			if (PX_QRCode_GetModuleBounded(qrcode, x, y) == runColor) {
				runY++;
				if (runY == 5)
					result += PX_QRCODE_PENALTY_N1;
				else if (runY > 5)
					result++;
			} else {
				PX_QRCode_FinderPenaltyAddHistory(runY, runHistory, qrsize);
				if (!runColor)
					result += PX_QRCode_FinderPenaltyCountPatterns(runHistory, qrsize) * PX_QRCODE_PENALTY_N3;
				runColor = PX_QRCode_GetModuleBounded(qrcode, x, y);
				runY = 1;
			}
		}
		result += PX_QRCode_FinderPenaltyTerminateAndCount(runColor, runY, runHistory, qrsize) * PX_QRCODE_PENALTY_N3;
	}

	for (y = 0; y < qrsize - 1; y++) {
		for (x = 0; x < qrsize - 1; x++) {
			px_bool color = PX_QRCode_GetModuleBounded(qrcode, x, y);
			if (  color == PX_QRCode_GetModuleBounded(qrcode, x + 1, y) &&
			      color == PX_QRCode_GetModuleBounded(qrcode, x, y + 1) &&
			      color == PX_QRCode_GetModuleBounded(qrcode, x + 1, y + 1))
				result += PX_QRCODE_PENALTY_N2;
		}
	}

	dark = 0;
	for (y = 0; y < qrsize; y++) {
		for (x = 0; x < qrsize; x++) {
			if (PX_QRCode_GetModuleBounded(qrcode, x, y))
				dark++;
		}
	}
	total = qrsize * qrsize;
	{
		px_long darkL = (px_long)dark;
		px_long totalL = (px_long)total;
		px_long diff = darkL * 20L - totalL * 10L;
		if (diff < 0) diff = -diff;
		k = (px_int)((diff + totalL - 1) / totalL) - 1;
	}
	result += k * PX_QRCODE_PENALTY_N4;
	return result;
}


static px_int PX_QRCode_FinderPenaltyCountPatterns(const px_int runHistory[7], px_int qrsize) {
	px_int n = runHistory[1];
	px_bool core;
	(px_void)qrsize;
	core = n > 0 && runHistory[2] == n && runHistory[3] == n * 3 && runHistory[4] == n && runHistory[5] == n;
	return (core && runHistory[0] >= n * 4 && runHistory[6] >= n ? 1 : 0)
	     + (core && runHistory[6] >= n * 4 && runHistory[0] >= n ? 1 : 0);
}


static px_int PX_QRCode_FinderPenaltyTerminateAndCount(px_bool currentRunColor, px_int currentRunLength, px_int runHistory[7], px_int qrsize) {
	if (currentRunColor) {
		PX_QRCode_FinderPenaltyAddHistory(currentRunLength, runHistory, qrsize);
		currentRunLength = 0;
	}
	currentRunLength += qrsize;
	PX_QRCode_FinderPenaltyAddHistory(currentRunLength, runHistory, qrsize);
	return PX_QRCode_FinderPenaltyCountPatterns(runHistory, qrsize);
}


static px_void PX_QRCode_FinderPenaltyAddHistory(px_int currentRunLength, px_int runHistory[7], px_int qrsize) {
	if (runHistory[0] == 0)
		currentRunLength += qrsize;
	PX_memmove(&runHistory[1], &runHistory[0], 6 * sizeof(runHistory[0]));
	runHistory[0] = currentRunLength;
}


px_int PX_QRCode_GetSize(const px_byte qrcode[]) {
	px_int result;
	if (!qrcode)
		return 0;
	result = qrcode[0];
	return result;
}


px_bool PX_QRCode_GetModule(const px_byte qrcode[], px_int x, px_int y) {
	px_int qrsize;
	if (!qrcode)
		return PX_FALSE;
	qrsize = qrcode[0];
	return (0 <= x && x < qrsize && 0 <= y && y < qrsize) && PX_QRCode_GetModuleBounded(qrcode, x, y);
}


static px_bool PX_QRCode_GetModuleBounded(const px_byte qrcode[], px_int x, px_int y) {
	px_int qrsize = qrcode[0];
	px_int index = y * qrsize + x;
	return PX_QRCode_GetBit(qrcode[(index >> 3) + 1], index & 7);
}


static px_void PX_QRCode_SetModuleBounded(px_byte qrcode[], px_int x, px_int y, px_bool isDark) {
	px_int qrsize = qrcode[0];
	px_int index = y * qrsize + x;
	px_int bitIndex = index & 7;
	px_int byteIndex = (index >> 3) + 1;
	if (isDark)
		qrcode[byteIndex] |= 1 << bitIndex;
	else
		qrcode[byteIndex] &= (1 << bitIndex) ^ 0xFF;
}


static px_void PX_QRCode_SetModuleUnbounded(px_byte qrcode[], px_int x, px_int y, px_bool isDark) {
	px_int qrsize = qrcode[0];
	if (0 <= x && x < qrsize && 0 <= y && y < qrsize)
		PX_QRCode_SetModuleBounded(qrcode, x, y, isDark);
}


static px_bool PX_QRCode_GetBit(px_int x, px_int i) {
	return ((x >> i) & 1) != 0;
}


px_bool PX_QRCode_IsNumeric(const px_char *text) {
	if (!text) return PX_FALSE;
	for (; *text != '\0'; text++) {
		if (*text < '0' || *text > '9')
			return PX_FALSE;
	}
	return PX_TRUE;
}


px_bool PX_QRCode_IsAlphanumeric(const px_char *text) {
	if (!text) return PX_FALSE;
	for (; *text != '\0'; text++) {
		if (PX_strchr(PX_QRCODE_ALPHANUMERIC_CHARSET, *text) == PX_NULL)
			return PX_FALSE;
	}
	return PX_TRUE;
}


px_int PX_QRCode_CalcSegmentBufferSize(PX_QRCODE_MODE mode, px_int numChars) {
	px_int temp = PX_QRCode_CalcSegmentBitLength(mode, numChars);
	if (temp == PX_QRCODE_LENGTH_OVERFLOW)
		return 0x7FFFFFFF;
	return (temp + 7) / 8;
}


static px_int PX_QRCode_CalcSegmentBitLength(PX_QRCODE_MODE mode, px_int numChars) {
	px_long result;
	if (numChars > 32767)
		return PX_QRCODE_LENGTH_OVERFLOW;
	result = (px_long)numChars;
	if (mode == PX_QRCODE_MODE_NUMERIC)
		result = (result * 10 + 2) / 3;
	else if (mode == PX_QRCODE_MODE_ALPHANUMERIC)
		result = (result * 11 + 1) / 2;
	else if (mode == PX_QRCODE_MODE_BYTE)
		result *= 8;
	else if (mode == PX_QRCODE_MODE_KANJI)
		result *= 13;
	else if (mode == PX_QRCODE_MODE_ECI && numChars == 0)
		result = 3 * 8;
	else
		return PX_QRCODE_LENGTH_OVERFLOW;
	if (result > 32767)
		return PX_QRCODE_LENGTH_OVERFLOW;
	return (px_int)result;
}


PX_QRCode_Segment PX_QRCode_MakeBytes(const px_byte data[], px_int len, px_byte buf[]) {
	PX_QRCode_Segment result;
	result.mode = PX_QRCODE_MODE_BYTE;
	result.bitLength = PX_QRCode_CalcSegmentBitLength(result.mode, len);
	result.numChars = len;
	if (len > 0)
		PX_memcpy(buf, data, len);
	result.data = buf;
	return result;
}


PX_QRCode_Segment PX_QRCode_MakeNumeric(const px_char *digits, px_byte buf[]) {
	PX_QRCode_Segment result;
	px_int len = PX_strlen(digits);
	px_int bitLen;
	px_uint accumData = 0;
	px_int accumCount = 0;

	result.mode = PX_QRCODE_MODE_NUMERIC;
	bitLen = PX_QRCode_CalcSegmentBitLength(result.mode, len);
	result.numChars = len;
	if (bitLen > 0)
		PX_memset(buf, 0, (bitLen + 7) / 8);
	result.bitLength = 0;

	for (; *digits != '\0'; digits++) {
		px_char c = *digits;
		accumData = accumData * 10 + (px_uint)(c - '0');
		accumCount++;
		if (accumCount == 3) {
			PX_QRCode_AppendBitsToBuffer(accumData, 10, buf, &result.bitLength);
			accumData = 0;
			accumCount = 0;
		}
	}
	if (accumCount > 0)
		PX_QRCode_AppendBitsToBuffer(accumData, accumCount * 3 + 1, buf, &result.bitLength);
	result.data = buf;
	return result;
}


PX_QRCode_Segment PX_QRCode_MakeAlphanumeric(const px_char *text, px_byte buf[]) {
	PX_QRCode_Segment result;
	px_int len = PX_strlen(text);
	px_int bitLen;
	px_uint accumData = 0;
	px_int accumCount = 0;

	result.mode = PX_QRCODE_MODE_ALPHANUMERIC;
	bitLen = PX_QRCode_CalcSegmentBitLength(result.mode, len);
	result.numChars = len;
	if (bitLen > 0)
		PX_memset(buf, 0, (bitLen + 7) / 8);
	result.bitLength = 0;

	for (; *text != '\0'; text++) {
		const px_char *temp = PX_strchr(PX_QRCODE_ALPHANUMERIC_CHARSET, *text);
		accumData = accumData * 45 + (px_uint)(temp - PX_QRCODE_ALPHANUMERIC_CHARSET);
		accumCount++;
		if (accumCount == 2) {
			PX_QRCode_AppendBitsToBuffer(accumData, 11, buf, &result.bitLength);
			accumData = 0;
			accumCount = 0;
		}
	}
	if (accumCount > 0)
		PX_QRCode_AppendBitsToBuffer(accumData, 6, buf, &result.bitLength);
	result.data = buf;
	return result;
}


PX_QRCode_Segment PX_QRCode_MakeEci(px_long assignVal, px_byte buf[]) {
	PX_QRCode_Segment result;
	result.mode = PX_QRCODE_MODE_ECI;
	result.numChars = 0;
	result.bitLength = 0;
	if (assignVal < 0) {
		result.data = buf;
		return result;
	} else if (assignVal < (1 << 7)) {
		PX_memset(buf, 0, 1);
		PX_QRCode_AppendBitsToBuffer((px_uint)assignVal, 8, buf, &result.bitLength);
	} else if (assignVal < (1 << 14)) {
		PX_memset(buf, 0, 2);
		PX_QRCode_AppendBitsToBuffer(2, 2, buf, &result.bitLength);
		PX_QRCode_AppendBitsToBuffer((px_uint)assignVal, 14, buf, &result.bitLength);
	} else if (assignVal < 1000000L) {
		PX_memset(buf, 0, 3);
		PX_QRCode_AppendBitsToBuffer(6, 3, buf, &result.bitLength);
		PX_QRCode_AppendBitsToBuffer((px_uint)(assignVal >> 10), 11, buf, &result.bitLength);
		PX_QRCode_AppendBitsToBuffer((px_uint)(assignVal & 0x3FF), 10, buf, &result.bitLength);
	}
	result.data = buf;
	return result;
}


static px_int PX_QRCode_GetTotalBits(const PX_QRCode_Segment segs[], px_int len, px_int version) {
	px_long result = 0;
	px_int i;
	if (!segs && len > 0) return PX_QRCODE_LENGTH_OVERFLOW;
	for (i = 0; i < len; i++) {
		px_int numChars  = segs[i].numChars;
		px_int bitLength = segs[i].bitLength;
		px_int ccbits = PX_QRCode_NumCharCountBits(segs[i].mode, version);
		if (numChars >= (1L << ccbits))
			return PX_QRCODE_LENGTH_OVERFLOW;
		result += 4L + ccbits + bitLength;
		if (result > 32767)
			return PX_QRCODE_LENGTH_OVERFLOW;
	}
	return (px_int)result;
}


static px_int PX_QRCode_NumCharCountBits(PX_QRCODE_MODE mode, px_int version) {
	px_int i = (version + 7) / 17;
	switch (mode) {
		case PX_QRCODE_MODE_NUMERIC     : { static const px_int temp[] = {10, 12, 14}; return temp[i]; }
		case PX_QRCODE_MODE_ALPHANUMERIC: { static const px_int temp[] = { 9, 11, 13}; return temp[i]; }
		case PX_QRCODE_MODE_BYTE        : { static const px_int temp[] = { 8, 16, 16}; return temp[i]; }
		case PX_QRCODE_MODE_KANJI       : { static const px_int temp[] = { 8, 10, 12}; return temp[i]; }
		case PX_QRCODE_MODE_ECI         : return 0;
		default: return -1;
	}
}

#undef PX_QRCODE_LENGTH_OVERFLOW
#undef PX_QRCODE_REED_SOLOMON_DEGREE_MAX
