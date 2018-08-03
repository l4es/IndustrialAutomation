program LDmicroCompilerLauncher;

uses
  Forms,
  W2kMain in 'W2kMain.pas' {Win2kAppForm},
  About in 'About.pas' {AboutBox},
  CompileDialog in 'CompileDialog.pas' {CompileDlg},
  OutputDialog in 'OutputDialog.pas' {OutputDlg};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'LDmicroCompilerLauncher';
  Application.CreateForm(TWin2kAppForm, Win2kAppForm);
  Application.CreateForm(TAboutBox, AboutBox);
  Application.CreateForm(TCompileDlg, CompileDlg);
  Application.CreateForm(TOutputDlg, OutputDlg);
  Application.Run;
end.
 
