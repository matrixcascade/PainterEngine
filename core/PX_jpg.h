/////////////////////////////////////////////////////////////////////
//sources by dannye https://github.com/dannye/jed with no license
//rewritten by dbinary 2022/08/09
////////////////////////////////////////////////////////////////////

#ifndef PX_JPG_H
#define PX_JPG_H

#include "PX_Memory.h"
#include "PX_Surface.h"

// Start of Frame markers, non-differential, Huffman coding
typedef enum
{
PX_JPG_MARKER_SOF0 = 0xC0, // Baseline DCT
PX_JPG_MARKER_SOF1 = 0xC1, // Extended sequential DCT
PX_JPG_MARKER_SOF2 = 0xC2, // Progressive DCT
PX_JPG_MARKER_SOF3 = 0xC3, // Lossless (sequential)

// Start of Frame markers, differential, Huffman coding
PX_JPG_MARKER_SOF5 = 0xC5, // Differential sequential DCT
PX_JPG_MARKER_SOF6 = 0xC6, // Differential progressive DCT
PX_JPG_MARKER_SOF7 = 0xC7, // Differential lossless (sequential)

// Start of Frame markers, non-differential, arithmetic coding
PX_JPG_MARKER_SOF9 = 0xC9, // Extended sequential DCT
PX_JPG_MARKER_SOF10 = 0xCA, // Progressive DCT
PX_JPG_MARKER_SOF11 = 0xCB, // Lossless (sequential)

// Start of Frame markers, differential, arithmetic coding
PX_JPG_MARKER_SOF13 = 0xCD, // Differential sequential DCT
PX_JPG_MARKER_SOF14 = 0xCE, // Differential progressive DCT
PX_JPG_MARKER_SOF15 = 0xCF, // Differential lossless (sequential)

// Define Huffman Table(s)
PX_JPG_MARKER_DHT = 0xC4,

// JPEG extensions
PX_JPG_MARKER_JPG = 0xC8,

// Define Arithmetic Coding Conditioning(s)
PX_JPG_MARKER_DAC = 0xCC,

// Restart interval Markers
PX_JPG_MARKER_RST0 = 0xD0,
PX_JPG_MARKER_RST1 = 0xD1,
PX_JPG_MARKER_RST2 = 0xD2,
PX_JPG_MARKER_RST3 = 0xD3,
PX_JPG_MARKER_RST4 = 0xD4,
PX_JPG_MARKER_RST5 = 0xD5,
PX_JPG_MARKER_RST6 = 0xD6,
PX_JPG_MARKER_RST7 = 0xD7,

// Other Markers
PX_JPG_MARKER_SOI = 0xD8, // Start of Image
PX_JPG_MARKER_EOI = 0xD9, // End of Image
PX_JPG_MARKER_SOS = 0xDA, // Start of Scan
PX_JPG_MARKER_DQT = 0xDB, // Define Quantization Table(s)
PX_JPG_MARKER_DNL = 0xDC, // Define Number of Lines
PX_JPG_MARKER_DRI = 0xDD, // Define Restart Interval
PX_JPG_MARKER_DHP = 0xDE, // Define Hierarchical Progression
PX_JPG_MARKER_EXP = 0xDF, // Expand Reference Component(s)

// APPN Markers
PX_JPG_MARKER_APP0 = 0xE0,
PX_JPG_MARKER_APP1 = 0xE1,
PX_JPG_MARKER_APP2 = 0xE2,
PX_JPG_MARKER_APP3 = 0xE3,
PX_JPG_MARKER_APP4 = 0xE4,
PX_JPG_MARKER_APP5 = 0xE5,
PX_JPG_MARKER_APP6 = 0xE6,
PX_JPG_MARKER_APP7 = 0xE7,
PX_JPG_MARKER_APP8 = 0xE8,
PX_JPG_MARKER_APP9 = 0xE9,
PX_JPG_MARKER_APP10 = 0xEA,
PX_JPG_MARKER_APP11 = 0xEB,
PX_JPG_MARKER_APP12 = 0xEC,
PX_JPG_MARKER_APP13 = 0xED,
PX_JPG_MARKER_APP14 = 0xEE,
PX_JPG_MARKER_APP15 = 0xEF,

// Misc Markers
PX_JPG_MARKER_JPG0 = 0xF0,
PX_JPG_MARKER_JPG1 = 0xF1,
PX_JPG_MARKER_JPG2 = 0xF2,
PX_JPG_MARKER_JPG3 = 0xF3,
PX_JPG_MARKER_JPG4 = 0xF4,
PX_JPG_MARKER_JPG5 = 0xF5,
PX_JPG_MARKER_JPG6 = 0xF6,
PX_JPG_MARKER_JPG7 = 0xF7,
PX_JPG_MARKER_JPG8 = 0xF8,
PX_JPG_MARKER_JPG9 = 0xF9,
PX_JPG_MARKER_JPG10 = 0xFA,
PX_JPG_MARKER_JPG11 = 0xFB,
PX_JPG_MARKER_JPG12 = 0xFC,
PX_JPG_MARKER_JPG13 = 0xFD,
PX_JPG_MARKER_COM = 0xFE,
PX_JPG_MARKER_TEM = 0x01,
}PX_JPG_MARKER;



typedef struct  {
    px_uint table[64];
    px_bool set;
}PX_JpgQuantizationTable;

typedef struct  {
    px_byte offsets[17] ;
    px_byte symbols[176] ;
    px_uint codes[176] ;
    px_bool set;
}PX_JpgHuffmanTable;

typedef struct  {
    px_byte horizontalSamplingFactor;
    px_byte verticalSamplingFactor;
    px_byte quantizationTableID;
    px_byte huffmanDCTableID;
    px_byte huffmanACTableID;
    px_bool usedInFrame;
    px_bool usedInScan;
}PX_JpgColorComponent;

typedef struct  {
    union {
        px_int y[64];
        px_int r[64];
    };
    union {
        px_int cb[64];
        px_int g [64];
    };
    union {
        px_int cr[64];
        px_int b [64];
    };
}PX_JpgBlock;

typedef struct
{
    px_memorypool* mp;
    px_int width;
    px_int height;

    PX_MemoryStream stream;
    px_int numComponents;
    px_bool zeroBased;
    PX_JPG_MARKER frameType;

    PX_JpgQuantizationTable quantizationTables[4];
    PX_JpgHuffmanTable huffmanDCTables[4];
    PX_JpgHuffmanTable huffmanACTables[4];
    PX_JpgColorComponent colorComponents[3];

    px_byte componentsInScan;
    px_byte startOfSelection;
    px_byte endOfSelection;
    px_byte successiveApproximationHigh;
    px_byte successiveApproximationLow;

    px_uint restartInterval;

    PX_JpgBlock* blocks;
    px_int blockHeight;
    px_int blockWidth;
    px_int blockHeightReal;
    px_int blockWidthReal;

    px_int horizontalSamplingFactor;
    px_int verticalSamplingFactor;

}PX_JpgDecoder;

px_bool PX_JpgDecoderInitialize(px_memorypool* mp, PX_JpgDecoder* decoder, px_byte* pbuffer, px_int size);
px_bool PX_JpgVerify(px_byte* pbuffer, px_int size);
px_int PX_JpgDecoderGetWidth(PX_JpgDecoder* decoder);
px_int PX_JpgDecoderGetHeight(PX_JpgDecoder* decoder);
px_void PX_JpgDecoderRenderToSurface(PX_JpgDecoder* decoder, px_surface* prendersurface);
px_void PX_JpgDecoderFree(PX_JpgDecoder* decoder);
#endif
