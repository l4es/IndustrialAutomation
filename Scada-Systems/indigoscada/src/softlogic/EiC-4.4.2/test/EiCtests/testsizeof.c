#include <string.h>
#include <assert.h>


int main(void)
{
    int a;
    long lv;
    char c;
    char ar[sizeof (long)];  
    
    assert(  strlen("hell of a day") == sizeof("hell of a day") - 1  );
    assert(  sizeof(8.8) == sizeof(double)                           );
    assert(  sizeof(15) == sizeof(int)                               );
    assert(  sizeof(struct {int x,y;}) == 2 * sizeof(int)            );
    assert(  sizeof c == sizeof(char)                                );
    assert(  sizeof a == sizeof(int)                                 );
    assert(  3 * sizeof(int *) == sizeof(int *[3])                   );
    assert(  sizeof(int *) == sizeof(int (*)[])                      );
    assert(  sizeof(long) == sizeof((long)1)                         ); 
    assert(  sizeof (4+5*3)  == sizeof (int)                         );
    assert(  sizeof(ar) == sizeof(long)                              );
    assert(  sizeof((long)(1==2)) == sizeof lv                       );

    return 0;
}


#ifdef EiCTeStS
main();
#endif

