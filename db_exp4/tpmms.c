#include "tpmms.h"
#define BLOCKSIZE 64
int tpmms()
{
    Buffer buf;// ����һ����������520�ֽڣ��ɴ��8����̿�
    unsigned char *blk; // һ��ָ����̿��ָ��
    int i = 0, j = 0;
    /* ��ȡ����������Ҫ�ı��� */
    int X = -1;
    int Y = -1;
    int count = 0;// ����������¼���ж���������������

    char str[5];
    int min_X = 100;// ��¼һ��ѭ�����ҵ�����Сֵ,��ʼ��Ϊ��ֵ�����ֵ�����ֵ
    char min_X_str[5];
    int min_Y;// ��¼Yֵ
    char min_Y_str[5];
    char str_X[5];
    char str_Y[5];
    int blk_num;// ��¼��Сֵ�ڵڼ���
    int tuple_num; // ��¼�ڿ��еĵڼ���Ԫ��
    /* ����һ��buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    /* ��ȡ���̿鵽buffer�� */
    /* ����count��¼���Ѿ��ź����Ԫ��ֵ�����������Ѿ�����Ĳ��֣�����������ɸѡ��Сֵ */
    /* ѭ���˴ν�buf���� */
    for (i = 1; i <= 8; i++)
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
    }
    blk = blk-(BLOCKSIZE+1)*7;// ��blk����ָ���һ�����ݿ�Ŀ�ͷ
    /* ÿһ�ε�ѭ���������ҵ�һ����Сֵ�����ѭ��7*8�� */
    for (int a = 0; a < 56; a++)
    {
        min_X = 100;// ÿ��ѭ�����ÿ�ʼ����СֵΪ��ֵ
        for (i = count/7; i < 8; i++) // ѭ����8��
        {
            /* iΪcount/7ʱ��j����count%7,�����0��ʼ */
            for (j = (i == count/7)? count%7 : 0; j < 7; j++)// ÿһ�����ݿ��ǰ7�д����7��Ԫ��
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
                        min_X_str[k] = str[k];// �����ַ���
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
                    blk_num = i+1;// ��¼�ҵ�����Сֵ�ڵڼ���
                    tuple_num = j+1;// ��¼�ڵڼ���
                }
            }
        }
        count++; // һ��ѭ�����ҵ�һ����Сֵ
        printf("��%d����Сֵ����%d, %d��\n",count, min_X, min_Y);
        printf("��Сֵ�����ڵ�%d��ĵ�%d��\n",blk_num, tuple_num);
        /* �ҳ��ĵ�nС��Ԫ�飬����Ӧ���ڵ�˳λλ���ϵ���ֵ���� */
        /* �����һС��Ԫ�飬Ӧ�����һ���һ�е���ֵ���� */
        /* blk��������СֵӦ���ڵ�λ��,count=1ʱ���������1�� */
        /* �ҳ���(count/7)+1���count%7�е�Ԫ�� */
        /* Ŀǰ��blkָ��������һ�����ݿ�Ŀ�ͷ����һ�������ݿ�Ŀ�ͷΪ:blk-(BLOCKSIZE+1)*7 */
        for (int k = 0; k < 4; k++)
        {
            str_X[k] = *(blk + (count-1)/7*(BLOCKSIZE+1) + (count-1)%7*8 + k);//һ�����ݿ���7�����õ����ݣ����Զ�7ȡ��
        }
        X = atoi(str_X);
        for (int k = 0; k < 4; k++)
        {
            str_Y[k] = *(blk + (count-1)/7*(BLOCKSIZE+1) + (count-1)%7*8 + k + 4);
        }
        Y = atoi(str_Y);
        /* ����Ԫ����н��� */
        printf("��%d���%d��(%d, %d)\n", count/7+1, count%7, X, Y);
        printf("blk_one:%c%c \n",*(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8),
               *(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8+1));
        printf("blk_two:%c%c\n",*(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8),
               *(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8+1));

        write_block(blk+(count-1)/7*(BLOCKSIZE+1)+(count-1)%7*8, min_X_str, min_Y_str);
        write_block(blk+(blk_num-1)*(BLOCKSIZE+1)+(tuple_num-1)*8, str_X, str_Y);
        printf("swap finished\n");

    }
    /* ѭ��Ϊ8��blkĩβ���ϵ�ַ��Ϣ���˴εı�����111.blk~118.blk */
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
/* Ϊÿ��������һ��д����һ��ĵ�ַ */
/* �������׵�ַ */
int writeAddrinBlk(unsigned char *blk, int addr)
{
    char str[5];
    // +48�洢ΪASCII��
    str[0] = (addr/100) + 48;
    str[1] = (addr/10)%10 + 48;
    str[2] = addr%10 + 48;
    write_block(blk+8*7, str, "");
}

