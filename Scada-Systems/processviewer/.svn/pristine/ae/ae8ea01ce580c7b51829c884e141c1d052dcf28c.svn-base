program view;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}
  cthreads,
  {$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, UnitMain, unittags, UnitFormsConfig, TAChartLazarusPkg,
  HTMLColorConversions,
  UnitIOThread, UnitConfig, unitaccessnames, UnitThreadLog, UnitTimers,
  UnitEnterValue, UnitMotor, UnitRegulator, UnitVer, modbustcp;

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TFormEnterValue, FormEnterValue);
  Application.CreateForm(TFormMotor, FormMotor);
  Application.CreateForm(TFormRegulator, FormRegulator);
  Application.Run;
end.

