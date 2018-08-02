/* Causes EiC to crash, not any longer it doesn't
   */

typedef struct
{
  char *key;
  char *value;
} T1;

typedef struct
{
  long type;
  char *value;
} T3;

T1 a[] =
{    
  {
    "",
    ((char *)
     &((T3) {1, (char *) 1})) /* this is not ISO C, anyway*/
  }
};


int main()
{
    return 0;
}
