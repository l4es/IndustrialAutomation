--These tests are to make sure that we are reading and writing data to
--and from the server correctly.

--The tests are broken into functions so that when something fails it can be
--isolated to make debugging easier.  The functions are all called at the very
--end of the file.

--Recursive function for printing an entire table to the screen
--  Used for debugging only, not really needed for the test
function print_table(table, indent)
  local indentstr = ""

  for n=0,indent-1 do
    indentstr = indentstr .. " "
  end

  for t,v in pairs(table) do
    if type(v)=="table" then
      print(indentstr .. t)
      print_table(v, indent+2)
    else
      print(indentstr .. t .." = " .. tostring(v))
    end
  end
end

--This takes the numbers in the array 'tests' and writes them to tag 'name'
--one at a time and then reads them back and compares to make sure that it
--was read back correctly.
function CheckSingle(name, tests)
  for t,v in ipairs(tests) do
    --print("Checking " .. name .. " = " .. v)
    tag_write(name, v)
    y = tag_read(name, 0)
    if y ~= v then error(name .. " Problem " .. y .." ~= " .. v) end
  end
end


function CheckArray(name, x)
  tag_write(name, x)
  
  y = tag_read(name, 0)
  for t,v in ipairs(x) do
    --print(tostring(y[t]).." = "..tostring(x[t]))
    if x[t] ~= y[t] then
      error("Array Problem for "..name)
    end
  end
end

--This function cycles through the output table and compares the values
--that it finds there with the values in the input table if they exist
--If all the values match it returns true otherwise false
--This function falls apart if the values are floating point
function compare_tables(input, output)
  local t, v, result

  for t,v in pairs(output) do
    --print("input." .. t .. " = " .. tostring(input[t]) .. " output." .. t .. " = " .. tostring(output[t]))
    if input[t] ~= nil then
      if type(v) =="table" then
        if compare_tables(input[t], v) == false then return false end
      else
      --TODO: Need to see if it's a REAL and change the comparison
        if input[t] ~= v then 
          print("input[" .. t .. "] ~= " .. tostring(v))
          return false 
        end
      end
    end
  end
  return true
end

--We start by creating tags of every kind of base datatype.
types = {"BOOL", "BYTE", "SINT", "WORD", "INT", "UINT", "DWORD", "DINT", 
         "UDINT", "TIME", "REAL", "LWORD", "LINT", "ULINT", "LREAL"}

for n=1,15 do
  tag_add("RWTest"..types[n], types[n], 1)
  tag_add("RWTest"..types[n].."Array", types[n], 10)
end


--This is where we test all the single tags.  This test checks the bounds of
--each data type and will tell us if we have any data formatting problems
function CheckSingles()
    --Single BOOL tag test
    tag = "RWTestBOOL"
    x = true
    tag_write(tag, x)
    y = tag_read(tag, 0)
    if y == false then error(tag .. " Problem") end
    x = false
    tag_write(tag, x)
    y = tag_read(tag, 0)
    if y == true then error(tag .. " Problem") end
    
    --Single integer type checks are easy enough
    tests = {255, 127, 0, 127, 255}
    CheckSingle("RWTestBYTE", tests)
    
    tests = {-128, 0, 127, 64, -1}
    CheckSingle("RWTestSINT", tests)
    
    tests = {65535, 0, 34000, 12, 65535}
    CheckSingle("RWTestWORD", tests)
    CheckSingle("RWTestUINT", tests)
    
    tests = {-32768, 0, 32767, -12000, -1}
    CheckSingle("RWTestINT", tests)
    
    tests = { 2^31 * -1, 2^31 -1, 0, -1}
    CheckSingle("RWTestDINT", tests)
    
    tests = { 2^32-1, 0, 2^31 - 2}
    CheckSingle("RWTestDWORD", tests)
    CheckSingle("RWTestUDINT", tests)
    CheckSingle("RWTestTIME", tests)
    
    tests = {2^63 * -1, 0, 2^62, -1}
    CheckSingle("RWTestLINT", tests)
    
    tests = {2^62-1, 0, 1, 265364764}  --Can't really check all 64 bits with Lua's number type
    CheckSingle("RWTestLWORD", tests)
    CheckSingle("RWTestULINT", tests)
    
    --Floating point checks are a bit trickier
    tests = {3.14159265358979, 2.345678E-23, 1.23446536E23, 0}
    name = "RWTestREAL"
    for t,v in ipairs(tests) do
      --print("Checking " .. name .. " = " .. v)
      tag_write(name, v)
      y = tag_read(name, 0)
      if math.abs(y - v) > (v * 0.01) then error(name .. " Problem " .. y .." ~= " .. v) end
    end
    
    --LREAL works well because this is really the same number that Lua uses internally
    tests = {3.14159265358979323846, 2.345678E-253, 1.2345734987E253, 0}
    name = "RWTestLREAL"
    for t,v in ipairs(tests) do
      --print("Checking " .. name .. " = " .. v)
      tag_write(name, v)
      y = tag_read(name, 0)
      if math.abs(y - v) > 0.0000000001 then error(name .. " Problem " .. y .." ~= " .. v) end
    end
end

--This checks the base datatype array tags
function CheckArrays()
    --Now we check writing arrays of base datatypes
    x = {}
    
    x[1] = true
    x[2] = false
    x[3] = true
    x[4] = false
    x[5] = true
    x[6] = false
    x[7] = true
    x[8] = false
    x[9] = true
    x[10] = false
    
    CheckArray("RWTestBOOLArray", x)
    
    --This little test is to see if the masked writing works without messing up
    --the array elements between the ones that we want to change.
    
    xx = {}
    xx[2] = true
    x[2] = true
    xx[4] = true
    x[4] = true
    xx[7] = false
    x[7] = false
    
    tag_write("RWTestBOOLArray", xx)
    
    y = tag_read("RWTestBOOLArray", 0)
    
    for n=1,10 do
      if x[n] ~= y[n] then error("Masked write failed for RWTestBOOLArray at index " .. n) end
    end
    
    --Now check the integer arrays
    x[1] = 2
    x[2] = 0
    x[3] = 3
    x[4] = 5
    x[5] = 8
    x[6] = 13
    x[7] = 22
    x[8] = 255
    x[9] = 57
    x[10]= 92
    
    CheckArray("RWTestBYTEArray", x)
    
    x[1] = -1
    x[2] = -128
    x[8] = 25
    x[10]= 127
    
    CheckArray("RWTestSINTArray", x)
    
    x[1] = -1
    x[2] = 2000
    x[3] = 3001
    x[4] = 5009
    x[5] = 0
    x[6] = 1375
    x[7] = 32767
    x[8] = -32768
    x[9] = 5799
    x[10]= -92
    
    CheckArray("RWTestINTArray", x)
    
    x[1] = 1
    x[8] = x[8] * -1
    x[10]= 65535
    
    CheckArray("RWTestWORDArray", x)
    CheckArray("RWTestUINTArray", x)
    
    x[2] = 2^31 * -1
    x[10] = 2^31 -1
    
    CheckArray("RWTestDINTArray", x)
    
    x[2] = 2^32-1
    
    CheckArray("RWTestUDINTArray", x)
    CheckArray("RWTestDWORDArray", x)
    CheckArray("RWTestTIMEArray", x)
    
    --[[ TODO: Finish this
    CheckArray("RWTestLWORDArray", x)
    CheckArray("RWTestLINTArray", x)
    CheckArray("RWTestULINTArray", x)
    
    
    CheckArray("RWTestREALArray", x)
    CheckArray("RWTestLREALArray", x)
    --]]
    
    --This little test is to see if the masked writing works without messing up
    --the array elements between the ones that we want to change.
    for n=1,10 do
      x[n] = n
    end
    
    tag_write("RWTestINTArray", x)
    
    xx = {}
    xx[2] = 22
    x[2] = 22
    xx[4] = 44
    x[4] = 44
    tag_write("RWTestINTArray", xx)
    
    y = tag_read("RWTestINTArray", 0)
    
    for n=1,10 do
      if x[n] ~= y[n] then error("Masked write failed for RWTestINTArray at " .. x[n]) end
    end
end


--This is where we start to read / write custom datatype tags\
members = {{"Int",   "INT",   1},
           {"Bool",  "BOOL",  1},
           {"reBool", "BOOL", 1},
           {"triBool", "BOOL",1},
           {"Dint",  "DINT",  1},
           {"Udint", "UDINT", 1}}
                                
test1 = cdt_create("RWTestSimple", members)

tag_add("RWTestSimple", test1, 1)
tag_add("RWTestSimpleArray", test1, 10)


members2 = {{"Udint",      "UDINT",         1},
            {"IntArray",   "INT",          10},
            {"Test",       "RWTestSimple",  1},
            {"TestArray",  "RWTestSimple", 10},
            {"Dint",       "DINT",          1}}

test2 = cdt_create("RWTest", members2)

tag_add("RWTest", test2, 1)
tag_add("RWTestArray", test2, 11)

--This function runs through the checks of RWTestSimple.
function CheckSimpleCDT()
    s = "RWTestSimple"
    x = {}
    
    x.Int = 123
    x.Bool = true
    x.reBool = false
    x.triBool = true
    x.Dint = 456
    x.Udint = 789
    
    tag_write(s, x)
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end
    
    
    x.Bool = true
    x.reBool = true
    x.triBool = true
    
    tag_write(s, x)
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end
    
    x.Int = 3333
    x.Dint = 44444
    tag_write(s, x)
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end
    
    
    --This tests the reading and writing of individual bits
    s = "RWTestSimple.Bool"
    v = true
    tag_write(s, v)
    y = tag_read(s, 0)
    if y ~= v then error(s .. " Should be " .. tostring(v)) end
    
    v = false
    tag_write(s, v)
    y = tag_read(s, 0)
    if y ~= v then error(s .. " Should be " .. tostring(v)) end
    
    s = "RWTestSimple.reBool"
    v = true
    tag_write(s, v)
    y = tag_read(s, 0)
    if y ~= v then error(s .. " Should be " .. tostring(v)) end
    
    v = false
    tag_write(s, v)
    y = tag_read(s, 0)
    if y ~= v then error(s .. " Should be " .. tostring(v)) end
    
    s = "RWTestSimple.triBool"
    v = true
    tag_write(s, v)
    y = tag_read(s, 0)
    if y ~= v then error(s .. " Should be " .. tostring(v)) end
    
    v = false
    tag_write(s, v)
    y = tag_read(s, 0)
    if y ~= v then error(s .. " Should be " .. tostring(v)) end
end

function CheckSimpleCDTArray()
    x = {}
    
    for n=1,10 do
      x[n] = {}
      x[n].Int = n * 10 +1
      x[n].Dint = n * 10 +2
      x[n].Udint = n * 10 +3
      if x[n].Int % 4 == 0 then
         x[n].Bool = false
         x[n].reBool = false
         x[n].triBool = false
      elseif x[n].Int % 4 == 1 then
         x[n].Bool = true
         x[n].reBool = false
         x[n].triBool = false
      elseif x[n].Int % 4 == 2 then
         x[n].Bool = true
         x[n].reBool = false
         x[n].triBool = true
      else
         x[n].Bool = false
         x[n].reBool = true
         x[n].triBool = false
      end      
    end
    
    s = "RWTestSimpleArray"
    tag_write(s, x)
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end

    x = {}
    x[2] = {}
    x[2].Dint = 3333
    tag_write(s, x)
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end
    
end

--This checks the second set of tags that have CDTs nested within one another
function CheckComplexCDT() 
    x = {}
    
    x.Dint = 31415
    x.IntArray = {}
    x.IntArray[1] = 111
    x.IntArray[2] = 222
    
    x.Test = {}
    x.Test.Int = 123
    x.Test.Dint = 234
    x.Test.Udint = 345
    x.Test.Bool = true
    x.Test.reBool = false
    x.Test.triBool = true
    
    x.TestArray = {}
    
    for n=1,10 do
      x.TestArray[n] = {}
      x.TestArray[n].Int = n*100 + n*10 + n
      x.TestArray[n].Dint = (n*100 + n*10 + n) * -1
      x.TestArray[n].Udint = n*1000 + n*100 + n*10 + n
      if x.TestArray[n].Int % 4 == 0 then
         x.TestArray[n].Bool = false
         x.TestArray[n].reBool = false
         x.TestArray[n].triBool = false
      elseif x.TestArray[n].Int % 4 == 1 then
         x.TestArray[n].Bool = true
         x.TestArray[n].reBool = false
         x.TestArray[n].triBool = false
      elseif x.TestArray[n].Int % 4 == 2 then
         x.TestArray[n].Bool = true
         x.TestArray[n].reBool = false
         x.TestArray[n].triBool = true
      else
         x.TestArray[n].Bool = false
         x.TestArray[n].reBool = true
         x.TestArray[n].triBool = false
      end      

    end
    
    s = "RWTest"
    tag_write(s, x)  
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end
    
    x.TestArray[3].Bool = true
    x.TestArray[3].reBool = true
    x.TestArray[3].triBool = true
    
    tag_write(s, x)  
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end
    
    s = "RWTest.TestArray[4].Int"
    v = 8675
    tag_write(s, v)
    y = tag_read(s,0)
    if y ~= v then error("Single Tag Read Failed for " .. s) end
    
    s = "RWTest.TestArray[6].Bool"
    v = false
    tag_write(s, v)
    y = tag_read(s,0)
    if y ~= v then error("Single Tag Read Failed for " .. s) end
    
    v = true
    tag_write(s, v)
    y = tag_read(s,0)
    if y ~= v then error("Single Tag Read Failed for " .. s) end
    
    s = "RWTest.TestArray[7].reBool"
    v = false
    tag_write(s, v)
    y = tag_read(s,0)
    if y ~= v then error("Single Tag Read Failed for " .. s) end
    
    v = true
    tag_write(s, v)
    y = tag_read(s,0)
    if y ~= v then error("Single Tag Read Failed for " .. s) end
    
    s = "RWTest.TestArray[0].triBool"
    v = false
    tag_write(s, v)
    y = tag_read(s,0)
    if y ~= v then error("Single Tag Read Failed for " .. s) end
    
    v = true
    tag_write(s, v)
    y = tag_read(s,0)
    if y ~= v then error("Single Tag Read Failed for " .. s) end
        
end


function CreateRWTestTable(n)
    local x
    x = {}
    
    x.Dint = 31415
    x.IntArray = {}
    x.IntArray[1] = 111
    x.IntArray[2] = 222
    
    x.Test = {}
    x.Test.Int = 123
    x.Test.Dint = 234
    x.Test.Udint = 345
    x.Test.Bool = true
    x.Test.reBool = false
    x.Test.triBool = true
    
    x.TestArray = {}
    
    for n=1,10 do
      x.TestArray[n] = {}
      x.TestArray[n].Int = n*100 + n*10 + n
      x.TestArray[n].Dint = (n*100 + n*10 + n) * -1
      x.TestArray[n].Udint = n*1000 + n*100 + n*10 + n
      if x.TestArray[n].Int % 4 == 0 then
         x.TestArray[n].Bool = false
         x.TestArray[n].reBool = false
         x.TestArray[n].triBool = false
      elseif x.TestArray[n].Int % 4 == 1 then
         x.TestArray[n].Bool = true
         x.TestArray[n].reBool = false
         x.TestArray[n].triBool = false
      elseif x.TestArray[n].Int % 4 == 2 then
         x.TestArray[n].Bool = true
         x.TestArray[n].reBool = false
         x.TestArray[n].triBool = true
      else
         x.TestArray[n].Bool = false
         x.TestArray[n].reBool = true
         x.TestArray[n].triBool = false
      end      
    end
    return x
end

function CheckComplexCDTArray()
    local i
    x={}
    for i=1,11 do
      x[i] = CreateRWTestTable(i)
    end

    s = "RWTestArray"
    tag_write(s, x)  
    y = tag_read(s, 0)
    if compare_tables(x,y) ~= true then error("Table Compare Failed") end

end

CheckSingles()
CheckArrays()
CheckSimpleCDT()
CheckSimpleCDTArray()
CheckComplexCDT()
CheckComplexCDTArray()
