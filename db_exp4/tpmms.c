#include "tpmms.h"

int tpmms()
{
    Buffer buf;// ����һ����������520�ֽڣ��ɴ��8����̿�
    unsigned char *blk; // һ��ָ����̿��ָ��
    int i = 0;
    /* ��ȡ����������Ҫ�ı��� */
    int X = -1;
    int Y = -1;
    int addr = -1;
    int count = 0;// ����������¼���ж���������������

    char str[5];
    int min_X = 100;// ��¼һ��ѭ�����ҵ�����Сֵ,��ʼ��Ϊ��ֵ�����ֵ�����ֵ
    int min_Y;// ��¼Yֵ
    int blk_num;// ��¼��Сֵ�ڵڼ���
    int tuple_num; // ��¼�ڿ��еĵڼ���Ԫ��
    /* ����һ��buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* �ڻ�������ȡ��һ���µ����ݿ�����д�� */
    blk_write = getNewBlockInBuffer(&buf);

    /* ��ȡ���̿鵽buffer�� */
    for (i = 1; i <= 8; i++) // ѭ����8��
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        for (int j = 0; j < 7; j++)// ÿһ�����ݿ��ǰ7�д����7��Ԫ��
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
                blk_num = i;// ��¼�ҵ�����Сֵ�ڵڼ���
                tuple_num = j;// ��¼�ڵڼ���
            }
        }
        count++; // һ��ѭ�����ҵ�һ����Сֵ
    }
    /* �ҳ��ĵ�nС��Ԫ�飬����Ӧ���ڵ�˳λλ���ϵ���ֵ���� */
    /* �����һС��Ԫ�飬Ӧ�����һ���һ�е���ֵ���� */
    /* blk��������СֵӦ���ڵ�λ��,count=1ʱ���������1�� */
    blk_one = readBlockFromDisk((count/7)+1, &buf);
    blk_two = readBlockFromDisk(blk_num, &buf);
    /* �ҳ���(count/7)+1���count%7�е�Ԫ�� */
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
