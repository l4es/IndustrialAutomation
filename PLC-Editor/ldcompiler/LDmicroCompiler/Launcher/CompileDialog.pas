unit CompileDialog;

interface

uses Windows, SysUtils, Classes, Graphics, Forms, Controls, StdCtrls, 
  Buttons, ExtCtrls, JvExStdCtrls, JvMemo, JvComponentBase, JvCreateProcess;

type
  TCompileDlg = class(TForm)
    OKBtn: TButton;
    Bevel1: TBevel;
    LDcompilerProcess: TJvCreateProcess;
    ConsoleMemo: TJvMemo;
    Button1: TButton;
    procedure OKBtnClick(Sender: TObject);
    procedure LDcompilerProcessRead(Sender: TObject; const S: String;
      const StartsOnNewLine: Boolean);
    procedure Button1Click(Sender: TObject);
  private
    FOutputFileName : string;
  public
    procedure Run(const Title, AppName, InputFile, OutputFile, CmdOptions : string);
  end;

var
  CompileDlg: TCompileDlg;

implementation

uses OutputDialog;

{$R *.dfm}

procedure TCompileDlg.Run(const Title, AppName, InputFile, OutputFile, CmdOptions : string);
begin
  Caption := Title;
  FOutputFileName := OutputFile;
  LDcompilerProcess.CommandLine :=
    AppName+' "'+InputFile+'" "'+OutputFile+'" '+CmdOptions;
  ConsoleMemo.Lines.Clear;
  Show;
  ConsoleMemo.Lines.Add(DateToStr(Now) + '  ' + TimeToStr(Now));
  ConsoleMemo.Lines.Add('');
  ConsoleMemo.Lines.Add(LDcompilerProcess.CommandLine);
  LDcompilerProcess.Run;
end;

procedure TCompileDlg.OKBtnClick(Sender: TObject);
begin
  Visible := false;
end;

procedure TCompileDlg.LDcompilerProcessRead(Sender: TObject;
  const S: String; const StartsOnNewLine: Boolean);
begin
  ConsoleMemo.Lines.Add(S);
end;

procedure TCompileDlg.Button1Click(Sender: TObject);
begin
  OutputDlg.Run(FOutputFileName);
end;

end.
