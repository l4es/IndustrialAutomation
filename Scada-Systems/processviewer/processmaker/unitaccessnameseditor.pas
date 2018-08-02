unit unitaccessnameseditor;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ExtCtrls;

type

  { TFormANsEditor }

  TFormANsEditor = class(TForm)
    btnClose: TButton;
    btnAdd: TButton;
    btnModify: TButton;
    btnDelete: TButton;
    lbAccessNames: TListBox;
    Panel1: TPanel;
    procedure btnAddClick(Sender: TObject);
    procedure btnDeleteClick(Sender: TObject);
    procedure btnModifyClick(Sender: TObject);
  private
    { private declarations }
  public
    { public declarations }
  end; 

var
  FormANsEditor: TFormANsEditor;

implementation

{$R *.lfm}

Uses
  UnitConfig, unitedan, UnitAccessNames;

{ TFormANsEditor }

procedure TFormANsEditor.btnDeleteClick(Sender: TObject);
var
  k:Integer;
begin
  k:=lbAccessNames.ItemIndex;
  if k>-1 then
  begin
    cfg.AccessNames.Extract(lbAccessNames.Items.Objects[k]);
    lbAccessNames.Items.Delete(k);
  end;
end;

procedure TFormANsEditor.btnModifyClick(Sender: TObject);
var
  k:Integer;
  an:TAccessName;
begin
  k:=lbAccessNames.ItemIndex;
  if k>-1 then
  begin
    an:=TAccessName(lbAccessNames.Items.Objects[k]);
    FormEdAN.ShowAN(an);
    if FormEdAN.ShowModal=mrOk then
    begin
      FormEdAN.AssignAN(FormEdAN.an,an);
      lbAccessNames.Items[k]:=an.Name;
    end;
  end;
end;

procedure TFormANsEditor.btnAddClick(Sender: TObject);
var
  k:Integer;
  an:TAccessName;
  sID:String;
begin
  an:=TAccessName.Create;
  if cfg.AccessNames.Count>0 then
  begin
    sID:=TAccessName(cfg.AccessNames[cfg.AccessNames.Count-1]).ID;
    k:=StrToInt(sID)+1;
    sID:=IntToStr(k);
  end
  else
    sID:='1';
  an.ID:=sID;
  an.IO_Type:=FormEdAN.cbIOserverType.Items[0];
  an.AdviseActiveItems:=True;
  FormEdAN.ShowAN(an);
  if FormEdAN.ShowModal=mrOk then
  begin
    FormEdAN.AssignAN(FormEdAN.an,an);
    k:=cfg.AccessNames.Add(an);
    lbAccessNames.Items.AddObject(an.Name,an);
    lbAccessNames.ItemIndex:=k;
  end;
end;

end.

