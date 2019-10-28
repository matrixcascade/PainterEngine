#include "PX_Arle.h"

typedef enum
{
	PX_ARLE_TYPE_UNKNOW,
	PX_ARLE_TYPE_UNEQUAL,
	PX_ARLE_TYPE_EQUAL,
}PX_ARLE_TYPE;


px_int PX_ArleCompressData(px_byte *_in,px_uchar size,PX_ARLE_TYPE type,px_byte *_out)
{
	px_uchar ecode;
	if (size==0)
	{
		return 0;
	}
	switch (type)
	{
	case PX_ARLE_TYPE_UNKNOW:
		{
			if (_out)
			{
				ecode=1;//0000 0001
				_out[0]=ecode;
				_out[1]=_in[0];
			}
			return 2;
		}
		break;
	case PX_ARLE_TYPE_UNEQUAL:
		{
			if (_out)
			{
				ecode=0;
				ecode|=size;
				_out[0]=ecode;
				PX_memcpy(_out+1,_in,size);
			}
			return size+1;

		}
		break;
	case PX_ARLE_TYPE_EQUAL:
		{
			if (_out)
			{
				ecode&=0x80;
				ecode|=size;
				_out[0]=ecode;
				_out[1]=_in[0];
			}
			return 2;
		}
		break;
	}
	return 0;
}

px_void PX_ArleCompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *_outsize)
{
	PX_ARLE_TYPE type=PX_ARLE_TYPE_UNKNOW;
	px_uint wsize;
	px_byte rlv=_in[0];//last value of read
	px_uint p_cursor=0,r_cursor=1,w_cursor=0;
	*_outsize=0;

	while (r_cursor<input_size)
	{
		if (r_cursor-p_cursor>=127)
		{
			wsize=PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type,_out?_out+w_cursor:PX_NULL);
			*_outsize+=wsize;
			w_cursor+=wsize;
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
					wsize=PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type,_out?_out+w_cursor:PX_NULL);
					*_outsize+=wsize;
					w_cursor+=wsize;
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
					wsize=PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type,_out?_out+w_cursor:PX_NULL);
					*_outsize+=wsize;
					w_cursor+=wsize;
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
	*_outsize+=PX_ArleCompressData(_in+p_cursor,r_cursor-p_cursor,type,_out?_out+w_cursor:PX_NULL);
	return;
}

px_void PX_ArleDecompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *_outsize)
{
	px_uint r_cursor=0,w_cursor=0;
	px_uchar rep,size;
	*_outsize=0;
	while (r_cursor<input_size)
	{
		rep=_in[r_cursor]&0x80;
		size=_in[r_cursor]&0x7f;
		if (rep)
		{
			if (_out)
			{
				PX_memset(_out+w_cursor,_in[r_cursor+1],size);
			}
			w_cursor+=size;
			*_outsize+=size;
			r_cursor+=2;
		}
		else
		{
			if (_out)
			{
				PX_memcpy(_out+w_cursor,&_in[r_cursor+1],size);
			}
			w_cursor+=size;
			*_outsize+=size;
			r_cursor+=size+1;
		}
	}
}

