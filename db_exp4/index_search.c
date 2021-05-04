#include "index_search.h"
/* 在（2）中排好序的S存放在221.blk~252.blk之中 */
int index_search()
{
    Buffer buf;// 定义一个缓冲区，520字节，可存放8块磁盘块
    unsigned char *blk; // 一个指向磁盘块的指针
    unsigned char *blk_write;// 指向新建的用于写入的磁盘块
    int SC_num;
    char s[3];
    /* 获取用户请求搜索的S.C值 */
    printf("#########################\n");
    printf("基于索引的关系选择 S.C=");
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
    /* 定义相关存储数据块blk中数据的变量 */
    int X = -1;
    int Y = -1;
    int Z = -1;
    int min_blk,max_blk;// 记录边界值的块号
    char str_X[5];
    char str_Y[5];
    char str_Z[5];
    int count = 0; // 计数
    int count_index = 0;// 便于对写入的数据块进行编号
    int count_sum = 0; // 记录一共找到多少符合的元组
    int flag = 0;// 若找到区间的最大值，则flag=1用于break整个读索引块循环
    /* 由于已经排序，因此我们可以建立索引文件便于选择 */
    /* 算法主要思路：先建立索引文件，后根据索引文件进行搜索 */
    /* 索引文件包括两个部分：数值和指向一个块的指针，我们可以通过写数据块实现索引块的建立 */
    /* 每个数据块存放7个元组(数值，该数值为最小值的磁盘块号) */
    // 建立索引，存放在301.blk～305.blk
    if (make_index() != 0)
    {
        perror("建立索引失败！");
        return -1;
    }
    // 循环读取索引块，找寻S.C的大概位置
    for (int i = 0; i < 5; i++)
    {
        /* 读取第i个磁盘块的内容 */
        if ((blk = readBlockFromDisk(301+i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入索引块：%d\n", 301+i);
        for (int j = 0; j < 7; j++)// 7个元组是有价值数据
        {
            for (int k = 0; k < 4; k++)
            {
                str_X[k] = *(blk + j*8 + k);
            }
            X =atoi(str_X);
            if (X < SC_num)// 索引值比需要搜索的值小，选择跳过
            {// 继续搜索下一个索引
                // 先保存好这个索引记录的块号
                for (int k = 0; k < 4; k++)
                {
                    str_Z[k] = *(blk + j*8 + 4 + k);
                }
                Z = atoi(str_Z);// 边界小值所在的块
                continue;
            }
            else if (X > SC_num)// 索引比需要搜索的值大或相等，则读取前一元组记录的块和这一块
            {
                for (int k = 0; k < 4; k++)
                {
                    str_Y[k] = *(blk + j*8 + 4 + k);
                }
                Y = atoi(str_Y);// 边界大值所在的块
                flag = 1;
                break;
            }
        }
        freeBlockInBuffer(blk, &buf);
        if (flag == 1)
        {
            flag = 0;
            break;
        }
    }
    min_blk = Z;
    max_blk = Y;
    /* 在缓冲区中取出一个新的数据块 */
    blk_write = getNewBlockInBuffer(&buf);
    /* 经过上述循环读取索引块，已经得到了SC_num属于(Z, Y) 块号之间的数据块*/
    for (int i = min_blk; i <= max_blk; i++)
    {
        /* 读取第i个磁盘块的内容 */
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", i);
        for (int j = 0; j < 7; j++)
        {
            for (int k = 0; k < 4; k ++)
            {
                str_X[k] = *(blk + j*8 +k);
            } 
            X = atoi(str_X);
            for (int k = 0; k < 4; k++)
            {
                str_Y[k] = *(blk + j*8 + 4 + k);
            }
            Y = atoi(str_Y);
            freeBlockInBuffer(blk, &buf);
            if (X == SC_num)
            {
                count_sum ++;
                printf("找到元组(%d, %d)\n",X, Y);
                write_block(blk_write+8*count, str_X, str_Y);
                count ++;
                if (count == 7)
                {
                    count = 0;
                    count_index ++;
                    writeAddrinBlk(blk_write, 310+count_index);
                    if (writeBlockToDisk(blk_write, 309+count_index, &buf) != 0)
                    {      
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    blk_write = getNewBlockInBuffer(&buf);
                    memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));
                }
            }
        }
    }
    count_index ++;
    // 将内存剩下的数值写入磁盘块
    writeAddrinBlk(blk_write, 310+count_index);
    if (writeBlockToDisk(blk_write, 309+count_index, &buf) != 0)
    {      
        perror("Writing Block Failed!\n");
        return -1;
    }
    printf("一共找到%d个符合要求的元组\n", count_sum);
    printf("IO次数为 %lu\n", buf.numIO);// 输出IO的次数
    printf("\n");
    return 0;
}
int make_index()
{
    Buffer buf;
    unsigned char *blk; // 一个指向磁盘块的指针
    unsigned char *blk_write;// 指向新建的用于写入的磁盘块
    /* 建立索引块不算入IO，因此这里的buf另初始化一个 */
    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    /* 定义相关存储数据块blk中数据的变量 */
    char str_X[5];
    char str_Y[5];
    int count = 0;// 记录已经有多少个元组被写入了blk_write
    /* 在缓冲区中取出一个新的数据块 */
    blk_write = getNewBlockInBuffer(&buf);
    /* 循环32次在221～252之间读取数据块 */
    for (int i = 0; i < 32; i++)
    {
        /* 读取第i个磁盘块的内容 */
        if ((blk = readBlockFromDisk(221+i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", 221+i);
        /* 默认读取第一个元组的X值，将(X, 221+i)写入新的块blk_write */
        for (int j = 0; j < 4; j++)
        {
            str_X[j] = *(blk + j);
        }
        freeBlockInBuffer(blk, &buf);
        /* 将221+i转为char */
        // +48将数字转为为ASCII码
        str_Y[0] = (221+i)/100 + 48;
        str_Y[1] = ((221+i)/10)%10 + 48;
        str_Y[2] = (221+i)%10 + 48;
        str_Y[3] = 0;
        str_Y[4] = '\0';
        write_block(blk_write+count*8, str_X, str_Y);
        count ++;
        if (count == 7) // 此时给最后一个元组写上下一块地址,并写入磁盘
        {
            /* 索引块从301开始存储 */
            /* i/8取值范围：0,1,2,3 */
            count = 0;
            writeAddrinBlk(blk_write, 301+i/8+1);
            if (writeBlockToDisk(blk_write, 301+i/8, &buf) != 0)
            {      
                perror("Writing Block Failed!\n");
                return -1;
            }
            blk_write = getNewBlockInBuffer(&buf);
            memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));
        }
    }
    // 将内存剩下的数值写入磁盘块
    writeAddrinBlk(blk_write, 306);
    if (writeBlockToDisk(blk_write, 305, &buf) != 0)
    {      
        perror("Writing Block Failed!\n");
        return -1;
    }
    return 0;
}