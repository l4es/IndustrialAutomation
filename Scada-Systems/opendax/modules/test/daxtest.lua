--This is the main daxtest lua script.  The tests are each hard coded into
--the daxtest module, but the tests are selected and run from this Lua script

PASS = 0
FAIL = 1

run_test("tests/random.lua", "Random Tag Addition Test")
run_test("tests/tagname.lua", "Tagname Addition Test")
run_test("tests/handles.lua", "Tag Handle Retrieval Test")

run_test("tests/status.lua", "Status Retrieve test")
run_test("tests/readwrite.lua", "Read / Write Test")
run_test("tests/typefail.lua", "Type Fail Test")
run_test("tests/tagmodify.lua", "Tag Modification Test")

run_test("tests/eventadd.lua", "Event Addition/Removal Test")
run_test("tests/eventwrite.lua", "Event Write Test")
run_test("tests/eventchange.lua", "Event Change Test")
run_test("tests/eventset.lua", "Event Set/Reset Test")
run_test("tests/eventequal.lua", "Event Equal Test")
run_test("tests/eventgreater.lua", "Event Greater Than Test")
run_test("tests/eventless.lua", "Event LessThan Test")
run_test("tests/eventdeadband.lua", "Event Deadband Test")

run_test("tests/events.lua", "Event Notification Test")

--run_test("tests/lazy.lua", "Lazy Programmer Test")

