#ifndef TPMMS_H_INCLUDED
#define TPMMS_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extmem.h"
#include "linear_search.h"
int tpmms();
int tpmms_one(int beginFileName, Buffer *buf, unsigned char *blk);
int writeAddrinBlk(unsigned char *blk, int addr);
int tpmms_R(Buffer *buf);
#endif // TPMMS_H_INCLUDED
