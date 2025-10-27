#ifndef __HUFFMAN_CODE_H__
#define __HUFFMAN_CODE_H__

typedef struct HuffmanData
{
  char item;
  int freq;
} HuffmanData;

int huffman_encoding (char *s, int len, char **out_str, int *out_str_len, BinTreeNode **root, HuffmanData **huffman_data);
void huffman_code_tree_print (BinTreeNode *root, char arr[], int top);
void huffman_decoding (BinTreeNode *root, char* encoded_str, int encoded_str_len, char **decoded_str, int *decoded_str_len);

#endif /* __HUFFMAN_CODE_H__ */