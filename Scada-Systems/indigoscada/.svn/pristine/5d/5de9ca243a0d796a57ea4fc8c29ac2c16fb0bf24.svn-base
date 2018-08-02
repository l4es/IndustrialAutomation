int f(void)
{   int i;
    for(i=0;i<3;++i)
        if(i>2)
            break;
    return i;
}
int g(void)
{   int k = 0,i = 2;
    while(i--)
        k += f();
    return k;
}
int main(void)
{   int i = 2;
    do {
        int k = g();
    } while(--i);
    return 0;
}
