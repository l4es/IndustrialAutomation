unit unittagdic;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, ExtCtrls,
  StdCtrls, ComCtrls,
  UnitTags;

type

  { TformTagDic }

  TformTagDic = class(TForm)
    btnAccessName: TButton;
    btnAlarmGroup: TButton;
    btnRight: TButton;
    btnNew: TButton;
    btnRestore: TButton;
    btnDelete: TButton;
    btnSelect: TButton;
    btnSave: TButton;
    btnLeft: TButton;
    btnCancel: TButton;
    btnClose: TButton;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    Button7: TButton;
    cbLogData: TCheckBox;
    cbLogEvents: TCheckBox;
    cbLoLo1: TCheckBox;
    cbLoLo2: TCheckBox;
    cbLoLo3: TCheckBox;
    cbLoLo4: TCheckBox;
    cbLoLo5: TCheckBox;
    cbLoLo6: TCheckBox;
    cbRetentiveValue: TCheckBox;
    cbTagType: TComboBox;
    cbLoLo: TCheckBox;
    edAlarmComment: TEdit;
    edComment: TEdit;
    edMaxRAW: TEdit;
    edAlarmLoLo: TEdit;
    edAlarmLow: TEdit;
    Edit13: TEdit;
    Edit14: TEdit;
    Edit15: TEdit;
    Edit16: TEdit;
    edAlarmHigh: TEdit;
    Edit18: TEdit;
    Edit19: TEdit;
    edOnMsg: TEdit;
    Edit20: TEdit;
    Edit21: TEdit;
    edAlarmHiHi: TEdit;
    Edit23: TEdit;
    Edit24: TEdit;
    Edit25: TEdit;
    Edit26: TEdit;
    Edit27: TEdit;
    Edit28: TEdit;
    Edit29: TEdit;
    edInitialValue1: TEdit;
    Edit30: TEdit;
    Edit31: TEdit;
    Edit32: TEdit;
    Edit33: TEdit;
    Edit34: TEdit;
    Edit37: TEdit;
    Edit39: TEdit;
    edEngUnits: TEdit;
    edDeadband: TEdit;
    edLogDead: TEdit;
    edMinEU: TEdit;
    edMaxEU: TEdit;
    edMinRAW: TEdit;
    edItem: TEdit;
    edOffMsg: TEdit;
    edTagName: TEdit;
    labAccessName: TLabel;
    labAlarmGroup: TLabel;
    Label1: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    Label18: TLabel;
    Label19: TLabel;
    Label2: TLabel;
    Label20: TLabel;
    Label21: TLabel;
    Label22: TLabel;
    Label23: TLabel;
    Label24: TLabel;
    Label25: TLabel;
    Label26: TLabel;
    Label27: TLabel;
    Label29: TLabel;
    Label3: TLabel;
    Label30: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    labOffMsg: TLabel;
    labOnMsg: TLabel;
    panToolbar: TPanel;
    pan10: TPanel;
    pan11: TPanel;
    pan12: TPanel;
    pan0: TPanel;
    pan3: TPanel;
    pan7: TPanel;
    pan1: TPanel;
    pan2: TPanel;
    pan4: TPanel;
    pan5: TPanel;
    pan9: TPanel;
    pan6: TPanel;
    rgReadWrite: TRadioGroup;
    RadioGroup2: TRadioGroup;
    rgACKModel: TRadioGroup;
    pan8: TRadioGroup;
    rgInitialValue: TRadioGroup;
    rgInputConversation: TRadioGroup;
    rgInitialValue2: TRadioGroup;
    procedure btnAccessNameClick(Sender: TObject);
    procedure btnDeleteClick(Sender: TObject);
    procedure btnLeftClick(Sender: TObject);
    procedure btnNewClick(Sender: TObject);
    procedure btnRightClick(Sender: TObject);
    procedure btnSaveClick(Sender: TObject);
    procedure cbTagTypeChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    procedure Save;
    procedure EnableLeftRight;
  public
    ActiveTag:TTag;
    procedure ShowPanels;
    procedure AlignPanelsByOrder;
    procedure ShowTag(tg:TTag);
  end;

var
  formTagDic: TformTagDic;

implementation

{$R *.lfm}

Uses
  UnitConfig, UnitAccessNames, unitaccessnameseditor;

{ TformTagDic }

procedure TformTagDic.FormCreate(Sender: TObject);
begin
  Height:=700;
  pan1.Align:=alNone;
  pan2.Align:=alNone;
  pan3.Align:=alNone;
  pan4.Align:=alNone;
  pan5.Align:=alNone;
  pan6.Align:=alNone;
  pan7.Align:=alNone;
  pan8.Align:=alNone;
  pan9.Align:=alNone;
  pan10.Align:=alNone;
  pan11.Align:=alNone;
  pan12.Align:=alNone;
end;

procedure TformTagDic.AlignPanelsByOrder;
const
  h=0;
var
  k:Integer;
begin
  k:=pan0.Top;
  if pan0.Visible then
    inc(k,pan0.Height+h);
  pan1.Top:=k;
  if pan1.Visible then
    inc(k,pan1.Height+h);
  pan2.Top:=k;
  if pan2.Visible then
    inc(k,pan2.Height+h);
  pan3.Top:=k;
  if pan3.Visible then
    inc(k,pan3.Height+h);
  pan4.Top:=k;
  if pan4.Visible then
    inc(k,pan4.Height+h);
  pan5.Top:=k;
  if pan5.Visible then
    inc(k,pan5.Height+h);
  pan6.Top:=k;
  if pan6.Visible then
    inc(k,pan6.Height+h);
  pan7.Top:=k;
  if pan7.Visible then
    inc(k,pan7.Height+h);
  pan8.Top:=k;
  if pan8.Visible then
    inc(k,pan8.Height+h);
  pan9.Top:=k;
  if pan9.Visible then
    inc(k,pan9.Height+h);
  pan10.Top:=k;
  if pan10.Visible then
    inc(k,pan10.Height+h);
  pan11.Top:=k;
  if pan11.Visible then
    inc(k,pan11.Height+h);
  pan12.Top:=k;
  if pan12.Visible then
    inc(k,pan12.Height+h);
  Height:=k;
end;

procedure TformTagDic.ShowPanels;
begin
  {
0  Memory Discrete
1  I/O Discrete
2  Indirect Discrete
3  Memory Integer
4  I/O Integer
5  Indirect Integer
6  Memory Real
7  I/O Real
8  Indirect Real
9  Memory Message
10  I/O Message
11  Indirect Message
12  Hist Trend
13  Tag ID
  }
  pan1.Visible:=false;
  pan2.Visible:=false;
  pan3.Visible:=false;
  pan5.Visible:=false;
  pan6.Visible:=false;
  pan4.Visible:=false;
  pan7.Visible:=false;
  pan8.Visible:=false;
  pan9.Visible:=false;
  pan10.Visible:=false;
  pan11.Visible:=false;
  pan12.Visible:=false;
  cbLogData.Visible:=true;
  cbLogEvents.Visible:=true;
  cbRetentiveValue.Visible:=true;
  rgInputConversation.Visible:=true;

  case cbTagType.ItemIndex of
    0://Memory Discrete
    begin
      pan8.Visible:=true;
      pan7.Visible:=true;
      pan3.Visible:=true;
      pan2.Visible:=true;
      pan1.Visible:=true;
      cbLogData.Visible:=true;
    end;
    1://I/O Discrete
    begin
      pan8.Visible:=true;
      pan7.Visible:=true;
      pan6.Visible:=true;
      pan3.Visible:=true;
      pan2.Visible:=true;
      pan1.Visible:=true;
      cbLogData.Visible:=true;
    end;
    2,5,8,11://Indirects
    begin
      pan2.Visible:=true;
      cbLogData.Visible:=false;
    end;
    3,6://Memory Integer, Memory Real
    begin
      pan11.Visible:=true;
      pan10.Visible:=true;
      pan9.Visible:=true;
      pan7.Visible:=true;
      pan4.Visible:=true;
      pan2.Visible:=true;
      pan1.Visible:=true;
      cbLogData.Visible:=true;
    end;
    4,7://I/O Integer, Real
    begin
      pan11.Visible:=true;
      pan10.Visible:=true;
      pan9.Visible:=true;
      pan7.Visible:=true;
      pan4.Visible:=true;
      pan6.Visible:=true;
      pan5.Visible:=true;
      pan2.Visible:=true;
      pan1.Visible:=true;
      cbLogData.Visible:=true;
    end;
    9://Memory Message
    begin
      pan12.Visible:=true;
      pan2.Visible:=true;
      pan1.Visible:=true;
      cbLogData.Visible:=false;
      rgInputConversation.Visible:=false;
    end;
    10://I/O Message
    begin
      pan12.Visible:=true;
      pan6.Visible:=true;
      pan2.Visible:=true;
      pan1.Visible:=true;
      cbLogData.Visible:=false;
      rgInputConversation.Visible:=false;
    end;
    12,13://Hist Trend, Tag ID
    begin
      pan2.Visible:=true;
      cbLogData.Visible:=false;
      cbLogEvents.Visible:=false;
      cbRetentiveValue.Visible:=false;
    end;
  end;
  AlignPanelsByOrder;
end;

procedure TformTagDic.ShowTag(tg:TTag);
var
  k:Integer;
begin
  ActiveTag:=tg;
  if Assigned(ActiveTag) then
  begin
    BeginFormUpdate;
    try
      k:=cbTagType.Items.IndexOf(ActiveTag.TagType);
      if k>-1 then
        cbTagType.ItemIndex:=k;
      edTagName.Text:=ActiveTag.TagName;
      labAlarmGroup.Caption:='$System';
      rgReadWrite.ItemIndex:=1;
      edComment.Text:=ActiveTag.Comment;
      if Assigned(ActiveTag.AccessName) then
        labAccessName.Caption:=TAccessName(ActiveTag.AccessName).Name
      else
        labAccessName.Caption:='';
      edItem.Text:=ActiveTag.Item;
      edEngUnits.Text:=ActiveTag.EngUnits;
      edMinEU.Text:=FloatToStr(ActiveTag.MinEU);
      edMaxEU.Text:=FloatToStr(ActiveTag.MaxEU);
      edMinRAW.Text:=FloatToStr(ActiveTag.MinRaw);
      edMaxRAW.Text:=FloatToStr(ActiveTag.MaxRaw);
      edOnMsg.Text:=ActiveTag.OnMsg;
      edOffMsg.Text:=ActiveTag.OffMsg;
      EnableLeftRight;
      btnSave.Enabled:=false;
      btnRestore.Enabled:=false;
    finally
      EndFormUpdate;
    end;
  end;
  ShowPanels;
end;

procedure TformTagDic.EnableLeftRight;
var
  k:Integer;
begin
  k:=-1;
  if Assigned(ActiveTag) then
    k:=cfg.Tags.IndexOf(ActiveTag)
  else
  if cfg.Tags.Count>0 then
    k:=0;

  btnLeft.Enabled:=k>0;
  btnRight.Enabled:=k<cfg.Tags.Count-1;
end;

procedure TformTagDic.btnLeftClick(Sender: TObject);
var
  k:Integer;
begin
  k:=-1;
  if Assigned(ActiveTag) then
  begin
    k:=cfg.Tags.IndexOf(ActiveTag);
    if (k>-1) then
      dec(k);
  end
  else
  if cfg.Tags.Count>0 then
    k:=0;

  if k>-1 then
  begin
    ActiveTag:=TTag(cfg.Tags[k]);
    if Assigned(ActiveTag) then
      ShowTag(ActiveTag);
  end;
end;

procedure TformTagDic.btnRightClick(Sender: TObject);
var
  k:Integer;
begin
  k:=-1;
  if Assigned(ActiveTag) then
  begin
    k:=cfg.Tags.IndexOf(ActiveTag);
    if (k>-1) then
      inc(k);
  end
  else
  if cfg.Tags.Count>0 then
    k:=0;

  if k>cfg.Tags.Count-1 then
    k:=-1;

  if k>-1 then
  begin
    ActiveTag:=TTag(cfg.Tags[k]);
    if Assigned(ActiveTag) then
      ShowTag(ActiveTag);
  end;
end;

procedure TformTagDic.btnAccessNameClick(Sender: TObject);
var
  an,an2:TAccessName;
  k:Integer;
begin
  if Assigned(ActiveTag) and Assigned(ActiveTag.AccessName) then
  begin
    an:=TAccessName(ActiveTag.AccessName);
    k:=FormANsEditor.lbAccessNames.Items.IndexOfObject(an);
    if k>-1 then
      FormANsEditor.lbAccessNames.ItemIndex:=k;
    if FormANsEditor.ShowModal=mrOk then
    begin
      k:=FormANsEditor.lbAccessNames.ItemIndex;
      if k>-1 then
      begin
        an2:=TAccessName(FormANsEditor.lbAccessNames.Items.Objects[k]);
        if Assigned(an2) and (an<>an2) then
        begin
          ActiveTag.AccessName:=an2;
          ActiveTag.AccessNameID:=an2.ID;
          if Assigned(ActiveTag.AccessName) then
            labAccessName.Caption:=TAccessName(ActiveTag.AccessName).Name
          else
            labAccessName.Caption:='';
        end;
      end;
    end;
  end;
end;

procedure TformTagDic.cbTagTypeChange(Sender: TObject);
begin
  ShowPanels;
end;

procedure TformTagDic.Save;
begin
  if Assigned(ActiveTag) then
  begin
    ActiveTag.TagType:=cbTagType.Text;
    ActiveTag.EngUnits:=edEngUnits.Text;
    ActiveTag.MaxEU:=TryStrToFloat(edMaxEU.Text);
    ActiveTag.MaxRAW:=TryStrToFloat(edMaxRAW.Text);
    ActiveTag.MinEU:=TryStrToFloat(edMinEU.Text);
    ActiveTag.MinRAW:=TryStrToFloat(edMinRAW.Text);
    ActiveTag.OffMsg:=edOffMsg.Text;
    ActiveTag.OnMsg:=edOnMsg.Text;
    ActiveTag.TagName:=edTagName.Text;
    ActiveTag.Comment:=edComment.Text;
    if cfg.Tags.IndexOf(ActiveTag)<0 then
      cfg.Tags.Add(ActiveTag);
  end;
end;

procedure TformTagDic.btnSaveClick(Sender: TObject);
begin
  Save;
end;

procedure TformTagDic.btnNewClick(Sender: TObject);
var
  tg:TTag;
begin
  tg:=TTag.Create;
  if Assigned(ActiveTag) then
  begin
    tg.TagType:=ActiveTag.TagType;
    tg.OnMsg:=ActiveTag.OnMsg;
    tg.OffMsg:=ActiveTag.OffMsg;
    tg.AccessName:=ActiveTag.AccessName;
    tg.AccessNameID:=ActiveTag.AccessNameID;
    tg.EngUnits:=ActiveTag.EngUnits;
    tg.MinEU:=ActiveTag.MinEU;
    tg.MaxEU:=ActiveTag.MaxEU;
    tg.MaxRAW:=ActiveTag.MaxRAW;
    tg.MinRAW:=ActiveTag.MinRAW;
  end;
  ShowTag(tg);
end;

procedure TformTagDic.btnDeleteClick(Sender: TObject);
begin
  if Assigned(ActiveTag) then
  begin
    cfg.Tags.Extract(ActiveTag);
    ActiveTag.Free;
    ActiveTag:=nil;
  end;
end;

end.

