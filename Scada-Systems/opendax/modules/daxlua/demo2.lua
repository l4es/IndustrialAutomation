if _firstrun then
  print("First Run of Demo 2")
  StaticTest = {}
  StaticTest.x = 20
  StaticTest.y = 200
  
  tag_add("LuaDemo2INT", "INT", 1)
  tag_add("LuaDemo2exec", "DINT", 1)
  tag_add("LuaDemo2lastscan", "DINT", 1)
  
  --The name of the script configured in daxlua.conf
  print(_name)
  --The full path to the script file
  print(_filename)
end

-- _executions is the number of times the script has been executed
tag_write("LuaDemo2exec", _executions)
-- _lastscan is the number of milliseconds that it took to run the script the last time
tag_write("LuaDemo2lastscan", _lastscan)


print("Demo2 - LuaDemoDINT = " .. LuaDemoDINT)
-- Since we registered this with mode of "R" this will have no effect on the
-- value to the server but it will change the value for the duration of the script
LuaDemoDINT = LuaDemoDINT + 1

Demo2NT = tag_read("LuaDemo2INT", 0)
Demo2NT = Demo2NT + 1
if Demo2NT > 50 then 
  Demo2NT = 0
  -- _rate is the script interval in mSec.  It is read/write so
  -- the script can change it's own rate.
  FAST_RATE = 500
  SLOW_RATE = 2000
  if _rate == FAST_RATE then
    _rate = SLOW_RATE
  else
    _rate = FAST_RATE
  end
end
tag_write("LuaDemo2INT", Demo2NT)


StaticTest.x = StaticTest.x + 1
StaticTest.y = StaticTest.y + 10

print("Demo 2 - StaticTest(x,y) = " .. StaticTest.x .. ", " .. StaticTest.y)
