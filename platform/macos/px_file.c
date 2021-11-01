
#include "../modules/px_file.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <errno.h>
#include "dirent.h"

int PX_SaveDataToFile(void *buffer,int size,const char path[])
{
    FILE *pf=fopen(path,"rb");
    if (pf)
    {
        fwrite(buffer,1,size,pf);
        fclose(pf);
        return 1;
    }
    return 0;
}


PX_IO_Data PX_LoadFileToIOData(const char path[])
{
    
    PX_IO_Data io;
    int fileoft=0;
    FILE *pf=fopen(path,"rb");
    int filesize;
    if (!pf)
    {
        goto _ERROR;
    }
    fseek(pf,0,SEEK_END);
    filesize=ftell(pf);
    fseek(pf,0,SEEK_SET);

    io.buffer=(unsigned char *)malloc(filesize+1);
    if (!io.buffer)
    {
        goto _ERROR;
    }

    while (!feof(pf))
    {
        fileoft+=(int)fread(io.buffer+fileoft,1,1024,pf);
    }
    fclose(pf);

    io.buffer[filesize]='\0';
    io.size=filesize;
    return io;
_ERROR:
    io.buffer = 0;
    io.size = 0;
    return io;
}

void PX_FreeIOData(PX_IO_Data *io)
{
    if (io->size&&io->buffer)
    {
        free(io->buffer);
        io->size=0;
        io->buffer=0;
    }
}

int PX_FileExist(const char path[])
{
    FILE *pf= fopen(path,"rb");
    if (pf)
    {
        fclose(pf);
        return 1;
    }
    return 0;
}




int PX_GetDirectoryFileCount(const char path[],PX_FILEENUM_TYPE type,const char *filter)
{
    struct dirent* ent = NULL;
    int count=0;
    DIR *pDir;

    if( (pDir=opendir(path)) == NULL)
    {
        return 0;
    }

    while( (ent=readdir(pDir)) != NULL )
    {
        switch (type)
        {
            case PX_FILEENUM_TYPE_ANY:
            {
                if (filter&&filter[0])
                {
                    if (strstr(ent->d_name,filter))
                    {
                        count++;
                    }
                } else
                {
                    count++;
                }
            }
                break;
            case PX_FILEENUM_TYPE_FILE:
            {
                if (ent->d_type==DT_REG)
                {
                    if (filter&&filter[0])
                    {
                        const char *pFilter=filter;
                        while (pFilter[0])
                        {
                            if (strstr(ent->d_name,pFilter))
                            {
                                count++;
                                break;
                            }
                            pFilter+=strlen(pFilter)+1;
                        }
                    } else
                    {
                        count++;
                    }
                }
            }
                break;
            case PX_FILEENUM_TYPE_FOLDER:
            {
                if (ent->d_type==DT_DIR)
                {
                    if (filter&&filter[0])
                    {
                        const char *pFilter=filter;
                        while (pFilter[0])
                        {
                            if (strstr(ent->d_name,pFilter))
                            {
                                count++;
                                break;
                            }
                            pFilter+=strlen(pFilter)+1;
                        }
                    } else
                    {
                        count++;
                    }
                }
            }
                break;
            case PX_FILEENUM_TYPE_DEVICE:
            {
                if (ent->d_type==DT_CHR)
                {
                    if (filter&&filter[0])
                    {
                        const char *pFilter=filter;
                        while (pFilter[0])
                        {
                            if (strstr(ent->d_name,pFilter))
                            {
                                count++;
                                break;
                            }
                            pFilter+=strlen(pFilter)+1;
                        }
                    } else
                    {
                        count++;
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    closedir(pDir);
    return  count;
}

int PX_GetDirectoryFileName(const char path[],int count,char *FileName[260],PX_FILEENUM_TYPE type,const char *filter)
{
    struct dirent* ent = NULL;
    int index=0;
    DIR *pDir;

    if( (pDir=opendir(path)) == NULL)
    {
        return 0;
    }

    while( (ent=readdir(pDir)) != NULL )
    {
        if (index>=count)
        {
            return index;
        }

        switch (type)
        {
            case PX_FILEENUM_TYPE_ANY:
            {
                if (filter&&filter[0])
                {
                    const char *pFilter=filter;
                    while (pFilter[0])
                    {
                        if (strstr(ent->d_name,pFilter))
                        {
                            strcpy(FileName[index],ent->d_name);
                            index++;
                            break;
                        }
                        pFilter+=strlen(pFilter)+1;
                    }
                } else
                {
                    strcpy(FileName[index],ent->d_name);
                    index++;
                }
            }
                break;
            case PX_FILEENUM_TYPE_FILE:
            {
                if (ent->d_type==DT_REG)
                {
                    if (filter&&filter[0])
                    {
                        const char *pFilter=filter;
                        while (pFilter[0])
                        {
                            if (strstr(ent->d_name,pFilter))
                            {
                                strcpy(FileName[index],ent->d_name);
                                index++;
                                break;
                            }
                            pFilter+=strlen(pFilter)+1;
                        }
                    } else
                    {
                        strcpy(FileName[index],ent->d_name);
                        index++;
                    }
                }
            }
                break;
            case PX_FILEENUM_TYPE_FOLDER:
            {
                if (ent->d_type==DT_DIR)
                {
                    if (filter&&filter[0])
                    {
                        const char *pFilter=filter;
                        while (pFilter[0])
                        {
                            if (strstr(ent->d_name,pFilter))
                            {
                                strcpy(FileName[index],ent->d_name);
                                index++;
                                break;
                            }
                            pFilter+=strlen(pFilter)+1;
                        }
                    } else
                    {
                        strcpy(FileName[index],ent->d_name);
                        index++;
                    }
                }
            }
                break;
            case PX_FILEENUM_TYPE_DEVICE:
            {
                if (ent->d_type==DT_CHR)
                {
                    if (filter&&filter[0])
                    {
                        const char *pFilter=filter;
                        while (pFilter[0])
                        {
                            if (strstr(ent->d_name,pFilter))
                            {
                                strcpy(FileName[index],ent->d_name);
                                index++;
                                break;
                            }
                            pFilter+=strlen(pFilter)+1;
                        }
                    } else
                    {
                        strcpy(FileName[index],ent->d_name);
                        index++;
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    closedir(pDir);
    return  index;
}
