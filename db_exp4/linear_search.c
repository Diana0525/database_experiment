#include "linear_search.h"

int linear_search(int SC_num)
{
    Buffer buf;// 定义一个缓冲区，520字节，可存放8块磁盘块
    unsigned char *blk; // 一个指向磁盘块的指针
    int i = 0;

    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* 在缓冲区中取出一个新的数据块 */
    blk = getNewBlockInBuffer(&buf);

    /* 在硬盘中读取数据块 */
    if ((blk = readBlockFromDisk(17, &buf)) == NULL)
    {
        perror("Reading Block Failed!\n");
        return -1;
    }

    /* 定义相关存储数据块blk中数据的变量 */

    int X = -1;
    int Y = -1;
    int addr = -1;

    char str[5];
    printf("block 17:\n");
    for (i = 0; i < 7; i++)// 一个blk存储7个元组加一个地址
    {
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i*8 + k);
        }
        X = atoi(str);// 获取第i个元组的第一个值
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i*8 + 4 +k);
        }
        Y = atoi(str);// 获取第二个值
        printf("(%d, %d) ",X, Y);
    }
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk + i*8 +k);
    }
    addr = atoi(str);
    printf("\nnext address = %d \n", addr);

    printf("\n");
    printf("IO次数为 %d\n", buf.numIO);// 输出IO的次数


}
