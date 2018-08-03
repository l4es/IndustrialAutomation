#ifndef S_LINE
//#define S_LINE "<!--"
//#define E_LINE "-->"
#define S_LINE ""
#define E_LINE ""

//#define FILE_PREFIX "cl_"
#define FILE_PREFIX ""

#endif



char *cl_fgets(char *s, int size, FILE *stream);
void UpdateNbrLinesUsedInRung( StrRung * BufRung );
void LoadAllRungs_V1(char * BaseName,StrRung * Rungs,int * TheFirst,int * TheLast,int * TheCurrent);
void LoadAllRungs(char * BaseName,StrRung * Rungs);
void SaveAllRungs(char * BaseName);
char * ConvRawLineOfNumbers(char * RawLine,char NbrParams,int * ValuesFnd, int DefaultValueToComplete);
char * ConvRawLineOfStringsOrNumbers(char * RawLine,int NbrParams,char ** ParamsStringsFnd,int ** ParamsIntFnd,char ** ParamsCharFnd);
int ConvBaseInMilliSecsToId(int NbrMilliSecs);
#ifdef OLD_TIMERS_MONOS_SUPPORT
char LoadTimersParams(char * FileName);
char SaveTimersParams(char * FileName);
char LoadMonostablesParams(char * FileName);
char SaveMonostablesParams(char * FileName);
#endif
char LoadCountersParams(char * FileName);
char SaveCountersParams(char * FileName);
char LoadNewTimersParams(char * FileName);
char SaveNewTimersParams(char * FileName);
char LoadRegistersParams(char * FileName);
char SaveRegistersParams(char * FileName);
void DumpRung(StrRung * TheRung);
char LoadArithmeticExpr(char * FileName);
char SaveArithmeticExpr(char * FileName);
char LoadSectionsParams(char * FileName);
char SaveSectionsParams(char * FileName);
char LoadIOConfParams(char * FileName);
char SaveIOConfParams(char * FileName);
char LoadModbusIOConfParams(char * FileName);
char SaveModbusIOConfParams(char * FileName);
char LoadSymbols(char * FileName);
char SaveSymbols(char * FileName);
char LoadGeneralParameters(char * FileName);
char SaveGeneralParameters(char * FileName);
char LoadConfigEventsLog(char * FileName);
char SaveConfigEventsLog(char * FileName);
char LoadProjectProperties(char * FileName);
char SaveProjectProperties(char * FileName);
char LoadModemConfig(char * FileName);
char SaveModemConfig(char * FileName);
char LoadRemoteAlarmsConfig(char * FileName);
char SaveRemoteAlarmsConfig(char * FileName);


void LoadAllLadderDatas(char * DatasDirectory);
void SaveAllLadderDatas(char * DatasDirectory);

void CleanTmpLadderDirectory( char DestroyDir );

