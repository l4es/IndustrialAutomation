
#ifndef _MONITOR_TRANSFER_H
#define _MONITOR_TRANSFER_H

#include "cJSON.h"

typedef struct StrFileTransfer
{
	char FileName[ 400 ];
	char IsSendTransfer; //send or received file
	FILE * pFileTransferDesc;
	int NumBlock;
	int NbrTotalBlocks;
	int SizeBlock; // size of real datas file (before base64 encoding)
	int NumFile;
	int SubNumFile;
	int StartTransferDone; // for the first time...
	char AskToAbort;
}StrFileTransfer;

void InitMonitorTransferFile( void );

void TransferMasterStart( void );
void TransferMasterAskToAbort( void );
StrFileTransfer * GetPtrFileTransForMaster( void );

void SecurityForAbortedTransferToSlave( void );

void CloseFileTransfer( StrFileTransfer * pFileTrans );
char StartToReadFileToSend( StrFileTransfer * pFileTrans );
char StartToWriteFileToReceive( StrFileTransfer * pFileTrans );

char * MonitorCreateRequestFileTransfer( void );
void MonitorParseResponseFileTransfer( cJSON *JsonRoot, char SlaveMode );

#endif
