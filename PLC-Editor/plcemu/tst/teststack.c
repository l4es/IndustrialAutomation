//#include "plcemu.h"
#include "plclib.h"
#include "parser.h"
main()
{
	unsigned char buf[2560];

	memset(buf, 0, 2560);
	int i, val = 0;
	for (i = 0; i < 10; i++)
	{
		printf("enter a value:\n");
		scanf("%d", &val);
		if (i < 9)
		{
			if (val > 0)
				push(BOOL + OR, TRUE);
			else
				push(BOOL + OR, FALSE);
		}

	}
	if (val > 0)
		strcat(buf, "TRUE ");
	else
		strcat(buf, "FALSE ");

	while (Stack != NULL)
	{
		if (Stack->operand > 0)
			strcat(buf, "OR TRUE ");
		else
			strcat(buf, "OR FALSE ");
		val = pop(val);
	}
	if (val > 0)
		strcat(buf, " = TRUE ");
	else
		strcat(buf, " = FALSE ");

	printf("%s\n", buf);

}
