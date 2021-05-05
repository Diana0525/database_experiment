#include "linear_search.h"

int linear_search()
{
    Buffer buf;// 定义一个缓冲区，520字节，可存放8块磁盘块
    unsigned char *blk; // 一个指向磁盘块的指针
    unsigned char *blk_write;// 指向新建的用于写入的磁盘块
    int i = 0,j = 0;
    int SC_num;
    char s[3];
    /* 获取用户请求搜索的S.C值 */
    printf("#########################\n");
    printf("基于线性搜索的选择算法 S.C=");
    do{
        fflush(stdin);//清除缓冲区
        s[0] = getchar();
        s[1] = getchar();
        SC_num = atoi(s);
    }while(SC_num < 40 || SC_num > 80);
    printf("\n#########################\n");
    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* 在缓冲区中取出一个新的数据块 */
    blk_write = getNewBlockInBuffer(&buf);

    /* 定义相关存储数据块blk中数据的变量 */
    int X = -1;
    int Y = -1;
    int addr = 17;
    int count = 0; // 记录写入数据块的次数
    int addr_add = 1;// 地址增量,用于写入数据块最后一行
    int count_sum = 0; // 记录一共找到多少符合的元组
    char str[5];
    char str_X[5];
    char str_Y[5];
    /* 循环在硬盘中读取数据块 */
    for (j = 0; j < 32; j++) // S有32块
    {
        /* 读取第i个磁盘块的内容 */
        if ((blk = readBlockFromDisk(addr, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", addr);
        for (i = 0; i < 7; i++)// 一个blk存储7个元组加一个地址
        {
            for (int k = 0; k < 4; k++)
            {
                str_X[k] = *(blk + i*8 + k);
            }
            X = atoi(str_X);// 获取第i个元组的第一个值
            for (int k = 0; k < 4; k++)
            {
                str_Y[k] = *(blk + i*8 + 4 +k);
            }
            Y = atoi(str_Y);// 获取第二个值
            if (X == SC_num) // 若符合select条件
            {
                count_sum ++;
                printf("找到元组(%d, %d)\n",X, Y);
                write_block(blk_write+8*count, str_X, str_Y);
                count ++;
                if (count == 7) // 需要将blk_write写入磁盘
                {
                    /* 将下一个地址写入blk_write最后一块 */
                    str_X[0] = '1';
                    str_X[1] = '0';
                    str_X[2] = '0'+addr_add;
                    addr_add++;
                    memset(str_Y, 0, sizeof(str_Y));// 赋空值
                    write_block(blk_write+8*count, str_X, str_Y);
                    count = 0;// 重置为0
                    /* 将一个磁盘块写入磁盘 */
                    if (writeBlockToDisk(blk_write, 100, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    blk_write = getNewBlockInBuffer(&buf);
                    memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));

                }
            }
        }
        for (int k = 0; k < 4; k++) // 获取下一个地址
        {
            str[k] = *(blk + i*8 +k);
        }
        addr = atoi(str);

        freeBlockInBuffer(blk, &buf);
    }
    int final_addr;
    str_X[0] = '1';
    str_X[1] = '0';
    str_X[2] = '0'+addr_add-1;
    final_addr = atoi(str_X);
    /* 将未填满的数据块也写入磁盘，下一个地址不再指定 */
    if (writeBlockToDisk(blk_write, final_addr, &buf) != 0)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
    printf("一共有%d个元组符合要求\n", count_sum);

    printf("IO次数为 %lu\n", buf.numIO);// 输出IO的次数
    printf("\n");
    return 0;
}
/* 写入数据块的每一行8个字节，运行一次写一行，即8个字节 */
void write_block(unsigned char *blk, char str_X[], char str_Y[])
{
    int i;
    for (i = 0; i < 4; i++)
    {
        *(blk + i) = str_X[i];
    }
    for (i = 4; i < 8; i++)
    {
        *(blk + i) = str_Y[i-4];
    }

    printf("\n");
}

