#include "linear_search.h"

int linear_search()
{
    Buffer buf;// ����һ����������520�ֽڣ��ɴ��8����̿�
    unsigned char *blk; // һ��ָ����̿��ָ��
    unsigned char *blk_write;
    int i = 0,j = 0;
    int SC_num;
    char s[3];
    printf("#########################\n");
    printf("��������������ѡ���㷨 S.C=");
    do{
        fflush(stdin);//���������
        s[0] = getchar();
        s[1] = getchar();
        SC_num = atoi(s);
        printf("%d\n", SC_num);
    }while(SC_num < 40 || SC_num > 80);
    printf("\n#########################\n");
    /* ����һ��buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* �ڻ�������ȡ��һ���µ����ݿ� */
    blk_write = getNewBlockInBuffer(&buf);

    /* ������ش洢���ݿ�blk�����ݵı��� */
    int X = -1;
    int Y = -1;
    int addr = 17;
    int count = 0; // ��¼д�����ݿ�Ĵ���
    int addr_add = 1;// ��ַ����,����д�����ݿ����һ��
    int count_sum = 0; // ��¼һ���ҵ����ٷ��ϵ�Ԫ��
    char str[5];
    char str_X[5];
    char str_Y[5];
    /* ѭ����Ӳ���ж�ȡ���ݿ� */
    for (j = 0; j < 32; j++) // S��32��
    {
        /* ��ȡ��i�����̿������ */
        if ((blk = readBlockFromDisk(addr, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("�������ݿ�%d\n", addr);
        for (i = 0; i < 7; i++)// һ��blk�洢7��Ԫ���һ����ַ
        {
            for (int k = 0; k < 4; k++)
            {
                str_X[k] = *(blk + i*8 + k);
            }
            X = atoi(str_X);// ��ȡ��i��Ԫ��ĵ�һ��ֵ
            for (int k = 0; k < 4; k++)
            {
                str_Y[k] = *(blk + i*8 + 4 +k);
            }
            Y = atoi(str_Y);// ��ȡ�ڶ���ֵ
            if (X == SC_num) // ������select����
            {
                count_sum ++;
                printf("�ҵ�Ԫ��(%d, %d)\n",X, Y);
                write_block(blk_write+8*count, str_X, str_Y);
                count ++;
                if (count == 7) // ��Ҫ��blk_writeд�����
                {
                    /* ����һ����ַд��blk_write���һ�� */
                    str_X[0] = '1';
                    str_X[1] = '0';
                    str_X[2] = '0'+addr_add;
                    addr_add++;
                    memset(str_Y, 0, sizeof(str_Y));// ����ֵ
                    write_block(blk_write+8*count, str_X, str_Y);
                    count = 0;// ����Ϊ0
                    /* ��һ�����̿�д����� */
                    if (writeBlockToDisk(blk_write, 100, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    freeBlockInBuffer(blk_write, &buf);
                    blk_write = getNewBlockInBuffer(&buf);
                    memset(blk_write, 0, buf.blkSize*sizeof(unsigned char));

                }
            }
        }
        for (int k = 0; k < 4; k++) // ��ȡ��һ����ַ
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
    /* ��δ���������ݿ�Ҳд����̣���һ����ַ����ָ�� */
    if (writeBlockToDisk(blk_write, final_addr, &buf) != 0)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
    printf("һ����%d��Ԫ�����Ҫ��\n", count_sum);

    printf("IO����Ϊ %d\n", buf.numIO);// ���IO�Ĵ���
    printf("\n");
    return 0;
}
/* д�����ݿ��ÿһ��8���ֽڣ�����һ��дһ�У���8���ֽ� */
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

