#include <stdio.h>
void message(void)
{
    const char *s = "Hello, world!";
    puts(s);
}

int main(void)
{
    message();
    return 0;
}
