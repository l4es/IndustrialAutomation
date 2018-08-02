--This test checks that handles are returned properly from the server.  It starts by setting up
--an environment of datatypes and tags.  Then it tries every combination that we can think of
--to get handles.  The parameters of what the returned handle should be are given and the
--called function tests these and if the PASS/FAIL test doesn't match then an error is returned
--and this test fails.  The CDT creation and tag addition functions should all pass.  If they
--fail then the error is allowed to raise to the caller and the test fail.

--TODO: Need to test the handle that is returned when tags are added

members = {{"Int5", "INT", 5},     --10
           {"Bool10", "BOOL", 10}, -- 2
           {"Dint1", "DINT", 1},   -- 4
           {"Dint3", "DINT", 3}}   --12
                                   --28 Total
test1 = cdt_create("Test1", members)

members = {{"Int3", "INT", 3},     --  6
           {"Test1", "Test1", 5}}  --140
                                   --146 Total
test2 = cdt_create("Test2", members)

members = {{"Int7", "INT", 7},      -- 14
           {"Bool30", "BOOL", 30},  --  1
           {"Bool32", "BOOL", 32},  --  1
           {"Test1", "Test1", 1},   -- 28
           {"Test2", "Test2", 2}}   --292
                                    --336 Total

test3 = cdt_create("Test3", members)

members = {{"Int", "INT", 1},
           {"Bool",  "BOOL",  1},
           {"reBool", "BOOL", 1},
           {"triBool", "BOOL",1},
           {"Dint",  "DINT",  1}}

test4 = cdt_create("Test4", members)

members = {{"Bool1", "BOOL", 10},
           {"Bool2",  "BOOL",  1},
           {"Bool3", "BOOL", 3}}

test5 = cdt_create("Test5", members)


tag_add("HandleBool1", "BOOL", 1)
tag_add("HandleBool7", "BOOL", 7)
tag_add("HandleBool8", "BOOL", 8)
tag_add("HandleBool9", "BOOL", 9)
tag_add("HandleBool33", "BOOL", 33)
tag_add("HandleInt", "INT", 2)
tag_add("HandleInt32", "INT", 32)
tag_add("HandleTest1", "Test1", 1)
tag_add("HandleTest2", test2, 5)
tag_add("HandleTest3", test3, 1)
tag_add("HandleTest4", test3, 10)
tag_add("HandleTestIV", test4, 1)
tag_add("HandleTest5", test5, 1)
tag_add("HandleTest5_10", test5, 10)


--Test that the handles returned are correct
--Each tag entry contains...
--    the tagname string to parse
--    item count  (N) - requested by test
-- Returned Values
--    byte offset (B)
--    bit offset  (b)
--    item count  (c) - returned by opendax
--    byte size   (s)
--    data type  
--    PASS/FAIL
--If FAIL then all arguments are ignored and the string/count is expected to fail.
--Otherwise the returned handle values must match those given
--  or the test will fail.
--           NAME                          N   B   b  c   s   TYPE      TEST  --
handle_test("HandleBool1",                 0,  0,  0, 1,  1,  "BOOL",   PASS)
handle_test("HandleBool7",                 0,  0,  0, 7,  1,  "BOOL",   PASS)
handle_test("HandleBool7[2]",              4,  0,  2, 4,  1,  "BOOL",   PASS)
handle_test("HandleBool7[6]",              1,  0,  6, 1,  1,  "BOOL",   PASS)
handle_test("HandleBool7[5]",              2,  0,  5, 2,  1,  "BOOL",   PASS)
handle_test("HandleBool7[6]",              2,  0,  6, 2,  1,  "BOOL",   FAIL)
handle_test("HandleBool7[7]",              1,  0,  2, 4,  1,  "BOOL",   FAIL)
handle_test("HandleBool8",                 0,  0,  0, 8,  1,  "BOOL",   PASS)
handle_test("HandleBool9",                 0,  0,  0, 9,  2,  "BOOL",   PASS)
handle_test("HandleBool9[8]",              0,  1,  0, 1,  1,  "BOOL",   PASS)
handle_test("HandleBool33",                0,  0,  0, 33, 5,  "BOOL",   PASS)
handle_test("HandleBool33[7]",             1,  0,  7, 1,  1,  "BOOL",   PASS)
handle_test("HandleBool33[3]",             8,  0,  3, 8,  2,  "BOOL",   PASS)
handle_test("HandleBool33[3]",             9,  0,  3, 9,  2,  "BOOL",   PASS)
handle_test("HandleBool33[3a]",            1,  0,  0, 0,  0,  "BOOL",   FAIL)
handle_test("HandleInt[1]",                1,  2,  0, 1,  2,  "INT",    PASS)
handle_test("HandleInt",                   0,  0,  0, 2,  4,  "INT",    PASS)
handle_test("HandleInt",                   1,  0,  0, 1,  2,  "INT",    PASS)
handle_test("HandleInt32",                 1,  0,  0, 1,  2,  "INT",    PASS)
handle_test("HandleInt[2]",                1,  0,  0, 0,  0,  "INT",    FAIL)
handle_test("HandleInt[2]",                5,  0,  0, 0,  0,  "INT",    FAIL)
handle_test("HandleTest1",                 0,  0,  0, 1,  28, "Test1",  PASS)
handle_test("HandleTest1.Dint3",           0,  16, 0, 3,  12, "DINT",   PASS)
handle_test("HandleTest1.Dint3[0]",        2,  16, 0, 2,  8,  "DINT",   PASS)
handle_test("HandleTest1.Dint3[1]",        2,  20, 0, 2,  8,  "DINT",   PASS)
handle_test("HandleTest1.Dint3[2]",        1,  24, 0, 1,  4,  "DINT",   PASS)
handle_test("HandleTest1.Dint3[2]",        2,  24, 0, 1,  4,  "DINT",   FAIL)
handle_test("HandleTest1.Dint1",           1,  12, 0, 1,  4,  "DINT",   PASS)
handle_test("HandleTest2[0].Test1[2]",     1,  62, 0, 1,  28, "Test1",  PASS)
handle_test("HandleTest2[0].Test1[1]",     2,  34, 0, 2,  56, "Test1",  PASS)
handle_test("HandleTest2[0].Test1[4]",     1,  118,0, 1,  28, "Test1",  PASS)
handle_test("HandleTest2[0].Test1[1]",     5,  32, 0, 1,  28, "Test1",  FAIL)
handle_test("HandleTest2[1].Test1",        0,  152,0, 5,  140,"Test1",  PASS)
handle_test("HandleTest2[0].Test1[0].Bool10[4]",
                                           1,  16, 4, 1,  1,  "BOOL",   PASS)
handle_test("HandleTest2[0].Test1[0].Bool10", 
                                           0,  16, 0, 10, 2,  "BOOL",   PASS)
handle_test("HandleTest2[0].Test1[1].Bool10", 
                                           0,  44, 0, 10, 2,  "BOOL",   PASS)
handle_test("HandleTest2[4].Test1[0].Bool10", 
                                           0,  600,0, 10, 2,  "BOOL",   PASS)
handle_test("HandleTest2[0].NotAMember",   0,  0,  0, 0,  0,  "Yup",    FAIL)
handle_test("NoTagName",                   0,  0,  0, 0,  0,  "Duh",    FAIL)
handle_test("",                            0,  0,  0, 0,  0,  "Yup",    FAIL)
handle_test("HandleTestIV.Bool",           0,  2,  0, 1,  1,  "BOOL",   PASS)
handle_test("HandleTestIV.reBool",         0,  2,  1, 1,  1,  "BOOL",   PASS)
handle_test("HandleTestIV.triBool",        0,  2,  2, 1,  1,  "BOOL",   PASS)

handle_test("HandleTest5",                 0,  0,  0, 1,  2,  "Test5",  PASS)
handle_test("HandleTest5.Bool1",           0,  0,  0, 10, 2,  "BOOL",   PASS)
handle_test("HandleTest5.Bool1[1]",        0,  0,  1, 1,  1,  "BOOL",   PASS)
handle_test("HandleTest5.Bool1[9]",        0,  1,  1, 1,  1,  "BOOL",   PASS)
handle_test("HandleTest5.Bool2",           0,  1,  2, 1,  1,  "BOOL",   PASS)
handle_test("HandleTest5.Bool3",           0,  1,  3, 3,  1,  "BOOL",   PASS)
handle_test("HandleTest5.Bool3[0]",        0,  1,  3, 1,  1,  "BOOL",   PASS)
handle_test("HandleTest5.Bool3[2]",        0,  1,  5, 1,  1,  "BOOL",   PASS)



