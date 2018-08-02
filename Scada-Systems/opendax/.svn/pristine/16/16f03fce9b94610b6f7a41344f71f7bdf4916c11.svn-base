--This test is used to make sure that modifying tags after they are accessed works
--correctly. 

--Create the tag and then access it to make sure that it's in the cache
tag_add("TagModifyTest", "INT", 8)
tag_read("TagModifyTest", 8)

--Modifying tags is allowed if the onlything that is changed is the length
--of the tag array gets larger.  Type and name cannot change.
--Modifiy it and then access the new part, if the cache has been cleared this should work
tag_add("TagModifyTest", "INT", 12)
tag_read("TagModifyTest[8]", 4)

tag_add("TagModifyBool", "BOOL", 4)
tag_write("TagModifyBool[1]", 1)

tag_add("TagModifyBool", "BOOL", 8)
tag_write("TagModifyBool[5]", 1)

tag_add("TagModifyBool", "BOOL", 12)

