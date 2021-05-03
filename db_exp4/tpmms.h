#ifndef TPMMS_H_INCLUDED
#define TPMMS_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extmem.h"
#include "linear_search.h"
int tpmms();
int tpmms_one(Buffer *buf, unsigned char *blk);
int writeAddrinBlk(unsigned char *blk, int addr);
int tpmms_R(Buffer *buf);
int tpmms_S(Buffer *buf);
int Tone_myWritetoDisk(unsigned char *blk, int beginFileName, Buffer *buf);
int Ttwo_myWritetoDisk(unsigned char *blk, int beginFileName, Buffer *buf);
unsigned char *myreadBlockFromDisk(unsigned int addr, Buffer *buf, unsigned char *blk);
#endif // TPMMS_H_INCLUDED
