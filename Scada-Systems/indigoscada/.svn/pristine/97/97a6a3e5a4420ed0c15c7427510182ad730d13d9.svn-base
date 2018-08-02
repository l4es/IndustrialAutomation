#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void tick(int i) { printf("tick\n"); return;}
void tock(int i) { printf("tock\n"); return;}

int main()
{
    int i = 0, cnt = 1;

    while(1) {
        signal(SIGINT,tick); // note you must reestablish the handler 
        sleep(1);
        raise(SIGINT);
        
        signal(SIGINT,tock);  
        sleep(1); 
	raise(SIGINT);
        if(i++==cnt) 
            break;         
    }
    signal(SIGINT,SIG_DFL);  // reset 
    return 0;				    
}
