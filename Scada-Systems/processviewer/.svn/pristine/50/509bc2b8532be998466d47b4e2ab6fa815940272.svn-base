unit UnitEnterValue;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, LResources, Forms, Controls, Graphics, Dialogs,
  ExtCtrls, StdCtrls,
  UnitTags;

type

  { TFormEnterValue }

  TFormEnterValue = class(TForm)
    btnBackspace: TButton;
    btnSet: TButton;
    btn8: TButton;
    btn9: TButton;
    btnDot: TButton;
    BtnPlusMinus: TButton;
    btn0: TButton;
    btn1: TButton;
    btn2: TButton;
    btn3: TButton;
    btn4: TButton;
    btn5: TButton;
    btn6: TButton;
    btn7: TButton;
    EditValue: TEdit;
    Lab4: TLabel;
    Label1: TLabel;
    LabComment: TLabel;
    Label2: TLabel;
    Lab3: TLabel;
    Label3: TLabel;
    LabLow: TLabel;
    LabHigh: TLabel;
    LabUnits: TLabel;
    LabTag: TLabel;
    Panel1: TPanel;
    procedure btnBackspaceClick(Sender: TObject);
    procedure btnDotClick(Sender: TObject);
    procedure BtnPlusMinusClick(Sender: TObject);
    procedure btnSetClick(Sender: TObject);
    procedure btn0Click(Sender: TObject);
    procedure EditValueKeyPress(Sender: TObject; var Key: char);
  private
    FTg:TTag;
    function DelSel:Boolean;
  public
    procedure ShowTag(tg:TTag);
  end; 

var
  FormEnterValue: TFormEnterValue;

implementation

Uses
  UnitConfig;

procedure TFormEnterValue.ShowTag(tg:TTag);
begin
  FTg:=tg;
  TagsLock;
  try
    LabTag.Caption:=FTg.TagName;
    LabUnits.Caption:=FTg.EngUnits;
    LabLow.Caption:=FormatFloat('0.00',FTg.MinEU);
    LabHigh.Caption:=FormatFloat('0.00',FTg.MaxEU);
    LabComment.Caption:=FTg.Comment;
    EditValue.Text:=FormatFloat('0.00',FTg.Value);
  finally
    TagsUnLock;
  end;
  EditValue.SelectAll;
  Show;
  EditValue.SetFocus;
end;

procedure TFormEnterValue.EditValueKeyPress(Sender: TObject; var Key: char);
begin
  if key=#13 then
    btnSetClick(Sender);
end;

procedure TFormEnterValue.btnSetClick(Sender: TObject);
begin
  try
    TagsLock;
    try
      FTg.SetValue:=StrToFloat(EditValue.Text);
      FTg.Value:=FTg.SetValue;
      FTg.Status:=0;
      FTg.NeedSet:=True;
    finally
      TagsUnlock;
    end;
  except
  end;
  Close;
end;

function TFormEnterValue.DelSel:Boolean;
var
  s1:String;
  CurPoint: TPoint;
  l:Integer;
begin
  l:=EditValue.SelLength;
  Result:=l>0;
  if Result then
  begin
    CurPoint.x:=EditValue.SelStart;
    s1:=EditValue.Text;
    Delete(s1,CurPoint.x+1,l);
    EditValue.Text:=s1;
    EditValue.CaretPos:=CurPoint;
  end;
end;

procedure TFormEnterValue.btn0Click(Sender: TObject);
var
  s,s1:String;
  CurPoint: TPoint;
begin
  DelSel;
  s:=TControl(Sender).Name;
  CurPoint:=EditValue.CaretPos;
  s1:=EditValue.Text;
  s1:=Copy(s1,1,CurPoint.x)+Copy(s,Length(s),1)+Copy(s1,CurPoint.x+1,Length(s1));
  EditValue.Text:=s1;
  CurPoint.x:=CurPoint.x+1;
  EditValue.CaretPos:=CurPoint;
  EditValue.SetFocus;
end;

procedure TFormEnterValue.btnDotClick(Sender: TObject);
var
  s1:String;
  CurPoint: TPoint;
begin
  DelSel;
  CurPoint:=EditValue.CaretPos;
  s1:=EditValue.Text;
  s1:=Copy(s1,1,CurPoint.x)+'.'+Copy(s1,CurPoint.x+1,Length(s1));
  EditValue.Text:=s1;
  CurPoint.x:=CurPoint.x+1;
  EditValue.CaretPos:=CurPoint;
  EditValue.SetFocus;
end;

procedure TFormEnterValue.BtnPlusMinusClick(Sender: TObject);
var
  s:String;
  CurPoint: TPoint;
begin
  s:=EditValue.Text;
  if s<>'' then
  begin
    if s[1]='-' then
      Delete(s,1,1)
    else
      s:='-'+s;
  end
  else
    s:='-'+s;
  EditValue.Text:=s;
  CurPoint.x:=Length(s);
  EditValue.CaretPos:=CurPoint;
  EditValue.SetFocus;
end;

procedure TFormEnterValue.btnBackspaceClick(Sender: TObject);
var
  s1:String;
  CurPoint: TPoint;
begin
  if not DelSel then
  begin
    s1:=EditValue.Text;
    if s1<>'' then
    begin
      CurPoint:=EditValue.CaretPos;
      Delete(s1,CurPoint.x,1);
      EditValue.Text:=s1;
      CurPoint.x:=CurPoint.x-1;
      EditValue.CaretPos:=CurPoint;
      EditValue.SetFocus;
    end;
  end;
end;

initialization

end.

