#ifndef PX_HUFFMAN_H
#define PX_HUFFMAN_H
#include "PX_Quicksort.h"
#include "PX_Memory.h"

struct _huffman_node
{
	px_word left, right;
	px_dword weight;
	px_dword data;
};
typedef struct _huffman_node px_huffman_node;

typedef enum
{
	PX_HUFFMAN_TREE_TYPE_FIXED=1,
	PX_HUFFMAN_TREE_TYPE_DYNAMIC=2,
}PX_HUFFMAN_TREE_TYPE;

px_bool PX_HuffmanInflateCodeData(const px_byte _in[], px_uint* pbit_position, px_uint in_size, px_memory* out, PX_HUFFMAN_TREE_TYPE type/*1 or 2*/);
px_bool PX_HuffmanDeflateCodeData(px_word* _in, px_uint in_size, px_memory* _out, PX_HUFFMAN_TREE_TYPE type);

#endif
