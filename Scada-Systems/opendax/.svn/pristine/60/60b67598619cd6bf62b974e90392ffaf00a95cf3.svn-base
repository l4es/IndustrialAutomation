--This checks that the tagnames that should fail do so and that tags
--that should be allowed are allowed
fail = {"1Tag", "-Tag", "Tag-name", "Tag&name", "TagNameIsWayTooLong12345678912345"}
pass = {"_Tag", "Tag1", "tAg_name", "t1Ag_name", "TagNameIsBarelyLongEnoughToFit12"}
duplicate = pass[2];

--This loop iterates through the tagnames from 'fail' to make sure that they
--are not allowed to be added
for index,value in ipairs(fail) do
    result = pcall(tag_add, value, "DINT", 1)
    if(result == true) then
       error("Should not have been able to add tag " .. value)
    end
end

--This loop iterates through the tagnames from 'pass' to make sure that they
--can be added
for index,value in ipairs(pass) do
    result = pcall(tag_add, value, "DINT", 1)
    if(result == false) then
       error("Should have been able to add tag " .. value)
    end
end

--And now we test for a duplicate
--First a resize of the tag should be allowed
tag_add(duplicate, "DINT", 2)

--But if the datatype is changed it should fail
result = pcall(tag_add, duplicate, "BOOL", 1)
if(result == true) then
   error("Should not have been able to add tag " .. value)
end
