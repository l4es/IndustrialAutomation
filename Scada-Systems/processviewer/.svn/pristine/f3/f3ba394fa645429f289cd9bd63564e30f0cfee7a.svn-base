program maker;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, tachartlazaruspkg, unitmain, unitver, htmlcolorconversions, modbustcp,
  unitaccessnames, unitconfig, unitentervalue, unitformsconfig, unitiothread,
  unitmotor, unitregulator, unittags, unitthreadlog, unittimers, unittagdic,
  unitaccessnameseditor, unitedan;

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TformTagDic, formTagDic);
  Application.CreateForm(TFormANsEditor, FormANsEditor);
  Application.CreateForm(TFormEdAN, FormEdAN);
  Application.Run;
end.

