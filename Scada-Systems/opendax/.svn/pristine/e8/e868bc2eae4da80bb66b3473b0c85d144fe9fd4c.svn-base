--Verification of the EQUAL event


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

tag_add("EventInt", "INT", 20)

e = event_add("EventInt[5]", 5, "EQUAL", 25, callback, HIT)

EventTest("EventInt[4]", 25, MISS)

EventTest("EventInt[5]", 24, MISS)
EventTest("EventInt[5]", 25, HIT)
EventTest("EventInt[5]", 25, MISS)
EventTest("EventInt[5]", 26, MISS)
EventTest("EventInt[5]", 25, HIT)

EventTest("EventInt[9]", 25, HIT)
EventTest("EventInt[10]", 25, MISS)

event_del(e)
