#include "tpmms.h"
#define BLOCKSIZE 64
int tpmms()
{
    Buffer buf;// 定义一个缓冲区，520字节，可存放8块磁盘块
    unsigned char *blk;
    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    /* 外层循环一次表示进行了一次内存排序 */
    /* 循环结束后，则结束了第一轮排序 */
    for (int i = 1; i <= 41; i+=8)
    {
        /* 读取磁盘块到buffer中 */
        /* 利用count记录的已经排好序的元组值，就能跳过已经排序的部分，在其他部分筛选最小值 */
        /* 循环八次将buf填满 */
        for (int j= i; j <= i+7; j++)
        {
            if ((blk = readBlockFromDisk(j, &buf)) == NULL)
            {
                perror("Reading Block Failed!\n");
                return -1;
            }
        }
        blk = blk-(BLOCKSIZE+1)*7;// 让blk重新指向第一个数据块的开头
        if (tpmms_one(&buf, blk) != 0)
        {
            perror("tpmms_one wrong!");
            return -1;
        }
        Tone_myWritetoDisk(blk, i, &buf);
    }
    /* 第二轮排序开始 */
    /* 先排序R */
    tpmms_R(&buf);
    tpmms_S(&buf);
    printf("IO次数为 %lu\n", buf.numIO);// 输出IO的次数
    return 0;
}

int tpmms_one(Buffer *buf, unsigned char *blk)
{
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
        // printf("第%d个最小值：（%d, %d）\n",count, min_X, min_Y);
        // printf("最小值出现在第%d块的第%d行\n",blk_num, tuple_num);
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
        // printf("第%d块第%d行(%d, %d)\n", count/7+1, count%7, X, Y);
        // printf("blk_one:%c%c \n",*(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8),
        //        *(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8+1));
        // printf("blk_two:%c%c\n",*(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8),
        //        *(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8+1));

        write_block(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8, min_X_str, min_Y_str);
        write_block(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8, str_X, str_Y);
        // printf("swap finished\n");
    }
    return 0;
}
/* 若在第一轮排序中，则循环为8块blk末尾加上地址信息*/
int Tone_myWritetoDisk(unsigned char *blk, int beginFileName, Buffer *buf)
{
    for (int k = 0; k < 8; k++)
    {
        writeAddrinBlk(blk + k*(BLOCKSIZE+1), 110+k+1+beginFileName);
        if (writeBlockToDisk(blk+k*(BLOCKSIZE+1), 110+k+beginFileName, buf) != 0)
        {      
            perror("Writing Block Failed!\n");
            return -1;
        }
    }
    return 0;
}
/* 若在第二轮排序，则只将第一块数据块写入磁盘 */
int Ttwo_myWritetoDisk(unsigned char *blk, int beginFileName, Buffer *buf)
{
    writeAddrinBlk(blk, 201+1+beginFileName);
    if (writeBlockToDisk(blk, 201+beginFileName, buf) != 0)
    {      
        perror("Writing Block Failed!\n");
        return -1;
    }

    return 0;
}
/* 为每个块的最后一行写入下一块的地址 */
/* 输入块的首地址 */
int writeAddrinBlk(unsigned char *blk, int addr)
{
    char str[5];
    char str_Y[5];
    // +48存储为ASCII码
    str[0] = (addr/100) + 48;
    str[1] = (addr/10)%10 + 48;
    str[2] = addr%10 + 48;
    memset(str_Y, 0, sizeof(str_Y));// 赋空值
    write_block(blk+8*7, str, str_Y);
    return 0;
}
/* 为R的16块数据块排序，第一轮排序结果存在111.blk~127.blk中，第二轮排序结果将存入201.blk~216.blk*/
int tpmms_R(Buffer *buf)
{
    unsigned char *blk; // 一个指向磁盘块的指针
    int flag=0; // 标识作用，用于在R的两个组里交替选择数据块
    /*分批次读取，第一轮读取111.blk~114.blk和119.blk~122.blk*/
    /* 读取磁盘块到buffer中 */
    for (int i = 111; i <= 114; i++)
    {
        if ((blk = readBlockFromDisk(i, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
    }
    for (int i = 119; i <= 122; i++)
    {
        if ((blk = readBlockFromDisk(i, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
    }
    blk = blk-(BLOCKSIZE+1)*7;// 让blk重新指向第一个数据块的开头
    /* 接下来再按照分组进行归并排序，例如再将115.blk读入第一块，再排序写入第一块后，接下来再读入123.blk到第一块 */
     while (flag < 8)
    {
        if (flag % 2 == 0)// 读取第一组的数据，在111~118
        {   
            if (tpmms_one(buf, blk) != 0)
            {
                printf("R第二轮第%d次排序失败！", flag);
                return -1;
            }
            Ttwo_myWritetoDisk(blk, flag, buf);
            /* 替换掉已经被写入磁盘的第一块blk,将写入115+0/+1/+2/+3 共4个值*/
            blk = myreadBlockFromDisk(115+flag/2, buf, blk);
        }
        else // 读取第二组的数据，119～126
        {
            if (tpmms_one(buf, blk) != 0)
            {
                printf("R第二轮第%d次排序失败！", flag);
                return -1;
            }
            Ttwo_myWritetoDisk(blk, flag, buf);
            /* 替换掉已经被写入磁盘的第一块blk,将写入123+1/+2/+3/ 共4个值*/
            blk = myreadBlockFromDisk(123+flag/2, buf, blk);
        }
        flag ++;
    }
    /* 将最后剩下的内存中的内容全部写到磁盘上 */
    if (tpmms_one(buf, blk) != 0)
    {
        printf("R第二轮第%d次排序失败！", flag);
        return -1;
    }
    Tone_myWritetoDisk(blk, 99, buf);
    return 0;
}
/* S的4*8=32共分成4组，分别进行排序 */
int tpmms_S(Buffer *buf)
{
    unsigned char *blk; // 一个指向磁盘块的指针
    int flag=0; // 标识作用，用于在R的两个组里交替选择数据块
    /* 分批次读取，第一次每组读取2块数据块 */
    /* 各组分别读取的是127-128,135-136,143-144,151-152 */
    for (int i = 0; i < 4; i++)
    {
        if ((blk = readBlockFromDisk(i*8+127, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        if ((blk = readBlockFromDisk(i*8+128, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
    }
    blk = blk-(BLOCKSIZE+1)*7;// 让blk重新指向第一个数据块的开头
    /* 接下来按照4分组进行归并排序,例如再依次将129、137、145、152按照次序放入buffer开头 */
    while (flag < 24)//还剩下24数据块，需要被依次放入内存之中
    {
        if (flag % 4 == 0)
        {
            if (tpmms_one(buf, blk) != 0)
            {
                printf("S第二轮第%d次排序失败！", flag);
                return -1;
            }
            Ttwo_myWritetoDisk(blk, flag+20, buf);
            /* 替换掉已经被写入磁盘的第一块blk，将会写入129+0/+1/+2/+3/+4/+5 */
            blk = myreadBlockFromDisk(129+flag/4, buf, blk);
        }
        else if (flag % 4 == 1)
        {
            if (tpmms_one(buf, blk) != 0)
            {
                printf("S第二轮第%d次排序失败！", flag);
                return -1;
            }
            Ttwo_myWritetoDisk(blk, flag+20, buf);
            /* 替换掉已经被写入磁盘的第一块blk，将会写入137+0/+1/+2/+3/+4/+5 */
            blk = myreadBlockFromDisk(137+flag/4, buf, blk);
        }
        else if (flag % 4 == 2)
        {
            if (tpmms_one(buf, blk) != 0)
            {
                printf("S第二轮第%d次排序失败！", flag);
                return -1;
            }
            Ttwo_myWritetoDisk(blk, flag+20, buf);
            /* 替换掉已经被写入磁盘的第一块blk，将会写入145+0/+1/+2/+3/+4/+5 */
            blk = myreadBlockFromDisk(145+flag/4, buf, blk);
        }
        else if (flag % 4 == 3)
        {
            if (tpmms_one(buf, blk) != 0)
            {
                printf("S第二轮第%d次排序失败！", flag);
                return -1;
            }
            Ttwo_myWritetoDisk(blk, flag+20, buf);
            /* 替换掉已经被写入磁盘的第一块blk，将会写入152+0/+1/+2/+3/+4/+5 */
            blk = myreadBlockFromDisk(152+flag/4, buf, blk);
        }
        flag ++;
    }
    /* 将最后剩下的内存中的内容全部写到磁盘上 */
    if (tpmms_one(buf, blk) != 0)
    {
        printf("R第二轮第%d次排序失败！", flag);
        return -1;
    }
    Tone_myWritetoDisk(blk, 135, buf);
    return 0;
    
    return 0;
}
/* 为了能够使得替换内存中的第一块数据块，根据extmem.c自定义一个函数，输入指向内存开头地址的blk指针即可 */
unsigned char *myreadBlockFromDisk(unsigned int addr, Buffer *buf, unsigned char *blk)
{
    char filename[40];
    unsigned char *bytePtr;
    char ch;
    if (buf->numFreeBlk == 0)
    {
        perror("Buffer Overflows!\n");
        return NULL;
    }


    // while (blk < buf->data + (buf->blkSize + 1) * buf->numAllBlk)
    // {
    //     if (*blk == BLOCK_AVAILABLE)
    //         break;
    //     else
    //         blk += buf->blkSize + 1;
    // }

    sprintf(filename, "data/%d.blk", addr);
    FILE *fp = fopen(filename, "r");

    if (!fp)
    {
        perror("Reading Block Failed!\n");
        return NULL;
    }

    // *blk = BLOCK_UNAVAILABLE;
    // blk++;
    bytePtr = blk;
    while (bytePtr < blk + buf->blkSize)
    {
        ch = fgetc(fp);
        *bytePtr = ch;
        bytePtr++;
    }

    fclose(fp);
    buf->numFreeBlk--;
    buf->numIO++;
    return blk;
}
