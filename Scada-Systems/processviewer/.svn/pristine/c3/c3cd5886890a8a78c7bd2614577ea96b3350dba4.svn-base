unit UnitMotor;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, LResources, Forms, Controls, Graphics, Dialogs,
  StdCtrls,
  UnitFormsConfig;

type

  { TFormMotor }

  TFormMotor = class(TForm)
    btnStart: TButton;
    btnStop: TButton;
    labMotorState: TLabel;
    labMotorName: TLabel;
    labMotorInfo: TLabel;
    procedure btnStartClick(Sender: TObject);
    procedure btnStopClick(Sender: TObject);
  private
    sh:TShowMotorWindow;
  public
    procedure ShowMotor(ShowMotorWindow:TShowMotorWindow);
  end;

var
  FormMotor: TFormMotor;

implementation

Uses
  UnitConfig;

procedure TFormMotor.ShowMotor(ShowMotorWindow:TShowMotorWindow);
begin
  sh:=ShowMotorWindow;
  TagsLock;
  try
    labMotorName.Caption:=ShowMotorWindow.StatusTag.TagName;
    labMotorInfo.Caption:=ShowMotorWindow.StatusTag.Comment;
    if (ShowMotorWindow.StatusTag.Status=0) then
    begin
      if (ShowMotorWindow.StatusTag.Value>0) then
      begin
        labMotorState.Caption:=ShowMotorWindow.StatusTag.OnMsg;
        labMotorState.Font.Color:=clGreen;
      end
      else
      begin
        labMotorState.Caption:=ShowMotorWindow.StatusTag.OffMsg;
        labMotorState.Font.Color:=clBlack;
      end;
    end
    else
      labMotorState.Caption:='Unknown';
    btnStart.Caption:=ShowMotorWindow.ManualStartTag.OnMsg;
    btnStop.Caption:=ShowMotorWindow.ManualStartTag.OffMsg;
  finally
    TagsUnlock;
  end;
  Show;
end;

procedure TFormMotor.btnStartClick(Sender: TObject);
begin
  TagsLock;
  try
    sh.ManualStartTag.SetValue:=1;
    sh.ManualStartTag.Value:=1;
    sh.ManualStartTag.Status:=0;
    sh.ManualStartTag.NeedSet:=True;
  finally
    TagsUnLock;
  end;
  Close;
end;

procedure TFormMotor.btnStopClick(Sender: TObject);
begin
  TagsLock;
  try
    sh.ManualStartTag.SetValue:=0;
    sh.ManualStartTag.Value:=0;
    sh.ManualStartTag.Status:=0;
    sh.ManualStartTag.NeedSet:=True;
  finally
    TagsUnLock;
  end;
  Close;
end;

initialization

end.

