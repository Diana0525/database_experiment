#include "tpmms.h"

int tpmms()
{
    Buffer buf;// 定义一个缓冲区，520字节，可存放8块磁盘块
    unsigned char *blk; // 一个指向磁盘块的指针
    int i = 0;
    /* 获取块中数据需要的变量 */
    int X = -1;
    int Y = -1;
    int addr = -1;
    int count = 0;// 计数器，记录已有多少数字排列整齐

    char str[5];
    int min_X = 100;// 记录一次循环中找到的最小值,初始化为比值域最大值还大的值
    int min_Y;// 记录Y值
    int blk_num;// 记录最小值在第几块
    int tuple_num; // 记录在块中的第几行元组
    /* 定义一个buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* 在缓冲区中取出一个新的数据块用于写入 */
    blk_write = getNewBlockInBuffer(&buf);

    /* 读取磁盘块到buffer中 */
    for (i = 1; i <= 8; i++) // 循环读8块
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        for (int j = 0; j < 7; j++)// 每一个数据块的前7行存放着7个元组
        {
            for (int k = 0; k < 4; k++)
            {
                str[k] = *(blk + j*8 +k);
            }
            X = atoi(str);
            if (X < min)
            {
                min = X;
                for (int k = 0; k < 4; k++)
                {
                    str[k] = *(blk + j*8 + 4 + k);
                }
                min_Y = atoi(str);
                blk_num = i;// 记录找到的最小值在第几块
                tuple_num = j;// 记录在第几行
            }
        }
        count++; // 一次循环能找到一个最小值
    }
    /* 找出的第n小的元组，与其应该在的顺位位置上的数值交换 */
    /* 例如第一小的元组，应该与第一块第一行的数值交换 */
    /* blk读出该最小值应该在的位置,count=1时，则读出第1块 */
    blk_one = readBlockFromDisk((count/7)+1, &buf);
    blk_two = readBlockFromDisk(blk_num, &buf);
    /* 找出第(count/7)+1块第count%7行的元组 */
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk_one + (count%7-1)*8 + k);
    }
    X = atoi(str);
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk_one + (count%7-1)*8 + k + 4);
    }
    Y = atoi(str);



}
