#include "PX_jpg.h"


const px_byte zigZagMap[] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};




PX_JpgHuffmanTable hDCTableY = {
    { 0, 0, 1, 6, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 12, 12, 12 },
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b },
    {0}
};

PX_JpgHuffmanTable hDCTableCbCr = {
    { 0, 0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12, 12 },
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b },
    {0}
};

PX_JpgHuffmanTable hACTableY = {
    { 0, 0, 2, 3, 6, 9, 11, 15, 18, 23, 28, 32, 36, 36, 36, 37, 162 },
    {
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
        0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
        0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
        0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
        0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
        0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
        0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
        0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
    },
    {0}
};

PX_JpgHuffmanTable hACTableCbCr = {
    { 0, 0, 2, 3, 5, 9, 13, 16, 20, 27, 32, 36, 40, 40, 41, 43, 162 },
    {
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
        0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
        0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
        0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
        0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
        0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
        0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
        0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
        0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
        0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
    },
    {0}
};

PX_JpgHuffmanTable*  dcTables[] = { &hDCTableY, &hDCTableCbCr, &hDCTableCbCr };
PX_JpgHuffmanTable*  acTables[] = { &hACTableY, &hACTableCbCr, &hACTableCbCr };

px_byte PX_JpgReadBit(PX_MemoryStream* pstream)
{
    if (pstream->bitpointer%8==0&&pstream->bitpointer)
    {
        px_int byteindex = pstream->bitpointer / 8;

        while (PX_TRUE)
        {
            if (pstream->bitstream[byteindex] == 0x00 && pstream->bitstream[byteindex - 1] == 0xFF)
            {
                pstream->bitpointer += 8;
                byteindex++;
                continue;
            }
            else if (pstream->bitstream[byteindex] == 0xFF)
            {
                if(pstream->bitstream[byteindex + 1] == 0xFF)
                {
                    pstream->bitpointer += 8;
                    byteindex++;
                    continue;
                }

                if (pstream->bitstream[byteindex + 1] >= PX_JPG_MARKER_RST0 && pstream->bitstream[byteindex + 1] <= PX_JPG_MARKER_RST7)
                {
                    pstream->bitpointer += 16;
                }
                else
                {
                    if (pstream->bitstream[byteindex + 1] != 0x00)
                    {
                        return -1;
                    }
                }
            }
            break;
        }

            
    }
    return PX_MemoryStreamReadBitBE(pstream);
}

px_uint PX_JpgReadBits(PX_MemoryStream* pstream,const px_uint length) {
    px_uint bits = 0;
    px_uint i;
    for (i = 0; i < length; ++i) 
    {
        px_uint bit = PX_JpgReadBit(pstream);
        if (bit == -1) {
            bits = -1;
            break;
        }
        bits = (bits << 1) | bit;
    }
    return bits;
}

// SOF specifies frame type, dimensions, and number of color components
px_bool PX_JpgReadStartOfFrame(PX_JpgDecoder* pJpgdecoder) 
{
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    px_byte precision;
    px_int i;
    //Reading SOF Marker";
    px_uint length;
    if (pJpgdecoder->numComponents != 0) {
       //"Error - Multiple SOFs detected\n";
        return PX_FALSE;
    }

    length = PX_MemoryStreamReadWord(pstream);

    precision = PX_MemoryStreamReadByte(pstream);
    if (precision != 8) {
       //"Error - Invalid precision: " << (px_uint)precision << '\n';
        return PX_FALSE;
    }

    pJpgdecoder->height = PX_MemoryStreamReadWord(pstream);
    pJpgdecoder->width = PX_MemoryStreamReadWord(pstream);

    if (pJpgdecoder->height == 0 || pJpgdecoder->width == 0) 
    {
        //"Error - Invalid dimensions\n";
        return PX_FALSE;
    }

    pJpgdecoder->blockHeight = (pJpgdecoder->height + 7) / 8;
    pJpgdecoder->blockWidth = (pJpgdecoder->width + 7) / 8;
    pJpgdecoder->blockHeightReal = pJpgdecoder->blockHeight;
    pJpgdecoder->blockWidthReal = pJpgdecoder->blockWidth;

    pJpgdecoder->numComponents = PX_MemoryStreamReadByte(pstream);
    if (pJpgdecoder->numComponents == 4) 
    {
        //"Error - CMYK color mode not supported\n";
        return PX_FALSE;
    }
    if (pJpgdecoder->numComponents != 1 && pJpgdecoder->numComponents != 3) 
    {
        // "Error - " << (px_uint)pJpgdecoder->numComponents << " color components given (1 or 3 required)\n";
        return PX_FALSE;
    }
    for (i = 0; i < pJpgdecoder->numComponents; ++i) 
    {
        PX_JpgColorComponent* component;
		px_byte samplingFactor;
        px_byte componentID = PX_MemoryStreamReadByte(pstream);
        // component IDs are usually 1, 2, 3 but rarely can be seen as 0, 1, 2
        // always force them into 1, 2, 3 for consistency
        if (componentID == 0 && i == 0) {
            pJpgdecoder->zeroBased = PX_TRUE;
        }
        if (pJpgdecoder->zeroBased) {
            componentID += 1;
        }
        if (componentID == 0 || componentID > pJpgdecoder->numComponents) {
            // "Error - Invalid component ID: " << (px_uint)componentID << '\n';
            return PX_FALSE;
        }
        component = &pJpgdecoder->colorComponents[componentID - 1];
        if (component->usedInFrame) {
            //"Error - Duplicate color component ID: " << (px_uint)componentID << '\n';
            return PX_FALSE;
        }
        component->usedInFrame = PX_TRUE;

        samplingFactor = PX_MemoryStreamReadByte(pstream);
        component->horizontalSamplingFactor = samplingFactor >> 4;
        component->verticalSamplingFactor = samplingFactor & 0x0F;
        if (componentID == 1) {
            if ((component->horizontalSamplingFactor != 1 && component->horizontalSamplingFactor != 2) ||
                (component->verticalSamplingFactor != 1 && component->verticalSamplingFactor != 2)) {
                // "Error - Sampling factors not supported\n";
                return PX_FALSE;
            }
            if (component->horizontalSamplingFactor == 2 && pJpgdecoder->blockWidth % 2 == 1) {
                pJpgdecoder->blockWidthReal += 1;
            }
            if (component->verticalSamplingFactor == 2 && pJpgdecoder->blockHeight % 2 == 1) {
                pJpgdecoder->blockHeightReal += 1;
            }

            pJpgdecoder->horizontalSamplingFactor = component->horizontalSamplingFactor;
            pJpgdecoder->verticalSamplingFactor = component->verticalSamplingFactor;
        }
        else {
            if (component->horizontalSamplingFactor != 1 || component->verticalSamplingFactor != 1) {
                // "Error - Sampling factors not supported\n";
           
                return PX_FALSE;
            }
        }

        component->quantizationTableID = PX_MemoryStreamReadByte(pstream);
        if (component->quantizationTableID > 3) 
        {
            // "Error - Invalid quantization table ID: " << (px_uint)component->quantizationTableID << '\n';
            return PX_FALSE;
        }
    }

    if (length - 8 - (3 * pJpgdecoder->numComponents) != 0) 
    {
        return PX_FALSE;
    }
    return PX_TRUE;
}

// DQT contains one or more quantization tables
px_bool PX_JpgReadQuantizationTable(PX_JpgDecoder* pJpgdecoder) {
    PX_JpgQuantizationTable* qTable;
    px_int i;
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    px_int length = PX_MemoryStreamReadWord(pstream);
    length -= 2;

    while (length > 0) {
        px_byte tableInfo = PX_MemoryStreamReadByte(pstream);
		px_byte tableID;
        length -= 1;
        tableID = tableInfo & 0x0F;

        if (tableID > 3) {
            // "Error - Invalid quantization table ID: " << (px_uint)tableID << '\n';
            return PX_FALSE;
        }
        qTable =&pJpgdecoder->quantizationTables[tableID];
        qTable->set = PX_TRUE;

        if (tableInfo >> 4 != 0) {
            for (i = 0; i < 64; ++i) {
                qTable->table[zigZagMap[i]] = PX_MemoryStreamReadWord(pstream);
            }
            length -= 128;
        }
        else {
            for (i = 0; i < 64; ++i) {
                qTable->table[zigZagMap[i]] = PX_MemoryStreamReadByte(pstream);
            }
            length -= 64;
        }
    }

    if (length != 0) {
        return PX_FALSE;
    }
    return PX_TRUE;
}



// generate all Huffman codes based on symbols from a Huffman table
px_void PX_JPG_generateCodes(PX_JpgHuffmanTable* hTable) {
    px_uint code = 0;
    px_uint i,j;
    for (i = 0; i < 16; ++i) {
        for (j = hTable->offsets[i]; j < hTable->offsets[i + 1]; ++j) {
            hTable->codes[j] = code;
            code += 1;
        }
        code <<= 1;
    }
}

// DHT contains one or more Huffman tables
px_bool PX_JpgReadHuffmanTable(PX_JpgDecoder* pJpgdecoder) {
    // "Reading DHT Marker\n";
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    PX_JpgHuffmanTable* hTable;
    px_uint allSymbols;
    px_uint i;
    px_int length = PX_MemoryStreamReadWord(pstream);
    length -= 2;

    while (length > 0) {
        px_byte tableInfo = PX_MemoryStreamReadByte(pstream);
        px_byte tableID = tableInfo & 0x0F;
        px_bool acTable = tableInfo >> 4;

        if (tableID > 3) {
            return PX_FALSE;
        }

        hTable = (acTable) ?
            &(pJpgdecoder->huffmanACTables[tableID]) :
            &(pJpgdecoder->huffmanDCTables[tableID]);

        hTable->set = PX_TRUE;

        hTable->offsets[0] = 0;
        allSymbols = 0;
        for (i = 1; i <= 16; ++i) {
            allSymbols += PX_MemoryStreamReadByte(pstream);
            hTable->offsets[i] = allSymbols;
        }
        if (allSymbols > 176) {
          // "Error - Too many symbols in Huffman table: " << allSymbols << '\n';
            
            return PX_FALSE;
        }

        for (i = 0; i < allSymbols; ++i) {
            hTable->symbols[i] = PX_MemoryStreamReadByte(pstream);
        }

        PX_JPG_generateCodes(hTable);

        length -= 17 + allSymbols;
    }

    if (length != 0) {
        return PX_FALSE;
    }
    return PX_TRUE;
}

// restart interval is needed to stay synchronized during data scans
px_bool PX_JpgReadRestartInterval(PX_JpgDecoder* pJpgdecoder) 
{
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    // "Reading DRI Marker\n";
    px_uint length = PX_MemoryStreamReadWord(pstream);

    pJpgdecoder->restartInterval = PX_MemoryStreamReadWord(pstream);
    if (length - 4 != 0) {
     
        return PX_FALSE;
    }
    return PX_TRUE;
}

// APPNs simply get skipped based on length
px_bool PX_JpgReadAPPN(PX_JpgDecoder* pJpgdecoder) {
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    px_uint i;
    //"Reading APPN Marker\n";
    px_uint length = PX_MemoryStreamReadWord(pstream);
    if (length < 2) {
        // "Error - APPN invalid\n";
        return PX_FALSE;
    }

    for ( i = 0; i < length - 2; ++i) {
        PX_MemoryStreamReadByte(pstream);
    }
    return PX_TRUE;
}

// comments simply get skipped based on length
px_bool PX_JpgReadComment(PX_JpgDecoder* pJpgdecoder) {
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    // "Reading COM Marker\n";
    px_uint length = PX_MemoryStreamReadWord(pstream);
    px_uint i;
    if (length < 2) {
        //  "Error - COM invalid\n";

        return PX_FALSE;
    }

    for (i = 0; i < length - 2; ++i) {
        PX_MemoryStreamReadByte(pstream);
    }
    return PX_TRUE;
}


static px_bool PX_JpgReadFrameHeader( PX_JpgDecoder* pJpgdecoder) {
    PX_MemoryStream* pstream=&pJpgdecoder->stream;
    // first two bytes must be 0xFF, SOI
    px_byte last = PX_MemoryStreamReadByte(pstream);
    px_byte current = PX_MemoryStreamReadByte(pstream);
    if (last != 0xFF || current != PX_JPG_MARKER_SOI) {
        return PX_FALSE;
    }
    last = PX_MemoryStreamReadByte(pstream);
    current = PX_MemoryStreamReadByte(pstream);

    // read markers until first scan
    while (PX_TRUE) 
    {
        if (PX_MemoryStreamIsEnd(pstream)) {
            return PX_FALSE;
        }
        if (last != 0xFF) {
            return PX_FALSE;
        }

        if (current == PX_JPG_MARKER_SOF0) {
            pJpgdecoder->frameType = PX_JPG_MARKER_SOF0;
            if (!PX_JpgReadStartOfFrame(pJpgdecoder))
                return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_SOF2) {
            pJpgdecoder->frameType = PX_JPG_MARKER_SOF2;
            if (!PX_JpgReadStartOfFrame(pJpgdecoder))
                return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_DQT) {
            if (!PX_JpgReadQuantizationTable(pJpgdecoder))
                return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_DHT) {
            if (!PX_JpgReadHuffmanTable(pJpgdecoder))
                return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_SOS) {
            // break from while loop at SOS
            break;
        }
        else if (current == PX_JPG_MARKER_DRI) {
            if (!PX_JpgReadRestartInterval(pJpgdecoder))
                return PX_FALSE;
        }
        else if (current >= PX_JPG_MARKER_APP0 && current <= PX_JPG_MARKER_APP15) {
            if (!PX_JpgReadAPPN(pJpgdecoder))
                return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_COM) {
            if (!PX_JpgReadComment(pJpgdecoder))
                return PX_FALSE;
        }
        // unused markers that can be skipped
        else if ((current >= PX_JPG_MARKER_JPG0 && current <= PX_JPG_MARKER_JPG13) ||
            current == PX_JPG_MARKER_DNL ||
            current == PX_JPG_MARKER_DHP ||
            current == PX_JPG_MARKER_EXP) {
            if(!PX_JpgReadComment( pJpgdecoder))
                return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_TEM) {
            // TEM has no size
        }
        // any number of 0xFF in a row is allowed and should be ignored
        else if (current == 0xFF) {
            current = PX_MemoryStreamReadByte(pstream);
            continue;
        }

        else if (current == PX_JPG_MARKER_SOI) {
            return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_EOI) {
            return PX_FALSE;
        }
        else if (current == PX_JPG_MARKER_DAC) {
            return PX_FALSE;
        }
        else if (current >= PX_JPG_MARKER_SOF0 && current <= PX_JPG_MARKER_SOF15) {
            return PX_FALSE;
        }
        else if (current >= PX_JPG_MARKER_RST0 && current <= PX_JPG_MARKER_RST7) {
            return PX_FALSE;
        }
        else {
            return PX_FALSE;
        }
        last = PX_MemoryStreamReadByte(pstream);
        current = PX_MemoryStreamReadByte(pstream);
    }
    return PX_TRUE;
}

px_bool PX_JpgVerify(px_byte* pbuffer, px_int size)
{
    px_uint32 p=0;
    // first two bytes must be 0xFF, SOI
    px_byte last = PX_ReadBitsLE(&p,pbuffer,8);
    px_byte current = PX_ReadBitsLE(&p, pbuffer, 8);
    if (last != 0xFF || current != PX_JPG_MARKER_SOI) {
        return PX_FALSE;
    }
    return PX_TRUE;
}

// SOS contains color component info for the next scan
px_bool PX_JpgReadStartOfScan(PX_JpgDecoder* pJpgdecoder) {
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    PX_JpgColorComponent* component;
    px_int length,i;
    px_byte successiveApproximation;
    //"Reading SOS Marker\n";
    if (pJpgdecoder->numComponents == 0) {
        //"Error - SOS detected before SOF\n";

        return PX_FALSE;
    }

    length = PX_MemoryStreamReadWord(pstream);

    for (i = 0; i < pJpgdecoder->numComponents; ++i) {
        pJpgdecoder->colorComponents[i].usedInScan = PX_FALSE;
    }

    // the number of components in the next scan might not be all
    //   components in the pJpgdecoder
    pJpgdecoder->componentsInScan = PX_MemoryStreamReadByte(pstream);
    if (pJpgdecoder->componentsInScan == 0) {
        return PX_FALSE;
    }
    for (i = 0; i < pJpgdecoder->componentsInScan; ++i) {
        px_byte huffmanTableIDs;
        px_byte componentID = PX_MemoryStreamReadByte(pstream);
        // component IDs are usually 1, 2, 3 but rarely can be seen as 0, 1, 2
        if (pJpgdecoder->zeroBased) {
            componentID += 1;
        }
        if (componentID == 0 || componentID > pJpgdecoder->numComponents) {
            // "Error - Invalid color component ID: " << (px_uint)componentID << '\n';
            return PX_FALSE;
        }
        component = &pJpgdecoder->colorComponents[componentID - 1];
        if (!component->usedInFrame) {
            //"Error - Invalid color component ID: " << (px_uint)componentID << '\n';
            return PX_FALSE;
        }
        if (component->usedInScan) {
            //"Error - Duplicate color component ID: " << (px_uint)componentID << '\n';
            return PX_FALSE;
        }
        component->usedInScan = PX_TRUE;

        huffmanTableIDs = PX_MemoryStreamReadByte(pstream);
        component->huffmanDCTableID = huffmanTableIDs >> 4;
        component->huffmanACTableID = huffmanTableIDs & 0x0F;
        if (component->huffmanDCTableID > 3) {
            // "Error - Invalid Huffman DC table ID: " << (px_uint)component->huffmanDCTableID << '\n';
            return PX_FALSE;
        }
        if (component->huffmanACTableID > 3) {
            // "Error - Invalid Huffman AC table ID: " << (px_uint)component->huffmanACTableID << '\n';
            return PX_FALSE;
        }
    }

    pJpgdecoder->startOfSelection = PX_MemoryStreamReadByte(pstream);
    pJpgdecoder->endOfSelection = PX_MemoryStreamReadByte(pstream);
    successiveApproximation = PX_MemoryStreamReadByte(pstream);
    pJpgdecoder->successiveApproximationHigh = successiveApproximation >> 4;
    pJpgdecoder->successiveApproximationLow = successiveApproximation & 0x0F;

    if (pJpgdecoder->frameType == PX_JPG_MARKER_SOF0) {
        // Baseline JPGs don't use spectral selection or successive approximtion
        if (pJpgdecoder->startOfSelection != 0 || pJpgdecoder->endOfSelection != 63) {
            //"Error - Invalid spectral selection\n";
            return PX_FALSE;
        }
        if (pJpgdecoder->successiveApproximationHigh != 0 || pJpgdecoder->successiveApproximationLow != 0) {
            // "Error - Invalid successive approximation\n";
            return PX_FALSE;
        }
    }
    else if (pJpgdecoder->frameType == PX_JPG_MARKER_SOF2) {
        if (pJpgdecoder->startOfSelection > pJpgdecoder->endOfSelection) {
            // "Error - Invalid spectral selection (start greater than end)\n";
            return PX_FALSE;
        }
        if (pJpgdecoder->endOfSelection > 63) {
            // "Error - Invalid spectral selection (end greater than 63)\n";
            return PX_FALSE;
        }
        if (pJpgdecoder->startOfSelection == 0 && pJpgdecoder->endOfSelection != 0) {
            // "Error - Invalid spectral selection (contains DC and AC)\n";
            return PX_FALSE;
        }
        if (pJpgdecoder->startOfSelection != 0 && pJpgdecoder->componentsInScan != 1) {
            // "Error - Invalid spectral selection (AC scan contains multiple components)\n";
            return PX_FALSE;
        }
        if (pJpgdecoder->successiveApproximationHigh != 0 &&
            pJpgdecoder->successiveApproximationLow != pJpgdecoder->successiveApproximationHigh - 1) {
            // "Error - Invalid successive approximation\n";
            return PX_FALSE;
        }
    }

    for (i = 0; i < pJpgdecoder->numComponents; ++i) {
         PX_JpgColorComponent* component = &pJpgdecoder->colorComponents[i];
        if (pJpgdecoder->colorComponents[i].usedInScan) {
            if (pJpgdecoder->quantizationTables[component->quantizationTableID].set == PX_FALSE) {
                // "Error - Color component using uninitialized quantization table\n";
                return PX_FALSE;
            }
            if (pJpgdecoder->startOfSelection == 0) {
                if (pJpgdecoder->huffmanDCTables[component->huffmanDCTableID].set == PX_FALSE) {
                    // "Error - Color component using uninitialized Huffman DC table\n";
                    return PX_FALSE;
                }
            }
            if (pJpgdecoder->endOfSelection > 0) {
                if (pJpgdecoder->huffmanACTables[component->huffmanACTableID].set == PX_FALSE) {
                    // "Error - Color component using uninitialized Huffman AC table\n";
                    return PX_FALSE;
                }
            }
        }
    }

    if (length - 6 - (2 * pJpgdecoder->componentsInScan) != 0) {
        // "Error - SOS invalid\n";
        return PX_FALSE;
    }
    return PX_TRUE;
}

// return the symbol from the Huffman table that corresponds to
//   the next Huffman code read from the BitReader
px_byte PX_JpgGetNextSymbol(PX_JpgDecoder* pJpgdecoder,  PX_JpgHuffmanTable* hTable) {
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    px_uint currentCode = 0;
    px_uint i,j;
    for (i = 0; i < 16; ++i) {
        px_int bit = PX_JpgReadBit(pstream);
        if (bit == -1) {
            return -1;
        }

        currentCode = (currentCode << 1) | bit;
        for (j = hTable->offsets[i]; j < hTable->offsets[i + 1]; ++j) {
            if (currentCode == hTable->codes[j]) {
                return hTable->symbols[j];
            }
        }
    }
    return -1;
}

// fill the coefficients of a block component based on Huffman codes
//   read from the BitReader
px_bool PX_JpgDecodeBlockComponent(
    PX_JpgDecoder* pJpgdecoder,
    px_int*  component,
    px_int* previousDC,
    px_uint* skips,
     PX_JpgHuffmanTable* dcTable,
     PX_JpgHuffmanTable* acTable
) 
{
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    px_uint i,j;

    if (pJpgdecoder->frameType == PX_JPG_MARKER_SOF0) 
    {
        // get the DC value for this block component
        px_int coeff, numZeroes, coeffLength;
        px_byte length = PX_JpgGetNextSymbol(pJpgdecoder, dcTable);

        if (length == (px_byte)-1) {
            // "Error - Invalid DC value\n";
            return PX_FALSE;
        }
        if (length > 11) {
            // "Error - DC coefficient length greater than 11\n";
            return PX_FALSE;
        }

        coeff = PX_JpgReadBits(pstream,length);
        if (coeff == -1) {
            // "Error - Invalid DC value\n";
            return PX_FALSE;
        }
        if (length != 0 && coeff < (1 << (length - 1))) {
            coeff -= (1 << length) - 1;
        }
        component[0] = coeff + (*previousDC);
        (*previousDC) = component[0];

        // get the AC values for this block component
        for (i = 1; i < 64; ++i) {
            px_byte symbol = PX_JpgGetNextSymbol(pJpgdecoder, acTable);

            if (symbol == (px_byte)-1) {
                // "Error - Invalid AC value\n";
                return PX_FALSE;
            }

            // symbol 0x00 means fill remainder of component with 0
            if (symbol == 0x00) {
                return PX_TRUE;
            }

            // otherwise, read next component coefficient
            numZeroes = symbol >> 4;
            coeffLength = symbol & 0x0F;
            coeff = 0;

            if (i + numZeroes >= 64) {
                // "Error - Zero run-length exceeded block component\n";
                return PX_FALSE;
            }
            i += numZeroes;

            if (coeffLength > 10) {
                // "Error - AC coefficient length greater than 10\n";
                return PX_FALSE;
            }
            coeff = PX_JpgReadBits(pstream,coeffLength);
            if (coeff == -1) {
                // "Error - Invalid AC value\n";
                return PX_FALSE;
            }
            if (coeff < (1 << (coeffLength - 1))) {
                coeff -= (1 << coeffLength) - 1;
            }
            component[zigZagMap[i]] = coeff;
        }
        return PX_TRUE;
    }
    else { // pJpgdecoder->frameType == SOF2
        if (pJpgdecoder->startOfSelection == 0 && pJpgdecoder->successiveApproximationHigh == 0) 
        {
            // DC first visit
            px_byte length = PX_JpgGetNextSymbol(pJpgdecoder, dcTable);
            px_int coeff;
            if (length == (px_byte)-1) {
                // "Error - Invalid DC value\n";
                return PX_FALSE;
            }
            if (length > 11) {
                // "Error - DC coefficient length greater than 11\n";
                return PX_FALSE;
            }

            coeff = PX_JpgReadBits(pstream, length);
            if (coeff == -1) {
                // "Error - Invalid DC value\n";
                return PX_FALSE;
            }
            if (length != 0 && coeff < (1 << (length - 1))) {
                coeff -= (1 << length) - 1;
            }
            coeff += (*previousDC);
            (*previousDC) = coeff;
            component[0] = coeff << pJpgdecoder->successiveApproximationLow;
            return PX_TRUE;
        }
        else if (pJpgdecoder->startOfSelection == 0 && pJpgdecoder->successiveApproximationHigh != 0) 
        {
            // DC refinement
            int bit = PX_JpgReadBit(pstream);
            if (bit == -1) {
                // "Error - Invalid DC value\n";
                return PX_FALSE;
            }
            component[0] |= bit << pJpgdecoder->successiveApproximationLow;
            return PX_TRUE;
        }
        else if (pJpgdecoder->startOfSelection != 0 && pJpgdecoder->successiveApproximationHigh == 0) {
            // AC first visit
            if ((*skips) > 0) {
                (*skips) -= 1;
                return PX_TRUE;
            }
            for (i = pJpgdecoder->startOfSelection; i <= pJpgdecoder->endOfSelection; ++i) {
                px_byte numZeroes, coeffLength;

                px_byte symbol = PX_JpgGetNextSymbol(pJpgdecoder, acTable);
                if (symbol == (px_byte)-1) {
                    // "Error - Invalid AC value\n";
                    return PX_FALSE;
                }

                numZeroes = symbol >> 4;
                coeffLength = symbol & 0x0F;

                if (coeffLength != 0) {
                    px_int coeff;
                    if (i + numZeroes > pJpgdecoder->endOfSelection) {
                        // "Error - Zero run-length exceeded spectral selection\n";
                        return PX_FALSE;
                    }
                    for (j = 0; j < numZeroes; ++j, ++i) {
                        component[zigZagMap[i]] = 0;
                    }
                    if (coeffLength > 10) {
                        // "Error - AC coefficient length greater than 10\n";
                        return PX_FALSE;
                    }

                    coeff = PX_JpgReadBits(pstream, coeffLength);
                    if (coeff == -1) {
                        // "Error - Invalid AC value\n";
                        return PX_FALSE;
                    }
                    if (coeff < (1 << (coeffLength - 1))) {
                        coeff -= (1 << coeffLength) - 1;
                    }
                    component[zigZagMap[i]] = coeff << pJpgdecoder->successiveApproximationLow;
                }
                else {
                    if (numZeroes == 15) {
                        if (i + numZeroes > pJpgdecoder->endOfSelection) {
                            // "Error - Zero run-length exceeded spectral selection\n";
                            return PX_FALSE;
                        }
                        for (j = 0; j < numZeroes; ++j, ++i) {
                            component[zigZagMap[i]] = 0;
                        }
                    }
                    else {
                        px_uint extraSkips;
                        (*skips) = (1 << numZeroes) - 1;
                        extraSkips = PX_JpgReadBits(pstream, numZeroes);
                        if (extraSkips == (px_uint)-1) {
                            // "Error - Invalid AC value\n";
                            return PX_FALSE;
                        }
                        (*skips) += extraSkips;
                        break;
                    }
                }
            }
            return PX_TRUE;
        }
        else { // pJpgdecoder->startOfSelection != 0 && pJpgdecoder->successiveApproximationHigh != 0
            // AC refinement
            px_int positive = 1 << pJpgdecoder->successiveApproximationLow;
            px_int negative = ((px_uint)-1) << pJpgdecoder->successiveApproximationLow;
            px_int i = pJpgdecoder->startOfSelection;
            if ((*skips) == 0) {
                for (; i <= pJpgdecoder->endOfSelection; ++i) {
                    px_byte numZeroes, coeffLength;
                    px_int coeff;
                    px_byte symbol = PX_JpgGetNextSymbol(pJpgdecoder, acTable);
                    if (symbol == (px_byte)-1) {
                        // "Error - Invalid AC value\n";
                        return PX_FALSE;
                    }

                    numZeroes = symbol >> 4;
                    coeffLength = symbol & 0x0F;
                    coeff = 0;

                    if (coeffLength != 0) {
                        if (coeffLength != 1) {
                            // "Error - Invalid AC value\n";
                            return PX_FALSE;
                        }
                        switch (PX_JpgReadBit(pstream)) {
                        case 1:
                            coeff = positive;
                            break;
                        case 0:
                            coeff = negative;
                            break;
                        default: // -1, data stream is empty
                            // "Error - Invalid AC value\n";
                            return PX_FALSE;
                        }
                    }
                    else {
                        if (numZeroes != 15) {
                            px_uint extraSkips;
                            (*skips) = 1 << numZeroes;
                            extraSkips = PX_JpgReadBits(pstream, numZeroes);
                            if (extraSkips == (px_uint)-1) {
                                // "Error - Invalid AC value\n";
                                return PX_FALSE;
                            }
                            (*skips) += extraSkips;
                            break;
                        }
                    }

                    do {
                        if (component[zigZagMap[i]] != 0) {
                            switch (PX_JpgReadBit(pstream)) {
                            case 1:
                                if ((component[zigZagMap[i]] & positive) == 0) {
                                    if (component[zigZagMap[i]] >= 0) {
                                        component[zigZagMap[i]] += positive;
                                    }
                                    else {
                                        component[zigZagMap[i]] += negative;
                                    }
                                }
                                break;
                            case 0:
                                // do nothing
                                break;
                            default: // -1, data stream is empty
                                // "Error - Invalid AC value\n";
                                return PX_FALSE;
                            }
                        }
                        else {
                            if (numZeroes == 0) {
                                break;
                            }
                            numZeroes -= 1;
                        }

                        i += 1;
                    } while (i <= pJpgdecoder->endOfSelection);

                    if (coeff != 0 && i <= pJpgdecoder->endOfSelection) {
                        component[zigZagMap[i]] = coeff;
                    }
                }
            }

            if ((*skips) > 0) {
                for (; i <= pJpgdecoder->endOfSelection; ++i) {
                    if (component[zigZagMap[i]] != 0) {
                        switch (PX_JpgReadBit(pstream)) {
                        case 1:
                            if ((component[zigZagMap[i]] & positive) == 0) {
                                if (component[zigZagMap[i]] >= 0) {
                                    component[zigZagMap[i]] += positive;
                                }
                                else {
                                    component[zigZagMap[i]] += negative;
                                }
                            }
                            break;
                        case 0:
                            // do nothing
                            break;
                        default: // -1, data stream is empty
                            // "Error - Invalid AC value\n";
                            return PX_FALSE;
                        }
                    }
                }
                (*skips) -= 1;
            }
            return PX_TRUE;
        }
    }
    return PX_FALSE;
}
// decode all the Huffman data and fill all MCUs

px_bool PX_JpgDecodeHuffmanData(PX_JpgDecoder* pJpgdecoder)
{
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    px_int previousDCs[3] = { 0 };
    px_uint skips = 0;
    px_int i,x,y,v,h;
    px_bool luminanceOnly = pJpgdecoder->componentsInScan == 1 && pJpgdecoder->colorComponents[0].usedInScan;
    px_uint yStep = luminanceOnly ? 1 : pJpgdecoder->verticalSamplingFactor;
    px_uint xStep = luminanceOnly ? 1 : pJpgdecoder->horizontalSamplingFactor;
    px_uint restartInterval = pJpgdecoder->restartInterval * xStep * yStep;
    
    for (y = 0; y < pJpgdecoder->blockHeight; y += yStep) {
        for (x = 0; x < pJpgdecoder->blockWidth; x += xStep) {
            if (restartInterval != 0 && (y * pJpgdecoder->blockWidthReal + x) % restartInterval == 0) {
                previousDCs[0] = 0;
                previousDCs[1] = 0;
                previousDCs[2] = 0;
                skips = 0;
                PX_MemoryStreamAlign(pstream);
            }

            for (i = 0; i < pJpgdecoder->numComponents; ++i)
            {
                 PX_JpgColorComponent* component = &pJpgdecoder->colorComponents[i];
                if (component->usedInScan) {
                    px_int vMax = luminanceOnly ? 1 : component->verticalSamplingFactor;
                    px_int hMax = luminanceOnly ? 1 : component->horizontalSamplingFactor;
                    for (v = 0; v < vMax; ++v) {
                        for (h = 0; h < hMax; ++h) 
                        {

                            if(i==0)
                                if (!PX_JpgDecodeBlockComponent(
                                    pJpgdecoder,
                                    pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].y,
                                    &previousDCs[i],
                                    &skips,
                                    &pJpgdecoder->huffmanDCTables[component->huffmanDCTableID],
                                    &pJpgdecoder->huffmanACTables[component->huffmanACTableID])) 
                                {
                                    return PX_FALSE;
                                }
                             

                            if (i == 1)
                                if (!PX_JpgDecodeBlockComponent(
                                    pJpgdecoder,
                                    pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].cb,
                                    &previousDCs[i],
                                    &skips,
                                    &pJpgdecoder->huffmanDCTables[component->huffmanDCTableID],
                                    &pJpgdecoder->huffmanACTables[component->huffmanACTableID])) 
                                {
                                    return PX_FALSE;
                                }
                           
                            if (i == 2)
                                if (!PX_JpgDecodeBlockComponent(
                                    pJpgdecoder,
                                    pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].cr,
                                    &previousDCs[i],
                                    &skips,
                                    &pJpgdecoder->huffmanDCTables[component->huffmanDCTableID],
                                    &pJpgdecoder->huffmanACTables[component->huffmanACTableID])) {
                                    return PX_FALSE;
                                }
                              
                        }
                    }
                }
            }
        }
    }
    return PX_TRUE;
}


px_bool PX_JpgReadScans(PX_JpgDecoder* pJpgdecoder) 
{
    PX_MemoryStream* pstream = &pJpgdecoder->stream;
    px_byte last, current;
    // decode first scan
    if (!PX_JpgReadStartOfScan(pJpgdecoder))
        return PX_FALSE;

    if (!PX_JpgDecodeHuffmanData(pJpgdecoder))
        return PX_FALSE;

    PX_MemoryStreamAlign(&pJpgdecoder->stream);
    while (pstream->bitpointer%8==0&& pstream->bitpointer)
    {
        if (pstream->bitstream[pstream->bitpointer / 8 - 1] == 0xff)
        {
            pstream->bitpointer += 8;
        }
        else
        {
			break;
		}
    }

    last = PX_MemoryStreamReadByte(pstream);
    current = PX_MemoryStreamReadByte(pstream);

    if (last != 0xFF) {
        // "Error - Expected a marker\n";
        return PX_FALSE;
    }
  
    return PX_TRUE;
}




// dequantize a block component based on a quantization table
px_void PX_JpgDequantizeBlockComponent(PX_JpgQuantizationTable* qTable, px_int* component) {
    px_uint i;
    for (i = 0; i < 64; ++i) {
        component[i] *= qTable->table[i];
    }
}

// dequantize all MCUs
px_bool PX_JpgDequantize(PX_JpgDecoder* pJpgdecoder) {
    px_int y, x, i, v, h;
    for (y = 0; y < pJpgdecoder->blockHeight; y += pJpgdecoder->verticalSamplingFactor) {
        for (x = 0; x < pJpgdecoder->blockWidth; x += pJpgdecoder->horizontalSamplingFactor) {
            for (i = 0; i < pJpgdecoder->numComponents; ++i) {
                PX_JpgColorComponent* component = &pJpgdecoder->colorComponents[i];
                for (v = 0; v < component->verticalSamplingFactor; ++v) {
                    for (h = 0; h < component->horizontalSamplingFactor; ++h) {
                        if (i == 0)
                            PX_JpgDequantizeBlockComponent(&pJpgdecoder->quantizationTables[component->quantizationTableID],
                                pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].y);
                        else if (i == 1)
                            PX_JpgDequantizeBlockComponent(&pJpgdecoder->quantizationTables[component->quantizationTableID],
                                pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].cb);
                        else if (i == 2)
                            PX_JpgDequantizeBlockComponent(&pJpgdecoder->quantizationTables[component->quantizationTableID],
                                pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].cr);
                        else
                        {
                            return PX_FALSE;
                        }
                    }
                }
            }
        }
    }
    return PX_TRUE;
}

// perform 1-D IDCT on all columns and rows of a block component
//   resulting in 2-D IDCT
px_void PX_JpgInverseDCTBlockComponent(px_int* component) {
    // IDCT scaling factors
    px_uint i;
    px_float m0 = 1.8477590650225739f;
    px_float m1 = 1.4142135623730949f;
    px_float m3 = 1.4142135623730949f;
    px_float m5 = 0.76536686473017945f;
    px_float m2 = 1.0823922002923945f;
    px_float m4 = 2.6131259297527532f;

    px_float s0 = 0.3535533905932739f;
    px_float s1 = 0.49039264020161516f;
    px_float s2 = 0.46193976625564348f;
    px_float s3 = 0.41573480615127262f;
    px_float s4 = 0.35355339059327373f;
    px_float s5 = 0.27778511650980114f;
    px_float s6 = 0.19134171618254486f;
    px_float s7 = 0.097545161008064124f;

    for (i = 0; i < 8; ++i) {
        px_float g0 = component[0 * 8 + i] * s0;
        px_float g1 = component[4 * 8 + i] * s4;
        px_float g2 = component[2 * 8 + i] * s2;
        px_float g3 = component[6 * 8 + i] * s6;
        px_float g4 = component[5 * 8 + i] * s5;
        px_float g5 = component[1 * 8 + i] * s1;
        px_float g6 = component[7 * 8 + i] * s7;
        px_float g7 = component[3 * 8 + i] * s3;

        px_float f0 = g0;
        px_float f1 = g1;
        px_float f2 = g2;
        px_float f3 = g3;
        px_float f4 = g4 - g7;
        px_float f5 = g5 + g6;
        px_float f6 = g5 - g6;
        px_float f7 = g4 + g7;

        px_float e0 = f0;
        px_float e1 = f1;
        px_float e2 = f2 - f3;
        px_float e3 = f2 + f3;
        px_float e4 = f4;
        px_float e5 = f5 - f7;
        px_float e6 = f6;
        px_float e7 = f5 + f7;
        px_float e8 = f4 + f6;

        px_float d0 = e0;
        px_float d1 = e1;
        px_float d2 = e2 * m1;
        px_float d3 = e3;
        px_float d4 = e4 * m2;
        px_float d5 = e5 * m3;
        px_float d6 = e6 * m4;
        px_float d7 = e7;
        px_float d8 = e8 * m5;

        px_float c0 = d0 + d1;
        px_float c1 = d0 - d1;
        px_float c2 = d2 - d3;
        px_float c3 = d3;
        px_float c4 = d4 + d8;
        px_float c5 = d5 + d7;
        px_float c6 = d6 - d8;
        px_float c7 = d7;
        px_float c8 = c5 - c6;

        px_float b0 = c0 + c3;
        px_float b1 = c1 + c2;
        px_float b2 = c1 - c2;
        px_float b3 = c0 - c3;
        px_float b4 = c4 - c8;
        px_float b5 = c8;
        px_float b6 = c6 - c7;
        px_float b7 = c7;

        component[0 * 8 + i] = (px_int)(b0 + b7);
        component[1 * 8 + i] = (px_int)(b1 + b6);
        component[2 * 8 + i] = (px_int)(b2 + b5);
        component[3 * 8 + i] = (px_int)(b3 + b4);
        component[4 * 8 + i] = (px_int)(b3 - b4);
        component[5 * 8 + i] = (px_int)(b2 - b5);
        component[6 * 8 + i] = (px_int)(b1 - b6);
        component[7 * 8 + i] = (px_int)(b0 - b7);
    }
    for (i = 0; i < 8; ++i) {
        px_float g0 = component[i * 8 + 0] * s0;
        px_float g1 = component[i * 8 + 4] * s4;
        px_float g2 = component[i * 8 + 2] * s2;
        px_float g3 = component[i * 8 + 6] * s6;
        px_float g4 = component[i * 8 + 5] * s5;
        px_float g5 = component[i * 8 + 1] * s1;
        px_float g6 = component[i * 8 + 7] * s7;
        px_float g7 = component[i * 8 + 3] * s3;

        px_float f0 = g0;
        px_float f1 = g1;
        px_float f2 = g2;
        px_float f3 = g3;
        px_float f4 = g4 - g7;
        px_float f5 = g5 + g6;
        px_float f6 = g5 - g6;
        px_float f7 = g4 + g7;

        px_float e0 = f0;
        px_float e1 = f1;
        px_float e2 = f2 - f3;
        px_float e3 = f2 + f3;
        px_float e4 = f4;
        px_float e5 = f5 - f7;
        px_float e6 = f6;
        px_float e7 = f5 + f7;
        px_float e8 = f4 + f6;

        px_float d0 = e0;
        px_float d1 = e1;
        px_float d2 = e2 * m1;
        px_float d3 = e3;
        px_float d4 = e4 * m2;
        px_float d5 = e5 * m3;
        px_float d6 = e6 * m4;
        px_float d7 = e7;
        px_float d8 = e8 * m5;

        px_float c0 = d0 + d1;
        px_float c1 = d0 - d1;
        px_float c2 = d2 - d3;
        px_float c3 = d3;
        px_float c4 = d4 + d8;
        px_float c5 = d5 + d7;
        px_float c6 = d6 - d8;
        px_float c7 = d7;
        px_float c8 = c5 - c6;

        px_float b0 = c0 + c3;
        px_float b1 = c1 + c2;
        px_float b2 = c1 - c2;
        px_float b3 = c0 - c3;
        px_float b4 = c4 - c8;
        px_float b5 = c8;
        px_float b6 = c6 - c7;
        px_float b7 = c7;

        component[i * 8 + 0] = (px_int)(b0 + b7);
        component[i * 8 + 1] = (px_int)(b1 + b6);
        component[i * 8 + 2] = (px_int)(b2 + b5);
        component[i * 8 + 3] = (px_int)(b3 + b4);
        component[i * 8 + 4] = (px_int)(b3 - b4);
        component[i * 8 + 5] = (px_int)(b2 - b5);
        component[i * 8 + 6] = (px_int)(b1 - b6);
        component[i * 8 + 7] = (px_int)(b0 - b7);
    }
}

// perform IDCT on all MCUs
px_bool PX_JpgInverseDCT(PX_JpgDecoder* pJpgdecoder) {

    px_int y, x, i, v, h;
    for (y = 0; y < pJpgdecoder->blockHeight; y += pJpgdecoder->verticalSamplingFactor) {
        for (x = 0; x < pJpgdecoder->blockWidth; x += pJpgdecoder->horizontalSamplingFactor) {
            for (i = 0; i < pJpgdecoder->numComponents; ++i) {
                PX_JpgColorComponent* component = &pJpgdecoder->colorComponents[i];
                for (v = 0; v < component->verticalSamplingFactor; ++v) {
                    for (h = 0; h < component->horizontalSamplingFactor; ++h) {
                        if (i == 0)
                            PX_JpgInverseDCTBlockComponent(pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].y);
                        else if (i == 1)
                            PX_JpgInverseDCTBlockComponent(pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].cb);
                        else if (i == 2)
                            PX_JpgInverseDCTBlockComponent(pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)].cr);
                        else
                            return PX_FALSE;
                    }
                }
            }
        }
    }
    return PX_TRUE;
}

// convert all pixels in a block from YCbCr color space to RGB
px_void PX_JpgYCbCrToRGBBlock(PX_JpgBlock* yBlock, PX_JpgBlock* cbcrBlock, px_uint vSamp, px_uint hSamp, px_uint v, px_uint h)
{
    px_uint x, y;
    for (y = 7; y < 8; --y) {
        for (x = 7; x < 8; --x) {
            px_uint pixel = y * 8 + x;
            px_uint cbcrPixelRow = y / vSamp + 4 * v;
            px_uint cbcrPixelColumn = x / hSamp + 4 * h;
            px_uint cbcrPixel = cbcrPixelRow * 8 + cbcrPixelColumn;
            px_int r = (px_int)(yBlock->y[pixel] + 1.402f * cbcrBlock->cr[cbcrPixel] + 128);
            px_int g = (px_int)(yBlock->y[pixel] - 0.344f * cbcrBlock->cb[cbcrPixel] - 0.714f * cbcrBlock->cr[cbcrPixel] + 128);
            px_int b = (px_int)(yBlock->y[pixel] + 1.772f * cbcrBlock->cb[cbcrPixel] + 128);
            if (r < 0)   r = 0;
            if (r > 255) r = 255;
            if (g < 0)   g = 0;
            if (g > 255) g = 255;
            if (b < 0)   b = 0;
            if (b > 255) b = 255;
            yBlock->r[pixel] = r;
            yBlock->g[pixel] = g;
            yBlock->b[pixel] = b;
        }
    }
}

// convert all pixels from YCbCr color space to RGB
px_void PX_JpgYCbCrToRGB(PX_JpgDecoder* pJpgdecoder)
{
    px_int x, y, v, h;
    px_int vSamp = pJpgdecoder->verticalSamplingFactor;
    px_int hSamp = pJpgdecoder->horizontalSamplingFactor;
    for (y = 0; y < pJpgdecoder->blockHeight; y += vSamp)
    {
        for (x = 0; x < pJpgdecoder->blockWidth; x += hSamp)
        {
            PX_JpgBlock* cbcrBlock = &pJpgdecoder->blocks[y * pJpgdecoder->blockWidthReal + x];
            for (v = vSamp - 1; v >=0 ; --v) 
            {
                for (h = hSamp - 1; h >=0; --h) 
                {
                    PX_JpgBlock* yBlock = &pJpgdecoder->blocks[(y + v) * pJpgdecoder->blockWidthReal + (x + h)];
                    PX_JpgYCbCrToRGBBlock(yBlock, cbcrBlock, vSamp, hSamp, v, h);
                }
            }
        }
    }
}


px_bool PX_JpgDecoderInitialize(px_memorypool* mp, PX_JpgDecoder* decoder, px_byte* pbuffer, px_int size)
{
    
    PX_memset(decoder, 0, sizeof(PX_JpgDecoder));
    decoder->mp = mp;
    PX_MemoryStreamInitialize(&decoder->stream, pbuffer, size);

    if (!PX_JpgReadFrameHeader(decoder))
    {
        return PX_FALSE;
    }

    decoder->blocks =(PX_JpgBlock *)MP_Malloc(mp,sizeof(PX_JpgBlock)* decoder->blockHeightReal * decoder->blockWidthReal);
    if (decoder->blocks == 0) {
        return PX_FALSE;
    }
    PX_memset(decoder->blocks, 0, sizeof(PX_JpgBlock) * decoder->blockHeightReal * decoder->blockWidthReal);
    if (!PX_JpgReadScans(decoder))
    {
        goto _ERROR;
    }

    // dequantize DCT coefficients
    if (!PX_JpgDequantize(decoder))
    {
        goto _ERROR;
    }

    // Inverse Discrete Cosine Transform
    if (!PX_JpgInverseDCT(decoder))
    {
        goto _ERROR;
    }

    // color conversion
    PX_JpgYCbCrToRGB(decoder);
    return PX_TRUE;
_ERROR:
    MP_Free(decoder->mp, decoder->blocks);
    return PX_FALSE;
}

px_int PX_JpgDecoderGetWidth(PX_JpgDecoder* decoder)
{
    return decoder->width;
}
px_int PX_JpgDecoderGetHeight(PX_JpgDecoder* decoder)
{
    return decoder->height;
}

px_void PX_JpgDecoderFree(PX_JpgDecoder* decoder)
{
    MP_Free(decoder->mp, decoder->blocks);
}

px_void PX_JpgDecoderRenderToSurface(PX_JpgDecoder* decoder, px_surface* prendersurface)
{
    px_int height = decoder->height < prendersurface->height ? decoder->height : prendersurface->height;
    px_int width = decoder->width < prendersurface->width ? decoder->width : prendersurface->width;
    px_int x, y;

    for (y = 0; y < height;y++) {
         px_int blockRow = y / 8;
         px_int pixelRow = y % 8;
        for (x = 0; x < width; ++x) {
             px_int blockColumn = x / 8;
             px_int pixelColumn = x % 8;
             px_int blockIndex = blockRow * decoder->blockWidthReal + blockColumn;
             px_int pixelIndex = pixelRow * 8 + pixelColumn;
             px_byte a, r, g, b;
            b = (px_byte)(decoder->blocks[blockIndex].b[pixelIndex]);
            g = (px_byte)(decoder->blocks[blockIndex].g[pixelIndex]);
            r = (px_byte)(decoder->blocks[blockIndex].r[pixelIndex]);
            a = 255;
            PX_SurfaceSetPixel(prendersurface, x, y, PX_COLOR(a, r, g, b));
        }
    }
    
}

