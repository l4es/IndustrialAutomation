unit W2kMain;

interface

uses Windows, Classes, Graphics, Forms, Controls, Menus,
  Dialogs, StdCtrls, Buttons, ExtCtrls, ComCtrls, ImgList, StdActns,
  ActnList, ToolWin, JvComponentBase, JvCreateProcess, JvExStdCtrls,
  JvHtControls, JvCombobox;

type
  TWin2kAppForm = class(TForm)
    OpenDialog: TOpenDialog;
    SaveDialog: TSaveDialog;
    ActionList1: TActionList;
    FileOpen1: TAction;
    FileSave1: TAction;
    FileSaveAs1: TAction;
    FileSend1: TAction;
    FileExit1: TAction;
    EditCut1: TEditCut;
    EditCopy1: TEditCopy;
    EditPaste1: TEditPaste;
    HelpAbout1: TAction;
    StatusBar: TStatusBar;
    ImageList1: TImageList;
    RichEdit1: TRichEdit;
    MainMenu1: TMainMenu;
    File1: TMenuItem;
    FileOpenItem: TMenuItem;
    FileSaveItem: TMenuItem;
    FileSaveAsItem: TMenuItem;
    N1: TMenuItem;
    FileSendItem: TMenuItem;
    N2: TMenuItem;
    FileExitItem: TMenuItem;
    Edit1: TMenuItem;
    CutItem: TMenuItem;
    CopyItem: TMenuItem;
    PasteItem: TMenuItem;
    Help1: TMenuItem;
    HelpAboutItem: TMenuItem;
    SaveDialog1: TSaveDialog;
    ToolBar1: TToolBar;
    ToolButton2: TToolButton;
    ToolButton3: TToolButton;
    ToolButton4: TToolButton;
    ToolButton5: TToolButton;
    ToolButton6: TToolButton;
    ToolButton7: TToolButton;
    ToolButton8: TToolButton;
    ToolButton9: TToolButton;
    PopupMenu1: TPopupMenu;
    Cut1: TMenuItem;
    Copy1: TMenuItem;
    Paste1: TMenuItem;
    ToolButton10: TToolButton;
    Compile: TAction;
    ToolButton11: TToolButton;
    Compiler1: TMenuItem;
    LDmicroCompiler1: TMenuItem;
    OutputFormatCB: TJvComboBox;
    procedure FileNew1Execute(Sender: TObject);
    procedure FileOpen1Execute(Sender: TObject);
    procedure FileSave1Execute(Sender: TObject);
    procedure FileSaveAs1Execute(Sender: TObject);
    procedure FileSend1Execute(Sender: TObject);
    procedure FileExit1Execute(Sender: TObject);
    procedure HelpAbout1Execute(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure CompileExecute(Sender: TObject);
    procedure LDcompilerProcessRead(Sender: TObject; const S: String;
      const StartsOnNewLine: Boolean);
  private
    FFileName: String;
    FLDmicroCompilerPath : string;
  protected
    procedure AddCompilerOutputFormat(DisplayName, CmdLineOption, FileExt : string);
  public
    { déclarations publiques }
  end;

var
  Win2kAppForm: TWin2kAppForm;

implementation

uses
  SysUtils, Mapi, about, SHFolder, CompileDialog;

{$R *.dfm}

resourcestring
  SUntitled  = 'Sans titre';
  SOverwrite = 'OK pour remplacer %s';
  SSendError = 'Erreur à l''envoi du courrier';

type
  TOutputFormat = class(TObject)
  private
    FDisplayName : string;
    FCmdLineOption : string;
    FFileExt : string;
  public
    constructor Create(DisplayName, CmdLineOption, FileExt : string);
  published
    property DisplayName : string read FDisplayName write FDisplayName;
    property CmdLineOption : string read FCmdLineOption write FCmdLineOption;
    property FileExt : string read FFileExt write FFileExt;
  end;

function DefaultSaveLocation: string;
var
  P: PChar;
begin
  {
    renvoie l'emplacement de 'Mes Documents' s'il existe, sinon renvoie
    le répertoire en cours.
  }
  P := nil;
  try
    P := AllocMem(MAX_PATH);
    if SHGetFolderPath(0, CSIDL_PERSONAL, 0, 0, P) = S_OK then
      Result := P
    else
      Result := GetCurrentDir;
  finally
    FreeMem(P);
  end;
end;

constructor TOutputFormat.Create(DisplayName, CmdLineOption, FileExt : string);
begin
  inherited Create;
  FDisplayName := DisplayName;
  FCmdLineOption := CmdLineOption;
  FFileExt := FileExt;
end;


procedure TWin2kAppForm.FileNew1Execute(Sender: TObject);
begin
  SaveDialog.InitialDir := DefaultSaveLocation;
  FFileName := SUntitled;
  RichEdit1.Lines.Clear;
  RichEdit1.Modified := False;
end;

procedure TWin2kAppForm.FileOpen1Execute(Sender: TObject);
begin
  if OpenDialog.Execute then
  begin
    RichEdit1.Lines.LoadFromFile(OpenDialog.FileName);
    FFileName := OpenDialog.FileName;
    RichEdit1.SetFocus;
    RichEdit1.Modified := False;
    RichEdit1.ReadOnly := ofReadOnly in OpenDialog.Options;
    Compile.Enabled := true;
  end;
end;

procedure TWin2kAppForm.FileSave1Execute(Sender: TObject);
begin
  if (FFileName = SUntitled) or (FFileName = '') then
    FileSaveAs1Execute(Sender)
  else
  begin
    RichEdit1.Lines.SaveToFile(FFileName);
    RichEdit1.Modified := False;
  end;
end;

procedure TWin2kAppForm.FileSaveAs1Execute(Sender: TObject);
begin
  with SaveDialog do
  begin
    FileName := FFileName;
    if Execute then
    begin
      if FileExists(FileName) then
        if MessageDlg(Format(SOverwrite, [FileName]),
          mtConfirmation, mbYesNoCancel, 0) <> idYes then Exit;
      RichEdit1.Lines.SaveToFile(FileName);
      FFileName := FileName;
      RichEdit1.Modified := False;
    end;
  end;
end;

procedure TWin2kAppForm.FileSend1Execute(Sender: TObject);
var
  MapiMessage: TMapiMessage;
  MError: Cardinal;
begin
  with MapiMessage do
  begin
    ulReserved := 0;
    lpszSubject := nil;
    lpszNoteText := PChar(RichEdit1.Lines.Text);
    lpszMessageType := nil;
    lpszDateReceived := nil;
    lpszConversationID := nil;
    flFlags := 0;
    lpOriginator := nil;
    nRecipCount := 0;
    lpRecips := nil;
    nFileCount := 0;
    lpFiles := nil;
  end;

  MError := MapiSendMail(0, Application.Handle, MapiMessage,
    MAPI_DIALOG or MAPI_LOGON_UI or MAPI_NEW_SESSION, 0);
  if MError <> 0 then MessageDlg(SSendError, mtError, [mbOK], 0);
end;

procedure TWin2kAppForm.FileExit1Execute(Sender: TObject);
begin
  Close;
end;

procedure TWin2kAppForm.HelpAbout1Execute(Sender: TObject);
begin
  AboutBox.ShowModal;
end;

procedure TWin2kAppForm.AddCompilerOutputFormat(DisplayName, CmdLineOption, FileExt : string);
begin
  OutputFormatCB.Items.AddObject(DisplayName, TOutputFormat.Create(DisplayName, CmdLineOption, FileExt));
end;

procedure TWin2kAppForm.FormCreate(Sender: TObject);
begin
   FLDmicroCompilerPath := ExtractFilePath(Application.ExeName)+ 'LDmicroCompiler.exe';
   if not FileExists(FLDmicroCompilerPath) then begin
     Application.MessageBox('LDmicroCompiler.exe not found', 'Error', MB_ICONERROR+MB_OK);
     Close;
   end;
//  FileNew1.Execute; { définit le nom de fichier par défaut et efface le contrôle RichEdit }
   AddCompilerOutputFormat('Convert to LD2 file format', '/f:ld2', '.ld2');
   AddCompilerOutputFormat('ANSI C output', '/f:c', '.c');
   AddCompilerOutputFormat('VM machine code', '/f:vm', '.vm');
   AddCompilerOutputFormat('CALM assembly source', '/f:calm', '.asm');
end;

procedure TWin2kAppForm.CompileExecute(Sender: TObject);
var
  OutputFormat : TOutputFormat;
  DestFileName : TFileName;
begin
  if OutputFormatCB.ItemIndex <0 then exit;
  OutputFormat := TOutputFormat(OutputFormatCB.Items.Objects[OutputFormatCB.ItemIndex]);
  DestFileName := ChangeFileExt(FFileName, OutputFormat.FileExt);
  SaveDialog.DefaultExt := Copy(OutputFormat.FileExt, 2, 255);
  SaveDialog.InitialDir := ExtractFilePath(DestFileName);
  SaveDialog.FileName := ExtractFileName(DestFileName);
  if not SaveDialog.Execute then exit;
  DestFileName := SaveDialog.FileName;
  CompileDlg.Run('LDmicroCompiler - '+ExtractFileName(FFileName),
    FLDmicroCompilerPath, ExtractFileName(FFileName), DestFileName, OutputFormat.CmdLineOption);
end;

procedure TWin2kAppForm.LDcompilerProcessRead(Sender: TObject;
  const S: String; const StartsOnNewLine: Boolean);
begin
  RichEdit1.Lines.Add(S);
end;


end.
