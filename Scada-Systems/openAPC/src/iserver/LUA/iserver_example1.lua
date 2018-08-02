
function oapc_ispace_recv_callback(nodeName,cmd,ios,val0,val1,val2,val3,val4,val5,val6,val7)
   print("Received: "..nodeName)
   if (nodeName=="/GOButton/out") then
      go=val1
	  print(val1)
   elseif (nodeName=="/system/exit") then
      doExit=val0
   end
end


doExit=false
go=false
pos=0
delay1=15
delay2=70
if oapc_ispace_connect("",0) then
   while (doExit==false) do
      if (go) then
         pos=pos+1
         if (pos==1) then
            oapc_ispace_set_value("/Line_1/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_10/in/0",false)
         elseif (pos==2) then
            oapc_ispace_set_value("/Line_2/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_1/in/0",false)
         elseif (pos==3) then
            oapc_ispace_set_value("/Line_3/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_2/in/0",false)
         elseif (pos==4) then
            oapc_ispace_set_value("/Line_4/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_3/in/0",false)
         elseif (pos==5) then
            oapc_ispace_set_value("/Line_5/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_4/in/0",false)
	     elseif (pos==6) then
            oapc_ispace_set_value("/Line_6/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_5/in/0",false)
	     elseif (pos==7) then
            oapc_ispace_set_value("/Line_7/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_6/in/0",false)
	     elseif (pos==8) then
            oapc_ispace_set_value("/Line_8/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_7/in/0",false)
	     elseif (pos==9) then
            oapc_ispace_set_value("/Line_9/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_8/in/0",false)
	     elseif (pos==10) then
            oapc_ispace_set_value("/Line_10/in/0",true)
            oapc_util_thread_sleep(delay1)
            oapc_ispace_set_value("/Line_9/in/0",false)
	        pos=0
		 end
	  end
	  oapc_thread_sleep(delay2)
   end
   oapc_ispace_disconnect()
end
