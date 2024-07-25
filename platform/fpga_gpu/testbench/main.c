#include "kernel/PX_Kernel.h"

px_byte cache[1024*1024*32];
px_memorypool mp;
px_int rotZ=0;
px_texture tex1,tex2,tex3;
px_texture tex;

px_byte bunny_inflate_data[] = {
#include "bunny.h"
};

int main()
{
  PX_Object* pObject;
  px_memory objdata;
  PX_3D_ObjectData data;
  px_int i;
  mp=MP_Create(cache,sizeof(cache));
  //check GPU available
  px_dword gpu_version;
  gpu_version=PX_GPU_CheckEnable();

  //
  px_dword *ptr1=(px_dword *)MP_Malloc(&mp,sizeof(px_dword)*65535);
  px_dword *ptr2=(px_dword *)MP_Malloc(&mp,sizeof(px_dword)*65535);
  for (i=0; i<65535; i++) {
    ptr1[i]=i+1;
  }
  PX_GPU_mempcy(ptr1, ptr2, 65535);
  for (i=0; i<65535; i++) 
  {
    volatile px_dword readvalue=ptr2[i];
    if(readvalue!=i+1)
    {
        //error
        return 0;
    }  
  }

//texture blender
  PX_TextureCreate(&mp,&tex1,800,600);
  PX_SurfaceClearAll(&tex1, PX_COLOR(128,64,88,99));
  PX_TextureCreate(&mp,&tex2,800,600);
  PX_SurfaceClearAll(&tex2, PX_COLOR(128,64,88,99));
  PX_TextureCreate(&mp,&tex3,800,600);
  PX_SurfaceClearAll(&tex3, PX_COLOR(88,168,99,129));
  PX_TextureRender(&tex1, &tex3, 800, 600, PX_ALIGN_LEFTTOP, 0);
  PX_GPU_Render(tex3.surfaceBuffer, 800, 800, 600, tex2.surfaceBuffer, 800, DVI_RGB_MODE_RGBA, 0x80808080);
  px_dword tex1_crc=PX_crc32(tex1.surfaceBuffer, tex1.width*tex1.height*4);
  px_dword tex2_crc=PX_crc32(tex2.surfaceBuffer, tex2.width*tex1.height*4);
  if (tex1_crc!=tex2_crc) 
  {
    //error
    return 0;
  }

  //display testbench
  PX_TextureCreate(&mp,&tex,800,600);
  PX_SurfaceClearAll(&tex,PX_COLOR_WHITE);

  PX_MemoryInitialize(&mp, &objdata);
  PX_RFC1951Inflate(bunny_inflate_data, sizeof(bunny_inflate_data), &objdata);
  PX_3D_ObjectDataInitialize(&mp, &data);
  if (!PX_3D_ObjectDataLoad(&data, objdata.buffer, objdata.usedsize))return PX_FALSE;

  pObject = PX_Object_3DModelCreate(&mp, 0, 400, 300, 600, 600, &data);
  PX_Object_3DModelSetWorld(pObject, 0, 0, 1.f, 0, 200, 0, 1);
  PX_ObjectRender(&tex, pObject, 0);
  while(1)
  {
      
      PX_GPU_Present((px_void *)tex.surfaceBuffer, 800, 600, VIDEO_DISPLAY_MODE_800_600, DVI_RGB_MODE_ARGB);
  }
}