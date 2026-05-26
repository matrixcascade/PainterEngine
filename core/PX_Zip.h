#ifndef PX_ZIP_H
#define PX_ZIP_H
#include "PX_Abi.h"
#include "PX_RFC1951.h"
////////////////////////////////////////
//input/output abi -- file[](abi) --- filename(string)
//                               |
//                                -- bin(data)
//
//eg
// PX_AbiSet_string(&abi,"file[0].filename","test.txt");
// PX_AbiSet_bin(&abi,"file[0].bin",data,size);
// PX_AbiSet_string(&abi,"file[1].filename","test2.txt");
// PX_AbiSet_bin(&abi,"file[1].bin",data,size);
// ...
// PX_MemoryInitialize(mp,&output);
// PX_ZipCompress(mp,&abi,&output,level);
// ...
// px_abi output_abi;
// PX_AbiCreate_DynamicWriter(&output_abi,mp);
// PX_ZipDecompress(mp,output.buffer,output.usedsize,&output_abi);
px_bool PX_ZipCompress(px_memorypool *mp, px_abi *input, px_memory *output, px_int level);
px_bool PX_ZipDecompress(px_memorypool *mp, const px_byte *input, px_int input_size, px_abi *output);



#endif