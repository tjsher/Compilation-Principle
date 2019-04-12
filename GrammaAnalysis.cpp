
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum symbol
{
    nul=0,         indent,         plus,      times,	lparen,
    rparen,    divide,     sub,
};

int sym;    //当前的符号
char ch;
int lmark = 0,rmark = 0;
FILE* fin;


void getsym();
void S();
void ED();
void D();
void E();
void TR();
void T();
void R();
void plusTR();
void minusTR();
void FP();
void F();
void P();
void timesFP();
void divFP();
void lrE();
int V(); //变量判定

int main()
{
    char *filename = "test4.txt";


    if((fin=fopen(filename,"r"))==NULL)
    {
        printf("can't open file.\n");
        return 0;
    }
    S();                //开始按S->ED  分析

    if(lmark != rmark)
    {
      printf("bracket don't match,error\n");
      return 0;
    }

        if (sym==nul)
        {
            printf("grammer correct\n");
        }
    else
    {
        printf("grammer error\n");
    }
    fclose(fin);

    return 0;
}


void E()
{
    printf("now is in E\n");
    printf("E->TR\n");
    TR();
}

void TR()
{
    T();
    R();
}

void T()
{
  printf("now is in T\n");
    printf("T->FP\n");
    FP();
}


void FP()
{

    F();
    P();
}

void F()
{
  printf("now is in F\n");

    if(ch =='+'||ch=='*'||ch=='/'||ch=='-')
      {
        printf("current operator is beside another operator,error\n");
        exit(0);
      }
    if(ch == '<' || ch =='>')
    {
      printf("too many relational operator,error\n");
      exit(0);
    }
    if(V())
    {
        printf("F->V\n");
    }
    if(sym == lparen || sym == rparen)  //处理括号E

    {
        printf("F->(E)\n");
        lrE();

    }
}

void lrE()
{
  printf("now is in lrE\n");

    if(sym == lparen)
    {
        getsym();
        E();
    }
    else if( sym == rparen)
    {
        getsym();
    }
}

void P()
{
  printf("now is in P\n");

    timesFP();
    divFP();

}

void timesFP()
{
    if(ch == '*')
    {
        printf("* is operator\n");
        getsym();
        F();
        P();
    }
}

void divFP()
{
    if(ch == '/')
    {
        printf("/ is operator\n");
        getsym();
        F();
        P();
    }
}

void R()
{
  printf("now is in R\n");

    plusTR();
    minusTR();
}

void plusTR()
{
    if(ch == '+')
    {
        printf("+ is operator\n");

        getsym();
        T();
        R();
    }
}

void minusTR()
{
    if(ch == '-')
    {
        printf("- is operator\n");

        getsym();
        T();
        R();
    }
}

void ED()
{

    printf("S->ED\n");
    E();
    D();

}
void D()
{
  printf("now is in D\n");

    if( ch == '<' || ch == '>')
    {
        printf("relational operator,%c\n",ch);
        getsym();
        E();
    }
    else
        printf("no relational operator\n");
}

void S()
{
  printf("now is in S\n");

    getsym();
    if(sym == indent || sym == lparen)
        ED();
    else
        printf("begin with a ooperator,error\n");
}

void getsym()
{
    ch=fgetc(fin);

    while (ch==' ' || ch==10 || ch==13 || ch==9)  /* 忽略空格、换行、回车和TAB */
        ch=fgetc(fin);

    if (ch==EOF)  sym=nul;

    else if ((ch>='a' && ch<='z') ||(ch>='A' && ch<='Z'))
        sym = indent;
    else if(ch == '+')
        sym = plus;
    else if (ch == '*')
        sym = times;
    else if (ch == '/')
        sym = divide;
    else if (ch == '-')
        sym = sub;
    else if (ch == '(')
    {
      sym = lparen;
      lmark += 1;

    }
    else if (ch == ')')
    {
      sym = rparen;
      rmark += 1;
    }

}


/*
V->a|...|z
*/
int V()
{
    if (sym==indent)          //当前单词为indent类型
    {
        printf("%c is variable\n",ch);
        getsym();
        return 1;
    }
    return 0;
}
