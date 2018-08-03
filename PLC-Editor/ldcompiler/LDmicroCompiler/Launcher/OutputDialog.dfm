object OutputDlg: TOutputDlg
  Left = 245
  Top = 108
  Width = 649
  Height = 525
  Caption = 'Dialogue'
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  DesignSize = (
    641
    491)
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 625
    Height = 438
    Anchors = [akLeft, akTop, akRight, akBottom]
    Shape = bsFrame
  end
  object OKBtn: TButton
    Left = 79
    Top = 457
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKBtnClick
  end
  object JvMemo1: TJvMemo
    Left = 16
    Top = 16
    Width = 609
    Height = 425
    Anchors = [akLeft, akTop, akRight, akBottom]
    Lines.Strings = (
      'JvMemo1')
    ScrollBars = ssBoth
    TabOrder = 1
  end
end
