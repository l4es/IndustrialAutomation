#include "plcemu.h"
#include "plclib.h"
#include "parser.h"

extern const char ErrMsg[][MED_STR];

main()
{
	int r = 0;
	unsigned char buf[MED_STR];

	struct instruction op;

	for (;;)
	{
		memset(&op, 0, sizeof(struct instruction));
		memset(buf, 0, MED_STR);
		printf("next line:");
		//scanf("%s",buf);
		fgets(buf, MED_STR, stdin);
		r = parse_il_line(buf, &op);
		if (r < 0)
			printf("%s\n", ErrMsg[-1 - r]);

//	printf("label:%ld\noperation:%d\nmodifier:%d\noperand:\nindex:%d\nbit:%d\n",op.label, op.operation, op.modifier, op.operand, op.byte, op.bit);
	}

}
