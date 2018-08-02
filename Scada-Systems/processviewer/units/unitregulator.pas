unit UnitRegulator;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, LResources, Forms, Controls, Graphics, Dialogs,
  ExtCtrls, StdCtrls, ComCtrls,
  UnitFormsConfig;

type

  { TFormRegulator }

  TFormRegulator = class(TForm)
    bevHiHi: TShape;
    bevLoLo: TShape;
    bevOpSp: TShape;
    bevExSp: TShape;
    btnExtSpOpSp: TToggleBox;
    btnClose: TButton;
    lab2: TLabel;
    lab3: TLabel;
    lab34: TLabel;
    lab35: TLabel;
    labForce: TLabel;
    lab4: TLabel;
    lab5: TLabel;
    lab6: TLabel;
    lab33: TLabel;
    lab7: TLabel;
    lab8: TLabel;
    labHiHi1: TLabel;
    labHiHi2: TLabel;
    labOutp: TLabel;
    labOutp1: TLabel;
    labOutp2: TLabel;
    labUnits: TLabel;
    Label1: TLabel;
    lab1: TLabel;
    labHiHi: TLabel;
    labLoLo: TLabel;
    labLoSc: TLabel;
    labOpSp: TLabel;
    labExSp: TLabel;
    labPV: TLabel;
    labHiSc: TLabel;
    labUnits1: TLabel;
    lineLoLo: TShape;
    Panel3: TPanel;
    Panel4: TPanel;
    panName: TPanel;
    panComment: TPanel;
    Panel1: TPanel;
    Panel2: TPanel;
    Shape1: TShape;
    btnAutoMan: TToggleBox;
    bevOutp: TShape;
    lineHiHi: TShape;
    ShapeActSp: TShape;
    ShapePV: TShape;
    ShapeActSp2: TShape;
    ShapeOutp: TShape;
    Timer1: TTimer;
    procedure btnAutoManClick(Sender: TObject);
    procedure btnCloseClick(Sender: TObject);
    procedure btnExtSpOpSpClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure labHiHiClick(Sender: TObject);
    procedure labLoLoClick(Sender: TObject);
    procedure labOpSpClick(Sender: TObject);
    procedure labOutpClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure labExSpClick(Sender: TObject);
  private
    sh:TShowRegulatorWindow;
    flItIsNotClick:Boolean;
  public
    procedure ShowRegulator(parentForm:TWinControl;ShowRegulatorWindow:TShowRegulatorWindow);
    procedure UpdateTagValues;
  end;

var
  FormRegulator: TFormRegulator;

implementation

Uses
  UnitConfig, UnitEnterValue;

{ TFormRegulator }

procedure TFormRegulator.FormCreate(Sender: TObject);
begin
  bevHiHi.OnClick:=@labHiHiClick;
  bevLoLo.OnClick:=@labLoLoClick;
end;

procedure TFormRegulator.btnCloseClick(Sender: TObject);
begin
  Close;
end;

procedure TFormRegulator.ShowRegulator(parentForm:TWinControl;ShowRegulatorWindow:TShowRegulatorWindow);
begin
  if parentForm<>nil then
    Parent:=parentForm;
  sh:=ShowRegulatorWindow;
  UpdateTagValues;
  Left:=Parent.Width-Width+1;
  Top:=0;
  Show;
  Timer1.Enabled:=True;
end;

procedure TFormRegulator.UpdateTagValues;
begin
  flItIsNotClick:=True;
  TagsLock;
  try
    panComment.Caption:=sh.PV_tag.Comment;
    panComment.ShowHint:=True;
    panComment.Hint:=sh.PV_tag.Comment;

    panName.Caption:=sh.PV_tag.TagName;
    panName.ShowHint:=True;
    panName.Hint:=sh.PV_tag.TagName;

    labPV.Caption:=FormatFloat(sh.DisplayFormat,sh.PV_tag.Value);
    labHiSc.Caption:=FormatFloat(sh.DisplayFormat,sh.PV_tag.MaxEU);
    labLoSc.Caption:=FormatFloat(sh.DisplayFormat,sh.PV_tag.MinEU);

    labOpSp.Caption:=FormatFloat(sh.DisplayFormat,sh.OpSp_tag.Value);
    labExSp.Caption:=FormatFloat(sh.DisplayFormat,sh.ExtSp_tag.Value);
    labHiHi.Caption:=FormatFloat(sh.DisplayFormat,sh.HiHiSp_tag.Value);
    labLoLo.Caption:=FormatFloat(sh.DisplayFormat,sh.LoLoSp_tag.Value);

    labUnits.Caption:=sh.PV_tag.EngUnits;

    labOutp.Caption:=FormatFloat(sh.DisplayFormat,sh.Output_tag.Value);

    if sh.ExtSpButton_tag=nil then
      btnExtSpOpSp.Hide
    else
    if sh.ExtSpButton_tag.Value>0 then
    begin
      btnExtSpOpSp.Checked:=True;
      btnExtSpOpSp.Caption:='ESP';
      labOpSp.Font.Color:=clBlack;
      lab3.Font.Color:=clBlack;
      labExSp.Font.Color:=clLime;
      lab4.Font.Color:=clLime;
      labOpSp.OnClick:=nil;
      lab3.OnClick:=nil;
      labExSp.OnClick:=@labExSpClick;
      lab4.OnClick:=@labExSpClick;
      bevExSp.OnClick:=@labExSpClick;
      bevExSp.Brush.Color:=$959595;
      bevOpSp.OnClick:=nil;
      bevOpSp.Brush.Color:=clGray;
    end
    else
    begin
      btnExtSpOpSp.Checked:=False;
      btnExtSpOpSp.Caption:='OSP';
      labOpSp.Font.Color:=clLime;
      lab3.Font.Color:=clLime;
      labExSp.Font.Color:=clBlack;
      lab4.Font.Color:=clBlack;
      labOpSp.OnClick:=@labOpSpClick;
      lab3.OnClick:=@labOpSpClick;
      labExSp.OnClick:=nil;
      lab4.OnClick:=nil;
      bevOpSp.OnClick:=@labOpSpClick;
      bevOpSp.Brush.Color:=$959595;
      bevExSp.OnClick:=nil;
      bevExSp.Brush.Color:=clGray;
    end;

    if sh.ManAutButton_tag=nil then
      btnAutoMan.Hide
    else
    if sh.ManAutButton_tag.Value>0 then
    begin
      btnAutoMan.Checked:=True;
      btnAutoMan.Caption:='MAN';
      labOutp.Font.Color:=clLime;
      lab33.Font.Color:=clLime;
      labOutp.OnClick:=@labOutpClick;
      lab33.OnClick:=@labOutpClick;
      bevOutp.OnClick:=@labOutpClick;
      bevOutp.Brush.Color:=$959595;
    end
    else
    begin
      btnAutoMan.Checked:=False;
      btnAutoMan.Caption:='AUTO';
      labOutp.Font.Color:=clBlack;
      lab33.Font.Color:=clBlack;
      labOutp.OnClick:=nil;
      lab33.OnClick:=nil;
      bevOutp.OnClick:=nil;
      bevOutp.Brush.Color:=clGray;
    end;

    if (sh.ForceBit_tag=nil) or (sh.ForceBit_tag.Value=0) then
      labForce.Hide
    else
      labForce.Visible:=sh.ForceBit_tag.Value>0;

    ShapeActSp.Height:=Round(449*(sh.ActSp_tag.Value-sh.ActSp_tag.MinEU)/(sh.ActSp_tag.MaxEU-sh.ActSp_tag.MinEU));
    ShapeActSp.Top:=30+449-ShapeActSp.Height;
    ShapeActSp2.Height:=ShapeActSp.Height;
    ShapeActSp2.Top:=ShapeActSp.Top;

    ShapePV.Height:=Round(449*(sh.PV_tag.Value-sh.PV_tag.MinEU)/(sh.PV_tag.MaxEU-sh.PV_tag.MinEU));
    ShapePV.Top:=30+449-ShapePV.Height;

    ShapeOutp.Height:=Round(449*(sh.Output_tag.Value-sh.Output_tag.MinEU)/(sh.Output_tag.MaxEU-sh.Output_tag.MinEU));
    ShapeOutp.Top:=30+449-ShapeOutp.Height;

    lineHiHi.Top:=30+449-Round(449*(sh.HiHiSp_tag.Value-sh.HiHiSp_tag.MinEU)/(sh.HiHiSp_tag.MaxEU-sh.HiHiSp_tag.MinEU));
    lineLoLo.Top:=30+449-Round(449*(sh.LoLoSp_tag.Value-sh.LoLoSp_tag.MinEU)/(sh.LoLoSp_tag.MaxEU-sh.LoLoSp_tag.MinEU))-4;

    if sh.HiHiBit_tag.Value>0 then
      bevHiHi.Pen.Color:=clRed
    else
      bevHiHi.Pen.Color:=clGray;
    if sh.LoLoBit_tag.Value>0 then
      bevLoLo.Pen.Color:=clRed
    else
      bevLoLo.Pen.Color:=clGray;

  finally
    TagsUnLock;
  end;
  flItIsNotClick:=False;
end;

procedure TFormRegulator.btnAutoManClick(Sender: TObject);
begin
  if flItIsNotClick then
    Exit;
  flItIsNotClick:=True;
  TagsLock;
  try
    sh.ManAutButton_tag.SetValue:=ord(btnAutoMan.Checked);
    sh.ManAutButton_tag.Value:=sh.ManAutButton_tag.SetValue;
    sh.ManAutButton_tag.NeedSet:=True;
    sh.ManAutButton_tag.Status:=0;
  finally
    TagsUnLock;
  end;
  UpdateTagValues;
  flItIsNotClick:=False;
end;

procedure TFormRegulator.btnExtSpOpSpClick(Sender: TObject);
begin
  if flItIsNotClick then
    Exit;
  flItIsNotClick:=True;
  TagsLock;
  try
    sh.ExtSpButton_tag.SetValue:=ord(btnExtSpOpSp.Checked);
    sh.ExtSpButton_tag.Value:=sh.ExtSpButton_tag.SetValue;
    sh.ExtSpButton_tag.NeedSet:=True;
    sh.ExtSpButton_tag.Status:=0;
  finally
    TagsUnLock;
  end;
  UpdateTagValues;
  flItIsNotClick:=False;
end;

procedure TFormRegulator.labOutpClick(Sender: TObject);
begin
  FormEnterValue.ShowTag(sh.Output_tag);
  UpdateTagValues;
end;

procedure TFormRegulator.FormClose(Sender: TObject;
  var CloseAction: TCloseAction);
begin
  Timer1.Enabled:=False;
end;

procedure TFormRegulator.Timer1Timer(Sender: TObject);
begin
  UpdateTagValues;
end;

procedure TFormRegulator.labOpSpClick(Sender: TObject);
begin
  FormEnterValue.ShowTag(sh.OpSp_tag);
  UpdateTagValues;
end;

procedure TFormRegulator.labExSpClick(Sender: TObject);
begin
  FormEnterValue.ShowTag(sh.ExtSp_tag);
  UpdateTagValues;
end;

procedure TFormRegulator.labHiHiClick(Sender: TObject);
begin
  FormEnterValue.ShowTag(sh.HiHiSp_tag);
  UpdateTagValues;
end;

procedure TFormRegulator.labLoLoClick(Sender: TObject);
begin
  FormEnterValue.ShowTag(sh.LoLoSp_tag);
  UpdateTagValues;
end;

initialization
  {$I unitregulator.lrs}

end.

