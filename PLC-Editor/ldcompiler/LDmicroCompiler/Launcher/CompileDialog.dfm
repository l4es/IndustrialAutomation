object CompileDlg: TCompileDlg
  Left = 245
  Top = 108
  Width = 736
  Height = 529
  Caption = 'LDmicroCompiler'
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  DesignSize = (
    728
    495)
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 712
    Height = 442
    Anchors = [akLeft, akTop, akRight, akBottom]
    Shape = bsFrame
  end
  object OKBtn: TButton
    Left = 80
    Top = 464
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKBtnClick
  end
  object ConsoleMemo: TJvMemo
    Left = 16
    Top = 16
    Width = 696
    Height = 426
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      'ConsoleMemo')
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 1
  end
  object Button1: TButton
    Left = 160
    Top = 464
    Width = 113
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Affiche la sortie'
    TabOrder = 2
    OnClick = Button1Click
  end
  object LDcompilerProcess: TJvCreateProcess
    CommandLine = 'LDmicroCompiler.exe'
    ConsoleOptions = [coOwnerData, coRedirect]
    OnRead = LDcompilerProcessRead
    Left = 680
    Top = 456
  end
end
