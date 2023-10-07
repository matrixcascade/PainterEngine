#include "PX_Huffman.h"

#define PX_HUFFMAN_MAX_BITS 15

static px_bool PX_HuffmanIsConflict(px_dword code_table[],px_dword code_bit_length[], px_int code_table_length)
{
	px_int i,j;
	for (i = 0; i < code_table_length; i++)
	{
		px_dword code = code_table[i];
		px_dword bit_length = code_bit_length[i];

		if (bit_length)
		{
			for (j = 0; j < code_table_length; j++)
			{
				if (i != j && bit_length <= code_bit_length[j])
				{
					px_dword code2 = code_table[j];
					px_dword bit_length2 = code_bit_length[j];
					code2 >>= (bit_length2 - bit_length);
					if (code == code2)
					{
						return PX_TRUE;
					}
				}
			}
		}
	}
	return PX_FALSE;
}

static px_dword PX_HuffmanSwapCode(px_dword code, px_dword len)
{
	px_dword swapCode = 0;
	while (len)
	{
		swapCode <<= 1;
		swapCode |= (code & 1);
		code >>= 1;
		len--;
	}
	return swapCode;
}

static px_dword PX_HuffmanGetCode(px_dword symbol, px_dword code_table[288],px_dword code_bit_length[288])
{
	return PX_HuffmanSwapCode(code_table[symbol], code_bit_length[symbol]);
}

static px_dword PX_HuffmanGetSymbol(px_uint32* bitpointer, const px_byte* bitstream, px_huffman_node tree[], px_uint tree_size)
{
	px_dword value = 0;
	px_dword cursor = 0;
	while (PX_TRUE)
	{
		px_bool bit;
		if (tree[cursor].left==0xffff&& tree[cursor].right==0xffff)
		{
			return tree[cursor].data;
		}
		bit = PX_ReadBitLE(bitpointer, bitstream);
		if (bit)
		{
			cursor = tree[cursor].right;
		}
		else
		{
			cursor = tree[cursor].left;
		}
		if (cursor>= tree_size)
		{
			return 0xffff;
		}
	}
}

static px_bool PX_HuffmanBuildTree(px_dword code_table[], px_dword code_bit_length[], px_huffman_node tree[],px_uint code_count)
{
	px_uint i;
	px_uint write_cursor=1;
	for (i = 0; i < code_count*2; i++)
	{
		tree[i].left = 0xffff;
		tree[i].right = 0xffff;
		tree[i].data = 0;
		tree[i].weight = 0;
	}
	for (i=0;i< code_count;i++)
	{
		if (code_bit_length[i])
		{
			px_dword symbol = i;
			px_dword code = code_table[i];
			px_dword length = code_bit_length[i];
			px_uint cursor=0;

			while (length)
			{
				px_byte bit = (code>>(length-1)) & 1;
				if (length)
				{
					if (bit)
					{
						if (tree[cursor].right == 0xffff)
						{
							tree[cursor].right = write_cursor;
							write_cursor++;
						}
						tree[cursor].weight += symbol;
						cursor = tree[cursor].right;
						if (cursor> code_count*2)
						{
							return PX_FALSE;
						}
					}
					else
					{
						if (tree[cursor].left == 0xffff)
						{
							tree[cursor].left = write_cursor;
							write_cursor++;
						}
						tree[cursor].weight += symbol;
						cursor = tree[cursor].left;
						if (cursor > code_count*2)
						{
							return PX_FALSE;
						}
					}
				}
				length--;
			}
			if (tree[cursor].left != 0xffff || tree[cursor].right != 0xffff)
			{
				return PX_FALSE;
			}
			tree[cursor].data = symbol;
			tree[cursor].weight = symbol;


		}
	}
	return PX_TRUE;
}

static px_void PX_HuffmanBuildCodeBitLengthTable(px_huffman_node *pNodeHead,px_huffman_node *node,px_dword length, px_dword* pTable)
{
	if (node->left)//is a node?
	{
		PX_HuffmanBuildCodeBitLengthTable(pNodeHead,&pNodeHead[node->left], length+1,pTable);
		PX_HuffmanBuildCodeBitLengthTable(pNodeHead,&pNodeHead[node->right], length+1,pTable);
	}
	else
	{
		if(length!=0)
			pTable[node->data]=length;
		else
			pTable[node->data] = 1;
	}
}

px_void PX_HuffmanBuildFixedSymbolTable(px_dword code_table[288], px_dword code_bit_length[288])
{
	px_dword i,t;
	t = 0;
	for (i=48;i<=191;i++)
	{
		code_bit_length[t] = 8;
		code_table[t] = i;
		t++;
	}


	for (i = 400; i <= 511; i++)
	{
		code_bit_length[t] = 9;
		code_table[t] = i;
		t++;
	}


	for (i=0;i<=23;i++)
	{
		code_bit_length[t] = 7;
		code_table[t] = i;
		t++;
	}


	for (i = 192; i <= 199; i++)
	{
		code_bit_length[t] = 8;
		code_table[t] = i;
		t++;
	}

}

px_void PX_HuffmanBuildFixedDistanceTable(px_dword code_table[30], px_dword code_bit_length[30])
{
	px_int i;

	for (i=0;i<30;i++)
	{
		code_table[i] = i;
		code_bit_length[i] = 5;
	}
}

static px_void PX_HuffmanBuildCodeTableFromCodeBitLengthTable(px_dword code_table[], px_dword code_bit_length[],px_uint codes_count)
{
	px_uint i;
	px_int bit_length_count[20] = { 0 };//1-18
	px_dword next_code[20] = {0};//1-18
	px_uint max_bit_length=1;

	for (i = 0; i < codes_count; i++)
	{
		if (code_bit_length[i] > max_bit_length)
			max_bit_length = code_bit_length[i];
	}

	//rfc1951
	//calculate bit_length_count(bl_count) 
	do
	{
		for (i = 0; i < codes_count; i++)
		{
			bit_length_count[code_bit_length[i]]++;
		}
		bit_length_count[0] = 0;
	} while (0);

	//step 2
	do
	{
		px_dword code = 0;
		px_uint bits;
		for (bits = 1; bits <= max_bit_length; bits++) {
			code = (code + bit_length_count[bits - 1]) << 1;
			next_code[bits] = code;
		}
	} while (0);

	//step 3
	for (i = 0; i < codes_count; i++)
	{
		px_int len = code_bit_length[i];
		if (len != 0)
		{
			code_table[i] = next_code[len];
			next_code[len]++;
		}
	}
}

static px_void PX_HuffmanBuildDymanicTable(px_dword *symbol_counter,px_uint symbol_count, px_dword code_table[],px_dword code_bit_length[])
{
	px_uint i;
	px_huffman_node nodes[576] = {0};
	

	px_uint sortCursor;
	px_uint max_code;
	

	PX_memset(nodes,0,sizeof(nodes));

	for (i=0;i<PX_COUNTOF(nodes); i++)
	{
		if(i< symbol_count)
			nodes[i].data = i;
		else
			nodes[i].data = 0xffff;
	}

	//set node weight
	for (i=0;i< symbol_count;i++)
	{
		nodes[i].weight= symbol_counter[i];
	}

	//sort first
	do 
	{
		px_huffman_node s_nodes[288] = {0};
		PX_QuickSortAtom s_Atom[288] = {0};
		 PX_memcpy(s_nodes,nodes,sizeof(s_nodes));
		 for(i=0;i< symbol_count;i++)
		 {
			 s_Atom[i].weight=(px_float)s_nodes[i].weight;
			 s_Atom[i].pData=&s_nodes[i];
		 }
		 PX_Quicksort_MaxToMin(s_Atom,0, symbol_count-1);
		 for(i=0;i< symbol_count;i++)
		 {
			 nodes[i]=*(px_huffman_node *)s_Atom[i].pData;
		 }
	} while (0);

	//nodes count
	sortCursor = symbol_count-1;
	while (sortCursor&&nodes[sortCursor].weight==0)
	{
		sortCursor--;
	}
	max_code=(sortCursor+1)*2-1;


	while (sortCursor>0)
	{
		px_huffman_node newNode;
		px_uint sortIndex;

		//move node to end
		nodes[sortCursor*2]=nodes[sortCursor];
		nodes[sortCursor*2-1]=nodes[sortCursor-1];

		newNode.left=sortCursor*2;
		newNode.right=sortCursor*2-1;
		newNode.data=0xffff;//just a dummy value
		newNode.weight=nodes[sortCursor*2].weight+nodes[sortCursor*2-1].weight;
		nodes[sortCursor-1]=newNode;
		sortCursor--;

		//re-sort
		sortIndex=sortCursor;
		while (sortIndex>0)
		{
			if (nodes[sortIndex].weight>nodes[sortIndex-1].weight)
			{
				px_huffman_node temp=nodes[sortIndex-1];
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
	PX_memset(code_bit_length, 0, sizeof(px_dword) * symbol_count);
	PX_memset(code_table, 0, sizeof(px_dword) * symbol_count);

	PX_HuffmanBuildCodeBitLengthTable(nodes,nodes,0,code_bit_length);
	PX_HuffmanBuildCodeTableFromCodeBitLengthTable(code_table, code_bit_length, symbol_count);

	if (PX_HuffmanIsConflict(code_table, code_bit_length, symbol_count))
	{
		PX_ASSERT();
	}

}

static px_bool PX_HuffmanInflateDynamicTree(const px_byte* in, px_uint* bp, px_uint in_size, px_huffman_node symbol_tree[576],px_huffman_node distance_index_tree[60])
{
	px_huffman_node raw_code_bl_tree[38] = { 0 };
	px_uint n, hlit, hdist, hclen, i;
	px_dword symbol_bitlen[288] = {0};
	px_dword distance_index_bitlen[30] = {0};
	

	hlit = PX_ReadBitsLE(bp, in, 5) + 257;	
	hdist = PX_ReadBitsLE(bp, in, 5) + 1;	
	hclen = PX_ReadBitsLE(bp, in, 4) + 4;	

	do {
		px_byte rfc1951_map[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
		
		px_dword raw_code_bl_code_bl[19] = {0};
		px_dword raw_code_bl_code[19] = {0};
		
		if ((*bp) / 8 >= in_size - 2)
			return PX_FALSE;

		for (i = 0; i < 19; i++) 
		{
			if (i < hclen) 
				raw_code_bl_code_bl[rfc1951_map[i]] = PX_ReadBitsLE(bp, in, 3);
			else 
				raw_code_bl_code_bl[rfc1951_map[i]] = 0;
		}
		
		PX_HuffmanBuildCodeTableFromCodeBitLengthTable(raw_code_bl_code, raw_code_bl_code_bl, 19);

		if (!PX_HuffmanBuildTree(raw_code_bl_code, raw_code_bl_code_bl, raw_code_bl_tree, 19))
			return PX_FALSE;
	} while (0);


	i = 0;
	while (i < hlit + hdist) 
	{	
		px_dword code = PX_HuffmanGetSymbol( bp, in, raw_code_bl_tree,38);
		if (code <= 15) 
		{	
			if (i < hlit) 
			{
				symbol_bitlen[i] = code;
			}
			else 
			{
				distance_index_bitlen[i - hlit] = code;
			}
			i++;
		}
		else if (code == 16) 
		{	
			px_uint replength = 3;	
			px_uint value;

			if ((*bp)/8 >= in_size) return PX_FALSE;
			
			replength += PX_ReadBitsLE(bp, in, 2);

			if ((i - 1) < hlit) 
				value = symbol_bitlen[i - 1];
			else 
				value = distance_index_bitlen[i - hlit - 1];
			

			
			for (n = 0; n < replength; n++) 
			{
				if (i >= hlit + hdist) 
					return PX_FALSE;
				
				if (i < hlit) 
					symbol_bitlen[i] = value;
				else 
					distance_index_bitlen[i - hlit] = value;

				i++;
			}
		}
		else if (code == 17)
		{	
			px_uint replength = 3;

			if ((*bp) /8  >= in_size) break;
		
			replength += PX_ReadBitsLE(bp, in, 3);


			for (n = 0; n < replength; n++) 
			{

				if (i >= hlit + hdist) 	break;

				if (i < hlit) 
					symbol_bitlen[i] = 0;
				else 
					distance_index_bitlen[i - hlit] = 0;

				i++;
			}
		}
		else if (code == 18) 
		{
			px_uint replength = 11;

			if ((*bp) / 8 >= in_size) break;
			replength += PX_ReadBitsLE(bp, in, 7);


			for (n = 0; n < replength; n++) 
			{
				if (i >= hlit + hdist)	break;
	
				if (i < hlit)
					symbol_bitlen[i] = 0;
				else
					distance_index_bitlen[i - hlit] = 0;
				i++;
			}
		}
		else 
		{
			return PX_FALSE;
		}
	}

	do
	{
		px_dword code_table[288] = {0};
		PX_HuffmanBuildCodeTableFromCodeBitLengthTable(code_table, symbol_bitlen, 288);
		if (!PX_HuffmanBuildTree(code_table, symbol_bitlen, symbol_tree, 288))
			return PX_FALSE;
	} while (0);

	
	do
	{
		px_dword code_table[30] = { 0 };
		PX_HuffmanBuildCodeTableFromCodeBitLengthTable(code_table, distance_index_bitlen, 30);
		if (!PX_HuffmanBuildTree(code_table, distance_index_bitlen, distance_index_tree, 30))
			return PX_FALSE;
	} while (0);
	
	return PX_TRUE;
}



px_bool PX_HuffmanInflateCodeData(const px_byte _in[], px_uint *pbit_position, px_uint in_size, px_memory* out, PX_HUFFMAN_TREE_TYPE type/*1 or 2*/)
{
	px_huffman_node symbol_tree[288*2], distance_tree[30*2];
	if(type ==PX_HUFFMAN_TREE_TYPE_FIXED)
	{
		px_dword code_table[288] = { 0 }, code_bit_length[288] = {0};
		PX_HuffmanBuildFixedSymbolTable(code_table, code_bit_length);
		if (!PX_HuffmanBuildTree(code_table, code_bit_length, symbol_tree, 288))
			return PX_FALSE;

		PX_HuffmanBuildFixedDistanceTable(code_table, code_bit_length);
		if (!PX_HuffmanBuildTree(code_table, code_bit_length, distance_tree, 30))
			return PX_FALSE;
	}
	else if (type == PX_HUFFMAN_TREE_TYPE_DYNAMIC)
	{
		if (!PX_HuffmanInflateDynamicTree(_in, pbit_position, in_size, symbol_tree, distance_tree))
			return PX_FALSE;
	}
	else
	{
		return PX_FALSE;
	}

	while (PX_TRUE)
	{
		px_dword code = PX_HuffmanGetSymbol(pbit_position,_in, symbol_tree,576);

		if (code== 0xffff)
		{
			return PX_FALSE;
		}

		if (code == 256) 
		{
			return PX_TRUE;
		}
		else if (code <= 255) 
		{
			if (!PX_MemoryCat(out, &code, 2))
				return PX_FALSE;
		}
		else if (code >= 257 && code <= 285) 
		{
			px_dword length_table[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,67, 83, 99, 115, 131, 163, 195, 227, 258};
			px_dword length_extra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,5, 5, 5, 0};
			px_dword distance_table[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
			px_dword distance_extra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,11, 11, 12, 12, 13, 13};
			px_dword length = length_table[code - 257];
			px_dword distance_code, distance, extra_bits;

			extra_bits = length_extra[code - 257];

			if ((*pbit_position/8) >= in_size) 
				return PX_FALSE;
			
			length += PX_ReadBitsLE(pbit_position, _in, extra_bits);

			distance_code = PX_HuffmanGetSymbol(pbit_position, _in, distance_tree,60);

			if (distance_code > 29) 
				return PX_FALSE;
			

			distance = distance_table[distance_code];

			extra_bits = distance_extra[distance_code];

			if (((*pbit_position)/8) >= in_size) 
				return PX_FALSE;

			distance += PX_ReadBitsLE(pbit_position, _in, extra_bits);

			if(!PX_MemoryCat(out, &code, 2))
				return PX_FALSE;
			if (!PX_MemoryCat(out, &length, 2))
				return PX_FALSE;
			if (!PX_MemoryCat(out, &distance, 2))
				return PX_FALSE;

			//printf("length:%d   distance:%d\n", length, distance);
		}
	}
}

px_bool PX_HuffmanDeflateCodeData(px_word* _in, px_uint in_size, px_memory* _out, PX_HUFFMAN_TREE_TYPE type)
{
	px_dword raw_code[288] = { 0 }, raw_code_bl[288] = {0};
	px_dword distance_code[30] = { 0 }, distance_code_bl[30] = {0};
	px_dword raw_code_bl_code[19] = { 0 }, raw_code_bl_code_bl[19] = { 0 };
	px_byte rfc1951_map[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	//build huffman symbol tree and length-distance tree
	if (type==PX_HUFFMAN_TREE_TYPE_FIXED)
	{
		PX_HuffmanBuildFixedSymbolTable(raw_code, raw_code_bl);
		PX_HuffmanBuildFixedDistanceTable(distance_code, distance_code_bl);
	}
	else
	{
		
		px_uint  hlit, hdist, hclen;
		do 
		{
			px_uint i;
			px_dword counter[288] = { 0 };
			for (i=0;i<in_size;i++)
			{
				counter[_in[i]]++;
				if (_in[i]>=257)
				{
					i += 2;
				}
			}
			if (counter[287]!=0|| counter[286] != 0)
			{
				PX_ASSERT();
			}
			PX_HuffmanBuildDymanicTable(counter, 288, raw_code, raw_code_bl);//code_bit_length [1-19] 5bit

			for (i = 0; i < 288; i++)
			{
				if (raw_code_bl[i] >= 16)
				{
					return PX_FALSE;
				}
			}

		} while (0);

		

		do
		{
			px_uint i;
			px_dword distance_index_counter[30] = {0};
			px_dword counter[30] = {0};
			for (i = 0; i < in_size; i++)
			{
				if (_in[i] >=257)
				{
					px_dword distance_table[30] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
					px_int j;
					px_dword length = _in[i + 2];
					for (j = 0; j < PX_COUNTOF(distance_table) - 1; j++)
					{
						if (length < distance_table[j + 1])
							break;
					}
					if (j>=30)
					{
						PX_ASSERT();
					}
					counter[j]++;
					distance_index_counter[j]++;
				}
			}
			PX_HuffmanBuildDymanicTable(distance_index_counter,30, distance_code, distance_code_bl);

		} while (0);

		do
		{
			px_int i;
			px_dword counter[19] = { 0 };
			
			for (i = 0; i < 288; i++)
			{
				if (raw_code_bl[i]>=19)
				{
					PX_ASSERT();
				}
				counter[raw_code_bl[i]]++;
			}

			for (i = 0; i < 30; i++)
			{
				if (distance_code_bl[i] >= 19)
				{
					PX_ASSERT();
				}
				counter[distance_code_bl[i]]++;
			}

			PX_HuffmanBuildDymanicTable(counter, 19, raw_code_bl_code, raw_code_bl_code_bl);//code_code_bit_length 

			for ( i = 0; i < 19; i++)
			{
				if (raw_code_bl_code_bl[i] > 7)
				{
					return PX_FALSE;
				}
			}

		} while (0);
		
		if (raw_code_bl[287] != 0|| raw_code_bl[286] != 0)
		{
			PX_ASSERT();
		}


		
		hlit = 288 - 257;
		hdist = 30-1;
		hclen = 19-4;
		while (raw_code_bl[hlit+257-1] == 0 && hlit > 0)hlit--;
		while (distance_code_bl[hdist] == 0 && hdist > 0)hdist--;
		while (raw_code_bl_code_bl[rfc1951_map[hclen + 4 - 1]] == 0 && hclen > 0)
			hclen--;

		if (hlit+257>286)
		{
			PX_ASSERT();
		}

		if(!PX_MemoryCatBits(_out, (px_byte*)&hlit, 5))return PX_FALSE;
		if (!PX_MemoryCatBits(_out, (px_byte*)&hdist, 5))return PX_FALSE;
			if (!PX_MemoryCatBits(_out, (px_byte*)&hclen, 4))return PX_FALSE;

		

		
		do
		{
			px_uint i;
			
			for (i = 0; i < hclen+4; i++)
			{
				px_byte code = (px_byte)raw_code_bl_code_bl[rfc1951_map[i]];
				PX_MemoryCatBits(_out, &code, 3);
			}

			//encode bit_len
			for (i = 0; i < hlit+257; i++)
			{
				px_dword symbol = raw_code_bl[i];
				px_dword code = PX_HuffmanGetCode(symbol, raw_code_bl_code, raw_code_bl_code_bl);
				if (raw_code_bl_code_bl[symbol]==0)
				{
					PX_ASSERT();
				}
				PX_MemoryCatBits(_out, (px_byte*)&code, raw_code_bl_code_bl[symbol]);
			}

			for (i = 0; i < hdist+1; i++)
			{
				px_dword symbol = distance_code_bl[i];
				px_dword code = PX_HuffmanGetCode(symbol, raw_code_bl_code, raw_code_bl_code_bl);

				if (raw_code_bl_code_bl[symbol] == 0)
				{
					PX_ASSERT();
				}
				PX_MemoryCatBits(_out, (px_byte*)&code, raw_code_bl_code_bl[symbol]);
			}

		} while (0);
		
	}

	
	//write data
	do
	{
		px_uint i = 0;
		for (i = 0; i < in_size; i++)
		{
			px_dword symbol = _in[i];
			if (symbol<=256)
			{
				px_dword code = PX_HuffmanGetCode(symbol, raw_code, raw_code_bl);
				PX_MemoryCatBits(_out, (px_byte*)&code, raw_code_bl[_in[i]]);
			}
			else
			{
				px_dword symbol, extra;
				px_dword code;
				px_dword length_table[29] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,67, 83, 99, 115, 131, 163, 195, 227, 258 };
				px_dword length_extra[29] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,5, 5, 5, 0 };
				px_dword distance_table[30] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
				px_dword distance_extra[30] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,11, 11, 12, 12, 13, 13 };
				px_dword length = _in[i + 1];
				px_dword distance = _in[i + 2];
				px_uint j;
				
				i+=2;

				if (length>258||distance>32768)
				{
					return PX_FALSE;
				}

				//length index
				for (j = 0; j < PX_COUNTOF(length_table) - 1; j++)
				{
					if (length < length_table[j + 1])
						break;
				}

				extra = length-length_table[j];

				symbol = j + 257;
				code = PX_HuffmanGetCode(symbol, raw_code, raw_code_bl);
				if (!PX_MemoryCatBits(_out, (px_byte*)&code, raw_code_bl[symbol]))return PX_FALSE;
				if (!PX_MemoryCatBits(_out, (px_byte *)& extra, length_extra[j]))return PX_FALSE;

				//distance index
				for (j = 0; j < PX_COUNTOF(distance_table) - 1; j++)
				{
					if (distance < distance_table[j + 1])
						break;
				}

				extra = distance-distance_table[j];

				symbol = j;
				if (distance_code_bl[symbol]==0)
				{
					PX_ASSERT();
				}
				code = PX_HuffmanGetCode(symbol, distance_code, distance_code_bl);
				if (!PX_MemoryCatBits(_out, (px_byte*)&code, distance_code_bl[symbol]))return PX_FALSE;
				if (!PX_MemoryCatBits(_out, (px_byte*)&extra, distance_extra[j]))return PX_FALSE;;
			}
			
		}
	} while (0);

	
	return PX_TRUE;
}

