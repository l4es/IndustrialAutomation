
struct st
{
    int  i;
    int i2;
    float f;
    double d;
    struct s1
    {
	struct s2
	{
	    struct s3
	    {
		int i3;
	    } s3;
	    int i2;
	} s2;
	union
	{
	    int a;
	    float b;
	} u;
	int i1;
    } s1;
};

struct st st;

st.i = 1;
st.f = 2.0;
st.d = 3.0;
st.s1.i1 = 4;
st.s1.s2.i2 = 5;
st.s1.s2.s3.i3 = 6;

printf ("struct st st =\n"
	"{\n"
	"   int i;      /* %d */\n"
	"   float f;    /* %f */\n"
	"   double d;   /* %g */\n"
	"   ...int i1;  /* %d */\n"
	"   ...int i2;  /* %d */\n"
	"   ...int i3;  /* %d */\n"
	"};\n", st.i, st.f, st.d,
	    st.s1.i1,
	    st.s1.s2.i2,
	    st.s1.s2.s3.i3);

