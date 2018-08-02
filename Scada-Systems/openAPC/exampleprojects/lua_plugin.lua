digiIN0=false
digiIN1=false
numIN2=0
numIN3=0
charIN4=""
charIN5=""
stepCounter=0
binIN=nil

--[[ input callback function is called whenever one of the inputs changes ]]--
function luaio_recv_data_callback(input)
   if (input==2) then
      numIN2=luaio_get_value(2)
   elseif (input==3) then
      numIN3=luaio_get_value(3)
   elseif (input==4) then
      charIN4=luaio_get_value(4)
	  luaio_set_value(4,charIN4.." - "..charIN5)
   elseif (input==5) then
      charIN5=luaio_get_value(5)
	  luaio_set_value(5,charIN5.." - "..charIN4)
   elseif (input==6) then
      --[[ binary data given at IN6 are just tunneled to OUT 7 ]]--

	  --[[ binIN is a copy of the input data... ]]--
      binIN=luaio_get_value(6)
	  --[[ ...that is not released by setting it to an output... ]]--
	  luaio_set_value(7,binIN)
	  --[[ ...thus it has to be deallocated manually as soon as it is no longer needed ]]--
	  lua_release_binary(binIN)
   end

   --[[ calculate sum of numerical inputs and divide them by each other, set the results at the outputs ]]--
   if ((input==2) or (input==3)) then
	  luaio_set_value(2,numIN2+numIN3)
	  if (numIN3~=0) then
	     luaio_set_value(3,numIN2/numIN3)
	  end
   end
end


--[[ the main loop ]]--
while (true) do

   --[[ digital inputs are polled ]]--
   if (luaio_has_new_value(0)) then
      digiIN0=luaio_get_value(0)
   end
   if (luaio_has_new_value(1)) then
      digiIN1=luaio_get_value(1)
   end

   --[[ depending on their input state an other pattern is emitted at outputs ]]--
   if ((digiIN0==false) and (digiIN1==false)) then
      luaio_set_value(0,false)
      luaio_set_value(1,false)
   elseif ((digiIN0==true) and (digiIN1==false)) then
      if (stepCounter % 2==1) then
         luaio_set_value(0,false)
         luaio_set_value(1,false)
	  else
         luaio_set_value(0,true)
         luaio_set_value(1,true)
	  end
   elseif ((digiIN0==false) and (digiIN1==true)) then
      if (stepCounter % 2==1) then
         luaio_set_value(0,true)
         luaio_set_value(1,false)
	  else
         luaio_set_value(0,false)
         luaio_set_value(1,true)
	  end
   else
      if (stepCounter % 4==1) then
         luaio_set_value(0,true)
         luaio_set_value(1,false)
	  elseif (stepCounter % 4==2) then
         luaio_set_value(0,true)
         luaio_set_value(1,true)
	  elseif (stepCounter % 4==3) then
         luaio_set_value(0,false)
         luaio_set_value(1,true)
	  else
         luaio_set_value(0,false)
         luaio_set_value(1,false)
	  end
   end

   --[[ sleep some time to let the callback work ]]--
   oapc_thread_sleep(300)
   stepCounter=stepCounter+1
end
