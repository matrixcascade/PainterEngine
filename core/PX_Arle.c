#include "PX_Arle.h"

typedef enum
{
	PX_ARLE_TYPE_UNKNOW,
	PX_ARLE_TYPE_UNEQUAL,
	PX_ARLE_TYPE_EQUAL,
}PX_ARLE_TYPE;


px_int PX_ArleCompressData(px_byte *_in,px_uchar size,PX_ARLE_TYPE type, px_memory* _out)
{
	px_uchar ecode=0;
	if (size==0)
	{
		return 1;
	}
	switch (type)
	{
	case PX_ARLE_TYPE_UNKNOW:
		{
			ecode=1;//0000 0001
			//_out[0]=ecode;
			//_out[1]=_in[0];
			if(!PX_MemoryCatByte(_out,ecode))return 0;
			if (!PX_MemoryCatByte(_out,_in[0]))return 0;
			return 2;
		}
		break;
	case PX_ARLE_TYPE_UNEQUAL:
		{
			
			ecode=0;
			ecode|=size;
			if (!PX_MemoryCatByte(_out,ecode))return 0;
			if (!PX_MemoryCat(_out,_in,size))return 0;
			return size+1;
		}
		break;
	case PX_ARLE_TYPE_EQUAL:
		{
			ecode=0x80;
			ecode|=size;
			//_out[0]=ecode;
			//_out[1]=_in[0];
			if (!PX_MemoryCatByte(_out,ecode))return 0;
			if (!PX_MemoryCatByte(_out,_in[0]))return 0;
			return 2;
		}
		break;
	}
	return 0;
}

px_bool PX_ArleCompress(px_byte *_in,px_uint input_size, px_memory* out)
{
	PX_ARLE_TYPE type=PX_ARLE_TYPE_UNKNOW;
	px_uint wsize;
	px_byte rlv=_in[0];//last value of read
	px_uint p_cursor=0,r_cursor=1;

	while (r_cursor<input_size)
	{
		if (r_cursor-p_cursor>=127)
		{
			wsize=PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type, out);
			p_cursor=r_cursor;
			type=PX_ARLE_TYPE_UNKNOW;
			rlv=_in[r_cursor];
			r_cursor++;
			continue;
		}

		switch (type)
		{
		case PX_ARLE_TYPE_UNKNOW:
			{
				if (rlv==_in[r_cursor])
				{
					type=PX_ARLE_TYPE_EQUAL;
				}
				else
				{
					type=PX_ARLE_TYPE_UNEQUAL;
				}
			}
			break;
		case PX_ARLE_TYPE_UNEQUAL:
			{
				if (rlv==_in[r_cursor])
				{
					//rollback
					r_cursor--;
					if(!PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type,out))
					{
						return PX_FALSE;
					}
					p_cursor=r_cursor;
					type=PX_ARLE_TYPE_EQUAL;
				}
			}
			break;
		case PX_ARLE_TYPE_EQUAL:
			{
				if (rlv!=_in[r_cursor])
				{
					//rollback
					if(!PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type,out))
					{
						return PX_FALSE;
					}
					p_cursor=r_cursor;
					type=PX_ARLE_TYPE_UNEQUAL;
				}
			}
			break;
		}
		rlv=_in[r_cursor];
		r_cursor++;
	}
	//encode last token
	if (!PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type,out))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_ArleDecompress(px_byte *_in,px_uint input_size, px_memory* out)
{
	px_uint r_cursor=0,w_cursor=0;
	px_uchar rep,size;
	while (r_cursor<input_size)
	{
		rep=_in[r_cursor]&0x80;
		size=_in[r_cursor]&0x7f;
		if (rep)
		{
			px_int i;
			for ( i = 0; i < size; i++)
			{
				if (!PX_MemoryCatByte(out,_in[r_cursor+1]))return PX_FALSE;

			}
			r_cursor+=2;
		}
		else
		{
			if (!PX_MemoryCat(out,_in+r_cursor+1,size))return PX_FALSE;
			r_cursor+=size+1;
		}
	}
	return PX_TRUE;
}

