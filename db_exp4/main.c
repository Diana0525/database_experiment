#include "main.h"
int menu_select()
{
    char s;
    int cn;
    printf("1.�������������Ĺ�ϵѡ��\n");
    printf("2.���׶ζ�·�鲢����\n");
    printf("3.���������Ĺ�ϵѡ��\n");
    printf("4.������������Ӳ���\n");
    printf("5.�����ϲ���\n");
    printf("6.�����ϲ���\n");
    printf("7.��ϲ���\n");
    printf("���������ѡ��");
    do{
        s = getchar();
        cn = (int)s-48;
    }while(cn<0 || cn>7);
    return cn;
}

int read_choice()
{
    int cn = 0;
    char s;
    for(;;)
    {
        switch(menu_select())
        {
            case 1:
                cn=linear_search();
                break;
            case 2:
                printf("2.���׶ζ�·�鲢����\n");
                break;
            case 3:
                printf("3.���������Ĺ�ϵѡ��\n");
                break;
            case 4:
                printf("4.������������Ӳ���\n");
                break;
            case 5:
                printf("5.�����ϲ���\n");
                break;
            case 6:
                printf("6.�����ϲ���\n");
                break;
            case 7:
                printf("7.��ϲ���\n");
                break;
            default:
                break;
        }
    }
    return 0;

}
int main()
{
    read_choice();//��ȡ�û���ѡ��
    return 0;
}
