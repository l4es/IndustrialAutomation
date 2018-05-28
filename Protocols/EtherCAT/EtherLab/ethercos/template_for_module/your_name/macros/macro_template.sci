function [x,y,typ] = macro_template(job,arg1,arg2) //file name and function name must be identical!
  x=[];y=[];typ=[];
  select job
  case 'plot' then
    standard_draw(arg1)

  case 'getinputs' then
    [x,y,typ]=standard_inputs(arg1)

  case 'getoutputs' then
    [x,y,typ]=standard_outputs(arg1)

  case 'getorigin' then
    [x,y]=standard_origin(arg1)

  case 'set' then
    x=arg1
    model=arg1.model;graphics=arg1.graphics;
    exprs=graphics.exprs;
    while %t do
      [ok,var1,var2,var3,exprs]=..         //put in your variables!
      getvalue('Set block parameters',.. // title of parameter dialog
      ['text to var1:';
	'text to var2:';
	'text to var3: ';],..
      list('vec',-1,'vec',-1,'vec',-1),exprs)  //you can use vec or str, the second number is the dimension
      if ~ok then break,end
	evtinport=1 //number of events in ports
	//evtindim=1 //dimension of event ports
	//evtoutport=1 //number of events out ports
	//evtoutdim=1
	inport=1	// input ports
	indim=[2]'
	outport=2	//output ports
	outdim=[1 1]'
       if  exists('evtinport') then evtin=ones(evtinport,1), else evtin=[], end
	if  exists('evtindim') then evtin=evtindim.*evtin, end
       if  exists('evtoutport') then evtout=ones(evtoutport,1), else evtout=[], end
	if  exists('evtoutdim') then evtout=evtoutdim.* evtout, end
       if exists('inport') then in=ones(inport,1), else in=[], end
	if  exists('indim') then in=indim.*in, end
      if exists('outport') then out=ones(outport,1), else out=[], end
	if  exists('outdim') then out=outdim.*out, end

      [model,graphics,ok]=check_io(model,graphics,in,out,evtin,evtout)
      if ok then
        graphics.exprs=exprs;
        model.ipar=[var1;var2];  //transmit integer variables to the c block 
        model.rpar=[var3];       //transmit double variables to the c block 
        model.dstate=[];
	model.dep_ut=[depu==1, dept==1]
        x.graphics=graphics;x.model=model
        break
      end
    end

  case 'define' then
    model=scicos_model()
    model.sim=list('rt_flagtest',4) // name of the c-function
     
	var1=1	//initial value for var1
	var2=2
	var3=9.2

    evtinport=1 //number of events in ports
        //evtindim=1 //dimension of event ports
        //evtoutport=1 //number of events out ports
        //evtoutdim=1
        inport=1        // input ports
        indim=[2]'
        outport=2       //output ports
        outdim=[1 1]'
       if  exists('evtinport') then model.evtin=ones(evtinport,1), else model.evtin=[], end
        if  exists('evtindim') then model.evtin=evtindim.*model.evtin, end
       if  exists('evtoutport') then model.evtout=ones(evtoutport,1), else model.evtout=[], end
        if  exists('evtoutdim') then model.evtout=evtoutdim.* model.evtout, end
       if exists('inport') then model.in=ones(inport,1), else model.in=[], end
        if  exists('indim') then model.in=indim.*model.in, end
      if exists('outport') then model.out=ones(outport,1), else model.out=[], end
        if  exists('outdim') then model.out=outdim.*model.out, end


    model.rpar=[var3]
    model.ipar=[var1;var2];
    model.dstate=[];
    model.blocktype='d'
    model.dep_ut=[depu==1, dept==1]
    exprs=[sci2exp(var1),sci2exp(var2),sci2exp(var3)]
    gr_i=['xstringb(orig(1),orig(2),[''blockname''],sz(1),sz(2),''fill'');']
    x=standard_define([3 2],model,exprs,gr_i)
  end
endfunction
