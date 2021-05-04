#include "main.h"
int menu_select()
{
    char s;
    int cn;
    printf("1.基于线性搜索的关系选择\n");
    printf("2.两阶段多路归并排序\n");
    printf("3.基于索引的关系选择\n");
    printf("4.基于排序的连接操作\n");
    printf("5.并集合操作\n");
    printf("6.交集合操作\n");
    printf("7.差集合操作\n");
    printf("请输入你的选择：");
    do{
        s = getchar();
        cn = (int)s-48;
    }while(cn<0 || cn>7);
    return cn;
}

int read_choice()
{
    char s;
    for(;;)
    {
        switch(menu_select())
        {
            case 1:
                linear_search();
                break;
            case 2:
                printf("2.两阶段多路归并排序\n");
                tpmms();
                break;
            case 3:
                printf("3.基于索引的关系选择\n");
                index_search();
                break;
            case 4:
                printf("4.基于排序的连接操作\n");
                sort_merge_join();
                break;
            case 5:
                printf("5.并集合操作\n");
                break;
            case 6:
                printf("6.交集合操作\n");
                break;
            case 7:
                printf("7.差集合操作\n");
                break;
            default:
                break;
        }
    }
    return 0;

}
int main()
{
    read_choice();//获取用户的选择
    return 0;
}
