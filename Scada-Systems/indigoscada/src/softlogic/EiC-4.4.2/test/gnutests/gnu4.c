int bar(int i)
{}

struct s {int x[5];};

int main()
{
        struct s {struct s *next; int i;};
	static struct s sa[2];
 
        /* bar(sa[0].next->x[4]); */
	printf("Test passed (if it compiled)\n");
	return 0;
        bar(sa[0].next->i);
}

#ifdef EiCTeStS
main();
#endif
