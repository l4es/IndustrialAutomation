#include "orte_all.h"

HANDLE                         killServiceEvent = NULL;
static SERVICE_STATUS          ssStatus;       // current status of the service
static SERVICE_STATUS_HANDLE   sshStatusHandle;
static DWORD                   dwErr = 0;
static char                    szErr[1024] = "";

char                           *name_service="ortemanager";
char                           *name_service_disp="ortemanager";

int managerInit(void);         //forward declaration
int managerStart(void);        //forward declaration
int managerStop(void);         //forward declaration

static void AddToMessageLog(char *lpszMsg) {
  char    szMsg[2048];
  HANDLE  hEventSource;
  char *  lpszStrings[2];

  dwErr = GetLastError();
  hEventSource = RegisterEventSource(NULL, name_service);
  sprintf(szMsg, "%s error: %li", name_service, dwErr);
  lpszStrings[0] = szMsg;
  lpszStrings[1] = lpszMsg;
  if(hEventSource != NULL) {
    ReportEvent(hEventSource, // handle of event source
                EVENTLOG_ERROR_TYPE,  // event type
                0,                    // event category
                0,                    // event ID
                NULL,                 // current user's SID
                2,                    // strings in lpszStrings
                0,                    // no bytes of raw data
                (LPCSTR*)lpszStrings, // array of error strings
                NULL);                // no raw data
    DeregisterEventSource(hEventSource);
  }
}

BOOL ReportStatusToSCMgr(DWORD dwCurrentState,DWORD dwWin32ExitCode,
                         DWORD dwWaitHint) {
  static DWORD dwCheckPoint = 1;
  BOOL fResult = TRUE;

  if(dwCurrentState == SERVICE_START_PENDING) ssStatus.dwControlsAccepted = 0;
  else ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  ssStatus.dwCurrentState = dwCurrentState;
  ssStatus.dwWin32ExitCode = dwWin32ExitCode;
  ssStatus.dwWaitHint = dwWaitHint;
  if((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED)) ssStatus.dwCheckPoint = 0;
  else ssStatus.dwCheckPoint = dwCheckPoint++;
  if(!(fResult = SetServiceStatus(sshStatusHandle, &ssStatus))) {
    AddToMessageLog("SetServiceStatus");
  }
  return fResult;
}

void WINAPI service_ctrl(DWORD dwCtrlCode) {
  switch(dwCtrlCode) {
    case SERVICE_CONTROL_STOP:
      ssStatus.dwCurrentState = SERVICE_STOP_PENDING;
      managerStop();
      SetEvent(killServiceEvent);
      break;
    case SERVICE_CONTROL_INTERROGATE:
      break;
    default:
      break;
  }
  ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}

void WINAPI service_main(DWORD dwArgc, char **lpszArgv) {
  int       err;
  sshStatusHandle = RegisterServiceCtrlHandler(name_service, service_ctrl);
  if(sshStatusHandle) {
    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;
    killServiceEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    if (!killServiceEvent) goto cleanup;
    if(!ReportStatusToSCMgr(SERVICE_START_PENDING,NO_ERROR,3000)) goto cleanup;
    err=managerInit();
    if (err<0) {
      AddToMessageLog("Starting communication");
      dwErr=NO_ERROR;
      goto cleanup;
    }
    managerStart();
    if(!ReportStatusToSCMgr(SERVICE_RUNNING,NO_ERROR, 0 )) goto cleanup;
    WaitForSingleObject(killServiceEvent,INFINITE);
    CloseHandle(killServiceEvent);
  }
cleanup:
  if(sshStatusHandle) {
    ReportStatusToSCMgr(SERVICE_STOP_PENDING,dwErr,0);
    ReportStatusToSCMgr(SERVICE_STOPPED,dwErr,0);
  }
}

void serviceDispatchTable(void) {
  SERVICE_TABLE_ENTRY dispatchTable[] = {
      { name_service, (LPSERVICE_MAIN_FUNCTION)service_main },
      { NULL, NULL }
  };
  if(!StartServiceCtrlDispatcher(dispatchTable)) {
    AddToMessageLog("StartServiceCtrlDispatcher failed.");
  }
}

char *GetLastErrorText( char *lpszBuf, DWORD dwSize ) {
  DWORD dwRet;
  char *lpszTemp = NULL;

  dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
                        NULL,
                        GetLastError(),
                        LANG_NEUTRAL,
                        (char *)&lpszTemp,
                        0,
                        NULL);
  // supplied buffer is not long enough
  if(!dwRet || ((long)dwSize < (long)dwRet+14)) {
    lpszBuf[0] = '\0';
  } else {
    lpszTemp[lstrlen(lpszTemp)-2] = '\0';  //remove cr and newline character
    sprintf(lpszBuf, "%s (%li)", lpszTemp, GetLastError());
  }
  if(lpszTemp) LocalFree((HLOCAL) lpszTemp );
  return lpszBuf;
}

void installService(void) {
  SC_HANDLE   schService;
  SC_HANDLE   schSCManager;
  TCHAR       szPath[512];

  if(GetModuleFileName( NULL, szPath, sizeof(szPath) - 1) == 0) {
     printf("Unable to install %s - %s\n",
             name_service,
             GetLastErrorText(szErr, sizeof(szErr)));
     return;
  }
  strcat(szPath," -s");
  schSCManager = OpenSCManager(NULL,  // machine (NULL == local)
                               NULL,  // database (NULL == default)
                               SC_MANAGER_ALL_ACCESS);   // access required
  if(schSCManager) {
    schService = CreateService(schSCManager,               // SCManager database
                               name_service,               // name of service
                               name_service_disp,          // name to display
                               SERVICE_ALL_ACCESS,         // desired access
                               SERVICE_WIN32_OWN_PROCESS,  // service type
                               SERVICE_AUTO_START,         // start type
                               SERVICE_ERROR_NORMAL,       // error control type
                               szPath,                     // service's binary
                               NULL,                       // no load ordering group
                               NULL,                       // no tag identifier
                               NULL,                       // dependencies
                               NULL,                       // LocalSystem account
                               NULL);                      // no password
    if(schService) {
      CloseServiceHandle(schSCManager);
      printf("Service name (%s) instaled!\n",name_service);
    } else {
       printf("OpenSCManager failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
    }
  }
}

void removeService(void) {
  SC_HANDLE   schService;
  SC_HANDLE   schSCManager;

  schSCManager = OpenSCManager(NULL,          // machine (NULL == local)
                               NULL,          // database (NULL == default)
                               SC_MANAGER_ALL_ACCESS );  // access required
  if(schSCManager) {
    schService = OpenService(schSCManager, name_service, SERVICE_ALL_ACCESS);
    if(schService) {
      // try to stop the service
      if(ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus )) {
        printf("Stopping %s.", name_service);
        Sleep(100);
        while(QueryServiceStatus(schService, &ssStatus )) {
          if(ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
            printf(".");
            Sleep(100);
          } else {
             break;
          }
        }
        if(ssStatus.dwCurrentState == SERVICE_STOPPED) {
          printf("\n%s stopped.\n", name_service);
        } else {
          printf("\n%s failed to stop.\n", name_service);
        }
      }
      // now remove the service
      if(DeleteService(schService)) {
        printf("%s removed.\n", name_service);
      } else {
        printf("DeleteService failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
      }
      CloseServiceHandle(schService);
    } else {
      printf("OpenService failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
    }
    CloseServiceHandle(schSCManager);
  } else {
    printf("OpenSCManager failed - %s\n", GetLastErrorText(szErr, sizeof(szErr)));
  }
}

