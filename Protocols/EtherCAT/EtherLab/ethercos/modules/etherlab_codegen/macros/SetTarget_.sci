function  SetTarget_()
  Cmenu='Open/Set'
  xinfo('Click on a Superblock (without activation output)'+..
        ' to obtain a coded block ! ')

  k=[]
  while %t
    if %pt==[] then
      [btn,%pt,win,Cmenu]=cosclick()

      if Cmenu<>[] then
        [%win,Cmenu]=resume(win,Cmenu)
      end
    else
      win=%win
    end

    xc=%pt(1);yc=%pt(2);%pt=[]
    k=getobj(scs_m,[xc;yc])
    if k<>[] then break,end
  end

  if scs_m.objs(k).model.sim(1)=='super' then
    disablemenus()
    all_scs_m=scs_m;
    lab=scs_m.objs(k).model.rpar.props.void3;
    if lab==[] then
	lab = ['etherlab','ode4','10'];
    end

    ode_x=['ode1';'ode2';'ode4'];

    while %t
      [ok,target,odefun,stp]=getvalue(..
          'Please fill the following values',..
          ['Target: ';
	  'Ode function: ';
	  'Step between sampling: '],..
          list('str',1,'str',1,'str',1),lab);
      if ~ok then break,end

      //TARGETDIR=SCI+'/contrib/RTAI/RT_templates';
      TARGETDIR = getenv('ETLPATH')+'/modules/etherlab_codegen/RT_templates';
      if exists('TARGET_DIR') then
        [fd,ierr]=mopen(TARGET_DIR+'/'+target+'.gen','r');
        if ierr==0 then
	   TARGETDIR=TARGET_DIR;
           mclose(fd);
        end
      end

      [fd,ierr]=mopen(TARGETDIR+'/'+target+'.gen','r');
      if ierr==0 then
        mclose(fd);
      else
        ok=%f;x_message('Target not valid '+target+'.gen');
      end
      
      if grep(odefun,ode_x) == [] then
         x_message('Ode function not valid');
         ok = %f;
      end
        
      if ok then
         lab=[target,odefun,stp];
	 scs_m.objs(k).model.rpar.props.void3 = lab;
         break;
      end
    end

    edited=%t;
    enablemenus()
  else
    x_message('Block is not a superblock');
  end
endfunction

