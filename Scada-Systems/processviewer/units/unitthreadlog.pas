unit UnitThreadLog;

interface

Uses
  Classes,
  SysUtils;

type
  TStringObject=class
    str:String;
  end;

  TLogThread = class(TThread)
  private
    Stopping:Boolean;
    LogQueue:TThreadList;
    procedure WriteToLog(msg:String);
  protected
    procedure Execute; override;
  public
    constructor Create();
    destructor Destroy; override;
  public
    Executed:Boolean;
    IsDebug:Boolean;

    procedure RunThread;
    procedure StopThread;

    procedure AddToLog(msg:String);
    Procedure AddDbg(msg:String);
  end;

var
  LogThr:TLogThread = nil;

implementation

{$IFDEF WINDOWS}
Uses
  ActiveX;
{$ENDIF}

constructor TLogThread.Create();
begin
  Executed:=False;
  Stopping:=False;
  FreeOnTerminate:=False;
  LogQueue:=TThreadList.Create;
  Inherited Create(True);
end;

destructor TLogThread.Destroy;
begin
  LogQueue.Free;
  inherited;
end;

Procedure TLogThread.AddDbg(msg:String);
begin
  AddToLog(msg);
end;

Procedure TLogThread.AddToLog(msg:String);
var
  sObj:TStringObject;
begin
  sObj:=TStringObject.Create;
  sObj.str:=msg;
  LogQueue.Add(sObj);
end;

procedure TLogThread.WriteToLog(msg:String);
var
  s,fs:String;
  f:TextFile;
  dt:TDateTime;
begin
  if not IsDebug then
    Exit;
  try
    dt:=now;
    s:='['+DateTimeToStr(dt)+'] '+msg;
    fs:=ExtractFilePath(ParamStr(0))+'Logs/';
    ForceDirectories(fs);
    fs:=fs+FormatDateTime('yyyy_mm_dd',dt)+'.log';
    AssignFile(f,fs);
    if FileExists(fs) then
      Append(f)
    else
      Rewrite(f);
    writeln(f,s);
    CloseFile(f);
  except
  end;
end;

//Threaded sending from messages queue
procedure TLogThread.Execute;
var
  lst:TList;
  s:String;
  sObj:TStringObject;
begin
  Executed:=True;
  {$IFDEF WINDOWS}
  CoInitialize(nil);
  {$ENDIF}
  while (not Terminated) and (not Stopping) do
  try
    s:='';
    try
      lst:=LogQueue.LockList;
      if lst.Count>0 then
      begin
        sObj:=TStringObject(lst.First);
        s:=sObj.str;
        lst.Remove(sObj);
        if Assigned(sObj) then
          sObj.Free;
      end;
    finally
      LogQueue.UnlockList;
    end;

    if s<>'' then
      WriteToLog(s);
    sleep(1);
  except
  end;
  Executed:=False;
  Stopping:=False;
  {$IFDEF WINDOWS}
  CoUninitialize;
  {$ENDIF}
end;

procedure TLogThread.RunThread;
begin
  if not Executed then
    Resume;
end;

procedure TLogThread.StopThread;
begin
  Stopping:=True;
end;

end.
