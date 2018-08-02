--Verification of the SET and RESET events

--Some definitions
HIT = 1
MISS = 0

lastevent = 0

function callback(x)
--   print("Lua Callback - lastevent = " .. x)
   lastevent = x
end

function EventTest(tagname, val, test)
    lastevent = MISS
    tag_write(tagname, val)
    event_poll()
    if test ~= lastevent then
        error("Event for " .. tagname .. " Failed", 2)
    end
end   

BoolTag = "EventSRBool"

arr={}

--Boolean Set Events
for n=1,20 do
  arr[n] = false;
end

tag_add(BoolTag, "BOOL", 20)

tag_write(BoolTag, arr)
e = event_add(BoolTag.."[3]", 9, "SET", 0, callback, HIT)

EventTest(BoolTag, arr, MISS)
arr[3] = true
EventTest(BoolTag, arr, MISS)
arr[4] = true
EventTest(BoolTag, arr, HIT)
arr[12] = true
EventTest(BoolTag, arr, HIT)
arr[13] = true
EventTest(BoolTag, arr, MISS)
--Check that we only get it once for each time we set the bit
EventTest(BoolTag.."[11]", true, MISS) --Already fired this one
EventTest(BoolTag.."[11]", false, MISS)
EventTest(BoolTag.."[11]", true, HIT)

--Delete the event when we are done
event_del(e)

--Boolean Reset Events
for n=1,20 do
  arr[n] = false;
end
tag_write(BoolTag, arr)
e = event_add(BoolTag.."[3]", 9, "RESET", 0, callback, HIT)

--Because some of the bits are already false and our mask in the event is 
--  all zeros the first write will fire the event.
EventTest(BoolTag, arr, HIT)

--Check that we only get it once for each time we set the bit
EventTest(BoolTag.."[11]", true, MISS)
EventTest(BoolTag.."[11]", false, HIT)
EventTest(BoolTag.."[11]", false, MISS)
EventTest(BoolTag.."[11]", true, MISS)
EventTest(BoolTag.."[11]", false, HIT)

--Writing all 1's shouldn't hit this message
for n=1,20 do
  arr[n] = true;
end
EventTest(BoolTag, arr, MISS)
--check the range
arr[3] = false
EventTest(BoolTag, arr, MISS)
arr[4] = false
EventTest(BoolTag, arr, HIT)
arr[12] = false
EventTest(BoolTag, arr, HIT)
arr[13] = false
EventTest(BoolTag, arr, MISS)

--Delete the event when we are done
event_del(e)
