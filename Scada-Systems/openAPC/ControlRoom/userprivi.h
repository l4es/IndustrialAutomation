#ifndef USERPRIVI_H
#define USERPRIVI_H

#define MAX_PRIVI_NUM 15

struct userprivi_data
{
   bool     enabled;
   wxInt32  privisEnabled;
   wxString priviName[MAX_PRIVI_NUM];
};

#endif
