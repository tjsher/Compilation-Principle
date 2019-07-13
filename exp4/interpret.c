#include <stdio.h>

#include "pl0.h"
#include <string.h>

/* 解释执行时使用的栈 */
#define stacksize 500

int main()
{

    memcpy(fname,"fa.tmp",al);
    fin = fopen(fname, "r");
    char  content = '@';
    int i=0,j=0,k=0,n=0;
    char s[3]= {'@','@','@'};
    int flag = 0;
    printf("读取文件开始\n");
    while(content != EOF)
    {

        if(flag) // 读完一个单词
        {   
            flag = 0;
            switch(j)
            {
            case 1: //指令序号，跳过不要
                break;
            case 2: //指令类型
                code[k].f = transport(s);
                //printf("code[k].f is %d\n",code[k].f);
                break;
            case 3://左操作数
                code[k].l = charArraytoInt(s);
                //printf("code[k].l is %d\n",code[k].l);
                break;
            case 0 ://右操作数
                code[k-1].a = charArraytoInt(s);
                //printf("code[k].a is %d\n\n",code[k].a);
                s[0] = '@';
                s[1] = '@';
                s[2] = '@'; 
                break;

            }
        }
        content = fgetc(fin);

        if (content == '\n') //读完一条指令
        {
            i = 0;
            j = 0;
            n++;
            k++;
            flag = 1;

            continue;

        }
        if(content == ' ' )  // 读完一个单词
        {
            i = 0;
            j++;
            flag = 1;
            continue;
        }
        s[i] = content;
        i++;
    }
    fclose(fin);
    printf("读取文件结束\n");
    printf("n is %d\n",n );
    for(i=0;i<n;i++)
        printf("%d,%d,%d \n",code[i].f,code[i].l,code[i].a );
    interpret();
    return 0;
}

int transport(char* sym)
{
    int i = 0;
    char s[4];
    for(; i<3; i++)
    {
        s[i] = sym[i];
    }
    s[3] = '\0';
    if(!strcmp("lit",s))
    {

        return 0;
    }
    if(!strcmp("opr",s))
    {
        return 1;
    }
    if(!strcmp("lod",s))
    {
        return 2;
    }
    if(!strcmp("sto",s))
    {
        return 3;
    }
    if(!strcmp("cal",s))
    {
        return 4;
    }
    if(!strcmp("int",s))
    {
        return 5;
    }
    if(!strcmp("jmp",s))
    {
        return 6;
    }
    if(!strcmp("jpc",s))
    {
        return 7;
    }

}

int charArraytoInt(char *s)
{
    int i = 2;
    int j = 1;
    int res = 0;

    for(; i>=0; i--)
    {

        if(s[i] < '0' || s[i] > '9')
            continue;
        res += (s[i] - 48) * j;
        j = j * 10;
    }
    return res;
}

/*
* 解释程序
*/
void interpret()
{
    int p, b, t, w = 0; /* 指令指针，指令基址，栈顶指针 */
    struct instruction i;   /* 存放当前指令 */
    int s[stacksize];   /* 栈 */

    printf("start pl0\n\n");
    t = 0;
    b = 0;
    p = 0;
    s[0] = s[1] = s[2] = 0;
    do
    {

        i = code[p];    /* 读当前指令 */
        //printf("p is %d,i.f is %d,i.a is %d\n", p, i.f, i.a);
        p++;
        switch (i.f)
        {
        case lit:   /* 将a的值取到栈顶 */
            s[t] = i.a;
            t++;
            break;
        case opr:   /* 数学、逻辑运算 */
            // printf("opr,%d\n", i.a );
            switch (i.a)
            {
            case 0:
                t = b;
                p = s[t + 2];
                b = s[t + 1];
                // printf("case 0 ,p is %d,b is %d\n", p, b);

                break;
            case 1:
                s[t - 1] = -s[t - 1];
                break;
            case 2:
                t--;
                s[t - 1] = s[t - 1] + s[t];
                // printf("++++++++++%d\n", s[t - 1]);
                break;
            case 3:
                t--;
                s[t - 1] = s[t - 1] - s[t];
                break;
            case 4:
                t--;
                s[t - 1] = s[t - 1] * s[t];
                break;
            case 5:
                t--;
                s[t - 1] = s[t - 1] / s[t];
                break;
            case 6:
                s[t - 1] = s[t - 1] % 2;
                break;
            case 8:
                t--;
                s[t - 1] = (s[t - 1] == s[t]);
                break;
            case 9:
                t--;
                s[t - 1] = (s[t - 1] != s[t]);
                break;
            case 10:
                t--;
                s[t - 1] = (s[t - 1] < s[t]);
                break;
            case 11:
                t--;
                s[t - 1] = (s[t - 1] >= s[t]);
                break;
            case 12:
                t--;
                s[t - 1] = (s[t - 1] > s[t]);
                break;
            case 13:
                t--;
                s[t - 1] = (s[t - 1] <= s[t]);
                break;
            case 14:
                printf("%d", s[t - 1]);
                fprintf(fa2, "%d", s[t - 1]);
                t--;
                break;
            case 15:
                printf("\n");
                fprintf(fa2, "\n");
                break;
            case 16:
                printf("?");
                fprintf(fa2, "?");
                scanf("%d", &(s[t]));
                fprintf(fa2, "%d\n", s[t]);
                t++;
                break;
            case 17:
                s[t] = base(i.l, s, b); // 将父过程基地址入栈
                s[t + 1] = b; // 将本过程基地址入栈，此两项用于base函数
                s[t + 2] = p; //将当前指令指针入栈
                for( w = 0; w < i.l; w++)
                {
                    s[t + 3 + w] = s[t - i.l + w];
                }
                break;
            case 18:
                t--;
                break;
            }
            break;
        case lod:   /* 取相对当前过程的数据基地址为a的内存的值到栈顶 */
            // printf("lod ------\n");
            s[t] = s[base(i.l, s, b) + i.a];
            t++;
            break;
        case sto:   /* 栈顶的值存到相对当前过程的数据基地址为a的内存 */
            t--;
            s[base(i.l, s, b) + i.a] = s[t];
            break;

        case cal:   /* 调用子过程 */
            s[t] = base(i.l, s, b); // 将父过程基地址入栈
            s[t + 1] = b; // 将本过程基地址入栈，此两项用于base函数
            s[t + 2] = p; //将当前指令指针入栈
            b = t;  // 改变基地址指针值为新过程的基地址
            p = i.a;   //跳转
            break;
        case inte:  /* 分配内存 */
            t += i.a;
            break;
        case jmp:   /* 直接跳转 */
            p = i.a;
            break;
        case jpc:   /* 条件跳转 */
            t--;
            if (s[t] == 0)
            {
                p = i.a;
            }
            break;
        }
    }
    while (p != 0);
}

/* 通过过程基址求上l层过程的基址 */
int base(int l, int *s, int b)
{
    int b1;
    b1 = b;
    while (l > 0)
    {
        b1 = s[b1];
        l--;
    }
    return b1;
}
