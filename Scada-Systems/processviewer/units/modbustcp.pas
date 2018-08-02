unit ModbusTCP;

{$mode objfpc}{$H+}

interface

uses
  SysUtils,
  Dialogs,
  LCLIntf,
  Sockets, Classes,
  UnitIOThread;

const
  SOCKET_ERROR=-1;
  INVALID_SOCKET=-1;

type
  TBufer = array[0..32768] of byte;
  TModbusBuf = array[0..255] of byte;
  TModbusRegs = array[0..255] of Word;

  TModbusTcpGate = class(TIOThread)
  private
    fAnswerBuff: TModbusBuf;
    FBufer: TBufer;
    fbytesToReceive: Integer;
    fbytesToSend: Integer;
    fSockConnected: Boolean;
    fSocket: TSocket;
    iPort: integer;
    sIP: string;
    FirstRun_ReadAlarms:Boolean;
  public
    constructor Create(ip: string; port: integer = 502);
    destructor Destroy; override;
    function Communicate: Boolean;
  private
    function SockConnect: Boolean;
    procedure SockDisconnect;
    function GetFunction(var MBreg:Integer):Integer;
  protected
    Procedure SetTimeout(Value:Integer);override;
    procedure DoExecute(Sender: TObject);
  public
    Regs:TModbusRegs;
    LastReadResult:Boolean;
    LastError:String;

    function ReadMultipleCoils(StartingAddress, NumberOfCoils: Integer): Boolean;
    function ReadMultipleRegisters(StartingAddress, NumberOfRegisters: Integer):
      Boolean;
    function WriteSingleCoil(Coil: Integer; NewValue: Boolean = true): Boolean;
    function WriteSingleRegister(RegisterNumber, NewValue: Integer): Boolean;

    function GetBitFromReg(RegW:Word;BitNo:Integer):Boolean;

    function ReadTags():Boolean;
    function WriteTags():Boolean;

    property Port:Integer read iPort write iPort;
    property HostIP:string read sIP write sIP;
  end;

implementation

Uses
  UnitConfig, UnitTags, UnitThreadLog;

{ TModbusTcpGate }

constructor TModbusTcpGate.Create(ip: string; port: integer);
begin
  Inherited CreateThread;
  sIP := ip;
  iPort := port;
  FOnExecute:=@DoExecute;
  FirstRun_ReadAlarms:=True;
end;
 
destructor TModbusTcpGate.Destroy; 
begin 
  SockDisconnect;
  inherited;
end; 
 
function TModbusTcpGate.Communicate: Boolean; 
var 
  res: Integer; 
begin 
  Lock; 
  Result := false; 
  try 
    try
      if not fSockConnected then
        if not SockConnect then
          Exit;
      res := fpsend(fSocket, @FBufer, fbytesToSend, 0);
      if res = SOCKET_ERROR then
      begin
        SockDisconnect;
        //EPrint('Ошибка отправки сообщения');
        SockConnect;
        Exit;
      end;
      res := fprecv(fSocket, @FBufer, fbytesToReceive, 0);
      if (res > 0) and (res <> fbytesToReceive) then
      begin
        //EPrint('Контроллер вернул ошибку');
        Exit;
      end;
      if res <= 0 then
      begin
        SockDisconnect;
        //EPrint('Ошибка получения ответа');
        SockConnect;
        Exit;
      end;
      Result := true;
    finally
      Unlock;
    end;
  except on E: Exception do
    begin 
      //EPrint(E.Message); 
      Unlock; 
    end; 
  end; 
end; 
 
Procedure TModbusTcpGate.SetTimeout(Value:Integer);
var
  von: Integer;
  vlen: Integer;
begin
  Inherited SetTimeout(Value);
  if fSocket<>INVALID_SOCKET then
  begin
    von := FTimeout;
    vlen := SizeOf(von);
    fpsetsockopt(fSocket, SOL_SOCKET, SO_SNDTIMEO, @von, vlen);
    fpsetsockopt(fSocket, SOL_SOCKET, SO_RCVTIMEO, @von, vlen);
  end;
end;

function TModbusTcpGate.SockConnect: Boolean;
var
  von:Integer;
  vlen:Integer;
  flag:Integer;
  bufsize:Integer;
  fSockAddr:TInetSockAddr;
begin
  Result := false;
  fSockConnected := true;
  fSocket := fpsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_IP);
  if fSocket = INVALID_SOCKET then
  begin
    fSockConnected := false;
    LastError:='Ошибка создания сокета';
    //Synchronize(ViewLastError);
    Exit;
  end;
  von := FTimeout;
  vlen := SizeOf(von);
  fpsetsockopt(fSocket, SOL_SOCKET, SO_SNDTIMEO, @von, vlen);
  fpsetsockopt(fSocket, SOL_SOCKET, SO_RCVTIMEO, @von, vlen);

  flag:=1;
  bufsize:= 1024*16;

  fpsetsockopt(FSocket, SOL_SOCKET,  SO_RCVBUF,    @bufsize,  sizeof(Integer));
  fpsetsockopt(FSocket, SOL_SOCKET,  SO_SNDBUF,    @bufsize,  sizeof(Integer));
  fpsetsockopt(FSocket, IPPROTO_TCP, TCP_NODELAY,  @flag,     sizeof(Integer));

  FillChar(fSockAddr, sizeof(fSockAddr), 0);

  fSockAddr.sin_family := AF_INET;
  fSockAddr.sin_port := htons(iPort);
  fSockAddr.sin_addr:=StrToNetAddr(sIP);
  if fpconnect(fSocket, @fSockAddr, sizeof(fSockAddr)) = SOCKET_ERROR then
  begin
    fSockConnected := false;
    LastError:='Ошибка подключения Sockets: '+ SysErrorMessage(SocketError);
    //Synchronize(ViewLastError);
    Exit;
  end;
  //MPrint('Успешно подключено к сокету:' + IntToStr(iPort) + ' по адресу: ' + sIP);
  Result := true; 
end; 
 
procedure TModbusTcpGate.SockDisconnect; 
begin 
  if fSockConnected then 
  begin 
    closesocket(fSocket); 
    fSockConnected := false; 
  end; 
end; 

function TModbusTcpGate.ReadMultipleCoils(StartingAddress, NumberOfCoils: Integer): Boolean;
var
  i,fnc: Integer;
begin
  fnc:=GetFunction(StartingAddress);
  FBufer[0] := 0; //2 байта - идентификатор передачи (любые)
  FBufer[1] := 0; //сервер скопирует в ответ
  FBufer[2] := 0; //2 байта - идентификатор протокола(всегда 0)
  FBufer[3] := 0;
  FBufer[4] := 0; //2 байта - количество последующих байт
  FBufer[5] := 6;
  FBufer[6] := 1; //1 байт - идентификатор узла
  FBufer[7] := fnc;//1; //1 байт - идентификатор команды
  FBufer[8] := Hi(Word(StartingAddress)); //2 байта - начальный адрес чтения
  FBufer[9] := Lo(Word(StartingAddress));
  FBufer[10] := Hi(Word(NumberOfCoils)); //2 байта - количество элементов для чтения
  FBufer[11] := Lo(Word(NumberOfCoils));
  fbytesToReceive := 9 + (NumberOfCoils) div 8;
  if NumberOfCoils mod 8 <> 0 then
    fbytesToReceive := fbytesToReceive + 1;
  fbytesToSend := 12;
  Result := Communicate;
  if Result then
    for i := 0 to NumberOfCoils - 1 do
    begin
      fAnswerBuff[i] := 1 and (FBufer[9 + i div 8] shr (i mod 8));
    end;
end;

function TModbusTcpGate.ReadMultipleRegisters(StartingAddress,
  NumberOfRegisters: Integer): Boolean;
var
  i,fnc: Integer;
begin
  fnc:=GetFunction(StartingAddress);
  FBufer[0] := 0; //2 байта - идентификатор передачи (любые)
  FBufer[1] := 0; //сервер скопирует в ответ
  FBufer[2] := 0; //2 байта - идентификатор протокола(всегда 0)
  FBufer[3] := 0;
  FBufer[4] := 0; //2 байта - количество последующих байт
  FBufer[5] := 6;
  FBufer[6] := 1; //1 байт - идентификатор узла
  FBufer[7] := fnc;//3; //1 байт - идентификатор команды
  FBufer[8] := Hi(Word(StartingAddress)); //2 байта - начальный адрес чтения
  FBufer[9] := Lo(Word(StartingAddress));
  FBufer[10] := Hi(Word(NumberOfRegisters)); //2 байта - количество элементов для чтения
  FBufer[11] := Lo(Word(NumberOfRegisters));
  if NumberOfRegisters>125 then
    NumberOfRegisters:=125;
  fbytesToReceive := 9 + NumberOfRegisters * 2;
  fbytesToSend := 12;
  Result := Communicate;
  if Result then
  begin
    for i := 0 to NumberOfRegisters - 1 do
      Regs[i]:=(FBufer[9 + i*2] shl 8)+FBufer[9 + i*2+1];
  end;
end;

function TModbusTcpGate.WriteSingleCoil(Coil: Integer; NewValue: Boolean = true): Boolean;
begin 
  GetFunction(Coil);
  FBufer[0] := 0; //2 байта - идентификатор передачи (любые)
  FBufer[1] := 0; //сервер скопирует в ответ
  FBufer[2] := 0; //2 байта - идентификатор протокола(всегда 0)
  FBufer[3] := 0;
  FBufer[4] := 0; //2 байта - количество последующих байт
  FBufer[5] := 6;
  FBufer[6] := 1; //1 байт - идентификатор узла
  FBufer[7] := 5; //1 байт - идентификатор команды
  FBufer[8] := Hi(Word(Coil)); //2 байта - адрес записи
  FBufer[9] := Lo(Word(Coil));
  if NewValue then //2 байта - значение 0xFF00 - включить 0x0000 - выключить
    FBufer[10] := $FF else
    FBufer[10] := 0;
  FBufer[11] := 0;
  fbytesToReceive := 12;
  fbytesToSend := 12;
  Result := Communicate;
end;

function TModbusTcpGate.WriteSingleRegister(RegisterNumber, NewValue: Integer):
  Boolean;
begin
  GetFunction(RegisterNumber);
  FBufer[0] := 0; //2 байта - идентификатор передачи (любые)
  FBufer[1] := 0; //сервер скопирует в ответ 
  FBufer[2] := 0; //2 байта - идентификатор протокола(всегда 0) 
  FBufer[3] := 0; 
  FBufer[4] := 0; //2 байта - количество последующих байт 
  FBufer[5] := 6;
  FBufer[6] := 1; //1 байт - идентификатор узла
  FBufer[7] := 6; //1 байт - идентификатор команды 
  FBufer[8] := Hi(Word(RegisterNumber)); //2 байта - адрес записи
  FBufer[9] := Lo(Word(RegisterNumber));
  FBufer[10] := Hi(Word(NewValue));
  FBufer[11] := Lo(Word(NewValue));
  fbytesToReceive := 12; 
  fbytesToSend := 12; 
  Result := Communicate; 
end;

function TModbusTcpGate.GetFunction(var MBreg:Integer):Integer;
var
  n:Integer;
begin
  n:=0;
  while MBReg>100000 do
  begin
    MBReg:=MBReg-100000;
    inc(n);
  end;
  while MBReg>10000 do
  begin
    MBReg:=MBReg-10000;
    inc(n);
  end;
  Dec(MBReg);
  Result:=n;
  case n of
    0: Result:=1;//DI
    1: Result:=2;//DO
    3: Result:=4;//3xxxx AI regs
    4: Result:=3;//4xxxx regs
  end;
end;

//BitNo=0..15
function TModbusTcpGate.GetBitFromReg(RegW:Word;BitNo:Integer):Boolean;
var
  w:Word;
begin
  w:=1;
  if BitNo>0 then
    w:=w shl BitNo;
  Result:=(RegW and w)>0;
end;

function BytesToSingle(b:PByteArray):Single;
begin
  Result:=PSingle(b)^;
end;

function ConvertRawToEU(f:Double;tg:TTag):Double;
begin
  if (tg.MaxRAW-tg.MinRAW)=0 then
    Result:=0
  else
  begin
    Result:=tg.MinEU+(f-tg.MinRAW)*(tg.MaxEU-tg.MinEU)/(tg.MaxRAW-tg.MinRAW);
    if Result<tg.MinEU then
      Result:=tg.MinEU;
    if Result>tg.MaxEU then
      Result:=tg.MaxEU;
  end;
end;

function ConvertEUToRaw(f:Double;tg:TTag):Double;
begin
  if (tg.MaxEU-tg.MinEU)=0 then
    Result:=0
  else
  begin
    Result:=tg.MinRAW+(f-tg.MinEU)*(tg.MaxRAW-tg.MinRAW)/(tg.MaxEU-tg.MinEU);
    if Result<tg.MinRAW then
      Result:=tg.MinRAW;
    if Result>tg.MaxRAW then
      Result:=tg.MaxRAW;
  end;
end;

function TModbusTcpGate.ReadTags():Boolean;
var
  i,fnc,reg1:Integer;
  tg:TTag;
  reg:Cardinal;
  regno,n:Integer;
  s,prefix:String;
  f:Double;
begin
  Result:=False;
  prefix:='TModbusTcpGate['+sIP+'] ';
  for i:=0 to Tags.Count-1 do
  begin
    TagsLock;
    try
      tg:=TTag(Tags[i]);
      s:=tg.Item;
    finally
      TagsUnLock;
    end;

    n:=pos('F',s);
    if n>0 then
    begin
      regno:=2;
      try
        reg:=StrToInt(trim(copy(s,1,n-1)));
      except
        regno:=0;
        LogThr.AddToLog(prefix+'except was in StrToInt: '+trim(copy(s,1,n-1)));
      end;
    end
    else
    begin
      regno:=1;
      try
        reg:=StrToInt(s);
      except
        regno:=0;
        LogThr.AddToLog(prefix+'except was in StrToInt: '+s);
      end;
    end;

    if regno>0 then
    begin
      reg1:=reg;
      fnc:=GetFunction(reg1);
      if fnc>2 then
      begin
        Result:=ReadMultipleRegisters(reg,regno);
        if Result then
        begin
          if n>0 then
            f:=BytesToSingle(@Regs[0])
          else
            f:=SmallInt(Regs[0]);

          TagsLock;
          try
            tg.Status:=0;
            tg.Value:=ConvertRawToEU(f,tg);
          finally
            TagsUnLock;
          end;
        end;
      end
      else
      begin
        Result:=ReadMultipleCoils(reg,1);
        if Result then
        begin
          TagsLock;
          try
            tg.Status:=0;
            tg.Value:=fAnswerBuff[0];
          finally
            TagsUnLock;
          end;
        end;
      end;
    end
    else
      Result:=False;
    if not Result then
    begin
      TagsLock;
      try
        tg.Status:=1;
      finally
        TagsUnLock;
      end;
    end;
    sleep(1);
  end;
end;

function TModbusTcpGate.WriteTags():Boolean;
var
  i,k:Integer;
  tg:TTag;
  reg:Integer;
  regno,n,reg1,fnc:Integer;
  s,prefix:String;
  fl,flDiscrete:Boolean;
  p:PWordArray;
  f:Double;
  fs:Single;
begin
  Result:=False;
  prefix:='TModbusTcpGate['+sIP+'] ';
  for i:=0 to Tags.Count-1 do
  begin
    TagsLock;
    try
      tg:=TTag(Tags[i]);
      s:=tg.Item;
      f:=tg.SetValue;
      fl:=tg.NeedSet;
      tg.NeedSet:=False;
    finally
      TagsUnLock;
    end;
    if not fl then
      Continue;

    flDiscrete:=False;
    n:=pos('F',s);
    if n>0 then
    begin
      fs:=ConvertEUToRaw(f,tg);
      regno:=2;
      p:=@fs;
      Regs[0]:=p^[0];
      Regs[1]:=p^[1];
      try
        reg:=StrToInt(trim(copy(s,1,n-1)));
      except
        regno:=0;
        LogThr.AddToLog(prefix+'except was in StrToInt('+trim(copy(s,1,n-1))+')');
      end;
    end
    else
    begin
      regno:=1;
      try
        reg:=StrToInt(s);
      except
        regno:=0;
        LogThr.AddToLog(prefix+'except was in StrToInt('+s+')');
      end;
      if (regno>0) then
      begin
        reg1:=reg;
        fnc:=GetFunction(reg1);
        flDiscrete:=fnc<3;
        if not flDiscrete then
        begin
          f:=ConvertEUToRaw(f,tg);
          Regs[0]:=Round(f);
        end;
      end;
    end;

    if regno>0 then
    begin
      if flDiscrete then
        Result:=WriteSingleCoil(reg,f>0)
      else
      begin
        for k:=0 to regno-1 do
          Result:=WriteSingleRegister(reg+k,Regs[k])
      end;
      if not result then
      begin
        TagsLock;
        try
          tg.NeedSet:=True;
        finally
          TagsUnLock;
        end;
      end;
    end
    else
      Result:=False;
    sleep(1);
  end;
end;

//Threaded reading
procedure TModbusTcpGate.DoExecute(Sender: TObject);
begin
  WriteTags();
  LastReadResult:=ReadTags();
end;

end.

