print "Starting DAXLua Init Script"

members = {{"Index", "INT", 1},
           {"Offset", "DINT", 1}}
 
cdt_create("LuaDemoCDT", members)

index = tag_add("LuaDemoTag", "LuaDemoCDT", 1)
index = tag_add("LuaDemoDINT", "DINT", 1)
index = tag_add("LuaTriggerTest", "DINT", 1)
--This registers a Lua variable for the script that will be kept
-- between calls to the script.
register_static("demo1", "StaticTest")

--This registers variables in the script that will represent a Dax
-- Tag of the same name.  The first argument is the name of the script
-- the second argument is the name of the tag and the third is the mode.
-- If the mode contains an 'R' then the tag will be read and the Lua
-- variable created before the script is run.  If the mode contains a 'W'
-- the Lua variable will be written to the DAX server after the script ends.
-- CDT's and arrays will become tables in the script.
register_tag("demo1","LuaDemoTag","RW")
register_tag("demo1","LuaDemoDINT","RW")

register_tag("demo2","LuaDemoDINT","R")
