unit UnitMain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, LResources, Forms, Controls, Graphics, Dialogs,
  ExtCtrls, StdCtrls, ComCtrls, Buttons,
  UnitTags;

type

  { TForm1 }

  TForm1 = class(TForm)
    PanelMain: TPanel;
    StatusBar1: TStatusBar;
    Timer1: TTimer;
    procedure FormCloseQuery(Sender: TObject; var CanClose: boolean);
    procedure FormCreate(Sender: TObject);
    procedure FormResize(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure ShowData(ControlElement:TObject);
    procedure ShowLineColor(ControlElementObj:TObject);
    procedure ShowFillColor(ControlElementObj:TObject);
    procedure ShowTextColor(ControlElementObj:TObject);
    procedure ShowVisible(ControlElementObj:TObject);
    procedure ShowDisable(ControlElementObj:TObject);
  private
    CommErrors:Integer;
  public
    procedure ShowTagValues;
  end;

var
  Form1: TForm1; 

implementation

{$R *.lfm}

{ TForm1 }

Uses
  UnitConfig, UnitFormsConfig, UnitAccessNames, UnitThreadLog, UnitTimers,
  UnitEnterValue, UnitVer,UnitRegulator,
  TAGraph, TASeries;

procedure TForm1.FormCreate(Sender: TObject);
var
  i:Integer;
  frm:TFormConfig;
begin
  Caption:=Caption+' '+ProgVersion;
  CommErrors:=0;
  cfg:=TConfig.Create;
  cfg.LoadConfig('../project/');
  LogThr.AddToLog('AccessNames='+IntToStr(cfg.AccessNames.Count)+'; '+
    'Tags='+IntToStr(cfg.Tags.Count)+'; '+
    'Forms='+IntToStr(cfg.Forms.Count));
  PanelMain.Color:=cfg.Forms.Color;
  for i:=0 to cfg.Forms.Count-1 do
  begin
    frm:=TFormConfig(cfg.Forms[i]);
    if Assigned(frm.Form) then
    begin
      frm.Form.Parent:=PanelMain;
      if frm.Left<0 then
        frm.Form.Left:=PanelMain.Width+frm.Left-1
      else
        frm.Form.Left:=frm.Left;
      frm.Form.Top:=frm.Top;
      if frm.Open then
        frm.Form.Show;
    end;
  end;
  cfg.AccessNames.StartIOServers;
  LogThr.AddToLog('Started OK');
  Timer1.Enabled:=True;
end;

procedure TForm1.FormResize(Sender: TObject);
var
  i:Integer;
  frm:TFormConfig;
begin
  for i:=0 to cfg.Forms.Count-1 do
  begin
    frm:=TFormConfig(cfg.Forms[i]);
    if Assigned(frm.Form) then
    begin
      frm.Form.Parent:=PanelMain;
      if frm.Left<0 then
        frm.Form.Left:=PanelMain.Width+frm.Left+1
      else
        frm.Form.Left:=frm.Left;
      frm.Form.Top:=frm.Top;
    end;
  end;
end;

procedure TForm1.FormCloseQuery(Sender: TObject; var CanClose: boolean);
begin
  Timer1.Enabled:=False;
  cfg.AccessNames.StopIOServers;
  CanClose:=True;
end;

procedure TForm1.Timer1Timer(Sender: TObject);
var
  tm:EventTimer;
  elapsed:LongInt;
begin
  NewTimerSecs(tm,1000);
  StatusBar1.Panels[8].Text:=FormatDateTime('dd.mm.yyyy hh:nn:ss',now);
  ShowTagValues;
  elapsed:=ElapsedTimeInMSecs(tm);
  if elapsed>10 then
    StatusBar1.Panels[1].Text:=IntToStr(elapsed)+'ms'
  else
    StatusBar1.Panels[1].Text:='<10ms';
  elapsed:=cfg.AccessNames.GetScanTimeMs;
  if elapsed>10 then
    StatusBar1.Panels[3].Text:=IntToStr(elapsed)+'ms'
  else
    StatusBar1.Panels[3].Text:='<10ms';
end;

procedure TForm1.ShowTagValues;
var
  frm:TFormConfig;
  k,i:Integer;
  ControlElement:TControlElement;
begin
  for k:=0 to cfg.Forms.Count-1 do
  begin
    frm:=TFormConfig(cfg.Forms[k]);
    if (Assigned(frm.Form)) and (frm.Form.Visible) and (frm.ControlElements.Count>0) then
    begin
      for i:=0 to frm.ControlElements.Count-1 do
      begin
        ControlElement:=TControlElement(frm.ControlElements[i]);
        if Assigned(ControlElement.Component) then
        begin
          if Assigned(ControlElement.ValueDisplay.Tag) then
          begin
            TagsLock;
            try
              ShowData(ControlElement);
            finally
              TagsUnLock;
            end;
          end;
          if Assigned(ControlElement.Colors.LineColor.Tag) then
          begin
            TagsLock;
            try
              ShowLineColor(ControlElement);
            finally
              TagsUnLock;
            end;
          end;
          if Assigned(ControlElement.Colors.FillColor.Tag) then
          begin
            TagsLock;
            try
              ShowFillColor(ControlElement);
            finally
              TagsUnLock;
            end;
          end;
          if Assigned(ControlElement.Colors.TextColor.Tag) then
          begin
            TagsLock;
            try
              ShowTextColor(ControlElement);
            finally
              TagsUnLock;
            end;
          end;
          if Assigned(ControlElement.Miscellaneous.Disable.Tag) then
          begin
            TagsLock;
            try
              ShowDisable(ControlElement);
            finally
              TagsUnLock;
            end;
          end;

          TagsLock;
          try
            ShowVisible(ControlElement);
          finally
            TagsUnLock;
          end;
        end;
      end;
    end;
  end;
end;

procedure TForm1.ShowData(ControlElement:TObject);
var
  s:String;
  comp:TControl;
  tg:TTag;
  ce:TControlElement;
begin
  ce:=TControlElement(ControlElement);
  comp:=TControl(ce.Component);

  tg:=ce.ValueDisplay.Tag;

  if (tg.Status=0) then
    comp.Enabled:=True
  else
  begin
    comp.Enabled:=False;
    s:='ERROR';
    inc(CommErrors);
    StatusBar1.Panels[7].Text:=IntToStr(CommErrors);
  end;

  if CompareText(ce.ValueDisplay.DisplayType,'Analog')=0 then
  begin
    if (tg.Status=0) then
      s:=FormatFloat(ce.ValueDisplay.Analog.DisplayFormat,tg.Value);

    if comp is TLabel then
      TLabel(comp).Caption:=s
    else
    if comp is TEdit then
    begin
       if TimerExpired(ce.EditTimer) then
         TEdit(comp).Text:=s;
    end
    else
    if comp is TTrackBar then
       TTrackBar(comp).Position:=Round(tg.Value)
    else
    if comp is TButton then
      TButton(comp).Caption:=s
    else
    if comp is TBitBtn then
      TBitBtn(comp).Caption:=s
    else
    if comp is TSpeedButton then
      TSpeedButton(comp).Caption:=s
    else
    if comp is TListBox then
      TListBox(comp).ItemIndex:=Round(tg.Value)
    else
    if comp is TComboBox then
      TComboBox(comp).ItemIndex:=Round(tg.Value)
    else
    if comp is TRadioGroup then
      TRadioGroup(comp).ItemIndex:=Round(tg.Value)
    else
    if ce.Component is TLineSeries then
      TLineSeries(ce.Component).AddXY(now,tg.Value);
  end
  else
  if CompareText(ce.ValueDisplay.DisplayType,'Discrete')=0 then
  begin
    if (tg.Status=0) then
    begin
      if tg.Value>0 then
        s:=ce.ValueDisplay.Discrete.OnMessage
      else
        s:=ce.ValueDisplay.Discrete.OffMessage;
    end;

    if comp is TLabel then
      TLabel(comp).Caption:=s
    else
    if comp is TCheckBox then
      TCheckBox(comp).Checked:=tg.Value>0
    else
    if comp is TRadioButton then
      TRadioButton(comp).Checked:=tg.Value>0;
  end;
end;

function GetLineColor(comp:TComponent):TColor;
begin
  Result:=clBlack;
  if comp is TShape then
    Result:=TShape(comp).Pen.Color;
end;

procedure SetLineColor(comp:TComponent;col:TColor);
begin
  if comp is TShape then
    TShape(comp).Pen.Color:=Col;
end;

procedure TForm1.ShowLineColor(ControlElementObj:TObject);
var
  comp:TControl;
  tg:TTag;
  ce:TControlElement;
  col:TColor;
begin
  ce:=TControlElement(ControlElementObj);
  comp:=TControl(ce.Component);

  tg:=ce.Colors.LineColor.Tag;

  if (tg.Status=0) then
  begin
    if tg.Value>0 then
      Col:=ce.Colors.LineColor.OnColor
    else
      Col:=ce.Colors.LineColor.OffColor;

    SetLineColor(comp,col);
  end;
end;

Procedure SetFillColor(comp:TComponent;col:TColor);
begin
  if comp is TShape then
    TShape(comp).Brush.Color:=Col
  else
  if comp is TButton then
    TButton(comp).Color:=Col
  else
  if comp is TBitBtn then
    TBitBtn(comp).Color:=Col
  else
  if comp is TSpeedButton then
    TSpeedButton(comp).Color:=Col
  else
  if comp is TLabel then
    TLabel(comp).Color:=Col
  else
  if comp is TEdit then
    TEdit(comp).Color:=Col
  else
  if comp is TCheckBox then
    TCheckBox(comp).Color:=Col
  else
  if comp is TRadioButton then
    TRadioButton(comp).Color:=Col
  else
  if comp is TListBox then
    TListBox(comp).Color:=Col
  else
  if comp is TComboBox then
    TComboBox(comp).Color:=Col
  else
  if comp is TGroupBox then
    TGroupBox(comp).Color:=Col
  else
  if comp is TRadioGroup then
    TRadioGroup(comp).Color:=Col
  else
  if comp is TPanel then
    TPanel(comp).Color:=Col
  else
  if comp is TBevel then
    TBevel(comp).Color:=Col
end;

Function GetFillColor(comp:TComponent):TColor;
begin
  Result:=clBlack;
  if comp is TShape then
    Result:=TShape(comp).Brush.Color
  else
  if comp is TButton then
    Result:=TButton(comp).Color
  else
  if comp is TBitBtn then
    Result:=TBitBtn(comp).Color
  else
  if comp is TSpeedButton then
    Result:=TSpeedButton(comp).Color
  else
  if comp is TLabel then
    Result:=TLabel(comp).Color
  else
  if comp is TEdit then
    Result:=TEdit(comp).Color
  else
  if comp is TCheckBox then
    Result:=TCheckBox(comp).Color
  else
  if comp is TRadioButton then
    Result:=TRadioButton(comp).Color
  else
  if comp is TListBox then
    Result:=TListBox(comp).Color
  else
  if comp is TComboBox then
    Result:=TComboBox(comp).Color
  else
  if comp is TGroupBox then
    Result:=TGroupBox(comp).Color
  else
  if comp is TRadioGroup then
    Result:=TRadioGroup(comp).Color
  else
  if comp is TPanel then
    Result:=TPanel(comp).Color
  else
  if comp is TBevel then
    Result:=TBevel(comp).Color
end;

procedure TForm1.ShowFillColor(ControlElementObj:TObject);
var
  comp:TControl;
  tg:TTag;
  ce:TControlElement;
  col:TColor;
begin
  ce:=TControlElement(ControlElementObj);
  comp:=TControl(ce.Component);

  tg:=ce.Colors.FillColor.Tag;

  if (tg.Status=0) then
  begin
    if tg.Value>0 then
      Col:=ce.Colors.FillColor.OnColor
    else
      Col:=ce.Colors.FillColor.OffColor;

    SetFillColor(comp,col);
  end;
end;

Procedure SetTextColor(Comp:TComponent;Col:TColor);
begin
  if comp is TButton then
    TButton(comp).Font.Color:=Col
  else
  if comp is TBitBtn then
    TBitBtn(comp).Font.Color:=Col
  else
  if comp is TSpeedButton then
    TSpeedButton(comp).Font.Color:=Col
  else
  if comp is TLabel then
    TLabel(comp).Font.Color:=Col
  else
  if comp is TEdit then
    TEdit(comp).Font.Color:=Col
  else
  if comp is TCheckBox then
    TCheckBox(comp).Font.Color:=Col
  else
  if comp is TRadioButton then
    TRadioButton(comp).Font.Color:=Col
  else
  if comp is TListBox then
    TListBox(comp).Font.Color:=Col
  else
  if comp is TComboBox then
    TComboBox(comp).Font.Color:=Col
  else
  if comp is TGroupBox then
    TGroupBox(comp).Font.Color:=Col
  else
  if comp is TRadioGroup then
    TRadioGroup(comp).Font.Color:=Col
  else
  if comp is TPanel then
    TPanel(comp).Font.Color:=Col
  else
  if comp is TBevel then
    TBevel(comp).Font.Color:=Col
end;

Function GetTextColor(Comp:TComponent):TColor;
begin
  Result:=clBlack;
  if comp is TButton then
    Result:=TButton(comp).Font.Color
  else
  if comp is TBitBtn then
    Result:=TBitBtn(comp).Font.Color
  else
  if comp is TSpeedButton then
    Result:=TSpeedButton(comp).Font.Color
  else
  if comp is TLabel then
    Result:=TLabel(comp).Font.Color
  else
  if comp is TEdit then
    Result:=TEdit(comp).Font.Color
  else
  if comp is TCheckBox then
    Result:=TCheckBox(comp).Font.Color
  else
  if comp is TRadioButton then
    Result:=TRadioButton(comp).Font.Color
  else
  if comp is TListBox then
    Result:=TListBox(comp).Font.Color
  else
  if comp is TComboBox then
    Result:=TComboBox(comp).Font.Color
  else
  if comp is TGroupBox then
    Result:=TGroupBox(comp).Font.Color
  else
  if comp is TRadioGroup then
    Result:=TRadioGroup(comp).Font.Color
  else
  if comp is TPanel then
    Result:=TPanel(comp).Font.Color
  else
  if comp is TBevel then
    Result:=TBevel(comp).Font.Color
end;

procedure TForm1.ShowTextColor(ControlElementObj:TObject);
var
  comp:TControl;
  tg:TTag;
  ce:TControlElement;
  col:TColor;
begin
  ce:=TControlElement(ControlElementObj);
  comp:=TControl(ce.Component);

  tg:=ce.Colors.TextColor.Tag;

  if (tg.Status=0) then
  begin
    if tg.Value>0 then
      Col:=ce.Colors.TextColor.OnColor
    else
      Col:=ce.Colors.TextColor.OffColor;

    SetTextColor(Comp,Col);
  end;
end;

procedure TForm1.ShowVisible(ControlElementObj:TObject);
var
  comp:TControl;
  tg:TTag;
  ce:TControlElement;
  flVisible:Boolean;
begin
  flVisible:=True;
  ce:=TControlElement(ControlElementObj);
  comp:=TControl(ce.Component);

  tg:=ce.Miscellaneous.Visibility.Tag;
  if (Assigned(tg)) and (tg.Status=0) then
  begin
    if (tg.Value>0)=ce.Miscellaneous.Visibility.VisibleState then
      flVisible:=True
    else
      flVisible:=False;
  end;

  tg:=ce.Miscellaneous.Blink.Tag;
  if (Assigned(tg)) and (tg.Status=0) then
  begin
    if (tg.Value>0) and (flVisible) then
    begin
      if TimerExpired(ce.Miscellaneous.Blink.BlinkTimer) then
      begin
        if CompareText(ce.Miscellaneous.Blink.BlinkType,'BlinkInvisible')=0 then
        begin
          if TControl(comp).Visible then
            flVisible:=false
          else
            flVisible:=true;
        end
        else
        if CompareText(ce.Miscellaneous.Blink.BlinkType,'BlinkVisibleWithColors')=0 then
        begin
          ce.Miscellaneous.Blink.Flash:=not ce.Miscellaneous.Blink.Flash;
          if ce.Miscellaneous.Blink.Flash then
          begin
            ce.Miscellaneous.Blink.PrevTextColor:=GetTextColor(Comp);
            SetTextColor(Comp,ce.Miscellaneous.Blink.TextColor);
            ce.Miscellaneous.Blink.PrevFillColor:=GetFillColor(Comp);
            SetFillColor(Comp,ce.Miscellaneous.Blink.FillColor);
            ce.Miscellaneous.Blink.PrevLineColor:=GetLineColor(Comp);
            SetLineColor(Comp,ce.Miscellaneous.Blink.LineColor);
          end
          else
          begin
            SetTextColor(Comp,ce.Miscellaneous.Blink.PrevTextColor);
            SetFillColor(Comp,ce.Miscellaneous.Blink.PrevFillColor);
            SetLineColor(Comp,ce.Miscellaneous.Blink.PrevLineColor);
          end;
        end;
        NewTimerSecs(ce.Miscellaneous.Blink.BlinkTimer,ce.Miscellaneous.Blink.BlinkSpeed);
      end;
    end;
  end;

  if TControl(comp).Visible<>flVisible then
    TControl(comp).Visible:=flVisible;
end;

procedure TForm1.ShowDisable(ControlElementObj:TObject);
var
  comp:TControl;
  tg:TTag;
  ce:TControlElement;
begin
  ce:=TControlElement(ControlElementObj);
  comp:=TControl(ce.Component);

  tg:=ce.Miscellaneous.Disable.Tag;

  if (tg.Status=0) then
  begin
    if (tg.Value>0)=ce.Miscellaneous.Disable.DisableState then
      TControl(comp).Enabled:=false
    else
      TControl(comp).Enabled:=true;
  end;
end;



initialization

end.

