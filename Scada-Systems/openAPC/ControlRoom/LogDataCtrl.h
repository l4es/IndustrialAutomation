#ifndef LOGDATACTRL_H
#define LOGDATACTRL_H

class LogDataCtrl
{
public:
   LogDataCtrl(struct hmiObjectLogData logData[MAX_LOG_TYPES]);
   virtual ~LogDataCtrl();

   void logDigi(wxByte val,flowObject *object);
   void logNum(wxFloat64 val,flowObject *object);
   void logChar(wxString val,flowObject *object);


private:
   struct hmiObjectLogData data[MAX_LOG_TYPES];
   wxByte                  prevDigiVal;
   wxFloat64               prevNumVal;
   wxString                prevCharVal;
   bool                    digiValid,numValid,charValid;
};

#endif //LOGDATACTRL_H
