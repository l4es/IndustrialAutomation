unit OutputDialog;

interface

uses Windows, SysUtils, Classes, Graphics, Forms, Controls, StdCtrls, 
  Buttons, ExtCtrls, JvExStdCtrls, JvMemo;

type
  TOutputDlg = class(TForm)
    OKBtn: TButton;
    Bevel1: TBevel;
    JvMemo1: TJvMemo;
    procedure OKBtnClick(Sender: TObject);
  private
    { déclarations privées }
  public
    procedure Run(FileName : TFileName);
  end;

var
  OutputDlg: TOutputDlg;

implementation

{$R *.dfm}

procedure TOutputDlg.Run(FileName : TFileName);
begin
  Caption := FileName;
  JvMemo1.Lines.LoadFromFile(FileName);
  Show;
end;


procedure TOutputDlg.OKBtnClick(Sender: TObject);
begin
  Visible := false;
end;

end.
