--Verification of the CHANGE event
--This is an include file for the event testing.  It contains some
--definitions and functions that are useful througout the event tests

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

BoolTag = "EventChBool"
IntTag = "EventChInt"

--Boolean Change Events
tag_add(BoolTag, "BOOL", 20)
arr = {}
for n=1,20 do
  arr[n] = false;
end


e = event_add(BoolTag.."[3]", 9, "CHANGE", 0, callback, HIT)
EventTest(BoolTag, arr, MISS)
arr[3] = true
EventTest(BoolTag, arr, MISS)
arr[4] = true
EventTest(BoolTag, arr, HIT)
arr[12] = true
EventTest(BoolTag, arr, HIT)
arr[13] = true
EventTest(BoolTag, arr, MISS)
--Delete the event when we are done
event_del(e)

tag_add(IntTag, "INT", 20)
event_add(IntTag.."[5]", 10, "CHANGE", 0, callback, HIT)

EventTest(IntTag.."[4]",  22, MISS)
EventTest(IntTag.."[5]",  22, HIT)
EventTest(IntTag.."[14]", 22, HIT)
EventTest(IntTag.."[15]", 22, MISS)
EventTest(IntTag.."[14]", 22, MISS)
EventTest(IntTag.."[14]", 23, HIT)

arr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} 
EventTest(IntTag, arr, HIT)
EventTest(IntTag, arr, MISS)

arr = {0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 17, 18, 19}
EventTest(IntTag, arr, MISS)
 
arr = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 17, 18, 19}
EventTest(IntTag, arr, HIT)

arr = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 17, 18, 19}
EventTest(IntTag, arr, MISS)

arr = {0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 14, 15, 16, 17, 18, 19}
EventTest(IntTag, arr, HIT)
