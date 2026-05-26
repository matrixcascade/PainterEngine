#ifndef __PX_QRCODE_H
#define __PX_QRCODE_H

#include "PX_Typedef.h"
#include "PX_BaseGeo.h"
/*
License
-------

Copyright © 2025 Project Nayuki. (MIT License)
[https://www.nayuki.io/page/qr-code-generator-library](https://www.nayuki.io/page/qr-code-generator-library)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

* The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

* The Software is provided "as is", without warranty of any kind, express or
  implied, including but not limited to the warranties of merchantability,
  fitness for a particular purpose and noninfringement. In no event shall the
  authors or copyright holders be liable for any claim, damages or other
  liability, whether in an action of contract, tort or otherwise, arising from,
  out of or in connection with the Software or the use or other dealings in the
  Software.


//demo: generate a QR Code from text and read its modules
//
//  px_byte tempBuffer[PX_QRCODE_BUFFER_LEN_MAX];
//  px_byte qrcode[PX_QRCODE_BUFFER_LEN_MAX];
//
//  px_bool ok = PX_QRCode_EncodeText("https://painterengine.com",
//      tempBuffer, qrcode,
//      PX_QRCODE_ECC_LOW, 1, 40, PX_QRCODE_MASK_AUTO, PX_TRUE);
//
//  if (ok)
//  {
//      px_int size = PX_QRCode_GetSize(qrcode);
//      px_int x, y;
//      for (y = 0; y < size; y++)
//      {
//          for (x = 0; x < size; x++)
//          {
//              if (PX_QRCode_GetModule(qrcode, x, y))
//              {
//                  // dark module
//              }
//              else
//              {
//                  // light module
//              }
//          }
//      }
//  }
*/

typedef enum
{
	PX_QRCODE_ECC_LOW = 0,
	PX_QRCODE_ECC_MEDIUM,
	PX_QRCODE_ECC_QUARTILE,
	PX_QRCODE_ECC_HIGH,
}PX_QRCODE_ECC;

typedef enum
{
	PX_QRCODE_MASK_AUTO = -1,
	PX_QRCODE_MASK_0 = 0,
	PX_QRCODE_MASK_1,
	PX_QRCODE_MASK_2,
	PX_QRCODE_MASK_3,
	PX_QRCODE_MASK_4,
	PX_QRCODE_MASK_5,
	PX_QRCODE_MASK_6,
	PX_QRCODE_MASK_7,
}PX_QRCODE_MASK;

typedef enum
{
	PX_QRCODE_MODE_NUMERIC      = 0x1,
	PX_QRCODE_MODE_ALPHANUMERIC = 0x2,
	PX_QRCODE_MODE_BYTE         = 0x4,
	PX_QRCODE_MODE_KANJI        = 0x8,
	PX_QRCODE_MODE_ECI          = 0x7,
}PX_QRCODE_MODE;



#define PX_QRCODE_VERSION_MIN   1
#define PX_QRCODE_VERSION_MAX  40

#define PX_QRCODE_BUFFER_LEN_FOR_VERSION(n)  ((((n) * 4 + 17) * ((n) * 4 + 17) + 7) / 8 + 1)

#define PX_QRCODE_BUFFER_LEN_MAX  PX_QRCODE_BUFFER_LEN_FOR_VERSION(PX_QRCODE_VERSION_MAX)


px_bool PX_QRCode_EncodeText(const px_char *text, px_byte tempBuffer[], px_byte qrcode[],PX_QRCODE_ECC ecl, px_int minVersion, px_int maxVersion, PX_QRCODE_MASK mask, px_bool boostEcl);

px_bool PX_QRCode_EncodeBinary(px_byte dataAndTemp[], px_int dataLen, px_byte qrcode[],	PX_QRCODE_ECC ecl, px_int minVersion, px_int maxVersion, PX_QRCODE_MASK mask, px_bool boostEcl);

px_void PX_QRCode_RenderText(px_surface *psurface, px_int lefttop_x, px_int lefttop_y, const px_char *text, PX_QRCODE_ECC ecl, px_int render_rect_size);

px_void PX_QRCode_RenderBinary(px_surface *psurface, px_int lefttop_x, px_int lefttop_y, const px_byte *data, px_int dataLen, PX_QRCODE_ECC ecl, px_int render_rect_size);


px_bool PX_QRCode_IsNumeric(const px_char *text);

px_bool PX_QRCode_IsAlphanumeric(const px_char *text);

px_int PX_QRCode_GetSize(const px_byte qrcode[]);

px_bool PX_QRCode_GetModule(const px_byte qrcode[], px_int x, px_int y);


#endif
