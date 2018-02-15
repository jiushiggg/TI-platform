
#ifndef __gunzip_h_
#define __gunzip_h_

#include "mini_fs.h"


#define SEGMENT_LEN 4096

#define FHCRC_MASK 2
#define FEXTRA_MASK 4
#define FNAME_MASK 8
#define FCOMMENT_MASK 16

#define BTYPE_NONE 0
#define BTYPE_DYNAMIC 2

#define MAX_BITS 16
#define MAX_CODE_LITERALS 287
#define MAX_CODE_DISTANCES 31
#define MAX_CODE_LENGTHS 18
#define EOB_CODE 256

#define MAX_LENGTHS_NUM     ((MAX_CODE_LENGTHS << 1) + MAX_BITS)
#define MAX_LITERALS_NUM    ((MAX_CODE_LITERALS << 1) + MAX_BITS)
#define MAX_DISTANCES_NUM   ((MAX_CODE_DISTANCES << 1) + MAX_BITS)

typedef unsigned char byte;

#define ERR_FORMAT          -1  // can't support the format of compression
#define ERR_LITERALS_NUM    -2  // literal overflow
#define ERR_DISTANCES_NUM   -3  // distance overflow

struct inflate_context {
    int index;
    int byte;
    int bit;
    int compressed_len;
    
    int lengths_tree[MAX_LENGTHS_NUM];
    int lengths_num;
    
    int literals_tree[MAX_LITERALS_NUM];
    int literals_num;
    
    int distances_tree[MAX_DISTANCES_NUM];
    int distances_num;
};

extern int gunzip(file_id_t zfd, int offset, int zlen, byte *ubuf);

#endif /* __gunzip_h_ */
