unit UnitTags;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, UnitTimers;

type
  TTags=class;

  //<Tag TagName="TEPm32040109" AccessName="1" Type="I/O Real" Item="400501 F" MinRaw="0" MaxRaw="32000" MinEU="0" MaxEU="100" EngUnits="grad.C" Comment=""></Tag>
  TTag=class
  public
    TagName:String;
    AccessNameID:String;
    AccessName:TObject;
    TagType:String;
    Item:String;
    MinRaw:Double;
    MaxRaw:Double;
    MinEU:Double;
    MaxEU:Double;
    EngUnits:String;
    Comment:String;
    OnMsg:String;
    OffMsg:String;
    //Runtime
    Status:Integer;
    Value:Double;
    SetValue:Double;
    NeedSet:Boolean;
  end;

  TTags=class(TList)
  private
    FOwnerConfig:TObject;
    FFileName:String;
  public
    constructor Create(AOwner:TObject);
    function GetTagByName(tag_name:String):TTag;
    procedure LoadConfig(file_name:String);
    procedure SaveConfig;
  end;

  function TryStrToFloat(s:String):Double;

implementation

Uses
    DOM, XMLRead, XMLWrite,
    UnitConfig, UnitAccessNames;

constructor TTags.Create(AOwner:TObject);
begin
  Inherited Create;
  FOwnerConfig:=AOwner;
  FFileName:='';
end;

function TTags.GetTagByName(tag_name:String):TTag;
var
  Tag:TTag;
  i:Integer;
begin
  Result:=nil;
  for i:= 0 to (Count - 1) do
  begin
    Tag:=TTag(Items[i]);
    if Tag.TagName=tag_name then
    begin
      Result:=Tag;
      break;
    end;
  end;
end;

function CompareTags(Item1, Item2: Pointer): Integer;
var
   tag1,tag2:TTag;
begin
  tag1:=TTag(Item1);
  tag2:=TTag(Item2);
  if tag1.AccessNameID<tag2.AccessNameID then
    Result:=-1
  else
  if tag1.AccessNameID>tag2.AccessNameID then
    Result:=1
  else
  if tag1.Item<tag2.Item then
    Result:=-1
  else
  if tag1.Item>tag2.Item then
    Result:=1
  else
    Result:=0;
end;

procedure TTags.LoadConfig(file_name:String);
var
  Doc:TXMLDocument;
  ndTags,Child,nd1:TDOMNode;
  j:Integer;
  Tag:TTag;
begin
  FFileName:=file_name;
  ReadXMLFile(Doc, file_name);
  ndTags:=Doc.DocumentElement.FindNode('Tags');
  cfg:=TConfig(FOwnerConfig);
  for j:= 0 to (ndTags.ChildNodes.Count - 1) do
  begin
    Tag:=TTag.Create;
    child:=ndTags.ChildNodes[j];
    //<Tag TagName="TEPm32040109" AccessName="1" Type="I/O Real" Item="400501 F" MinRaw="0" MaxRaw="32000" MinEU="0" MaxEU="100" EngUnits="grad.C" Comment=""></Tag>
    Tag.TagName:=child.Attributes.GetNamedItem('TagName').NodeValue;
    Tag.AccessNameID:=child.Attributes.GetNamedItem('AccessName').NodeValue;
    Tag.AccessName:=cfg.AccessNames.GetAccessNameByID(Tag.AccessNameID);
    if Assigned(Tag.AccessName) then
    begin
      Tag.TagType:=child.Attributes.GetNamedItem('Type').NodeValue;
      Tag.Item:=child.Attributes.GetNamedItem('Item').NodeValue;
      Tag.MinRaw:=StrToFloat(child.Attributes.GetNamedItem('MinRaw').NodeValue);
      Tag.MaxRaw:=StrToFloat(child.Attributes.GetNamedItem('MaxRaw').NodeValue);
      Tag.MinEU:=StrToFloat(child.Attributes.GetNamedItem('MinEU').NodeValue);
      Tag.MaxEU:=StrToFloat(child.Attributes.GetNamedItem('MaxEU').NodeValue);
      Tag.EngUnits:=child.Attributes.GetNamedItem('EngUnits').NodeValue;
      Tag.Comment:=child.Attributes.GetNamedItem('Comment').NodeValue;
      Tag.Status:=1;//Error in start
      nd1:=child.Attributes.GetNamedItem('OnMsg');
      if Assigned(nd1) then
        Tag.OnMsg:=nd1.NodeValue;
      nd1:=child.Attributes.GetNamedItem('OffMsg');
      if Assigned(nd1) then
        Tag.OffMsg:=nd1.NodeValue;
      Add(Tag);
    end;
  end;

  Sort(@CompareTags);

  for j:=0 to Count-1 do
    TTags(TAccessName(TTag(Items[j]).AccessName).Tags).Add(Items[j]);

  Doc.Free;
end;

procedure TTags.SaveConfig;
var
  Doc:TXMLDocument;
  ndRoot,ndAccessNames,Child:TDOMNode;
  i:Integer;
  tg:TTag;
begin
  Doc:=TXMLDocument.Create;
  Doc.Encoding:='UTF-8';
  ndRoot:=doc.CreateElement('body');
  TDOMElement(ndRoot).SetAttribute('version', '1.0');
  doc.Appendchild(ndRoot);

  ndRoot:=doc.DocumentElement;

  ndAccessNames:=doc.CreateElement('Tags');
  ndRoot.AppendChild(ndAccessNames);

  for i:=0 to (Count-1) do
  begin
    tg:=TTag(Items[i]);
    child:=doc.CreateElement('Tag');

    TDOMElement(child).SetAttribute('TagName',tg.TagName);
    TDOMElement(child).SetAttribute('AccessName',TAccessName(tg.AccessName).ID);
    TDOMElement(child).SetAttribute('Type',tg.TagType);
    TDOMElement(child).SetAttribute('Item',tg.Item);
    TDOMElement(child).SetAttribute('MinRaw',FloatToStr(tg.MinRaw));
    TDOMElement(child).SetAttribute('MaxRaw',FloatToStr(tg.MaxRaw));
    TDOMElement(child).SetAttribute('MinEU',FloatToStr(tg.MinEU));
    TDOMElement(child).SetAttribute('MaxEU',FloatToStr(tg.MaxEU));
    TDOMElement(child).SetAttribute('EngUnits',tg.EngUnits);
    TDOMElement(child).SetAttribute('Comment',tg.Comment);
    TDOMElement(child).SetAttribute('OnMsg',tg.OnMsg);
    TDOMElement(child).SetAttribute('OffMsg',tg.OffMsg);

    ndAccessNames.AppendChild(child);
  end;
  writeXMLFile(Doc, FFileName);
end;

function TryStrToFloat(s:String):Double;
begin
  Result:=0;
  if s<>'' then
  begin
    try
      Result:=StrToFloat(s);
    except
    end;
  end;
end;

end.

