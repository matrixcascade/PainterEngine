#include "PX_Huffman.h"

struct _huffman_node
{
	px_word left,right;
	px_uint weight;
	px_byte data;
};
typedef struct _huffman_node huffman_node;

typedef struct
{
	union
	{
		px_word left;
		px_word bNode;
	};
	union
	{
		px_word right;
		px_byte data;
	};
}huffman_snode;

typedef struct
{
	px_dword len;
	px_byte encode[32];
}huffman_map;

px_void px_huffmanTraversalTree(huffman_node *pNodeHead,huffman_node *node,huffman_map map,huffman_map *pTable)
{
	huffman_map genMap;
	if (node->left)
	{
		genMap=map;
		genMap.len++;
		px_huffmanTraversalTree(pNodeHead,&pNodeHead[node->left],genMap,pTable);
		genMap=map;
		genMap.encode[genMap.len/8]|=(1<<(genMap.len%8));
		genMap.len++;
		px_huffmanTraversalTree(pNodeHead,&pNodeHead[node->right],genMap,pTable);
	}
	else
	{
		pTable[node->data]=map;
	}
}

px_void PX_huffmanCompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size)
{
	px_uint i,j;
	huffman_node nodes[511];
	huffman_snode snodes[511];
	px_uint sortCursor=255;
	huffman_map huffmanMap[256]={0};
	px_uint encodeNecessaryNodeCount;
	px_uint sortIndex;
	px_uint w_size=0;
	huffman_map mapnode;
	px_byte w_byte=0;
	px_uchar w_byte_offset=0;

	PX_memset(nodes,0,sizeof(nodes));
	for (i=0;i<256;i++)
	{
		nodes[i].data=(px_byte)i;
	}
	for (i=0;i<input_size;i++)
	{
		nodes[_in[i]].weight++;
	}
	//sort first
	do 
	{
		 huffman_node s_nodes[256];
		 PX_QuickSortAtom s_Atom[256];
		 PX_memcpy(s_nodes,nodes,sizeof(s_nodes));
		 for(i=0;i<256;i++)
		 {
			 s_Atom[i].weight=(px_float)s_nodes[i].weight;
			 s_Atom[i].pData=&s_nodes[i];
		 }
		 PX_Quicksort_MaxToMin(s_Atom,0,255);
		 for(i=0;i<256;i++)
		 {
			 nodes[i]=*(huffman_node *)s_Atom[i].pData;
		 }
	} while (0);

	while (sortCursor&&nodes[sortCursor].weight==0)
	{
		sortCursor--;
	}
	encodeNecessaryNodeCount=(sortCursor+1)*2-1;

	if (_out)
	{
		_out[0]=sortCursor;
	}
	w_size+=1;
	if (_out)
	{
		PX_memcpy(_out+1,&input_size,4);
	}
	w_size+=4;

	while (sortCursor>0)
	{
		huffman_node newNode;
		//move node to end
		nodes[sortCursor*2]=nodes[sortCursor];
		nodes[sortCursor*2-1]=nodes[sortCursor-1];
		newNode.left=sortCursor*2;
		newNode.right=sortCursor*2-1;
		newNode.data=0;
		newNode.weight=nodes[sortCursor*2].weight+nodes[sortCursor*2-1].weight;
		nodes[sortCursor-1]=newNode;
		sortCursor--;
		//re-sort
		sortIndex=sortCursor;
		while (sortIndex>0)
		{
			if (nodes[sortIndex].weight>nodes[sortIndex-1].weight)
			{
				huffman_node temp=nodes[sortIndex-1];
				nodes[sortIndex-1]=nodes[sortIndex];
				nodes[sortIndex]=temp;
			}
			else
			{
				break;
			}
			sortIndex--;
		}
	}

	PX_memset(snodes,0,sizeof(snodes));
	for (i=0;i<encodeNecessaryNodeCount;i++)
	{
		snodes[i].left=nodes[i].left;
		if (nodes[i].right)
		{
			snodes[i].right=nodes[i].right;
		}
		else
		{
			snodes[i].data=nodes[i].data;
		}
	}
	
	if (_out)
	{
		PX_memcpy(_out+w_size,snodes,encodeNecessaryNodeCount*sizeof(huffman_snode));
	}
	w_size+=encodeNecessaryNodeCount*sizeof(huffman_snode);
	mapnode.len=0;
	PX_memset(&mapnode.encode,0,sizeof(mapnode.encode));
	px_huffmanTraversalTree(nodes,nodes,mapnode,huffmanMap);

	//encoding
	
	for (i=0;i<input_size;i++)
	{
		px_byte *pencode=PX_NULL;
		px_uchar pencode_len;
		px_uchar pencode_offset=0;
		
		pencode=huffmanMap[_in[i]].encode;
		pencode_len=huffmanMap[_in[i]].len;

		for (j=0;j<pencode_len;j++)
		{
			if (pencode[j/8]&(1<<j%8))
			{
				w_byte|=(1<<w_byte_offset);
			}
			w_byte_offset++;
			if (w_byte_offset>=8)
			{
				w_byte_offset=0;
				if(_out)
				_out[w_size]=w_byte;
				w_byte=0;
				w_size++;
			}
		}
	}
	//last
	if(_out)
	_out[w_size]=w_byte;
	w_size++;
	*out_size=w_size;
}

px_void PX_huffmanDecompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size)
{
	px_uint r_offset/*read*/,w_offset;
	px_uchar leaf;
	px_uint bit_offset;
	px_uint bit;
	px_dword datasize;
	huffman_snode *pheader,*pNode;
	*out_size=*(px_dword *)(_in+1);
	if (!_out)
	{
		return;
	}

	datasize=*(px_dword *)(_in+1);
	pheader=(huffman_snode *)(_in+1+4);
	if (!pheader->bNode)
	{
		//only one node
		if(_out)
		PX_memset(_out,pheader->data,datasize);
	}
	
	leaf=_in[0];
	bit_offset=0;
	w_offset=0;
	pNode=pheader;
	r_offset=1+4+((leaf+1)*2-1)*sizeof(huffman_snode);

	while (r_offset<input_size)
	{
		bit=_in[r_offset]&(1<<bit_offset);
		if(bit)
		{
			pNode=&pheader[pNode->right];
			if (!pNode->bNode)
			{
				_out[w_offset]=pNode->data;
				w_offset++;
				pNode=pheader;
				if (w_offset>=datasize)
				{
					return;
				}
			}
			
		}
		else
		{
			pNode=&pheader[pNode->left];
			if (!pNode->bNode)
			{
				_out[w_offset]=pNode->data;
				w_offset++;
				pNode=pheader;
				if (w_offset>=datasize)
				{
					return;
				}
			}
		}

		bit_offset++;
		if (bit_offset>=8)
		{
			r_offset++;
			bit_offset=0;
		}
	}
	//error
	PX_ASSERT();
}

