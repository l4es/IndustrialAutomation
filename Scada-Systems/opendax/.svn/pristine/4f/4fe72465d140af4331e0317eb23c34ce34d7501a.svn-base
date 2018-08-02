--This test is to make sure that we get errors when we send a bad
--datatype to the dax_tag_add() function

result = pcall(tag_add, "TypeFailTest", 0, 1)
if(result == true) then
  error("Datatype of 0 should fail")
end

result = pcall(tag_add, "TypeFailTest2", "BADTYPE", 1)
if(result == true) then
  error("Datatype of \"BADTYPE\" should fail")
end
