unit UnitTimers;

{$mode objfpc}{$H+}
{$R-}

interface

uses
  lclintf;

type
  EventTimer = record
    StartTicks : LongInt;  {Tick count when timer was initialized}
    ExpireTicks : LongInt; {Tick count when timer will expire}
  end;

procedure NewTimer(var ET : EventTimer; MsTicks : LongInt);
procedure NewTimerSecs(var ET : EventTimer; Secs : LongInt);
procedure NewTimerMSecs(var ET : EventTimer; MSecs : LongInt);
function TimerExpired(ET : EventTimer) : Boolean;
function ElapsedTime(ET : EventTimer) : LongInt;
function ElapsedTimeInSecs(ET : EventTimer) : LongInt;
function ElapsedTimeInMSecs(ET : EventTimer) : LongInt;
function RemainingTime(ET : EventTimer) : LongInt;
function RemainingTimeInSecs(ET : EventTimer) : LongInt;

function GetTickCountFast : DWord;
function GetMsDiff(smaller,bigger:Cardinal):Integer;

implementation

const
  MaxTicks    = MaxLongInt;

procedure NewTimer(var ET : EventTimer; MsTicks : LongInt);
  {-Returns a set EventTimer that will expire in Ticks}
begin
  {Max acceptable value is MaxTicks}
  with ET do
  begin
    StartTicks := GetTickCountFast;
    ExpireTicks := StartTicks + MsTicks;
  end;
end;

procedure NewTimerSecs(var ET : EventTimer; Secs : LongInt);
  {-Returns a set EventTimer}
begin
  NewTimer(ET, Secs*1000);
end;

procedure NewTimerMSecs(var ET : EventTimer; MSecs : LongInt);
  {-Returns a set EventTimer}
begin
  NewTimer(ET, MSecs);
end;

function TimerExpired(ET : EventTimer) : Boolean;
  {-Returns True if ET has expired}
var
  CurTicks : LongInt;
begin
  with ET do begin
    {Get current Ticks; assume timer has expired}
    CurTicks := GetTickCountFast;
    TimerExpired := True;
    {Check normal expiration}
    if CurTicks > ExpireTicks then
      Exit;
    {Check wrapped CurTicks}
    if (CurTicks < StartTicks) and
       ((CurTicks + MaxTicks) > ExpireTicks) then
      Exit;

    {If we get here, timer hasn't expired yet}
    TimerExpired := False;
  end;
end;

function ElapsedTime(ET : EventTimer) : LongInt;
  {-Returns elapsed time, in Ticks, for this timer}
var
  CurTicks : LongInt;
begin
  with ET do begin
    CurTicks := GetTickCountFast;
    if CurTicks >= StartTicks then
      {No wrap yet}
      ElapsedTime := CurTicks - StartTicks
    else
      {Got a wrap, account for it}
      ElapsedTime := (MaxTicks - StartTicks) + CurTicks;
  end;
end;

function ElapsedTimeInSecs(ET : EventTimer) : LongInt;
  {-Returns elapsed time, in seconds, for this timer}
begin
  ElapsedTimeInSecs := ElapsedTime(ET) div 1000;
end;

function ElapsedTimeInMSecs(ET : EventTimer) : LongInt;
  {-Returns elapsed time, in miliseconds, for this timer}
begin
  ElapsedTimeInMSecs := ElapsedTime(ET);
end;

function RemainingTime(ET : EventTimer) : LongInt;
  {-Returns remaining time, in Ticks, for this timer}
var
  CurTicks : LongInt;
  RemainingTicks : LongInt;
begin
  with ET do begin
    CurTicks := GetTickCountFast;
    if CurTicks >= StartTicks then
      {No wrap yet}
      RemainingTicks := ExpireTicks - CurTicks
    else
      {Got a wrap, account for it}
      RemainingTicks := (ExpireTicks - MaxTicks) - CurTicks;
  end;
  if RemainingTicks < 0 then
    RemainingTime := 0
  else
    RemainingTime := RemainingTicks;
end;

function RemainingTimeInSecs(ET : EventTimer) : LongInt;
  {-Returns remaining time, in seconds, for this timer}
begin
  RemainingTimeInSecs := RemainingTime(ET) div 1000;
end;

{ Centralized timer method -- }
{ if avail, return is the number of ms since the system started }
function GetTickCountFast : DWord;
begin
  Result:=GetTickCount;
end;

function GetMsDiff(smaller,bigger:Cardinal):Integer;
var
   tmp:Cardinal;
begin
  if bigger>=smaller then
    Result:=bigger-smaller
  else
  begin
    tmp:=High(Cardinal);
    tmp:=tmp-smaller;
    tmp:=tmp+bigger;
    inc(tmp);
    Result:=tmp;
  end;
end;

initialization

end.
