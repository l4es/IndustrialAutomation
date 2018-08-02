--Verification of the DEADBAND event

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

IntTag = "EventDbInt"

tag_add(IntTag, "INT", 20)
event_add(IntTag.."[5]", 10, "DEADBAND", 2, callback, HIT)

EventTest(IntTag.."[4]",  22, MISS)
EventTest(IntTag.."[5]",  22, HIT)
EventTest(IntTag.."[5]",  22, MISS)
EventTest(IntTag.."[5]",  23, MISS)
EventTest(IntTag.."[5]",  24, HIT)
EventTest(IntTag.."[5]",  24, MISS)
EventTest(IntTag.."[5]",  23, MISS)
EventTest(IntTag.."[5]",  22, HIT)
EventTest(IntTag.."[5]",  240, HIT)
EventTest(IntTag.."[5]",  241, MISS)
EventTest(IntTag.."[5]",  242, HIT)

EventTest(IntTag.."[14]", 22, HIT)
EventTest(IntTag.."[15]", 22, MISS)
EventTest(IntTag.."[14]", 22, MISS)
EventTest(IntTag.."[14]", 23, MISS)
EventTest(IntTag.."[14]", 24, HIT)

--[[
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
--]]