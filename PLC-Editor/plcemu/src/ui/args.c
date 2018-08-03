#include <stdio.h>  
#include <string.h>  
#define MAX	80  
#define PMAX	128  
#define ARGS	48  
#define YES	 1  
#define NO	 0  
#define CANORM	 0  
#define SEPAR	"()[]=,<>|&#^%"

char arg_s[ARGS][MAX];
int arg_n;

void rmtb(char *s)
{
	int l;

	if (s == NULL )
		return;
	l = strlen(s) - 1;

	while ((l > 0) && (s[l] == 32 || s[l] == 9))
		--l;
	s[l] = '\0';
	return;
}

char *rmb(char *s)
{

	if (s == NULL )
		return (NULL );
	while (*s == 32 || *s == 9)
		++s;
	return (s);
}

void rmnl(char *s)
{
	char *e;

	if ((e = strchr(s, 10)) != NULL )
		*e = '\0';
}

int caps(char *s)
{
	for (; *s != '\0'; ++s)
		if (*s >= 'a' && *s <= 'z')
			*s += 'A' - 'a';
}

int smalls(char *s)
{
	for (; *s != '\0'; ++s)
		if (*s >= 'A' && *s <= 'Z')
			*s += 'a' - 'A';
}

int beep()
{
	printf("%c", 7);
}

int q_move(char *t, char *s, int n)
{
	int i;

	for (i = 0; i < n; ++i, ++t, ++s)
		*t = *s;
}

int q_copy(char *t, char *s, int n)
{
	q_move(t, s, n);
	t[n] = '\0';
}

int q_len(char *s)
{
	int i;

	if (strlen(s) == 0)
		return (0);
	if (strchr(SEPAR, *s) != NULL )
		return (1);
	if (*s == 34){
		for(i = 1, ++s; i < MAX 
		   && *s != '\0' 
		   && *s != 34; ++s)
			++i;
		return (i + 1);
	}
	for (i = 0; i < MAX 
	&& *s != '\0' 
	&& *s != 32 
	&& *s != 9; ++i, ++s)
		if (strchr(SEPAR, *s) != NULL )
			return (i);
	return (i);
}

int args(char *s)
{
	int l;
	arg_n = 0;
	do{
		s = rmb(s);
		l = q_len(s);
		if (l > 0){
			q_copy(arg_s[arg_n++], s, l);
			s += l;
		}
	} while (l > 0 && arg_n < ARGS);
	if (l > 0){
		fprintf(stderr, "* Too many arguments. Command discarded *\n");
		arg_n = 0;
	}
	return (arg_n);
}

int inv_args(char *s)
{
	int i;

	s[0] = '\0';
	for (i = 0; i < arg_n; ++i){
		strcat(s, arg_s[i]);
		strcat(s, " ");
	}
	return (arg_n);
}

void rmquotes(char *s)
{
	char c[MAX];
	char *p;

	if (*s != 34)
		return;
	p = s;
	++p;
	strcpy(c, p);
	p = c;
	while (*p != '\0'){
		if (*p == 34)
			*p = '\0';
		else
			++p;
	}
	strcpy(s, c);

}

