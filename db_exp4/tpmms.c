#include "tpmms.h"
#define BLOCKSIZE 64
int tpmms()
{
    Buffer buf;// 定义一个缓冲区，520字节，可存放8块磁盘块
    unsigned char *blk; // 一个指向磁盘块的指针
    int i = 0, j = 0;
    /* 获取块中数据需要的变量 */
    int X = -1;
    int Y = -1;
    int count = 0;// 计数器，记录已有多少数字排列整齐

    char str[5];
    int min_X = 100;// 记录一次循环中找到的最小值,初始化为比值域最大值还大的值
    char min_X_str[5];
    int min_Y;// 记录Y值
    char min_Y_str[5];
    char str_X[5];
    char str_Y[5];
    int blk_num;// 记录最小值在第几块
    int tuple_num; // 记录在块中的第几行元组
    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    /* 读取磁盘块到buffer中 */
    /* 利用count记录的已经排好序的元组值，就能跳过已经排序的部分，在其他部分筛选最小值 */
    /* 循环八次将buf填满 */
    for (i = 1; i <= 8; i++)
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
    }
    blk = blk-(BLOCKSIZE+1)*7;// 让blk重新指向第一个数据块的开头
    /* 每一次的循环，可以找到一个最小值，如今循环7*8次 */
    for (int a = 0; a < 56; a++)
    {
        min_X = 100;// 每次循环重置开始的最小值为大值
        for (i = count/7; i < 8; i++) // 循环读8块
        {
            /* i为count/7时，j才是count%7,否则从0开始 */
            for (j = (i == count/7)? count%7 : 0; j < 7; j++)// 每一个数据块的前7行存放着7个元组
            {
                for (int k = 0; k < 4; k++)
                {
                    str[k] = *(blk + i*(BLOCKSIZE+1) + j*8 +k);
                }
                X = atoi(str);
                if (X < min_X)
                {
                    min_X = X;
                    for (int k = 0; k < 5; k++)
                    {
                        min_X_str[k] = str[k];// 保存字符串
                    }
                    for (int k = 0; k < 4; k++)
                    {
                        str[k] = *(blk + i*(BLOCKSIZE+1) + j*8 + 4 + k);
                    }
                    min_Y = atoi(str);
                    for (int k = 0; k < 5; k++)
                    {
                        min_Y_str[k] = str[k];
                    }
                    blk_num = i+1;// 记录找到的最小值在第几块
                    tuple_num = j+1;// 记录在第几行
                }
            }
        }
        count++; // 一次循环能找到一个最小值
        printf("第%d个最小值：（%d, %d）\n",count, min_X, min_Y);
        printf("最小值出现在第%d块的第%d行\n",blk_num, tuple_num);
        /* 找出的第n小的元组，与其应该在的顺位位置上的数值交换 */
        /* 例如第一小的元组，应该与第一块第一行的数值交换 */
        /* blk读出该最小值应该在的位置,count=1时，则读出第1块 */
        /* 找出第(count/7)+1块第count%7行的元组 */
        /* 目前的blk指向的是最后一个数据块的开头，第一个块数据块的开头为:blk-(BLOCKSIZE+1)*7 */
        for (int k = 0; k < 4; k++)
        {
            str_X[k] = *(blk + (count-1)/7*(BLOCKSIZE+1) + (count-1)%7*8 + k);//一个数据块存放7组有用的数据，所以对7取余
        }
        X = atoi(str_X);
        for (int k = 0; k < 4; k++)
        {
            str_Y[k] = *(blk + (count-1)/7*(BLOCKSIZE+1) + (count-1)%7*8 + k + 4);
        }
        Y = atoi(str_Y);
        /* 两组元组进行交换 */
        printf("第%d块第%d行(%d, %d)\n", count/7+1, count%7, X, Y);
        printf("blk_one:%c%c \n",*(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8),
               *(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8+1));
        printf("blk_two:%c%c\n",*(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8),
               *(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8+1));

        write_block(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8, min_X_str, min_Y_str);
        write_block(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8, str_X, str_Y);
        printf("swap finished\n");

    }
    /* 循环为8块blk末尾加上地址信息，此次的保存在111.blk~118.blk */
    for (int k = 0; k <8; k++)
    {
        writeAddrinBlk(blk + k*(BLOCKSIZE+1), 111 + k + 1);
        if (writeBlockToDisk(blk+k*(BLOCKSIZE+1), 111+k, &buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
    }
    return 0;
}
/* 为每个块的最后一行写入下一块的地址 */
/* 输入块的首地址 */
int writeAddrinBlk(unsigned char *blk, int addr)
{
    char str[5];
    // +48存储为ASCII码
    str[0] = (addr/100) + 48;
    str[1] = (addr/10)%10 + 48;
    str[2] = addr%10 + 48;
    write_block(blk+8*7, str, "");
}

