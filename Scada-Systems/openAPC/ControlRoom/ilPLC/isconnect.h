#ifndef ISCONNECT_H
#define ISCONNECT_H

#define MAX_IS_CONNECTIONS 2

struct is_data
{
	void            *handle;
	int              digiFBlock,numFBlock,charFBlock;
	struct var_entry fromIS;
	int              fromISIO;
	char             fromISDone;
};

extern struct is_data    isData[MAX_IS_CONNECTIONS];
extern struct is_data    isDatai[MAX_IS_CONNECTIONS];
extern struct is_data    isDatam[MAX_IS_CONNECTIONS];

extern void ispace_callback0(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS]);
extern void ispace_callback0i(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS]);
extern void ispace_callback0m(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS]);

#endif
