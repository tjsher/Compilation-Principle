/*
翻译模式
start->   DS.
D->B; D
D->ε
B->int L  { L.type := int }
        |real L  { L.type := real }
            |bool L {L.type := bool}
L->id   { A.Type := L.type  enter(v.entry,L.type)}  A
A-> ，idA   { A1.Type := A.type  enter(v.entry,A.type)}
A->ε

S→ V := E1 { gen( ":=", E.place,0,V.place) } H
H→；S | ε
E1->EQ
Q-> >E|<E|E
E->T    { R.i:=T.place}      R     {E.place:=R.s}
R->+T    { R1.i:= newtemp; gen( "+", R.i, T.place , R1.i) }     R1   {R.s:= R1.s; }
R->-T    { R1.i:= newtemp; gen( "-", R.i, T.place , R1.i) }     R1   {R.s:= R1.s; }
R-> ε    {Rs=R.i}
T->F{}      P{}
P->*F{}      P1{}
P->/F{}      P1{}
P->ε   {Ps=P.i}
F->( E )  { F.place := E.place}
F->id     {F.place:=position (id)}
V->id     {V.place:=position (id)}
*/

#include "mean.h"

int main()
{
    char* filename = "t6.txt";

    if((fin=fopen(filename,"r"))==NULL)
    {
        printf("不能打开文件.\n");
        exit(0);
    }

    init();                  //初始化
    getsym();                //读第一个单词，将单词类别放入sym中，单字词值放入id中
    start();                //开始按start->DS.  分析

    if (sym==eeof)
    {
        if(check() == 1)
        {
            printf("语法正确\n\n");
            filename = "res.txt";
            if(strlen(filename)<=0) return 0;
            if((fout=fopen(filename,"w"))==NULL)
            {
                printf("不能打开文件.\n");
                exit(0);
            }
            for (int cx1=0; cx1<cx; cx1++)
                fprintf(fout,"(%s,%s,%s,%s)\n",code[cx1].f,code[cx1].l,code[cx1].r,code[cx1].t);
            return 0;
        }
        else
        {
            printf("语义错误，操作符或赋值语句两边不是同种类型的变量\n\n");
        }
    }
    else
    {
        printf("语法错1:  . 后不能再有句子");
        exit(0);
    }
    fclose(fin);
    fclose(fout);
}

int check()
{
    printf("check---------------\n");
    int i = 0;

    enum symbol temp1,temp2;
    for (i = 0 ; i < cx ; i++)
    {
        printf("code[%d].f is %s, mark is %d\n",i,code[i].f,mark);
        if(mark == 0)
            return 2;

        if(code[i].l[0] >= 'a' &&code[i].l[0] <= 'z')
            {
                printf("temp1 is in a-z\n");
                temp1 = table[position(code[i].l)].type;
            }
        else temp1 = find(code[i].l);
        printf("temp1 = %d\n",temp1);
        if(code[i].r[0] >= 'a' &&code[i].r[0] <= 'z')
        {
            temp2 = table[position(code[i].r)].type;
            printf("temp2 is in a-z\n");
        }
        else temp2 = find(code[i].r);
        printf("temp2 = %d\n",temp2);

        if(temp2 >10000)
            temp2 = temp1;
        if(temp1 >10000)
            temp1 = temp2;
        printf("after judgement temp1 = %d ,temp2 = %d\n",temp1,temp2);
        if(temp1 != temp2)
            return 2;

    }
    return 1;
}

enum symbol find(char *temp)
{
    if(mark == 0)
        return nul;
    int i = 0;
    enum symbol temp1,temp2;
    for(; i<cx; i++)
    {

        if(strcmp(temp,code[i].t) == 0)
        {
            if(code[i].l[0] == 'T')
                temp1 = find(code[i].l);
            else
                temp1 = table[position(code[i].l)].type;

            if(code[i].r[0] == 'T')
                temp2 = find(code[i].r);
            else
                temp2 = table[position(code[i].r)].type;
        }
    }
    if(temp2 >10000)
            temp2 = temp1;
        if(temp1 >10000)
            temp1 = temp2;

    if(temp1 == temp2)
        return temp1;
    else mark = 0;

}

//初始化函数
void init()
{
    int i;
    mark = 1;
    /* 设置单字符符号 */
    for (i=0; i<=255; i++)
    {
        ssym[i] = nul;            //不正确的单字符符号为nul，先预置初值nul
    }
    ssym['+'] = plus;
    ssym['-'] = sub;
    ssym['*'] = times;
    ssym['/'] = divide;
    ssym['('] = lparen;
    ssym[')'] = rparen;
    ssym['.'] = period;
    ssym[','] = comma;
    ssym[';'] = semicolon;
    ssym['<'] = lessthan;
    ssym['>'] = morethan;
    /* 设置保留字名字 */
    strcpy(&(word[0][0]), "real");
    strcpy(&(word[1][0]), "int");
    strcpy(&(word[2][0]), "bool");
    /* 设置保留字符号 */
    wsym[0] = realsym;
    wsym[1] = intsym;
    wsym[2] = boolsym;
    tv=100;           //临时变量指针初值，让Tx和tv的取值没有交集，区别到底是临时变变量和声明的变量
    tx=0;           //表指针初值
    cx=0;     //指令计数器

}


/*
* 词法分析，获取一个符号
*/
int getsym()
{
    int i,k;
    ch=fgetc(fin);
    printf("getsym gets char %c\n",ch);
    if (ch==EOF)
    {
        sym=eeof;    //文件结束
        return 0;
    }

    while (ch==' ' || ch==10 || ch==13 || ch==9)  /* 忽略空格、换行、回车和TAB */
        ch=fgetc(fin);

    if (ch>='a' && ch<='z')
    {
        /* 名字或保留字以a..z开头 ，*/
        k = 0;
        do
        {
            if(k<al)                  /* 符号的最大长度为al ，超长就截尾处理*/
            {
                a[k] = ch;
                k++;
            }
            ch=fgetc(fin);
        }
        while ((ch>='a' && ch<='z') || (ch>='0' && ch<='9'));

        a[k] = 0;
        strcpy(id, a);
        fseek(fin,-1,1);

        for (i=0; i<norw; i++)      /* 搜索当前符号是否为保留字 */
            if (strcmp(id,word[i]) == 0)
                break;
        if (i <norw)
        {
            sym = wsym[i];
        }
        else
        {
            sym = ident;          /* 搜索失败则，类型是标识符 */
        }
    }
    else if(ch == ':') /* 检测赋值符号 */
    {
        ch=fgetc(fin);
        if (ch == '=')
        {
            sym = becomes;
        }
        else
        {
            sym = nul;  /* 不能识别的符号 */
        }
    }
    else sym = ssym[ch];     /* 当符号不满足上述条件时，全部按照单字符符号处理 */
    return 0;
}


/*
* 在符号表中加入一项
*/

void enter(enum symbol type)
{
    tx=tx+1;
    int i = 0;
    if (tx > txmax)
    {
        printf(" 符号表越界 ");           /* 符号表越界 */
        return;
    }
    for(i=0; i<tx ; i++)
    {

        if(strcmp(table[i].name, id) == 0)
        {
            printf("重复的标识符");
            exit(0);
        }
    }
    strcpy(table[tx].name, id); /* 全局变量id中已存有当前名字的名字,Tx为插入当前符号之前表尾指针 */
    table[tx].type = type;

}



/*
* 查找名字的位置.
* 找到则返回在名字表中的位置,否则返回0.
*
* idt:    要查找的名字
* tx:     当前名字表尾指针，全局变量
*/
int position(char* idt)
{
    int i;
    strcpy(table[0].name, idt);
    i = tx;
    while (strcmp(table[i].name, idt) != 0)
    {
        i--;
    }
    return i;
}


/* 中间代码*/
int gen(enum symbol op, int arg1, int arg2,int result )
{
    char temp1[al+1],temp2[al+1],temp3[al+1];
    if(arg1>=100)                            //模拟申请临时变量
    {
        wsprintf(temp1,"T%d",arg1);
    }
    else
    {
        strcpy(temp1, table[arg1].name);
    }

    if(arg2>=100)
    {
        wsprintf(temp2,"T%d",arg2);
    }
    else
    {
        strcpy(temp2, table[arg2].name);
    }

    if(result>=100)
    {
        wsprintf(temp3,"T%d",result);
    }
    else
    {
        strcpy(temp3,table[result].name);
    }

    if (op==becomes)
    {

        printf("(:=,%s,%s,%s)\n",temp1,temp2,temp3);
        writecode(":=",temp1,temp2,temp3);
    }
    else if (op==plus)                          //+运算
    {

        writecode("+",temp1,temp2,temp3);
        printf("(+,%s,%s,%s)\n",temp1,temp2,temp3);
    }
    else if (op==sub)                          //-运算
    {

        writecode("-",temp1,temp2,temp3);
        printf("(-,%s,%s,%s)\n",temp1,temp2,temp3);
    }
    else if (op==times)                          //*运算
    {

        writecode("*",temp1,temp2,temp3);
        printf("(*,%s,%s,%s)\n",temp1,temp2,temp3);
    }
    else if (op==divide)                          // /运算
    {

        writecode("/",temp1,temp2,temp3);
        printf("(/,%s,%s,%s)\n",temp1,temp2,temp3);
    }
    else if (op==morethan)                          // >运算
    {

        writecode(">",temp1,temp2,temp3);
        printf("(>,%s,%s,%s)\n",temp1,temp2,temp3);
    }
    else if (op==lessthan)                          // <运算
    {

        writecode("<",temp1,temp2,temp3);
        printf("(<,%s,%s,%s)\n",temp1,temp2,temp3);
    }

    return 0;
}

//处理代码段
void writecode(char op[al+1], char arg1[al+1], char arg2[al+1],char result[al+1] )
{

    if (cx >= cxmax)
    {
        printf("Program too long"); /* 程序过长 */
        return ;
    }
    strcpy(code[cx].f, op);
    strcpy(code[cx].l,arg1);
    strcpy(code[cx].r,arg2);
    strcpy(code[cx].t,result);

    cx++;
    return ;
}


/*分析产生式     P->DS.   */

void  start()
{
    printf("start---------\n");
    if (sym==intsym ||sym==realsym||sym==boolsym)
    {
        D();
        S();
        if (sym==period)
        {
            getsym();
            return;
        }
        else
        {
            printf("语法错2： 缺少程序结束.");
            exit(0);
        }
    }
    else
    {
        printf("语法错3: 程序只能用int,和real开始，而且区分大小写");
        exit(0);
    }
}

/*递归下降分析
D->  B; D
D->ε

*/
void  D()
{
    printf("D-------------\n");

    if (sym==intsym ||sym==realsym||sym == boolsym)
    {
        B();
        if (ch=';')
        {
            getsym();
            D();
        }
        else
        {
            printf("语法错D1");
            exit(0);
        }
    }
    else if(sym==ident || sym==period)  return;

    else
    {
        printf("语法错D2");
        exit(0);
    }
}

/*
B->   int L    { L.type := int }|real L    { L.type := real }
*/
void  B()
{
    printf("B-------------\n");

    if (sym==intsym )
    {
        getsym();
        L(intsym);
    }
    else if (sym==realsym)
    {
        getsym();
        L(realsym);
    }
    else if (sym==boolsym)
    {
        getsym();
        L(boolsym);
    }
    else
    {
        printf("语法错B");
        exit(0);
    }
}


/*
L->   id   { A.Type := L.type  enter(v.entry,L.type)}   A       V.entry通过全局变量tx隐性传递
*/
void L(enum symbol type)
{
    printf("L-------------\n");

    if (sym==ident)
    {

        enter(type);
        getsym();
        A(type);
    }
    else
    {
        printf("语法错L");
        exit(0);
    }
}


/*
A-> ，id  A   { A1.Type := A.type  enter(v.entry,A.type)}
A->ε
*/

void A(enum symbol type)
{
    printf("A-------------\n");

    if (sym==comma)          //当前单词为，
    {
        getsym();
        if (sym==ident)
        {
            enter(type);
            getsym();
            A(type);
        }
        else
        {
            printf("语法错A1");
            exit(0);
        }
    }
    else if (sym== semicolon)   return ;//当前单词为；即A的follow集元素，相当于进行A->ε
    else
    {
        printf("语法错A2");
        exit(0);
    }
}



/*
S→ V := E { gen( ":=", E.place,0,V.place) } H
*/
void S()
{
    printf("S-------------,sym is %d\n",sym);

    int vplace,Eplace;
    if (sym==ident)
    {
        vplace=V();
        //getsym();
        if (sym==becomes)     //当前单词为:=
        {
            getsym();
            Eplace=E1();
            printf("S------------Eplace = %d\n",Eplace);
            gen(becomes,Eplace,-1,vplace);
            H();
        }
        else
        {
            printf("语法错S1");
            exit(0);
        }
    }
    else
    {
        printf("语法错S2");
        exit(0);
    }
}

/*
H→；S | ε
*/
void H()
{
    printf("H-------------\n");

    if (sym==semicolon)          //当前单词为indent类型
    {
        getsym();
        S();
    }
    else if (sym==period)   return ;
    else
    {
        printf("语法错H");
        exit(0);
    }
}

int E1()
{
    printf("E1-------------,sym is %d\n",sym);
    int Eplace;
    if(sym == ident)
    {
        Eplace = E();
        printf("E1------------Eplace = %d\n",Eplace);

        return Q(Eplace);
    }
    else
    {
        printf("语法错E1");
        exit(0);
    }
    return Eplace;
}

/*
E->T      { R.i:=T.place}      R     {E.place:=R.s}
*/

int Q(int Eplace)
{
    printf("Q-------------,sym is %d\n",sym);
    int r;
    if(sym == morethan)
    {
        getsym();
        r=E();
        printf("Q------------Eplace = %d\n",Eplace);
        printf("Q------------r = %d\n",r);

        tv ++;
        gen(morethan,Eplace,r,tv);

    }
    else if (sym == lessthan)
    {
        getsym();
        r=E();
        tv ++;

        gen(lessthan,Eplace,r,tv);

    }
    return tv;

}

int E()
{
    printf("E-------------,sym is %d\n",sym);

    int ri,tplace,Rs;
    if (sym==ident || sym== lparen)
    {
        tplace=T();
        printf("E------------tplace = %d\n",tplace);
        ri=tplace;
        Rs=R(ri);
        printf("E------------Rs = %d\n",Rs);

    }
    else
    {
        printf("语法错E");
        exit(0);
    }
    return Rs;
}

/*
R->+T     { R1.i:= newtemp; gen( "+", R.i, T.place , R1.i) }     R1   {R.s:= R1.s; }
@R->-T     { R1.i:= newtemp; gen( "-", R.i, T.place , R1.i) }     R1   {R.s:= R1.s; }

R-> ε    {R.s=R.i}
*/

int R(int Ri)
{
    printf("R-------------,sym is %d\n",sym);

    int Rs,tplace;
    if (sym==plus)
    {
        getsym();
        tplace=T();
        printf("R------------tplace = %d\n",tplace);

        tv=tv+1;            //生成临时变量
        Rs=R(tv);
        printf("R------------Rs = %d\n",Rs);

        gen(plus,Ri,tplace,tv);

    }
    else if (sym==sub)//@
    {
        getsym();
        tplace=T();
        printf("R------------tplace = %d\n",tplace);

        tv=tv+1;            //生成临时变量
        Rs=R(tv);
        printf("R------------Rs = %d\n",Rs);
        gen(sub,Ri,tplace,tv);

    }
    else if (sym== semicolon || sym==rparen|| sym==period
             || sym == lessthan || sym == morethan)
    {
        Rs=Ri;
    }
    else
    {
        printf("语法错R");
        exit(0);
    }
    return Rs;
}

int T()
{
    printf("T-------------,sym is %d\n",sym);

    int fplace,Pi,Ts;
    if(sym == ident)
    {
        fplace = F();
        Pi = fplace;
        printf("T------------fplace = %d\n",fplace);

        Ts = P(Pi);
        printf("T------------Ts = %d\n",Ts);

    }
    else
    {
        printf("语法错T");
        exit(0);
    }
    return Ts;

}


int P(int Pi)
{
    printf("P-------------,sym is %d\n",sym);
    int Ps,fplace;
    if(sym == plus || sym == sub || sym == lessthan || sym == morethan)
    {
        return Pi;

    }
    else if (sym==times)
    {
        getsym();
        fplace=F();
        printf("P------------fplace = %d\n",fplace);

        tv=tv+1;            //生成临时变量
        gen(times,Pi,fplace,tv);
        Ps=P(tv);
        printf("P------------Ps = %d\n",Ps);



    }
    else if (sym==divide)
    {
        getsym();
        fplace=F();
        printf("P------------fplace = %d\n",fplace);

        tv=tv+1;            //生成临时变量
        gen(divide,Pi,fplace,tv);
        Ps=P(tv);
        printf("P------------Ps = %d\n",Ps);


    }
    else if (sym== semicolon || sym==rparen|| sym==period)
    {
        Ps=Pi;
        printf("P------------Ps = %d\n",Ps);

    }
    else
    {
        printf("语法错P");
        exit(0);
    }
    return Ps;
}
/*

F->( E )  { F.place := E.place}
F->id     {F.place:=position (id)}
*/
int F()
{
    printf("F-------------,sym is %d\n",sym);

    int Fplace;
    if (sym==ident)
    {
        Fplace=position (id);
        if (Fplace==0)
        {
            printf("变量没有声明");
            exit(0);
        }
        if(table[Fplace].type == boolsym)
        {
            printf("bool型变量不能进行代数或关系运算\n");
            exit(0);
        }

        printf("F------------Fplace = %d\n",Fplace);

        getsym();
    }
    else if (sym== lparen)
    {
        getsym();
        Fplace=E();
        if (sym== rparen)
            getsym();
        else
        {
            printf("语法错，缺)");
            exit(0);
        }
    }
    else
    {
        printf("语法错,缺(");
        exit(0);
    }
    return Fplace;
}



/*

V->id     {V.place:=position(id)}
*/
int V()
{
    printf("V-------------,sym is %d\n",sym);

    int Vplace;
    if (sym==ident)
    {
        Vplace=position (id);
        if (Vplace==0)
        {
            printf("变量没有声明");
            exit(0);
        }
        getsym();
    }
    else
    {
        printf("语法错V");
        exit(0);
    }
    return Vplace;
}
