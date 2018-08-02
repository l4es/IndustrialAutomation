unit UnitIOThread;

{$mode objfpc}{$H+}

interface

uses
  {$IFDEF UNIX}
  cthreads,
  {$ENDIF}
  Classes, SysUtils, LCLIntf,
  UnitTimers
  {$IFDEF WINDOWS}
  ,ActiveX;
  {$ELSE}
  ;
  {$ENDIF}

type
  TIOThread = class(TThread)
  protected
    fCS: LongWord;//TRTLCriticalSection;
    Stopping:Boolean;
    FIntervalMs,FTimeout:Integer;
    FOnStatus:TNotifyEvent;
    FOnExecute:TNotifyEvent;
    ScanTimeTm:EventTimer;
    Procedure SetTimeout(Value:Integer);virtual;
    procedure Execute; override;
  public
    Executed:Boolean;
    Tags:TList;
    ScanTime:LongInt;

    constructor CreateThread;
    destructor Destroy; override;

    procedure StartThread;
    procedure StopThread;

    procedure Lock;
    procedure Unlock;

    property IntervalMs: Integer read FIntervalMs write FIntervalMs;
    property Timeout: Integer read FTimeout write SetTimeout;

    property OnStatus: TNotifyEvent read FOnStatus write FOnStatus;
    property OnExecute: TNotifyEvent read FOnExecute write FOnExecute;
  end;

implementation

Uses
  UnitThreadLog;

constructor TIOThread.CreateThread;
begin
  Inherited Create(True,DefaultStackSize);
  InitializeCriticalSection(fCS);
  Executed:=False;
  Stopping:=False;
  FreeOnTerminate:=False;
  FTimeout:=1000;
  FIntervalMs:=500;
  FOnStatus:=nil;
  FOnExecute:=nil;
end;

destructor TIOThread.Destroy;
begin
  DeleteCriticalSection(fCS);
  inherited;
end;

procedure TIOThread.Lock;
begin
  EnterCriticalSection(fCS);
end;

procedure TIOThread.Unlock;
begin
  LeaveCriticalSection(fCS);
end;

Procedure TIOThread.SetTimeout(Value:Integer);
begin
  FTimeout:=Value;
end;

//Threaded reading
procedure TIOThread.Execute;
begin
  Executed:=True;
  {$IFDEF WINDOWS}
  CoInitialize(nil);
  {$ENDIF}
  while (not Terminated) and (not Stopping) do
  begin
    try
      NewTimerSecs(ScanTimeTm,1000);

      if Assigned(FOnExecute) then
        FOnExecute(Self);

      if Assigned(FOnStatus) then
        FOnStatus(Self);

      Lock;
      try
        ScanTime:=ElapsedTimeInMSecs(ScanTimeTm);
      finally
        Unlock;
      end;
    except
      on e:Exception do
      begin
        LogThr.AddToLog('TIOThread Error: '+e.Message);
      end;
    end;
    sleep(FIntervalMs);
  end;
  Executed:=False;
  Stopping:=False;
  {$IFDEF WINDOWS}
  CoUninitialize;
  {$ENDIF}
end;

procedure TIOThread.StartThread;
begin
  if not Executed then
    Resume;
end;

procedure TIOThread.StopThread;
begin
  Stopping:=True;
end;

end.

