object AboutBox: TAboutBox
  Left = 243
  Top = 108
  ActiveControl = OKButton
  BorderStyle = bsDialog
  Caption = 'A propos'
  ClientHeight = 257
  ClientWidth = 367
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 120
  TextHeight = 16
  object Panel1: TPanel
    Left = 10
    Top = 10
    Width = 346
    Height = 198
    BevelOuter = bvLowered
    TabOrder = 0
    object ProgramIcon: TImage
      Left = 10
      Top = 10
      Width = 80
      Height = 70
      IsControl = True
    end
    object ProductName: TLabel
      Left = 108
      Top = 20
      Width = 86
      Height = 16
      Caption = 'Nom du produit'
      IsControl = True
    end
    object Version: TLabel
      Left = 108
      Top = 49
      Width = 46
      Height = 16
      Caption = 'Version'
      IsControl = True
    end
    object Copyright: TLabel
      Left = 10
      Top = 98
      Width = 57
      Height = 16
      Caption = 'Copyright'
      IsControl = True
    end
    object Comments: TLabel
      Left = 10
      Top = 128
      Width = 64
      Height = 16
      Caption = 'Commentaires'
      WordWrap = True
      IsControl = True
    end
  end
  object OKButton: TButton
    Left = 145
    Top = 219
    Width = 80
    Height = 32
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
    IsControl = True
  end
end
