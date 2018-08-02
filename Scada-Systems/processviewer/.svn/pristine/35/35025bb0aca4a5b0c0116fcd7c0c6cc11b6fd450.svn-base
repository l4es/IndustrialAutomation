unit unitedan;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, StdCtrls,
  ExtCtrls, ComCtrls, UnitAccessNames;

type

  { TFormEdAN }

  TFormEdAN = class(TForm)
    btnCancel: TButton;
    btnOk: TButton;
    cbIOserverType: TComboBox;
    edAccess: TEdit;
    edIPaddr: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    pcIOServers: TPageControl;
    Panel1: TPanel;
    rgWhenToAdvise: TRadioGroup;
    tsMBTCP: TTabSheet;
    tsMBSerial: TTabSheet;
    tsDDE: TTabSheet;
    tsOPC: TTabSheet;
    tsSuitelink: TTabSheet;
    procedure cbIOserverTypeChange(Sender: TObject);
    procedure edAccessChange(Sender: TObject);
    procedure edIPaddrChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure rgWhenToAdviseClick(Sender: TObject);
  private
    { private declarations }
  public
    an:TAccessName;
    procedure ShowAN(an_obj:TAccessName);
    procedure AssignAN(an_from:TAccessName;an_to:TAccessName);
  end;

var
  FormEdAN: TFormEdAN;

implementation

{$R *.lfm}

{ TFormEdAN }

procedure TFormEdAN.FormCreate(Sender: TObject);
begin
  an:=TAccessName.Create;
end;

procedure TFormEdAN.AssignAN(an_from:TAccessName;an_to:TAccessName);
begin
  an_to.ID:=an_from.ID;
  an_to.IO_Type:=an_from.IO_Type;
  an_to.Address:=an_from.Address;
  an_to.Name:=an_from.Name;
  an_to.AdviseActiveItems:=an_from.AdviseActiveItems;
end;

procedure TFormEdAN.ShowAN(an_obj:TAccessName);
var
  k:Integer;
begin
  AssignAN(an_obj,an);
  edAccess.Text:=an.Name;
  k:=cbIOserverType.Items.IndexOf(an.IO_Type);
  cbIOserverType.ItemIndex:=k;
  pcIOServers.ActivePageIndex:=k;
  edIPaddr.Text:=an.Address;
  rgWhenToAdvise.ItemIndex:=ord(an.AdviseActiveItems);
end;

procedure TFormEdAN.cbIOserverTypeChange(Sender: TObject);
var
  k:Integer;
begin
  k:=cbIOserverType.ItemIndex;
  an.IO_Type:=cbIOserverType.Items[k];
  pcIOServers.ActivePageIndex:=k;
end;

procedure TFormEdAN.edAccessChange(Sender: TObject);
begin
  an.Name:=edAccess.Text;
end;

procedure TFormEdAN.edIPaddrChange(Sender: TObject);
begin
  an.Address:=edIPaddr.Text;
end;

procedure TFormEdAN.rgWhenToAdviseClick(Sender: TObject);
begin
  an.AdviseActiveItems:=rgWhenToAdvise.ItemIndex=1;
end;

end.

