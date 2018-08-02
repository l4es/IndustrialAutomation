-- _firstrun is only true the first time the script is called
if _firstrun then
  print("First Run of Demo 1")
  StaticTest = {}
  StaticTest.x = 10
  StaticTest.y = 100
  tag_add("LuaDemo1INT", "INT", 1)
  tag_add("LuaDemo1exec", "DINT", 1)
  tag_add("LuaDemo1lastscan", "DINT", 1)
  
  --The name of the script configured in daxlua.conf
  print(_name)
  --The full path to the script file
  print(_filename)
end

-- _executions is the number of times the script has been executed
tag_write("LuaDemo1exec", _executions)
-- _lastscan is the number of milliseconds that it took to run the script the last time
tag_write("LuaDemo1lastscan", _lastscan)


--These Tags were registered in init.lua so they are always available
LuaDemoDINT = LuaDemoDINT + 1
LuaDemoTag.Index = LuaDemoTag.Index + 1
LuaDemoTag.Offset = LuaDemoTag.Index + 5

--Tags can be read/written manually from the script
DemoINT = tag_read("LuaDemo1INT", 0)
DemoINT = DemoINT + 1
if DemoINT > 5000 then DemoINT = 0 end
tag_write("LuaDemo1INT", DemoINT)

--This is the static variable that we registered.  It will not be sent to
-- the server but will be available between calls to this script
StaticTest.x = StaticTest.x + 1
StaticTest.y = StaticTest.y + 10

print("Demo 1 - StaticTest(x,y) = " .. StaticTest.x .. ", " .. StaticTest.y)

