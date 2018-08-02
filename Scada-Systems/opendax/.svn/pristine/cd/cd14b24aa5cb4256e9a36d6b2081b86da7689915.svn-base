--Verification of the WRITE event

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

--First we check the write event

tag_add("EventWDint", "DINT", 20)
e = event_add("EventWDint[5]", 10, "WRITE", 0, callback, HIT)

EventTest("EventWDint[4]",  22, MISS)
EventTest("EventWDint[5]",  22, HIT)
EventTest("EventWDint[14]", 22, HIT)
EventTest("EventWDint[15]", 22, MISS)

event_del(e)