unit UnitFormsConfig;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs,
  ExtCtrls, StdCtrls, ComCtrls, Buttons,
  TAGraph, TASeries,
  DOM,
  UnitTags, UnitTimers;

type
  TFormConfig=class;//forward

  TEnterValue=record
    TagName:String;
    //Runtime
    Tag:TTag;
  end;

  TSetDiscreteValue=record
    TagName:String;
    Action:String;//Direct, Reverse, Toggle, Set, Reset
    //Runtime
    Tag:TTag;
  end;

  TWindowRec=record
    ID:String;
    Form:TFormConfig;
  end;

  TShowWindows=record
    Action:String;//Show, Hide
    Windows:Array of TWindowRec;
  end;

  TShowMotorWindow=record
    WindowCaption:String;
    StatusTagName:String;
    ManualStartTagName:String;
    //Runtime
    StatusTag:TTag;
    ManualStartTag:TTag;
  end;

  //<ShowRegulatorWindow HiHiBit="" LoLoBit="" ManAutButton="" ExtSpButton="" Output="" PV="" ForceBit="" OpSp="" ExtSp="" ActSp="" HiHiSp="" LoLoSp="" />
  TShowRegulatorWindow=record
    DisplayFormat:String;
    HiHiBit:String;
    LoLoBit:String;
    ManAutButton:String;
    ExtSpButton:String;
    Output:String;
    PV:String;
    ForceBit:String;
    OpSp:String;
    ExtSp:String;
    ActSp:String;
    HiHiSp:String;
    LoLoSp:String;
    //Runtime
    HiHiBit_tag:TTag;
    LoLoBit_tag:TTag;
    ManAutButton_tag:TTag;
    ExtSpButton_tag:TTag;
    Output_tag:TTag;
    PV_tag:TTag;
    ForceBit_tag:TTag;
    OpSp_tag:TTag;
    ExtSp_tag:TTag;
    ActSp_tag:TTag;
    HiHiSp_tag:TTag;
    LoLoSp_tag:TTag;
  end;

  TOnClick=record
    ActionType:String;//None, SetDiscreteValue, ActionScript, ShowWindows, EnterValue
    SetDiscreteValue:TSetDiscreteValue;
    ActionScript:String;//Not used yet
    ShowWindows:TShowWindows;
    EnterValue:TEnterValue;
    ShowMotorWindow:TShowMotorWindow;
    ShowRegulatorWindow:TShowRegulatorWindow;
  end;

  TDiscrete=record
    OnMessage:String;
    OffMessage:String;
  end;

  TAnalog=record
    DisplayFormat:String;
  end;

  TValueDisplay=record
    DisplayType:String;
    Discrete:TDiscrete;
    Analog:TAnalog;
    TagName:String;
    //Runtime
    Tag:TTag;
  end;

  TOnChange=record
    TagName:String;
    //Runtime
    Tag:TTag;
  end;

  TSetColor=record
    TagName:String;
    OnColor:TColor;
    OffColor:TColor;
    //Runtime
    Tag:TTag;
  end;

  TColors=record
    LineColor:TSetColor;
    FillColor:TSetColor;
    TextColor:TSetColor;
  end;

  TVisibility=record
    TagName:String;
    VisibleState:Boolean;
    //Runtime
    Tag:TTag;
  end;

  TDisable=record
    TagName:String;
    DisableState:Boolean;
    //Runtime
    Tag:TTag;
  end;

  TBlink=record
    TagName:String;
    BlinkSpeed:Integer;//1s
    BlinkType:String;//BlinkInvisible, BlinkVisibleWithColors
    TextColor:TColor;
    LineColor:TColor;
    FillColor:TColor;
    //Runtime
    Tag:TTag;
    BlinkTimer:EventTimer;
    Flash:Boolean;
    PrevTextColor:TColor;
    PrevLineColor:TColor;
    PrevFillColor:TColor;
  end;

  TMiscellaneous=record
    Visibility:TVisibility;
    Disable:TDisable;
    Blink:TBlink;
  end;

  TControlElement=class
    ControlName:String;
    OnClick:TOnClick;
    OnChange:TOnChange;
    ValueDisplay:TValueDisplay;
    Colors:TColors;
    Miscellaneous:TMiscellaneous;
    //Runtime
    Component:TComponent;
    EditTimer:EventTimer;
  end;

  TControlElements=class(TList)
    function GetControlElementByName(ctrl_name:String):TControlElement;
  end;

  TFormConfig=class
  public
    ID:String;
    FileName:String;
    Open:Boolean;
    Left,Top:Integer;
    ControlElements:TControlElements;
    //Runtime
    Form:TForm;
  end;

  TFormConfigs=class(TList)
  private
    procedure OnFindClass(Reader: TReader; const AClassName: string; var ComponentClass: TComponentClass);
    function LoadForm(file_name:String):TForm;
    function LoadOnClick(ControlElement:TControlElement;ndControlElement:TDOMNode):Boolean;
    function LoadValueDisplay(ControlElement:TControlElement;ndControlElement:TDOMNode):Boolean;
  public
    Color:TColor;
    constructor Create;
    procedure LoadConfig(file_name:String);
    function GetFormConfigByID(ID:String):TFormConfig;
    function GetFormConfigByTForm(frm:TForm):TFormConfig;
    procedure AssignOnMouseUpDown(comp:TComponent);
    procedure AssignWindows;

    procedure TrackBarChange(Sender: TObject);
    procedure CheckBoxChange(Sender: TObject);
    procedure EditKeyPress(Sender: TObject; var Key: char);
    procedure DoOnClick(Sender: TObject);
    procedure DoOnMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
    procedure DoOnMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
  end;

implementation

Uses
    XMLRead,
    LResources,
    HTMLColorConversions,
    UnitConfig,
    UnitThreadLog,
    UnitEnterValue,
    UnitMotor,
    UnitRegulator;

function TControlElements.GetControlElementByName(ctrl_name:String):TControlElement;
var
  item:TControlElement;
  i:Integer;
begin
  Result:=nil;
  for i:= 0 to (Count - 1) do
  begin
    item:=TControlElement(Items[i]);
    if item.ControlName=ctrl_name then
    begin
      Result:=item;
      break;
    end;
  end;
end;

constructor TFormConfigs.Create;
begin
  Inherited Create;
end;

procedure TFormConfigs.OnFindClass(Reader: TReader; const AClassName: string; var ComponentClass: TComponentClass);
begin
  if CompareText(AClassName,'TGroupBox')=0 then
    ComponentClass:=TGroupBox
  else if CompareText(AClassName,'TCheckBox')=0 then
    ComponentClass:=TCheckBox
  else if CompareText(AClassName,'TLabel')=0 then
    ComponentClass:=TLabel
  else if CompareText(AClassName,'TEdit')=0 then
    ComponentClass:=TEdit
  else if CompareText(AClassName,'TRadioButton')=0 then
    ComponentClass:=TRadioButton
  else if CompareText(AClassName,'TButton')=0 then
    ComponentClass:=TButton
  else if CompareText(AClassName,'TMemo')=0 then
    ComponentClass:=TMemo
  else if CompareText(AClassName,'TListBox')=0 then
    ComponentClass:=TListBox
  else if CompareText(AClassName,'TComboBox')=0 then
    ComponentClass:=TComboBox
  else if CompareText(AClassName,'TRadioGroup')=0 then
    ComponentClass:=TRadioGroup
  else if CompareText(AClassName,'TCheckGroup')=0 then
    ComponentClass:=TCheckGroup
  else if CompareText(AClassName,'TPanel')=0 then
    ComponentClass:=TPanel
  else if CompareText(AClassName,'TImage')=0 then
    ComponentClass:=TImage
  else if CompareText(AClassName,'TShape')=0 then
    ComponentClass:=TShape
  else if CompareText(AClassName,'TBevel')=0 then
    ComponentClass:=TBevel
  else if CompareText(AClassName,'TProgressBar')=0 then
    ComponentClass:=TProgressBar
  else if CompareText(AClassName,'TTrackBar')=0 then
    ComponentClass:=TTrackBar
  else if CompareText(AClassName,'TBitBtn')=0 then
    ComponentClass:=TBitBtn
  else if CompareText(AClassName,'TSpeedButton')=0 then
    ComponentClass:=TSpeedButton
  else if CompareText(copy(AClassName,1,5),'TForm')=0 then
    ComponentClass:=TForm
  else if CompareText(AClassName,'TChart')=0 then
    ComponentClass:=TChart
  else if CompareText(AClassName,'TLineSeries')=0 then
    ComponentClass:=TLineSeries
end;

function TFormConfigs.GetFormConfigByID(ID:String):TFormConfig;
var
  item:TFormConfig;
  i:Integer;
begin
  Result:=nil;
  for i:= 0 to (Count - 1) do
  begin
    item:=TFormConfig(Items[i]);
    if item.ID=ID then
    begin
      Result:=item;
      break;
    end;
  end;
end;

function TFormConfigs.GetFormConfigByTForm(frm:TForm):TFormConfig;
var
  item:TFormConfig;
  i:Integer;
begin
  Result:=nil;
  for i:= 0 to (Count - 1) do
  begin
    item:=TFormConfig(Items[i]);
    if item.Form=frm then
    begin
      Result:=item;
      break;
    end;
  end;
end;

function TFormConfigs.LoadForm(file_name:String):TForm;
var
  AStream:TFileStream;
begin
  Result:=nil;

  AStream:=TFileStream.Create(file_name,fmOpenRead);
  try
    AStream.Size:=0;
    AStream.Position:=0;
    ReadComponentFromTextStream(AStream,Result,@OnFindClass,nil);
  finally
    AStream.Free;
  end;
end;

procedure TFormConfigs.TrackBarChange(Sender: TObject);
var
  ctrl:TTrackBar;
  i:Integer;
  form:TFormConfig;
  ControlElement:TControlElement;
begin
  ctrl:=TTrackBar(Sender);
  form:=GetFormConfigByTForm(TForm(ctrl.Parent));
  if Assigned(form) then
  begin
    ControlElement:=form.ControlElements.GetControlElementByName(ctrl.Name);
    if Assigned(ControlElement) then
    begin
      TagsLock;
      try
        ControlElement.OnChange.Tag.Status:=0;
        ControlElement.OnChange.Tag.SetValue:=ctrl.Position;
        ControlElement.OnChange.Tag.Value:=ctrl.Position;
        ControlElement.OnChange.Tag.NeedSet:=True;
      finally
        TagsUnLock;
      end;
    end;
  end;
end;

procedure TFormConfigs.CheckBoxChange(Sender: TObject);
var
  ctrl:TCheckBox;
  i:Integer;
  form:TFormConfig;
  ControlElement:TControlElement;
begin
  ctrl:=TCheckBox(Sender);
  form:=GetFormConfigByTForm(TForm(ctrl.Parent));
  if Assigned(form) then
  begin
    ControlElement:=form.ControlElements.GetControlElementByName(ctrl.Name);
    if Assigned(ControlElement) then
    begin
      TagsLock;
      try
        ControlElement.OnChange.Tag.Status:=0;
        ControlElement.OnChange.Tag.SetValue:=Ord(ctrl.Checked);
        ControlElement.OnChange.Tag.Value:=ControlElement.OnChange.Tag.SetValue;
        ControlElement.OnChange.Tag.NeedSet:=True;
      finally
        TagsUnLock;
      end;
    end;
  end;
end;

procedure TFormConfigs.EditKeyPress(Sender: TObject; var Key: char);
var
  ctrl:TEdit;
  i:Integer;
  form:TFormConfig;
  ControlElement:TControlElement;
begin
  ctrl:=TEdit(Sender);
  ControlElement:=nil;
  form:=GetFormConfigByTForm(TForm(ctrl.Parent));
  if Assigned(form) then
    ControlElement:=form.ControlElements.GetControlElementByName(ctrl.Name);
  if Assigned(ControlElement) then
  begin
    TagsLock;
    try
      NewTimerSecs(ControlElement.EditTimer,5);
    finally
      TagsUnLock;
    end;
    if Key=#13 then
    begin
      TagsLock;
        try
          ControlElement.OnChange.Tag.SetValue:=TryStrToFloat(ctrl.Text);
          ControlElement.OnChange.Tag.Value:=ControlElement.OnChange.Tag.SetValue;
          ControlElement.OnChange.Tag.Status:=0;
          ControlElement.OnChange.Tag.NeedSet:=True;
        except
        end;
      TagsUnLock;
    end;
  end;
end;

function GetForm(ctrl:TWinControl):TWinControl;
begin
  Result:=ctrl;
  if ( not(ctrl is TForm)) and (Assigned(ctrl.Parent)) then
    Result:=GetForm(ctrl.Parent);
end;

procedure TFormConfigs.DoOnClick(Sender: TObject);
var
  ctrl:TControl;
  i,k:Integer;
  form:TFormConfig;
  ControlElement:TControlElement;
  fl:Boolean;
  par:TWinControl;
begin
  ctrl:=TControl(Sender);
  par:=GetForm(TWinControl(ctrl));
  form:=GetFormConfigByTForm(TForm(par));
  if Assigned(form) then
  begin
    ControlElement:=form.ControlElements.GetControlElementByName(ctrl.Name);
    if Assigned(ControlElement) then
    begin
      if CompareText(ControlElement.OnClick.ActionType,'SetDiscreteValue')=0 then
      begin
        //Direct, Reverse, Toggle, Set, Reset
        if CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Toggle')=0 then
        begin
          TagsLock;
          try
            fl:=ControlElement.OnClick.SetDiscreteValue.Tag.Value>0;
            ControlElement.OnClick.SetDiscreteValue.Tag.SetValue:=ord(not fl);
            ControlElement.OnClick.SetDiscreteValue.Tag.Value:=ControlElement.OnClick.SetDiscreteValue.Tag.SetValue;
            ControlElement.OnClick.SetDiscreteValue.Tag.Status:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.NeedSet:=True;
          finally
            TagsUnLock;
          end;
        end
        else
        if CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Set')=0 then
        begin
          TagsLock;
          try
            ControlElement.OnClick.SetDiscreteValue.Tag.SetValue:=1;
            ControlElement.OnClick.SetDiscreteValue.Tag.Value:=1;
            ControlElement.OnClick.SetDiscreteValue.Tag.Status:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.NeedSet:=True;
          finally
            TagsUnLock;
          end;
        end
        else
        if CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Reset')=0 then
        begin
          TagsLock;
          try
            ControlElement.OnClick.SetDiscreteValue.Tag.SetValue:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.Value:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.Status:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.NeedSet:=True;
          finally
            TagsUnLock;
          end;
        end;
      end
      else
      if CompareText(ControlElement.OnClick.ActionType,'ShowWindows')=0 then
      begin
        for k:=0 to Length(ControlElement.OnClick.ShowWindows.Windows)-1 do
        begin
          if (Assigned(ControlElement.OnClick.ShowWindows.Windows[k].Form)) and
            (Assigned(ControlElement.OnClick.ShowWindows.Windows[k].Form.Form)) then
          begin
            if CompareText(ControlElement.OnClick.ShowWindows.Action,'Show')=0 then
            begin
              ControlElement.OnClick.ShowWindows.Windows[k].Form.Form.Show;
              ControlElement.OnClick.ShowWindows.Windows[k].Form.Form.BringToFront;
            end
            else
            if CompareText(ControlElement.OnClick.ShowWindows.Action,'Hide')=0 then
              ControlElement.OnClick.ShowWindows.Windows[k].Form.Form.Hide;
          end
          else
            ShowMessage('Form('+ControlElement.OnClick.ShowWindows.Windows[k].Form.FileName+')=nil');
        end;
      end
      else
      if CompareText(ControlElement.OnClick.ActionType,'EnterValue')=0 then
      begin
        FormEnterValue.ShowTag(ControlElement.OnClick.EnterValue.Tag);
      end
      else
      if CompareText(ControlElement.OnClick.ActionType,'ShowMotorWindow')=0 then
      begin
        FormMotor.ShowMotor(ControlElement.OnClick.ShowMotorWindow);
      end
      else
      if CompareText(ControlElement.OnClick.ActionType,'ShowRegulatorWindow')=0 then
      begin
        FormRegulator.ShowRegulator(par.Parent,ControlElement.OnClick.ShowRegulatorWindow);
      end
    end;
  end;
end;

procedure TFormConfigs.DoOnMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  ctrl:TControl;
  i:Integer;
  form:TFormConfig;
  ControlElement:TControlElement;
  fl:Boolean;
begin
  ctrl:=TControl(Sender);
  form:=GetFormConfigByTForm(TForm(ctrl.Parent));
  if Assigned(form) then
  begin
    ControlElement:=form.ControlElements.GetControlElementByName(ctrl.Name);
    if Assigned(ControlElement) then
    begin
      if CompareText(ControlElement.OnClick.ActionType,'SetDiscreteValue')=0 then
      begin
        //Direct, Reverse, Toggle, Set, Reset
        if CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Direct')=0 then
        begin
          TagsLock;
          try
            ControlElement.OnClick.SetDiscreteValue.Tag.SetValue:=1;
            ControlElement.OnClick.SetDiscreteValue.Tag.Value:=1;
            ControlElement.OnClick.SetDiscreteValue.Tag.Status:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.NeedSet:=True;
          finally
            TagsUnLock;
          end;
        end
        else
        if CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Reverse')=0 then
        begin
          TagsLock;
          try
            ControlElement.OnClick.SetDiscreteValue.Tag.SetValue:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.Value:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.Status:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.NeedSet:=True;
          finally
            TagsUnLock;
          end;
        end
      end;
    end;
  end;
end;

procedure TFormConfigs.DoOnMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  ctrl:TControl;
  form:TFormConfig;
  ControlElement:TControlElement;
begin
  ctrl:=TControl(Sender);
  form:=GetFormConfigByTForm(TForm(ctrl.Parent));
  if Assigned(form) then
  begin
    ControlElement:=form.ControlElements.GetControlElementByName(ctrl.Name);
    if Assigned(ControlElement) then
    begin
      if CompareText(ControlElement.OnClick.ActionType,'SetDiscreteValue')=0 then
      begin
        //Direct, Reverse, Toggle, Set, Reset
        if CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Direct')=0 then
        begin
          TagsLock;
          try
            ControlElement.OnClick.SetDiscreteValue.Tag.SetValue:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.Value:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.Status:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.NeedSet:=True;
          finally
            TagsUnLock;
          end;
        end
        else
        if CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Reverse')=0 then
        begin
          TagsLock;
          try
            ControlElement.OnClick.SetDiscreteValue.Tag.SetValue:=1;
            ControlElement.OnClick.SetDiscreteValue.Tag.Value:=1;
            ControlElement.OnClick.SetDiscreteValue.Tag.Status:=0;
            ControlElement.OnClick.SetDiscreteValue.Tag.NeedSet:=True;
          finally
            TagsUnLock;
          end;
        end
      end;
    end;
  end;
end;

function LoadTag(nd:TDOMNode;node_name:String;var tag_name:String):TTag;
begin
  tag_name:=nd.Attributes.GetNamedItem(node_name).NodeValue;
  if tag_name<>'' then
    Result:=cfg.Tags.GetTagByName(tag_name)
  else
    Result:=nil;
end;

function TFormConfigs.LoadOnClick(ControlElement:TControlElement;ndControlElement:TDOMNode):Boolean;
var
  i,k,n,len:Integer;
  nd1,nd2,nd3:TDOMNode;
begin
  Result:=false;
  ControlElement.OnClick.ActionType:='None';
  for i:=0 to ndControlElement.ChildNodes.Count-1 do
  begin
    nd1:=ndControlElement.ChildNodes[i];
    if CompareText(nd1.NodeName,'OnClick')=0 then
    begin
      //None, SetDiscreteValue, ActionScript, ShowWindows
      ControlElement.OnClick.ActionType:=nd1.Attributes.GetNamedItem('ActionType').NodeValue;
      for k:=0 to nd1.ChildNodes.Count-1 do
      begin
        nd2:=nd1.ChildNodes[k];
        if CompareText(nd2.NodeName,'SetDiscreteValue')=0 then
        begin
          ControlElement.OnClick.SetDiscreteValue.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.OnClick.SetDiscreteValue.TagName<>'' then
            ControlElement.OnClick.SetDiscreteValue.Tag:=cfg.Tags.GetTagByName(ControlElement.OnClick.SetDiscreteValue.TagName);
          if Assigned(ControlElement.OnClick.SetDiscreteValue.Tag) then
          begin
            ControlElement.OnClick.SetDiscreteValue.Action:=nd2.Attributes.GetNamedItem('Action').NodeValue;
            if CompareText(ControlElement.OnClick.ActionType,'SetDiscreteValue')=0 then
              Result:=True;
          end
          else
            ControlElement.OnClick.SetDiscreteValue.Action:='None';
        end
        else
        if CompareText(nd2.NodeName,'ActionScript')=0 then
        begin
          if Assigned(nd2.FirstChild) then
          begin
            ControlElement.OnClick.ActionScript:=trim(nd2.FirstChild.NodeValue);
            if CompareText(ControlElement.OnClick.ActionType,'ActionScript')=0 then
              Result:=True;
          end
          else
            ControlElement.OnClick.ActionScript:='';
        end
        else
        if CompareText(nd2.NodeName,'ShowWindows')=0 then
        begin
          ControlElement.OnClick.ShowWindows.Action:=nd2.Attributes.GetNamedItem('Action').NodeValue;
          len:=0;
          SetLength(ControlElement.OnClick.ShowWindows.Windows,len);
          for n:=0 to nd2.ChildNodes.Count-1 do
          begin
            nd3:=nd2.ChildNodes[n];
            if CompareText(nd3.NodeName,'Window')=0 then
            begin
              SetLength(ControlElement.OnClick.ShowWindows.Windows,len+1);
              ControlElement.OnClick.ShowWindows.Windows[len].ID:=nd3.Attributes.GetNamedItem('ID').NodeValue;
              if CompareText(ControlElement.OnClick.ActionType,'ShowWindows')=0 then
                Result:=True;
              inc(len);
            end;
          end;
        end
        else
        if CompareText(nd2.NodeName,'EnterValue')=0 then
        begin
          ControlElement.OnClick.EnterValue.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.OnClick.EnterValue.TagName<>'' then
            ControlElement.OnClick.EnterValue.Tag:=cfg.Tags.GetTagByName(ControlElement.OnClick.EnterValue.TagName);
          if (Assigned(ControlElement.OnClick.EnterValue.Tag)) and
            (CompareText(ControlElement.OnClick.ActionType,'EnterValue')=0) then
            Result:=True;
        end
        else
        if CompareText(nd2.NodeName,'ShowMotorWindow')=0 then
        begin
          ControlElement.OnClick.ShowMotorWindow.WindowCaption:=nd2.Attributes.GetNamedItem('WindowCaption').NodeValue;

          ControlElement.OnClick.ShowMotorWindow.StatusTagName:=nd2.Attributes.GetNamedItem('StatusTagName').NodeValue;
          if ControlElement.OnClick.ShowMotorWindow.StatusTagName<>'' then
            ControlElement.OnClick.ShowMotorWindow.StatusTag:=cfg.Tags.GetTagByName(ControlElement.OnClick.ShowMotorWindow.StatusTagName);

          ControlElement.OnClick.ShowMotorWindow.ManualStartTagName:=nd2.Attributes.GetNamedItem('ManualStartTagName').NodeValue;
          if ControlElement.OnClick.ShowMotorWindow.ManualStartTagName<>'' then
            ControlElement.OnClick.ShowMotorWindow.ManualStartTag:=cfg.Tags.GetTagByName(ControlElement.OnClick.ShowMotorWindow.ManualStartTagName);

          if (Assigned(ControlElement.OnClick.ShowMotorWindow.StatusTag)) and
            (Assigned(ControlElement.OnClick.ShowMotorWindow.ManualStartTag)) and
            (CompareText(ControlElement.OnClick.ActionType,'ShowMotorWindow')=0) then
            Result:=True;
        end
        else
        if CompareText(nd2.NodeName,'ShowRegulatorWindow')=0 then
        begin
          with ControlElement.OnClick.ShowRegulatorWindow do
          begin
            DisplayFormat:=nd2.Attributes.GetNamedItem('DisplayFormat').NodeValue;
            HiHiBit:=nd2.Attributes.GetNamedItem('HiHiBit').NodeValue;
            LoLoBit:=nd2.Attributes.GetNamedItem('LoLoBit').NodeValue;
            ManAutButton:=nd2.Attributes.GetNamedItem('ManAutButton').NodeValue;
            ExtSpButton:=nd2.Attributes.GetNamedItem('ExtSpButton').NodeValue;
            Output:=nd2.Attributes.GetNamedItem('Output').NodeValue;
            PV:=nd2.Attributes.GetNamedItem('PV').NodeValue;
            ForceBit:=nd2.Attributes.GetNamedItem('ForceBit').NodeValue;
            OpSp:=nd2.Attributes.GetNamedItem('OpSp').NodeValue;
            ExtSp:=nd2.Attributes.GetNamedItem('ExtSp').NodeValue;
            ActSp:=nd2.Attributes.GetNamedItem('ActSp').NodeValue;
            HiHiSp:=nd2.Attributes.GetNamedItem('HiHiSp').NodeValue;
            LoLoSp:=nd2.Attributes.GetNamedItem('LoLoSp').NodeValue;

            HiHiBit_tag:=LoadTag(nd2,'HiHiBit',HiHiBit);
            LoLoBit_tag:=LoadTag(nd2,'LoLoBit',LoLoBit);
            ManAutButton_tag:=LoadTag(nd2,'ManAutButton',ManAutButton);
            ExtSpButton_tag:=LoadTag(nd2,'ExtSpButton',ExtSpButton);
            Output_tag:=LoadTag(nd2,'Output',Output);
            PV_tag:=LoadTag(nd2,'PV',PV);
            ForceBit_tag:=LoadTag(nd2,'ForceBit',ForceBit);
            OpSp_tag:=LoadTag(nd2,'OpSp',OpSp);
            ExtSp_tag:=LoadTag(nd2,'ExtSp',ExtSp);
            ActSp_tag:=LoadTag(nd2,'ActSp',ActSp);
            HiHiSp_tag:=LoadTag(nd2,'HiHiSp',HiHiSp);
            LoLoSp_tag:=LoadTag(nd2,'LoLoSp',LoLoSp);
          end;
          if (CompareText(ControlElement.OnClick.ActionType,'ShowRegulatorWindow')=0) then
            Result:=True;
        end;
      end;
      break;
    end;
  end;
end;

procedure TFormConfigs.AssignOnMouseUpDown(comp:TComponent);
begin
  if (comp is TButton) then
  begin
    TButton(comp).OnMouseDown:=@DoOnMouseDown;
    TButton(comp).OnMouseUp:=@DoOnMouseUp;
  end
  else
  if (comp is TBitBtn) then
  begin
    TBitBtn(comp).OnMouseDown:=@DoOnMouseDown;
    TBitBtn(comp).OnMouseUp:=@DoOnMouseUp;
  end
  else
  if (comp is TSpeedButton) then
  begin
    TSpeedButton(comp).OnMouseDown:=@DoOnMouseDown;
    TSpeedButton(comp).OnMouseUp:=@DoOnMouseUp;
  end
  else
  if (comp is TCheckBox) then
  begin
    TCheckBox(comp).OnMouseDown:=@DoOnMouseDown;
    TCheckBox(comp).OnMouseUp:=@DoOnMouseUp;
  end
  else
  if (comp is TPanel) then
  begin
    TPanel(comp).OnMouseDown:=@DoOnMouseDown;
    TPanel(comp).OnMouseUp:=@DoOnMouseUp;
  end
  else
  if (comp is TLabel) then
  begin
    TLabel(comp).OnMouseDown:=@DoOnMouseDown;
    TLabel(comp).OnMouseUp:=@DoOnMouseUp;
  end
  else
  if (comp is TImage) then
  begin
    TImage(comp).OnMouseDown:=@DoOnMouseDown;
    TImage(comp).OnMouseUp:=@DoOnMouseUp;
  end;
end;

procedure TFormConfigs.AssignWindows;
var
  i,j,k:Integer;
  form:TFormConfig;
  ControlElement:TControlElement;
begin
  for i:=0 to Count-1 do
  begin
    form:=TFormConfig(Items[i]);
    for j:=0 to form.ControlElements.Count-1 do
    begin
      ControlElement:=TControlElement(form.ControlElements[j]);
      for k:=0 to Length(ControlElement.OnClick.ShowWindows.Windows)-1 do
        ControlElement.OnClick.ShowWindows.Windows[k].Form:=GetFormConfigByID(ControlElement.OnClick.ShowWindows.Windows[k].ID);
    end;
  end;
end;

function TFormConfigs.LoadValueDisplay(ControlElement:TControlElement;ndControlElement:TDOMNode):Boolean;
var
  i,k:Integer;
  nd1,nd2:TDOMNode;
  s:String;
begin
  Result:=false;
  ControlElement.ValueDisplay.DisplayType:='None';
  for i:=0 to ndControlElement.ChildNodes.Count-1 do
  begin
    nd1:=ndControlElement.ChildNodes[i];
    if CompareText(nd1.NodeName,'OnChange')=0 then
    begin
      ControlElement.OnChange.TagName:=nd1.Attributes.GetNamedItem('TagName').NodeValue;
      if ControlElement.OnChange.TagName<>'' then
        ControlElement.OnChange.Tag:=cfg.Tags.GetTagByName(ControlElement.OnChange.TagName);
      if Assigned(ControlElement.OnChange.Tag) then
        Result:=True;
    end
    else
    if CompareText(nd1.NodeName,'Colors')=0 then
    begin
      for k:=0 to nd1.ChildNodes.Count-1 do
      begin
        nd2:=nd1.ChildNodes[k];
        if CompareText(nd2.NodeName,'LineColor')=0 then
        begin
          ControlElement.Colors.LineColor.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.Colors.LineColor.TagName<>'' then
            ControlElement.Colors.LineColor.Tag:=cfg.Tags.GetTagByName(ControlElement.Colors.LineColor.TagName);
          if Assigned(ControlElement.Colors.LineColor.Tag) then
          begin
            ControlElement.Colors.LineColor.OnColor:=HTML2Color(nd2.Attributes.GetNamedItem('OnColor').NodeValue);
            ControlElement.Colors.LineColor.OffColor:=HTML2Color(nd2.Attributes.GetNamedItem('OffColor').NodeValue);
            Result:=True;
          end;
        end
        else
        if CompareText(nd2.NodeName,'FillColor')=0 then
        begin
          ControlElement.Colors.FillColor.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.Colors.FillColor.TagName<>'' then
            ControlElement.Colors.FillColor.Tag:=cfg.Tags.GetTagByName(ControlElement.Colors.FillColor.TagName);
          if Assigned(ControlElement.Colors.FillColor.Tag) then
          begin
            ControlElement.Colors.FillColor.OnColor:=HTML2Color(nd2.Attributes.GetNamedItem('OnColor').NodeValue);
            ControlElement.Colors.FillColor.OffColor:=HTML2Color(nd2.Attributes.GetNamedItem('OffColor').NodeValue);
            Result:=True;
          end;
        end
        else
        if CompareText(nd2.NodeName,'TextColor')=0 then
        begin
          ControlElement.Colors.TextColor.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.Colors.TextColor.TagName<>'' then
            ControlElement.Colors.TextColor.Tag:=cfg.Tags.GetTagByName(ControlElement.Colors.TextColor.TagName);
          if Assigned(ControlElement.Colors.TextColor.Tag) then
          begin
            ControlElement.Colors.TextColor.OnColor:=HTML2Color(nd2.Attributes.GetNamedItem('OnColor').NodeValue);
            ControlElement.Colors.TextColor.OffColor:=HTML2Color(nd2.Attributes.GetNamedItem('OffColor').NodeValue);
            Result:=True;
          end;
        end;
      end;
    end
    else
    if CompareText(nd1.NodeName,'ValueDisplay')=0 then
    begin
      //None, Discrete, Analog
      ControlElement.ValueDisplay.DisplayType:=nd1.Attributes.GetNamedItem('DisplayType').NodeValue;
      ControlElement.ValueDisplay.TagName:=nd1.Attributes.GetNamedItem('TagName').NodeValue;
      if ControlElement.ValueDisplay.TagName<>'' then
        ControlElement.ValueDisplay.Tag:=cfg.Tags.GetTagByName(ControlElement.ValueDisplay.TagName);
      if Assigned(ControlElement.ValueDisplay.Tag) then
      begin
        for k:=0 to nd1.ChildNodes.Count-1 do
        begin
          nd2:=nd1.ChildNodes[k];
          if CompareText(nd2.NodeName,'Discrete')=0 then
          begin
            ControlElement.ValueDisplay.Discrete.OnMessage:=nd2.Attributes.GetNamedItem('OnMessage').NodeValue;
            ControlElement.ValueDisplay.Discrete.OffMessage:=nd2.Attributes.GetNamedItem('OffMessage').NodeValue;
            if CompareText(ControlElement.ValueDisplay.DisplayType,'Discrete')=0 then
              Result:=True;
          end
          else
          if CompareText(nd2.NodeName,'Analog')=0 then
          begin
            ControlElement.ValueDisplay.Analog.DisplayFormat:=nd2.Attributes.GetNamedItem('DisplayFormat').NodeValue;
            if CompareText(ControlElement.ValueDisplay.DisplayType,'Analog')=0 then
              Result:=True;
          end;
        end;
      end;
    end
    else
    if CompareText(nd1.NodeName,'Miscellaneous')=0 then
    begin
      for k:=0 to nd1.ChildNodes.Count-1 do
      begin
        nd2:=nd1.ChildNodes[k];
        if CompareText(nd2.NodeName,'Visibility')=0 then
        begin
          ControlElement.Miscellaneous.Visibility.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.Miscellaneous.Visibility.TagName<>'' then
            ControlElement.Miscellaneous.Visibility.Tag:=cfg.Tags.GetTagByName(ControlElement.Miscellaneous.Visibility.TagName);
          if Assigned(ControlElement.Miscellaneous.Visibility.Tag) then
          begin
            ControlElement.Miscellaneous.Visibility.VisibleState:=CompareText(nd2.Attributes.GetNamedItem('VisibleState').NodeValue,'True')=0;
            Result:=True;
          end;
        end
        else
        if CompareText(nd2.NodeName,'Disable')=0 then
        begin
          ControlElement.Miscellaneous.Disable.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.Miscellaneous.Disable.TagName<>'' then
            ControlElement.Miscellaneous.Disable.Tag:=cfg.Tags.GetTagByName(ControlElement.Miscellaneous.Disable.TagName);
          if Assigned(ControlElement.Miscellaneous.Disable.Tag) then
          begin
            ControlElement.Miscellaneous.Disable.DisableState:=CompareText(nd2.Attributes.GetNamedItem('DisableState').NodeValue,'True')=0;
            Result:=True;
          end;
        end
        else
        if CompareText(nd2.NodeName,'Blink')=0 then
        begin
          ControlElement.Miscellaneous.Blink.TagName:=nd2.Attributes.GetNamedItem('TagName').NodeValue;
          if ControlElement.Miscellaneous.Blink.TagName<>'' then
            ControlElement.Miscellaneous.Blink.Tag:=cfg.Tags.GetTagByName(ControlElement.Miscellaneous.Blink.TagName);
          if Assigned(ControlElement.Miscellaneous.Blink.Tag) then
          begin
            s:=nd2.Attributes.GetNamedItem('BlinkSpeed').NodeValue;
            if s[length(s)]='s' then
              System.Delete(s,length(s),1);
            try
              ControlElement.Miscellaneous.Blink.BlinkSpeed:=StrToInt(trim(s));
            except
              ControlElement.Miscellaneous.Blink.BlinkSpeed:=1;
            end;
            NewTimerSecs(ControlElement.Miscellaneous.Blink.BlinkTimer,ControlElement.Miscellaneous.Blink.BlinkSpeed);
            //BlinkInvisible, BlinkVisibleWithColors
            ControlElement.Miscellaneous.Blink.BlinkType:=nd2.Attributes.GetNamedItem('BlinkType').NodeValue;
            ControlElement.Miscellaneous.Blink.FillColor:=HTML2Color(nd2.Attributes.GetNamedItem('FillColor').NodeValue);
            ControlElement.Miscellaneous.Blink.LineColor:=HTML2Color(nd2.Attributes.GetNamedItem('LineColor').NodeValue);
            ControlElement.Miscellaneous.Blink.TextColor:=HTML2Color(nd2.Attributes.GetNamedItem('TextColor').NodeValue);
            ControlElement.Miscellaneous.Blink.Flash:=False;
            Result:=True;
          end;
        end
      end;
    end;
  end;
end;

procedure TFormConfigs.LoadConfig(file_name:String);
var
  Doc:TXMLDocument;
  ndForms,Child,nd2:TDOMNode;
  i,j,k:Integer;
  form:TFormConfig;
  s:String;
  ControlElement:TControlElement;
  comp:TComponent;
  fl1,fl2:Boolean;
begin
  ReadXMLFile(Doc, file_name);
  ndForms:=Doc.DocumentElement.FindNode('Forms');
  s:=ndForms.Attributes.GetNamedItem('Color').NodeValue;
  Color:=HTML2Color(s);
  for j := 0 to (ndForms.ChildNodes.Count - 1) do
  begin
    form:=TFormConfig.Create;
    child:=ndForms.ChildNodes[j];
    form.ID:=child.Attributes.GetNamedItem('ID').NodeValue;
    form.FileName:=child.Attributes.GetNamedItem('FileName').NodeValue;
    form.Open:=CompareText(child.Attributes.GetNamedItem('Open').NodeValue,'True')=0;
    form.Left:=StrToInt(child.Attributes.GetNamedItem('Left').NodeValue);
    form.Top:=StrToInt(child.Attributes.GetNamedItem('Top').NodeValue);

    form.Form:=LoadForm(ExtractFilePath(file_name)+form.FileName);
    if Assigned(form.Form) then
    begin
      form.Form.BorderStyle:=bsNone;
      //form.Form.Left:=abs(form.Left);
      //form.Form.Top:=form.Top;
      form.Form.Color:=Color;
      form.ControlElements:=TControlElements.Create;
      for k:=0 to (child.ChildNodes.Count-1) do
      begin
        nd2:=child.ChildNodes[k];
        ControlElement:=TControlElement.Create;
        ControlElement.ControlName:=nd2.Attributes.GetNamedItem('ElementName').NodeValue;
        NewTimerSecs(ControlElement.EditTimer,-10);
        fl1:=LoadOnClick(ControlElement,nd2);
        fl2:=LoadValueDisplay(ControlElement,nd2);
        if fl1 or fl2 then
        begin
          for i:=0 to form.Form.ComponentCount-1 do
          begin
            comp:=form.Form.Components[i];
            if CompareText(comp.Name,ControlElement.ControlName)=0 then
            begin
              ControlElement.Component:=comp;

              if comp is TTrackBar then
                TTrackBar(comp).OnChange:=@TrackBarChange
              else
              if comp is TCheckBox then
                TCheckBox(comp).OnChange:=@CheckBoxChange
              else
              if comp is TEdit then
                TEdit(comp).OnKeyPress:=@EditKeyPress;

              if (CompareText(ControlElement.OnClick.ActionType,'None')<>0)
                and (ControlElement.OnClick.ActionType<>'') then
                if (comp is TButton) or
                  (comp is TBitBtn) or
                  (comp is TSpeedButton) or
                  (comp is TCheckBox) or
                  (comp is TPanel) or
                  (comp is TLabel) or
                  (comp is TImage) or
                  (comp is TShape) or
                  (comp is TBevel)
                then
                begin
                  if (CompareText(ControlElement.OnClick.ActionType,'SetDiscreteValue')=0) and
                    ( (CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Direct')=0)or
                    (CompareText(ControlElement.OnClick.SetDiscreteValue.Action,'Reverse')=0) ) then
                  begin
                    AssignOnMouseUpDown(comp);
                  end
                  else
                    TControl(comp).OnClick:=@DoOnClick;
                end;

              break;
            end;
          end;
          form.ControlElements.Add(ControlElement);
        end;
      end;
    end;
    Add(form);
  end;
  AssignWindows;

  Doc.Free;
end;

end.

