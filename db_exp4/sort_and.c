#include "sort_and.h"
/* 求S和R的交集 */
int sort_and()
{
    Buffer buf;// 定义一个缓冲区，520字节，可存放8块磁盘块
    unsigned char *blk_S; // 一个指向S磁盘块的指针，存在221.blk～252.blk
    unsigned char *blk_R; // 一个指向R磁盘块的指针，存在201.blk～216.blk
    unsigned char *blk_write;// 指向新建的用于写入的磁盘块

    /* 定义相关存储数据块blk中数据的变量 */
    int SC = -1;
    int SD = -1;
    int RA = -1;
    int RB = -1;
    int r = 201; // 读取R磁盘块的索引
    int flag = 0;
    int count_index = 0;
    int count_and = 0;// 统计并集的元素
    int count = 0;// 用于写入数据块元组计数，达到7就准备写地址，并写入磁盘
    char str_SC[5];
    char str_SD[5];
    char str_RA[5];
    char str_RB[5];

    printf("#########################\n");
    printf("       交集合操作\n");
    printf("#########################\n");

    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* 在缓冲区中取出一个新的数据块 */
    blk_write = getNewBlockInBuffer(&buf);
    /* 以S.C作为比较的基准，在外层循环读入 */
    for (int i = 221; i <=252; i++)
    {
        /* 读取第i个磁盘块的内容 */
        if ((blk_S = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        for (int j = 0; j < 7; j++)// 7个元组是有价值数据
        {
            for (int k = 0; k < 4; k++)
            {
                str_SC[k] = *(blk_S + j*8 +k);
            }
            SC = atoi(str_SC);
            for (int k = 0; k < 4; k++)
            {
                str_SD[k] = *(blk_S + j*8 +k +4);
            }
            SD = atoi(str_SD);
            r = 201;
            while (r <= 216)
            {
                /* 读取第i个磁盘块的内容 */
                if ((blk_R = readBlockFromDisk(r, &buf)) == NULL)
                {
                    perror("Reading Block Failed!\n");
                    return -1;
                }
                for (int a = 0; a < 7; a++) // 7个元组是有价值的数据
                {
                    for (int b = 0; b < 4; b++)
                    {
                        str_RA[b] = *(blk_R + a*8 + b);
                    }
                    RA = atoi(str_RA);
                    for (int b = 0; b < 4; b++)
                    {
                        str_RB[b] = *(blk_R + a*8 + 4 + b);
                    }
                    RB = atoi(str_RB);
                    if (RA < SC || (RA == SC && RB != SD))
                    {  
                        continue;// 继续读取下一个元组
                    }
                    else if (RA > SC)// 留此次读取的块编号，换一个S元组进行比较
                    {
                        flag = 1;
                        break;
                    }
                    else if (RA == SC && RB == SD)// 发现能可能成为并集的数据
                    {
                        // 发现SC=RA,SD=RB的元组
                        count_and ++;
                        if (RA == SC && RB != SD && RA == 43)
                        write_block(blk_write+8*count, str_SC, str_SD);
                        count++;
                        
                        if (count == 7)
                        {
                            count = 0;
                            count_index ++;
                            writeAddrinBlk(blk_write, 501+count_index);
                            if (writeBlockToDisk(blk_write, 500+count_index, &buf) != 0)
                            {      
                                perror("Writing Block Failed!\n");
                                return -1;
                            }
                            printf("结果写入磁盘：%d\n", 500+count_index);
                            blk_write = getNewBlockInBuffer(&buf);
                            memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));
                        }
                    }
                }
                freeBlockInBuffer(blk_R, &buf);
                if (flag == 1)
                {
                    flag = 0;
                    break;
                }
                r ++;
                
            }
        }
        freeBlockInBuffer(blk_S, &buf);
    }
    count_index ++;
    // 将内存剩下的数值写入磁盘块
    writeAddrinBlk(blk_write, 501+count_index);
    if (writeBlockToDisk(blk_write, 500+count_index, &buf) != 0)
    {      
        perror("Writing Block Failed!\n");
        return -1;
    }
    printf("结果写入磁盘：%d\n", 500+count_index);
    printf("\n");
    printf("S和R的交集一共有%d个元组\n", count_and);
    return 0;
}