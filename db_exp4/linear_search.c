#include "linear_search.h"

int linear_search(int SC_num)
{
    Buffer buf;// ����һ����������520�ֽڣ��ɴ��8����̿�
    unsigned char *blk; // һ��ָ����̿��ָ��
    int i = 0;

    /* ����һ��buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* �ڻ�������ȡ��һ���µ����ݿ� */
    blk = getNewBlockInBuffer(&buf);

    /* ��Ӳ���ж�ȡ���ݿ� */
    if ((blk = readBlockFromDisk(17, &buf)) == NULL)
    {
        perror("Reading Block Failed!\n");
        return -1;
    }

    /* ������ش洢���ݿ�blk�����ݵı��� */

    int X = -1;
    int Y = -1;
    int addr = -1;

    char str[5];
    printf("block 17:\n");
    for (i = 0; i < 7; i++)// һ��blk�洢7��Ԫ���һ����ַ
    {
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i*8 + k);
        }
        X = atoi(str);// ��ȡ��i��Ԫ��ĵ�һ��ֵ
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i*8 + 4 +k);
        }
        Y = atoi(str);// ��ȡ�ڶ���ֵ
        printf("(%d, %d) ",X, Y);
    }
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk + i*8 +k);
    }
    addr = atoi(str);
    printf("\nnext address = %d \n", addr);

    printf("\n");
    printf("IO����Ϊ %d\n", buf.numIO);// ���IO�Ĵ���


}
