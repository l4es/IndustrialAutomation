#ifndef UILOG_H
#define UILOG_H

enum uiLogLevel
{
    other=0,
    userevent,
    message,
    info,
    warn,
    error,
    critical
};

extern void uilog(const enum uiLogLevel level, const char *format,...);
extern void uilog_open(const enum uiLogLevel level, const char *file);
extern void uilog_close();

#endif
