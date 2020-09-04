#include "PX_Wave.h"

px_uint PX_WaveGetPCMSize(px_byte *buffer,px_int size)
{
	px_int offset=0,pcmSize;
	PX_WAVE_DATA_BLOCK *pBlock;
	PX_WAVE_RIFF_HEADER *pHeader=(PX_WAVE_RIFF_HEADER *)buffer;
	PX_WAVE_FMT_BLOCK  *pfmt_block;
	if (!PX_WaveVerify(buffer,size))
	{
		return 0;
	}
	offset+=sizeof(PX_WAVE_RIFF_HEADER);
	pfmt_block=(PX_WAVE_FMT_BLOCK  *)(buffer+sizeof(PX_WAVE_RIFF_HEADER));
	offset+=8;
	offset+=pfmt_block->dwFmtSize;
	pcmSize=0;
	while (offset+(px_int)sizeof(PX_WAVE_DATA_BLOCK)<size)
	{
		pBlock=(PX_WAVE_DATA_BLOCK*)(buffer+offset);
		if(!PX_memequ(pBlock->szDataID,"data",4))
		{
			offset+=pBlock->dwDataSize+sizeof(PX_WAVE_DATA_BLOCK);
			continue;
		}
		offset+=sizeof(PX_WAVE_DATA_BLOCK);
		offset+=pBlock->dwDataSize;
		pcmSize+=pBlock->dwDataSize;
	}
	return pcmSize;
}

px_byte* PX_WaveGetPCMDataPtr(px_int index,px_byte *data,px_int datasize)
{
	if (PX_WaveVerify(data,datasize))
	{
		px_uint offset=0,pcmSize;
		pcmSize=PX_WaveGetPCMSize(data,datasize);

		if (pcmSize!=0)
		{
			PX_WAVE_DATA_BLOCK *pBlock;
			PX_WAVE_RIFF_HEADER *pHeader=(PX_WAVE_RIFF_HEADER *)data;
			PX_WAVE_FMT_BLOCK  *pfmt_block;

			pfmt_block=(PX_WAVE_FMT_BLOCK  *)(data+sizeof(PX_WAVE_RIFF_HEADER));
			offset+=sizeof(PX_WAVE_RIFF_HEADER);
			offset+=8;
			offset+=pfmt_block->dwFmtSize;
			pcmSize=0;

			while (offset<(px_uint)datasize)
			{
				pBlock=(PX_WAVE_DATA_BLOCK*)(data+offset);
				if(!PX_memequ(pBlock->szDataID,"data",4))
				{
					offset+=pBlock->dwDataSize+sizeof(PX_WAVE_DATA_BLOCK);
					continue;
				}
				offset+=sizeof(PX_WAVE_DATA_BLOCK);
				if (index==0)
				{
					return data+offset;
				}
				else
				{
					offset+=pBlock->dwDataSize;
				}
			}
			return PX_NULL;
		}
		else
			return PX_NULL;
	}
	else
	{
		return PX_NULL;
	}
}

px_bool PX_WaveVerify(px_byte *buffer,px_int size)
{
	PX_WAVE_RIFF_HEADER *pHeader=(PX_WAVE_RIFF_HEADER *)buffer;
	PX_WAVE_FMT_BLOCK  *pfmt_block;
	if (size<sizeof(PX_WAVE_RIFF_HEADER)+sizeof(PX_WAVE_FMT_BLOCK)+sizeof(PX_WAVE_DATA_BLOCK))
	{
		return PX_FALSE;
	}
	if (!PX_memequ(pHeader->szRiffId,"RIFF",4))
	{
		return PX_FALSE;
	}
	if (!PX_memequ(pHeader->szRiffFormat,"WAVE",4))
	{
		return PX_FALSE;
	}
	pfmt_block=(PX_WAVE_FMT_BLOCK  *)(buffer+sizeof(PX_WAVE_RIFF_HEADER));
	if (!PX_memequ(pfmt_block->szFmtID,"fmt",3))
	{
		return PX_FALSE;
	}
	if (pfmt_block->wavFormat.wChannels!=1&&pfmt_block->wavFormat.wChannels!=2)
	{
		return PX_FALSE;
	}
	if (pfmt_block->wavFormat.wBitsPerSample!=16)
	{
		return PX_FALSE;
	}
	if (pfmt_block->wavFormat.dwSamplesPerSec!=44100)
	{
		return PX_FALSE;
	}
	if (pfmt_block->wavFormat.wFormatTag!=0x0001)
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_uint PX_WaveGetChannel(px_byte *buffer,px_int size)
{
	PX_WAVE_FMT_BLOCK  *pfmt_block;
	if (!PX_WaveVerify(buffer,size))
	{
		return 0;
	}
	pfmt_block=(PX_WAVE_FMT_BLOCK  *)(buffer+sizeof(PX_WAVE_RIFF_HEADER));
	return pfmt_block->wavFormat.wChannels;
}
