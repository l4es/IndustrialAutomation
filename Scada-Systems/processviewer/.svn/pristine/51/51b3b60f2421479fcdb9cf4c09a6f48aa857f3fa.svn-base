unit UnitAccessNames;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils,
  UnitIOThread;

type
  //<AccessName ID="1" TYPE="MODBUS/TCP" ADDRESS="127.0.0.1">LocalPLC</AccessName>
  TAccessName=class
  public
    ID:String;
    IO_Type:String;
    Address:String;
    Name:String;
    AdviseActiveItems:Boolean;
    Tags:TList;
    IOServer:TIOThread;
  end;

  TAccessNames=class(TList)
  private
    FOwnerConfig:TObject;
    FFileName:String;
    function ConstructIOServer(AccessName:TAccessName):TIOThread;
  public
    constructor Create(AOwner:TObject);
    function GetAccessNameByID(id:String):TAccessName;
    procedure StartIOServers;
    procedure StopIOServers;
    procedure LoadConfig(file_name:String);
    procedure SaveConfig;
    function GetScanTimeMs:LongInt;
  end;

implementation

Uses
    DOM, XMLRead, XMLWrite,
    ModbusTCP, UnitTags;

constructor TAccessNames.Create(AOwner:TObject);
begin
  Inherited Create;
  FOwnerConfig:=AOwner;
  FFileName:='';
end;

function TAccessNames.GetAccessNameByID(id:String):TAccessName;
var
  AccessName:TAccessName;
  i:Integer;
begin
  Result:=nil;
  for i:= 0 to (Count - 1) do
  begin
    AccessName:=TAccessName(Items[i]);
    if AccessName.ID=id then
    begin
      Result:=AccessName;
      break;
    end;
  end;
end;

procedure TAccessNames.StartIOServers;
var
  i:Integer;
  an:TAccessName;
begin
  for i:=0 to (Count - 1) do
  begin
    an:=TAccessName(Items[i]);
    if Assigned(an.IOServer) then
      an.IOServer.StartThread;
  end;
end;

procedure TAccessNames.StopIOServers;
var
  i:Integer;
  an:TAccessName;
begin
  for i:= 0 to (Count - 1) do
  begin
    an:=TAccessName(Items[i]);
    if Assigned(an.IOServer) then
      an.IOServer.StopThread;
  end;
end;

function TAccessNames.ConstructIOServer(AccessName:TAccessName):TIOThread;
var
  mbtcp:TModbusTcpGate;
begin
  Result:=nil;
  if CompareText(AccessName.IO_Type,'MODBUS/TCP')=0 then
  begin
    mbtcp:=TModbusTcpGate.Create(AccessName.Address);
    mbtcp.Timeout:=3000;
    mbtcp.IntervalMs:=10;
    Result:=mbtcp;
  end;
  if Assigned(Result) then
    Result.Tags:=AccessName.Tags;
end;

procedure TAccessNames.LoadConfig(file_name:String);
var
  Doc:TXMLDocument;
  ndAccessNames,Child,nd2:TDOMNode;
  j:Integer;
  AccessName:TAccessName;
begin
  FFileName:=file_name;
  ReadXMLFile(Doc, file_name);
  ndAccessNames:=Doc.DocumentElement.FindNode('AccessNames');
  for j := 0 to (ndAccessNames.ChildNodes.Count - 1) do
  begin
    AccessName:=TAccessName.Create;
    child:=ndAccessNames.ChildNodes[j];
    //<AccessName ID="1" TYPE="MODBUS/TCP" ADDRESS="127.0.0.1">LocalPLC</AccessName>
    AccessName.Name:=child.FirstChild.NodeValue;
    AccessName.ID:=child.Attributes.GetNamedItem('ID').NodeValue;
    AccessName.IO_Type:=child.Attributes.GetNamedItem('TYPE').NodeValue;
    AccessName.Address:=child.Attributes.GetNamedItem('ADDRESS').NodeValue;
    nd2:=child.Attributes.GetNamedItem('AdviseActiveItems');
    if nd2=nil then
      AccessName.AdviseActiveItems:=True
    else
      AccessName.AdviseActiveItems:=CompareText(nd2.NodeValue,'True')=0;
    AccessName.Tags:=TTags.Create(FOwnerConfig);
    AccessName.IOServer:=ConstructIOServer(AccessName);
    if Assigned(AccessName.IOServer) then
      Add(AccessName);
  end;
  Doc.Free;
end;

procedure TAccessNames.SaveConfig;
var
  Doc:TXMLDocument;
  ndRoot,ndAccessNames,Child,nd2:TDOMNode;
  i:Integer;
  AccessName:TAccessName;
begin
  Doc:=TXMLDocument.Create;
  ndRoot:=doc.CreateElement('body');
  TDOMElement(ndRoot).SetAttribute('version', '1.0');
  doc.Appendchild(ndRoot);

  ndRoot:=doc.DocumentElement;

  ndAccessNames:=doc.CreateElement('AccessNames');
  ndRoot.AppendChild(ndAccessNames);

  for i:=0 to (Count-1) do
  begin
    AccessName:=TAccessName(Items[i]);
    child:=doc.CreateElement('AccessName');
    //<AccessName ID="1" TYPE="MODBUS/TCP" ADDRESS="127.0.0.1">LocalPLC</AccessName>
    TDOMElement(child).SetAttribute('ID',AccessName.ID);
    TDOMElement(child).SetAttribute('TYPE',AccessName.IO_Type);
    TDOMElement(child).SetAttribute('ADDRESS',AccessName.Address);
    TDOMElement(child).SetAttribute('AdviseActiveItems',BoolToStr(AccessName.AdviseActiveItems,True));
    child.AppendChild(doc.CreateTextNode(AccessName.Name));
    ndAccessNames.AppendChild(child);
  end;
  writeXMLFile(Doc, FFileName);
  Doc.Free;
end;

function TAccessNames.GetScanTimeMs:LongInt;
var
  i:Integer;
  an:TAccessName;
  k,max:LongInt;
begin
  max:=0;
  for i:=0 to (Count - 1) do
  begin
    an:=TAccessName(Items[i]);
    if Assigned(an.IOServer) then
    begin
      an.IOServer.Lock;
      try
        k:=an.IOServer.ScanTime;
      finally
        an.IOServer.UnLock;
      end;
      if k>max then
        max:=k;
    end;
  end;
  Result:=max;
end;

end.

