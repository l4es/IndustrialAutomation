function etherlab_config(hDlg,hSrc)
% Purpose: This file sets some default Configuration Parameters when
%          EtherLab RTW Target is selected
% 
% Copyright 1994-2005 IgH Essen GmbH
% $Revision$  $Date$

  % Simulation Time
  slConfigUISetVal(hDlg,hSrc,'StartTime','0.0');
  slConfigUISetVal(hDlg,hSrc,'StopTime','inf');

  % Solver options
  slConfigUISetVal(hDlg,hSrc,'SolverType','Fixed-step');
  slConfigUISetEnabled(hDlg,hSrc,'SolverType','off');
  slConfigUISetVal(hDlg,hSrc,'Solver','FixedStepDiscrete');
  slConfigUISetVal(hDlg,hSrc,'SolverMode','SingleTasking');
  slConfigUISetVal(hDlg,hSrc,'AutoInsertRateTranBlk','off');

  % Data Import/Export
  slConfigUISetVal(hDlg,hSrc,'LoadExternalInput','off');
  slConfigUISetVal(hDlg,hSrc,'LoadInitialState','off');
  slConfigUISetVal(hDlg,hSrc,'SaveTime','off');
  slConfigUISetVal(hDlg,hSrc,'SaveState','off');
  slConfigUISetVal(hDlg,hSrc,'SaveOutput','off');
  slConfigUISetVal(hDlg,hSrc,'SaveFinalState','off');
  slConfigUISetVal(hDlg,hSrc,'SignalLogging','off');
  slConfigUISetVal(hDlg,hSrc,'LimitDataPoints','off');

  slConfigUISetEnabled(hDlg,hSrc,'LoadExternalInput','off');
  slConfigUISetEnabled(hDlg,hSrc,'LoadInitialState','off');
  slConfigUISetEnabled(hDlg,hSrc,'SaveTime','off');
  slConfigUISetEnabled(hDlg,hSrc,'SaveState','off');
  slConfigUISetEnabled(hDlg,hSrc,'SaveOutput','off');
  slConfigUISetEnabled(hDlg,hSrc,'SaveFinalState','off');
  slConfigUISetEnabled(hDlg,hSrc,'SignalLogging','off');
  slConfigUISetEnabled(hDlg,hSrc,'LimitDataPoints','off');

  % Optimization
  slConfigUISetVal(hDlg,hSrc,'BlockReduction','off');
  slConfigUISetVal(hDlg,hSrc,'ConditionallyExecuteInputs','off');
  slConfigUISetVal(hDlg,hSrc,'BufferReuse','off');
  slConfigUISetVal(hDlg,hSrc,'InlineParams','off');
  slConfigUISetVal(hDlg,hSrc,'BooleanDataType','on');
  slConfigUISetVal(hDlg,hSrc,'OptimizeBlockIOStorage','off');

  % Hardware
  %slConfigUISetVal(hDlg,hSrc,'ProdHWDeviceType', ...
  %	'32-bit xPC Target (Intel Pentium)');
  %slConfigUISetEnabled(hDlg,hSrc,'ProdHWDeviceType', 'off');

  % Interface
  slConfigUISetVal(hDlg,hSrc,'RTWCAPISignals','on');
  slConfigUISetEnabled(hDlg,hSrc,'RTWCAPISignals','off');
  slConfigUISetVal(hDlg,hSrc,'RTWCAPIParams','on');
  slConfigUISetEnabled(hDlg,hSrc,'RTWCAPIParams','off');
  slConfigUISetVal(hDlg,hSrc,'ExtMode','off');
  slConfigUISetVal(hDlg,hSrc,'GenerateASAP2','off');
  slConfigUISetVal(hDlg,hSrc,'ExtModeTesting','off');
  slConfigUISetVal(hDlg,hSrc,'ExtModeTransport','0');
  slConfigUISetVal(hDlg,hSrc,'RTWVerbose','on');
  slConfigUISetVal(hDlg,hSrc,'MatFileLogging','off');
  slConfigUISetEnabled(hDlg,hSrc,'MatFileLogging','off');

  slConfigUISetVal(hDlg, hSrc, 'ModelReferenceCompliant', 'on');
