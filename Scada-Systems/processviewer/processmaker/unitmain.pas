unit unitmain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, ExtCtrls,
  CheckLst, StdCtrls;

type

  { TForm1 }

  TForm1 = class(TForm)
    Label2: TLabel;
    lbTools: TListBox;
    lbWindows: TCheckListBox;
    Label1: TLabel;
    Panel1: TPanel;
    Panel2: TPanel;
    Panel3: TPanel;
    Panel4: TPanel;
    PanelMain: TPanel;
    Splitter1: TSplitter;
    Splitter2: TSplitter;
    Timer1: TTimer;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure lbToolsDblClick(Sender: TObject);
    procedure lbWindowsClickCheck(Sender: TObject);
    procedure lbWindowsDblClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
  private
    { private declarations }
  public
    { public declarations }
  end; 

var
  Form1: TForm1; 

implementation

uses
  UnitVer, UnitConfig, UnitFormsConfig, UnitThreadLog, unittagdic,
  unitaccessnameseditor, UnitAccessNames, UnitTags;

{$R *.lfm}

var
  ActiveTag:TTag;

{ TForm1 }

procedure TForm1.FormCreate(Sender: TObject);
var
  frm:TFormConfig;
  i,k:Integer;
begin
  Caption:=Caption+' '+ProgVersion;
  ActiveTag:=nil;
  cfg:=TConfig.Create;
  cfg.LoadConfig('../project/');
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
      k:=lbWindows.Items.AddObject(frm.FileName,frm);
      lbWindows.Checked[k]:=frm.Open;
      if frm.Open then
        frm.Form.Show;
    end;
  end;
  if cfg.Tags.Count>0 then
    ActiveTag:=TTag(cfg.Tags[0]);
  Timer1.Enabled:=true;
  LogThr.AddToLog('Started OK');
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  cfg.SaveConfig;
  cfg.Free;
end;

procedure TForm1.lbWindowsClickCheck(Sender: TObject);
var
  frm:TFormConfig;
  k:Integer;
begin
  k:=lbWindows.ItemIndex;
  if k>-1 then
  begin
    frm:=TFormConfig(lbWindows.Items.Objects[k]);
    frm.Open:=lbWindows.Checked[k];
    if frm.Form<>nil then
    begin
      frm.Form.Visible:=frm.Open;
      if frm.Open then
        frm.Form.BringToFront;
    end;
  end;
end;

procedure TForm1.lbWindowsDblClick(Sender: TObject);
var
  k:Integer;
begin
  k:=lbWindows.ItemIndex;
  if k>-1 then
  begin
    lbWindows.Checked[k]:=True;
    lbWindowsClickCheck(Sender);
  end;
end;

procedure TForm1.Timer1Timer(Sender: TObject);
var
  i:Integer;
begin
  Timer1.Enabled:=False;
  FormANsEditor.lbAccessNames.Clear;
  for i:=0 to cfg.AccessNames.Count-1 do
    FormANsEditor.lbAccessNames.Items.AddObject(TAccessName(cfg.AccessNames[i]).Name,TObject(cfg.AccessNames[i]));
end;

procedure TForm1.lbToolsDblClick(Sender: TObject);
var
  k:Integer;
begin
  k:=lbTools.ItemIndex;
  if (k>-1) then
  begin
    if (lbTools.Items[k]='Tagname Dictionary') then
    begin
      formTagDic.ShowTag(ActiveTag);
      formTagDic.ShowModal;
    end
    else
      if (lbTools.Items[k]='Access Names') then
        FormANsEditor.ShowModal;
  end;
end;

end.

