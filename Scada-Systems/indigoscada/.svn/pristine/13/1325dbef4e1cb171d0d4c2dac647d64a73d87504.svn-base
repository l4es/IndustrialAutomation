#ifndef STABH_
#define STABH_

typedef struct {
    char ** strs;
    int n;
}stab_t;


/** PROTOTYPES from stab.c **/
char * EiC_stab_SaveString(stab_t *stab, char *s);
size_t EiC_stab_NextEntryNum(stab_t *stab);
void EiC_stab_CleanUp(stab_t *stab, size_t bot);
void EiC_stab_Mark(stab_t *stab, char mark);
void EiC_stab_ShowStrings(stab_t *stab);
int EiC_stab_FindString(stab_t *stab, char *s);
int EiC_stab_RemoveString(stab_t *stab, char *s);

#endif
