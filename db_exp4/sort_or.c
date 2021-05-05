#include "sort_or.h"
/* 求S和R的并集 */
int sort_or()
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
    int count = 0;
    int count_or = 0;
    int count_index = 0;
    char str_SC[5];
    char str_SD[5];
    char str_RA[5];
    char str_RB[5];

    printf("#########################\n");
    printf("       并集合操作\n");
    printf("#########################\n");

    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    /* 外层循环指针锁定在S的数据块和元组上 */
    /* 内层循环指针锁定在R的数据块和元组上 */
    /* 锁定一个S中的元组，在R中遍历，SC与RA做比较，若小于则继续读取R中的下一个 */
    /* 若等于，则判断RB是否与SD相等，若相等，则返回S循环，并将该元组写入并集；若不相等，则继续遍历R中下一个 */
    /* 若大于，则返回S循环，并将该元组写入并集中 */
    /* SC大于60的直接加入并集 */
    /* 上述循环结束后还缺少R中的一些S中没有的元组，再对R和初步并集做同样的循环即可找出所有的并集 */
    /* 只有一点例外，当出现相同的，不再写入并集 */

    /* 在缓冲区中取出一个新的数据块 */
    blk_write = getNewBlockInBuffer(&buf);

    /* 最外层是SC的循环 */
    for (int i = 221; i <= 252; i++)
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
                /* 读取第r个磁盘块的内容 */
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
                        continue;
                    }
                    else if ((RA == SC && RB == SD) || RA > SC)
                    {
                        flag = 1;
                        break;
                    }
                }
                freeBlockInBuffer(blk_R, &buf);
                if (flag == 1)
                {
                    break;
                }
                r ++;
            }
            if (flag == 1)// 说明需要写入
            {
                flag = 0;
                count_or ++;// 并集元素数目计数
                write_block(blk_write+8*count, str_SC, str_SD);
                count++;
            }
            
            if (count == 7)
            {
                count = 0;
                count_index ++;
                writeAddrinBlk(blk_write, 503+count_index);
                if (writeBlockToDisk(blk_write, 502+count_index, &buf) != 0)
                {      
                    perror("Writing Block Failed!\n");
                    return -1;
                }
                printf("结果写入磁盘：%d\n", 502+count_index);
                blk_write = getNewBlockInBuffer(&buf);
                memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));
            }
        }
        freeBlockInBuffer(blk_S, &buf);
    }
    
    // // 将内存剩下的数值写入磁盘块
    // if (count != 7 && count != 0)
    // {
    //     count_index ++;
    //     writeAddrinBlk(blk_write, 502+count_index);
    //     if (writeBlockToDisk(blk_write, 501+count_index, &buf) != 0)
    //     {      
    //         perror("Writing Block Failed!\n");
    //         return -1;
    //     }
    // }
    /* 将S中大于等于60的值直接写入并集中 */
    for (int i = 221; i <= 252; i++)
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
                str_SD[k] = *(blk_S + j*8 +k + 4);
            }
            SD = atoi(str_SD);
            if (SC >= 60)
            {
                count_or ++;// 并集元素数目计数
                write_block(blk_write+8*count, str_SC, str_SD);
                count++;
                if (count == 7)
                {
                    count = 0;
                    count_index ++;
                    writeAddrinBlk(blk_write, 503+count_index);
                    if (writeBlockToDisk(blk_write, 502+count_index, &buf) != 0)
                    {      
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    printf("结果写入磁盘：%d\n", 502+count_index);
                    blk_write = getNewBlockInBuffer(&buf);
                    memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));
                }
            }
        }
        freeBlockInBuffer(blk_S, &buf);
    }
    // 将内存剩下的数值写入磁盘块
    if (count != 7 && count != 0)
    {
        count_index ++;
        writeAddrinBlk(blk_write, 503+count_index);
        if (writeBlockToDisk(blk_write, 502+count_index, &buf) != 0)
        {      
            perror("Writing Block Failed!\n");
            return -1;
        }
    }
    /* 目前已有的并集存在于503.blk～533.blk中 */
    r = 221;
    /* R与已经存在的并集再次进行上述循环的操作 */
    for (int i = 201; i <= 216; i++)
    {
        /* 读取第i个磁盘块的内容 */
        if ((blk_R = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        for (int j = 0; j < 7; j ++)// 7个元组是有价值数据
        {
            for (int k = 0; k < 4; k++)
            {
                str_RA[k] = *(blk_R + j*8 +k);
            }
            RA = atoi(str_RA);
            for (int k = 0; k < 4; k++)
            {
                str_RB[k] = *(blk_R + j*8 +k +4);
            }
            RB = atoi(str_RB);
            r = 221;
            while (r <= 252)
            {
                /* 读取第i个磁盘块的内容 */
                if ((blk_S = readBlockFromDisk(r, &buf)) == NULL)
                {
                    perror("Reading Block Failed!\n");
                    return -1;
                }
                for (int a = 0; a < 7; a++) // 7个元组是有价值的数据
                {
                    for (int b = 0; b < 4; b++)
                    {
                        str_SC[b] = *(blk_S + a*8 + b);
                    }
                    SC = atoi(str_SC);
                    for (int b = 0; b < 4; b++)
                    {
                        str_SD[b] = *(blk_S + a*8 + 4 + b);
                    }
                    SD = atoi(str_SD);
                    if (RA < SC)
                    {
                        flag = 1;
                        break;
                    }
                    else if (RA == SC && RB == SD)
                    {
                        flag = 0;
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                freeBlockInBuffer(blk_S, &buf);
                if (flag == 1)
                {
                    break;
                }
                r ++;
            }
            if (flag == 1)// 说明需要写入
            {
                flag = 0;
                count_or ++;// 并集元素数目计数
                write_block(blk_write+8*count, str_RA, str_RB);
                count++;
            }
            if (count == 7)
            {
                count = 0;
                count_index ++;
                writeAddrinBlk(blk_write, 503+count_index);
                if (writeBlockToDisk(blk_write, 502+count_index, &buf) != 0)
                {      
                    perror("Writing Block Failed!\n");
                    return -1;
                }
                printf("结果写入磁盘：%d\n", 502+count_index);
                blk_write = getNewBlockInBuffer(&buf);
                memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));
            }

        }
        freeBlockInBuffer(blk_R, &buf);
    }
    printf("R和S的并集里有%d个元素\n", count_or);
    return 0;
}