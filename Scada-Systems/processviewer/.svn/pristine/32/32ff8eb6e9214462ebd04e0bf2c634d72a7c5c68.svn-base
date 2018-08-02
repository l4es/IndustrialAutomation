unit UnitConfig;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils,
  UnitFormsConfig, UnitTags, UnitAccessNames;

type
  TConfig=class
  public
    AccessNames:TAccessNames;
    Tags:TTags;
    Forms:TFormConfigs;

    constructor Create;
    destructor Destroy;override;

    procedure LoadConfig(dir:String);
    procedure SaveConfig;
  end;

var
  cfg:TConfig;

procedure TagsLock;
procedure TagsUnlock;

implementation

Uses
  SyncObjs, UnitThreadLog;

var
  critTags:TCriticalSection;

procedure TagsLock;
begin
  critTags.Enter;
end;

procedure TagsUnlock;
begin
  critTags.Leave;
end;

constructor TConfig.Create;
begin
  LogThr:=TLogThread.Create;
  LogThr.IsDebug:=True;
  LogThr.RunThread;
  LogThr.AddToLog('Starting...');
  critTags:=TCriticalSection.Create;
  AccessNames:=TAccessNames.Create(Self);
  Tags:=TTags.Create(Self);
  Forms:=TFormConfigs.Create;
end;

destructor TConfig.Destroy;
begin
  Forms.Free;
  Tags.Free;
  AccessNames.Free;
  critTags.Free;
  LogThr.StopThread;
  LogThr.Free;
  inherited;
end;

procedure TConfig.LoadConfig(dir:String);
begin
  AccessNames.LoadConfig(dir+'accessnames.xml');
  Tags.LoadConfig(dir+'tags.xml');
  Forms.LoadConfig(dir+'forms.xml');
end;

procedure TConfig.SaveConfig;
begin
  AccessNames.SaveConfig;
  Tags.SaveConfig;
  //Forms.SaveConfig;
end;

end.

