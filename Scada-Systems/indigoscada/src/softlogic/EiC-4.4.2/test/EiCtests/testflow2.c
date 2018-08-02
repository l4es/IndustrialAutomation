#if defined(debug) && defined(EiCTeStS)
#define show(x);  :show x
:listcode
#else
#define show(x);  
#endif

int f0(int a)
{
    return 0;
}
show(f0);

int f1(int a)
{
    if(a>2) {
	return 2;
	a = 5;       /* unreachable code */
   } else
	return 4;
}
show(f1);

int f2(int a)
{
    if(a>2) 
	return 2;
    else {
	return 4;
	a = 5;       /* unreachable code */
    }
}
show(f2);

int f3(int a)
{
    if(a>2) 
	return 2;
    else
	return 4;
    a = 5;       /* unreachable code */
}
show(f3);

int f4(int a)
{
    while(a>0)
	a--;
    return 5;
    a = 6;         /* unreachable code */
}                  
show(f4);

int f5(int a)
{
    switch(a) {
      case 1: return 1;
      case 2: return 2;
      case 3: return 3;
      default:
	return 100;
    }
}
show(f5);

int f6(int a)
{
    switch(a) {
      case 1: return 1;
      case 2: return 2;
      case 3: return 3;
    }
} /* control reaches end of non-void function*/
show(f6);

int f7(int a)
{
    switch(a) {
      case 1: return 1;
      case 2: break;
      case 3: return 3;
      default:
	return 100;
    }
} /* control reaches end of non-void function*/
show(f7);

int f8(int a)
{
    switch(a) {
      case 1: return 1;
	a = 6;           /* unreachable code */
      case 2: return 2;
      case 3: return 3;
      default:
	return 100;
    }
}
show(f8);

int f10(int a)
{
    if(a < 4)
	return 2;
    else if(a < 6) 
	return 5;
} /* flow reaches end of non-void function */
show(f10);


int f11(int a)
{
    int i=0;
    do
	a--;
    while(a>i);
    return a;
}
show(f11);

int f12(int a)
{
    int i=0;
    do
	a--;
    while(a>i);
} /* flow reaches end of non-void function */
show(f12);

int f13(int a)
{
    int i;
    for(i=0;i<a;i++) {
	a--;
	if(a == i)
	    return a;
    }
    return -1;
}
show(f13);

int f14(int a)
{
    int i;
    for(i=0;i<a;i++) {
	a--;
	if(a == i)
	    return a;
	else
	    return 2;
    }
    return -1;           /* unreachable code */
}
show(f14);

int f15(int a)
{
    int i;
    for(i=0;i<a;i++) {
	a--;
	if(a == i)
	    return a;
    }
} /* flow reaches end of non-void function */
show(f15);


void f16(int a)
{
    for(;;)
	if(a == 5)
	    return ;
	else
	    break;
    return ;
}
show(f16);


void f17(int a)
{
    int b[5];
    for(;;)
	if(a == 5)
	    return ;
	else
	    break;
    return ;
}
show(f17);


int main()
{
/* empty function */
    return 0;
}

#ifdef EiCTeStS
main();
#endif





