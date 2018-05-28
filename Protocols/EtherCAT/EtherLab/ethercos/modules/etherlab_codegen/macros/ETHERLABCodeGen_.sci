//** INRIA / SCILAB / Roberto Bucher / Simone Mannori / Paolo Gai
//** 16 Jan 2008
//**
// Modified by Roberto Bucher roberto.bucher@supsi.ch from the original
// CodeGeneration_.sci
// Modified by Andreas Stewering-Bone ab@igh-essen.com from the RTAI-CVS-R1.22
// CodeGeneration_.sci
//
// Input editor function of Scicos code generator
//
//
//** 10 Set 2007 : cleaner startup code by Simone Mannori

function ETHERLABCodeGen_()

    //Set format for correct output
    // Added for EtherLAB Codegeneration
    format(20);


//** ------------- Preliminary I/O section ___________________________________________________________________________
    k = [] ; //** index of the CodeGen source superbloc candidate

    xc = %pt(1); //** last valid click position 
    yc = %pt(2); 
    
    %pt = []   ;
    Cmenu = [] ;

    k  = getobj(scs_m,[xc;yc]) ; //** look for a block 
    //** check if we have clicked near an object
    if k==[] then
      return
    //** check if we have clicked near a block
    elseif typeof(scs_m.objs(k))<>"Block" then
      return
    end

    //** If the clicked/selected block is really a superblock 
    //**         <k>
    if scs_m.objs(k).model.sim(1)=="super" then
      
        XX = scs_m.objs(k); //** isolate the super block to use 
        
//---------------------------------------------------->       THE REAL CODE GEN IS HERE --------------------------------
        //** the real code generator is here 
        [ok, XX, alreadyran, flgcdgen, szclkINTemp, freof] =  do_compile_superblock42(XX, scs_m, k, alreadyran);
        
        
        //**quick fix for sblock that contains scope
        gh_curwin = scf(curwin)
    
    else
      //** the clicked/selected block is NOT a superblock 
      message("Generation Code only work for a Super Block ! ")
    end

endfunction

//==========================================================================
//BlockProto : generate prototype
//            of a scicos block
//
//
//16/06/07 Author : A.Layec
//Copyright INRIA

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function [txt]=BlockProto(bk)

  nin=inpptr(bk+1)-inpptr(bk);  //* number of input ports */
  nout=outptr(bk+1)-outptr(bk); //* number of output ports */

  //**
  ftyp=funtyp(bk)
  if ftyp>2000 then ftyp=ftyp-2000,end
  if ftyp>1000 then ftyp=ftyp-1000,end

  //** check function type
  if ftyp < 0 then //** ifthenelse eselect blocks
      txt = [];
      return;
  else
    if (ftyp<>0 & ftyp<>1 & ftyp<>2 & ftyp<>3 & ftyp<>4) then
      disp("types other than 0,1,2,3 or 4 are not yet supported.")
      txt = [];
      return;
    end
  end

  //** add comment
  txt=[get_comment('proto_blk',list(funs(bk),funtyp(bk),bk));]

  select ftyp
    //** zero funtyp
    case 0 then

      //*********** prototype definition ***********//
      txtp=['(int *, int *, double *, double *, double *, int *, double *, \';
            ' int *, double *, int *, double *, int *,int *, int *, \';
            ' double *, int *, double *, int *);'];
      if (funtyp(bk)>2000 & funtyp(bk)<3000)
        blank = get_blank('void '+funs(bk)+'(');
        txtp(1) = 'void '+funs(bk)+txtp(1);
      elseif (funtyp(bk)<2000)
        txtp(1) = 'void C2F('+funs(bk)+')'+txtp(1);
        blank = get_blank('void C2F('+funs(bk)+')');
      end
      txtp(2:$) = blank + txtp(2:$);
      txt = [txt;txtp];
      //*******************************************//


    //**
    case 1 then

      //*********** prototype definition ***********//
      txtp=['(int *, int *, double *, double *, double *, int *, double *, \';
            ' int *, double *, int *, double *, int *,int *, int *';]
      if (funtyp(bk)>2000 & funtyp(bk)<3000)
        blank = get_blank('void '+funs(bk)+'(');
        txtp(1) = 'void '+funs(bk)+txtp(1);
      elseif (funtyp(bk)<2000)
        txtp(1) = 'void C2F('+funs(bk)+')'+txtp(1);
        blank = get_blank('void C2F('+funs(bk)+')');
      end
      if nin>=1 | nout>=1 then
        txtp($)=txtp($)+', \'
        txtp=[txtp;'']
        if nin>=1 then
          for k=1:nin
            txtp($)=txtp($)+' double *, int * ,'
          end
          txtp($)=part(txtp($),1:length(txtp($))-1); //remove last ,
        end
        if nout>=1 then
          if nin>=1 then
            txtp($)=txtp($)+', \'
            txtp=[txtp;'']
          end
          for k=1:nout
            txtp($)=txtp($)+' double *, int * ,'
          end
          txtp($)=part(txtp($),1:length(txtp($))-1); //remove last ,
        end
      end

      if ztyp(bk) then
        txtp($)=txtp($)+', \'
        txtp=[txtp;' double *,int *);'];
      else
        txtp($)=txtp($)+');';
      end

      txtp(2:$) = blank + txtp(2:$);
      txt = [txt;txtp];
      //*******************************************//

    //**
    case 2 then

      //*********** prototype definition ***********//

      txtp=['void '+funs(bk)+...
            '(int *, int *, double *, double *, double *, int *, double *, \';
            ' int *, double *, int *, double *, int *, int *, int *, \'
            ' double **, int *, int *, double **,int *, int *'];
      if ~ztyp(bk) then
        txtp($)=txtp($)+');';
      else
        txtp($)=txtp($)+', \';
        txtp=[txtp;
              ' double *,int *);']
      end
      blank = get_blank('void '+funs(bk));
      txtp(2:$) = blank + txtp(2:$);
      txt = [txt;txtp];
      //********************************************//

    //**
    case 4 then
      txt=[txt;
           'void '+funs(bk)+'(scicos_block *, int );'];

  end
endfunction

//==========================================================================
//CallBlock : generate C calling sequence
//            of a scicos block
//
//inputs : bk   : bloc index
//         pt   : evt activation number
//         flag : flag
//
//output : txt  :
//
//16/06/07 Authors : Alan Layec
//Copyright INRIA

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function txt=call_block42(bk,pt,flag)

  txt=[]
  //**
  if flag==2 & ((zptr(bk+1)-zptr(bk))+..
                (ozptr(bk+1)-ozptr(bk))+..
                (xptr(bk+1)-xptr(bk))==0 |..
                pt<=0) & ~(stalone & or(bk==actt(:,1))) then
    return // block without state or continuously activated
  end
  if flag==0 & ((xptr(bk+1)-xptr(bk))==0) then
    return // block without continuous state
  end
  if flag==9 & ((zcptr(bk+1)-zcptr(bk))==0) then
    return // block without continuous state
  end
  if flag==3 & ((clkptr(bk+1)-clkptr(bk))==0) then
    return
  end

  //** adjust pt
  if ~(flag==3 & ((zcptr(bk+1)-zcptr(bk))<>0)) then
    pt=abs(pt)
  end

  //** add comment
  txt=[get_comment('call_blk',list(funs(bk),funtyp(bk),bk));]

  //** set nevprt and flag for called block
  txt=[txt;
       'block_'+rdnom+'['+string(bk-1)+'].nevprt = '+string(pt)+';'
       'local_flag = '+string(flag)+';']

  //**see if its bidon, actuator or sensor
  if funs(bk)=='bidon' then
    txt=[];
    return
  elseif funs(bk)=='bidon2' then
    txt=[];
    return
  elseif or(bk==actt(:,1)) then
    ind=find(bk==actt(:,1))
    uk=actt(ind,2)
    nuk_1=actt(ind,3)
    nuk_2=actt(ind,4)
    uk_t=actt(ind,5)
    txt = [txt;
           'nport = '+string(ind)+';']
    txt = [txt;
           rdnom+'_actuator(&local_flag, &nport, &block_'+rdnom+'['+string(bk-1)+'].nevprt, \'
           get_blank(rdnom+'_actuator')+' &t, ('+mat2scs_c_ptr(outtb(uk))+' *)'+rdnom+'_block_outtbptr['+string(uk-1)+'], \'
           get_blank(rdnom+'_actuator')+' &nrd_'+string(nuk_1)+', &nrd_'+string(nuk_2)+', &nrd_'+string(uk_t)+',bbb);']
//    txt = [txt;
//           'if(local_flag < 0) return(5 - local_flag);']
    return
  elseif or(bk==capt(:,1)) then
    ind=find(bk==capt(:,1))
    yk=capt(ind,2);
    nyk_1=capt(ind,3);
    nyk_2=capt(ind,4);
    yk_t=capt(ind,5);
    txt = [txt;
           'nport = '+string(ind)+';']
    txt = [txt;
           rdnom+'_sensor(&local_flag, &nport, &block_'+rdnom+'['+string(i-1)+'].nevprt, \'
           get_blank(rdnom+'_sensor')+' &t, ('+mat2scs_c_ptr(outtb(yk))+' *)'+rdnom+'_block_outtbptr['+string(yk-1)+'], \'
           get_blank(rdnom+'_sensor')+' &nrd_'+string(nyk_1)+', &nrd_'+string(nyk_2)+', &nrd_'+string(yk_t)+',aaa);']
//    txt = [txt;
//           'if(local_flag < 0) return(5 - local_flag);']
    return
  end

  //**
  nx=xptr(bk+1)-xptr(bk);
  nz=zptr(bk+1)-zptr(bk);
  nrpar=rpptr(bk+1)-rpptr(bk);
  nipar=ipptr(bk+1)-ipptr(bk);
  nin=inpptr(bk+1)-inpptr(bk);  //* number of input ports */
  nout=outptr(bk+1)-outptr(bk); //* number of output ports */

  //**
  //l'adresse du pointeur de ipar
  if nipar<>0 then ipar=ipptr(bk), else ipar=1;end
  //l'adresse du pointeur de rpar
  if nrpar<>0 then rpar=rpptr(bk), else rpar=1; end
  //l'adresse du pointeur de z attention -1 pas sur
  if nz<>0 then z=zptr(bk)-1, else z=0;end
  //l'adresse du pointeur de x
  if nx<>0 then x=xptr(bk)-1, else x=0;end

  //**
  ftyp=funtyp(bk)
  if ftyp>2000 then ftyp=ftyp-2000,end
  if ftyp>1000 then ftyp=ftyp-1000,end

  //** check function type
  if ftyp < 0 then //** ifthenelse eselect blocks
      txt = [];
      return;
  else
    if (ftyp<>0 & ftyp<>1 & ftyp<>2 & ftyp<>3 & ftyp<>4) then
      disp("types other than 0,1,2,3 or 4 are not supported.")
      txt = [];
      return;
    end
  end

  select ftyp

    case 0 then
      //**** input/output addresses definition ****//
      if nin>1 then
        for k=1:nin
          uk=inplnk(inpptr(bk)-1+k);
          nuk=size(outtb(uk),'*');
          txt=[txt;
               'rdouttb['+string(k-1)+']=(double *)'+rdnom+'_block_outtbptr['+string(uk-1)+'];']
        end
        txt=[txt;
             'args[0]=&(rdouttb[0]);']
      elseif nin==0
        uk=0;
        nuk=0;
        txt=[txt;
             'args[0]=(double *)'+rdnom+'_block_outtbptr[0];']
      else
        uk=inplnk(inpptr(bk));
        nuk=size(outtb(uk),'*');
        txt=[txt;
             'args[0]=(double *)'+rdnom+'_block_outtbptr['+string(uk-1)+'];']
      end

      if nout>1 then
        for k=1:nout
          yk=outlnk(outptr(bk)-1+k);
          nyk=size(outtb(yk),'*');
          txt=[txt;
               'rdouttb['+string(k+nin-1)+']=(double *)'+rdnom+'_block_outtbptr['+string(yk-1)+'];'];
        end
        txt=[txt;
             'args[1]=&(rdouttb['+string(nin)+']);'];
      elseif nout==0
        yk=0;
        nyk=0;
        txt=[txt;
             'args[1]=(double *)'+rdnom+'_block_outtbptr[0];'];
      else
        yk=outlnk(outptr(bk));
        nyk=size(outtb(yk),'*'),;
        txt=[txt;
             'args[1]=(double *)'+rdnom+'_block_outtbptr['+string(yk-1)+'];'
	     'nrd_0 = '+string(nuk)+';'
	     'nrd_1 = '+string(nyk)+';'
	    ];
      end
      //*******************************************//

      //*********** call seq definition ***********//
      txtc=['(&local_flag,&block_'+rdnom+'['+string(bk-1)+'].nevprt,&t,block_'+rdnom+'['+string(bk-1)+'].xd, \';
            'block_'+rdnom+'['+string(bk-1)+'].x,&block_'+rdnom+'['+string(bk-1)+'].nx, \';
            'block_'+rdnom+'['+string(bk-1)+'].z,&block_'+rdnom+'['+string(bk-1)+'].nz,block_'+rdnom+'['+string(bk-1)+'].evout, \';
            '&block_'+rdnom+'['+string(bk-1)+'].nevout,block_'+rdnom+'['+string(bk-1)+'].rpar,&block_'+rdnom+'['+string(bk-1)+'].nrpar, \';
            'block_'+rdnom+'['+string(bk-1)+'].ipar,&block_'+rdnom+'['+string(bk-1)+'].nipar, \';
            '(double *)args[0],&nrd_'+string(nuk)+',(double *)args[1],&nrd_'+string(nyk)+');'];
      if (funtyp(bk)>2000 & funtyp(bk)<3000)
        blank = get_blank(funs(bk)+'( ');
        txtc(1) = funs(bk)+txtc(1);
      elseif (funtyp(bk)<2000)
        txtc(1) = 'C2F('+funs(bk)+')'+txtc(1);
        blank = get_blank('C2F('+funs(bk)+') ');
      end
      txtc(2:$) = blank + txtc(2:$);

      // Check for Blacklist GUI-Blocks
      // Etherlab Codegenerator
      // Andreas Stewering-Bone
      if check_fun_blacklist(funs(bk)) < 0 then //Skip Block
        txt = [txt;
	       '// Non-RT Block Function, Do not call it!';
	       '// Etherlab Codegenerator';
	       '/*';
	       txtc;
	       '*/'];
      else	
        txt = [txt;
	       txtc];
      end

      //*******************************************//


    //**
    case 1 then
      //*********** call seq definition ***********//
      txtc=['(&local_flag,&block_'+rdnom+'['+string(bk-1)+'].nevprt,&t,block_'+rdnom+'['+string(bk-1)+'].xd, \';
            'block_'+rdnom+'['+string(bk-1)+'].x,&block_'+rdnom+'['+string(bk-1)+'].nx, \';
            'block_'+rdnom+'['+string(bk-1)+'].z,&block_'+rdnom+'['+string(bk-1)+'].nz,block_'+rdnom+'['+string(bk-1)+'].evout, \';
            '&block_'+rdnom+'['+string(bk-1)+'].nevout,block_'+rdnom+'['+string(bk-1)+'].rpar,&block_'+rdnom+'['+string(bk-1)+'].nrpar, \';
            'block_'+rdnom+'['+string(bk-1)+'].ipar,&block_'+rdnom+'['+string(bk-1)+'].nipar'];
      if (funtyp(bk)>2000 & funtyp(bk)<3000)
        blank = get_blank(funs(bk)+'( ');
        txtc(1) = funs(bk)+txtc(1);
      elseif (funtyp(bk)<2000)
        txtc(1) = 'C2F('+funs(bk)+')'+txtc(1);
        blank = get_blank('C2F('+funs(bk)+') ');
      end
      if nin>=1 | nout>=1 then
        txtc($)=txtc($)+', \'
        txtc=[txtc;'']
        if nin>=1 then
          for k=1:nin
            uk=inplnk(inpptr(bk)-1+k);
            nuk=size(outtb(uk),'*');
            txtc($)=txtc($)+'(double *)'+rdnom+'_block_outtbptr['+string(uk-1)+'],&nrd_'+string(nuk)+',';
          end
          txtc($)=part(txtc($),1:length(txtc($))-1); //remove last ,
        end
        if nout>=1 then
          if nin>=1 then
            txtc($)=txtc($)+', \'
            txtc=[txtc;'']
          end
          for k=1:nout
            yk=outlnk(outptr(bk)-1+k);
            nyk=size(outtb(yk),'*');
            txtc($)=txtc($)+'(double *)'+rdnom+'_block_outtbptr['+string(yk-1)+'],&nrd_'+string(nyk)+',';
          end
          txtc($)=part(txtc($),1:length(txtc($))-1); //remove last ,
        end
      end

      if ztyp(bk) then
        txtc($)=txtc($)+', \'
        txtc=[txtc;
              'block_'+rdnom+'['+string(bk-1)+'].g,&block_'+rdnom+'['+string(bk-1)+'].ng);']
      else
        txtc($)=txtc($)+');';
      end

      txtc(2:$) = blank + txtc(2:$);

      if check_fun_blacklist(funs(bk)) < 0 then //Skip Block
        txt = [txt;
	       '// Non-RT Block Function, Do not call it!';
	       '// Etherlab Codegenerator';
	       '/*';
	       txtc;
	       '*/'];
      else	
        txt = [txt;
	       txtc];
      end

      //*******************************************//

    //**
    case 2 then

      //*********** call seq definition ***********//
      txtc=[funs(bk)+'(&local_flag,&block_'+rdnom+'['+string(bk-1)+'].nevprt,&t,block_'+rdnom+'['+string(bk-1)+'].xd, \';
            'block_'+rdnom+'['+string(bk-1)+'].x,&block_'+rdnom+'['+string(bk-1)+'].nx, \';
            'block_'+rdnom+'['+string(bk-1)+'].z,&block_'+rdnom+'['+string(bk-1)+'].nz,block_'+rdnom+'['+string(bk-1)+'].evout, \';
            '&block_'+rdnom+'['+string(bk-1)+'].nevout,block_'+rdnom+'['+string(bk-1)+'].rpar,&block_'+rdnom+'['+string(bk-1)+'].nrpar, \';
            'block_'+rdnom+'['+string(bk-1)+'].ipar,&block_'+rdnom+'['+string(bk-1)+'].nipar, \';
            '(double **)block_'+rdnom+'['+string(bk-1)+'].inptr,block_'+rdnom+'['+string(bk-1)+'].insz,&block_'+rdnom+'['+string(bk-1)+'].nin, \';
            '(double **)block_'+rdnom+'['+string(bk-1)+'].outptr,block_'+rdnom+'['+string(bk-1)+'].outsz, &block_'+rdnom+'['+string(bk-1)+'].nout'];
      if ~ztyp(bk) then
        txtc($)=txtc($)+');';
      else
        txtc($)=txtc($)+', \';
        txtc=[txtc;
              'block_'+rdnom+'['+string(bk-1)+'].g,&block_'+rdnom+'['+string(bk-1)+'].ng);']
      end
      blank = get_blank(funs(bk)+'( ');
      txtc(2:$) = blank + txtc(2:$);

      if check_fun_blacklist(funs(bk)) < 0 then //Skip Block
        txt = [txt;
	       '// Non-RT Block Function, Do not call it!';
	       '// Etherlab Codegenerator';
	       '/*';
	       txtc;
	       '*/'];
      else	
        txt = [txt;
	       txtc];
      end


      //*******************************************//

    //**
    case 4 then
      if check_fun_blacklist(funs(bk)) < 0 then //Skip Block
        txt = [txt;
	       '// Non-RT Block Function, Do not call it!';
	       '// Etherlab Codegenerator';
	       '/*';
	       funs(bk)+'(&block_'+rdnom+'['+string(bk-1)+'],local_flag);'
	       '*/'];
      else	
        txt = [txt;
	       funs(bk)+'(&block_'+rdnom+'['+string(bk-1)+'],local_flag);'];
      end

  end

//  txt =[txt;'if(local_flag < 0) return(5 - local_flag);']

endfunction

//==========================================================================
//utilitary fonction used to format long C instruction
//t : a string containing a C instruction
//l : max line length allowed

// Copyright INRIA
//Author : Rachid Djenidi
function t1=cformatline(t ,l)

  sep=[',','+']
  l1=l-2
  t1=[]
  kw=strindex(t,' ')
  nw=0
  if kw<>[] then
    if kw(1)==1 then // there is leading blanks
      k1=find(kw(2:$)-kw(1:$-1)<>1)
      if k1==[] then // there is a single blank
	nw=1
      else
	nw=kw(k1(1))
      end
    end
  end
  t=part(t,nw+1:length(t));
  bl=part(' ',ones(1,nw))
  l1=l-nw;first=%t
  while %t 
    if length(t)<=l then t1=[t1;bl+t],return,end
    k=strindex(t,sep);
    if k==[] then t1=[t1;bl+t],return,end
    k($+1)=length(t)+1 // positions of the commas
    i=find(k(1:$-1)<=l&k(2:$)>l) //nearest left comma (reltively to l)
    if i==[] then i=1,end
    t1=[t1;bl+part(t,1:k(i))]
    t=part(t,k(i)+1:length(t))
    if first then l1=l1-2;bl=bl+'  ';first=%f;end
  end
endfunction

//==========================================================================
//used in do_compile_superblock
function vec=codebinaire(v,szclkIN)

  vec=zeros(1,szclkIN)
  for i=1:szclkIN
    w=v/2;
    vec(i)=v-2*int(w);
    v=int(w);
  end
endfunction

//==========================================================================

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function ok = compile_standalone()
//compile rt standalone executable for standalone
// 22.01.2004
//Author : Roberto Bucher (roberto.bucher@die.supsi.ch)


  xinfo('Compiling standalone');
  wd = pwd();
  chdir(rpat);

  if getenv('WIN32','NO')=='OK' then
     unix_w('nmake -f Makefile.mak');
  else
     unix_w('make')
  end
  chdir(wd);
  ok = %t;
endfunction

//==========================================================================
// Transforms a given Scicos discrete and continuous SuperBlock into a C defined Block
// Copyright INRIA
//

// Original file from Project Metalau - INRIA
// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function  [ok,XX,alreadyran,flgcdgen,szclkINTemp,freof] = do_compile_superblock42(XX,all_scs_m,numk,alreadyran)

  scs_m = XX.model.rpar ; //** isolate the super block scs_m data structure 
  par = scs_m.props;
  
  //** SAFETY : terminate current simulation 
  if alreadyran then
    //** terminate current simulation
    do_terminate() ; 
    alreadyran  = %f ;
  end

  hname = scs_m.props.title(1);  //** super block name

  //***********************************************************
  //Check blocks properties and adapt them if necessary
  //***********************************************************


  //**  These blocks are not ALLOWED for Emb code generation 
  vorbidden_items=["CLKOUT_f","activation (events) output ports";
                   "IN_f","input ports";
                   "OUT_f","output ports";
                   "CLKOUTV_f","activation outputs";
                   "CLOCK_c","clocks";
                   "CLOCK_f","clocks";
                   "SampleCLK","clocks";
                   "RFILE_f","Read block";
                   "READC_f","Read_block";
                   "WFILE_f","Write block";
                   "WRITEC_f","Write block"]

  clkIN = [];
  
  //** scan 
  for i=1:size(scs_m.objs)

    //** BLOCKS 
    if typeof(scs_m.objs(i))=="Block" then
      ind=find(vorbidden_items==scs_m.objs(i).gui);
      if(ind~=[]) then
        ok = %f ;
        %cpr = list();
        message(vorbidden_items(ind(1),2)+" not allowed in Superblock");
        return; // EXIT point
      
      elseif scs_m.objs(i).gui=="CLKINV_f" then //** input clock from external diagram 
        //** replace event input ports by  fictious block
        scs_m.objs(i).gui="EVTGEN_f";
        scs_m.objs(i).model.sim(1)="bidon"
        if clkIN==[] then
          clkIN = 1;
        else
          ok = %f;
          %cpr = list();
          message("Only one activation block allowed!");
          return; // EXIT point
        end
      end
    end
  end

  
  szclkIN = size(clkIN,2);
  

  flgcdgen = szclkIN; //** pass the clock 
  
  //** 
  //** BEWARE : the Scicos compiler is called here ! 
  //**
  [bllst,connectmat,clkconnect,cor,corinv,ok,scs_m,flgcdgen,freof] = c_pass1(scs_m,flgcdgen);

  if ~ok then
    message("Sorry: problem in the pre-compilation step.")
    return ; //** EXIT point 
  end

  szclkINTemp = szclkIN;
  szclkIN = flgcdgen;


  for i=1:size(bllst)
    if (bllst(i).sim(1)=="bidon") then //** clock input 
      howclk = i;
    end
  end

  
  //** BEWARE : update to new graphics instructions ! 
  %windo = xget('window') ; 
  
  cpr = c_pass2(bllst,connectmat,clkconnect,cor,corinv)

  if cpr==list() then
      ok = %f
      return ; //** exit point 
  end


  //** Remove work vector handling, replaced by blacklist handling
  //** EtherLAB Codegeneration, Andreas Stewering-Bone

  //** Alan's patch 5/07/07: try to solve
  //   which blocks use work
  //** 
  funs_save = cpr.sim.funs           ;
  funtyp_save = cpr.sim.funtyp       ;
  //** with_work = zeros(cpr.sim.nblk,1)  ;


  //** ------ Windows cleaning for internal Scopes -------------------
  //**
  //**
  //** retrieve all open Scilab windows with winsid()
  //**

  //** BeforeCG_WinList = winsid(); 

  //** ierr=execstr('[state,t]=scicosim(cpr.state,0,0,cpr.sim,'+..
  //**              '''start'',scs_m.props.tol)','errcatch')
  //** if ierr==0 then
  //**   for i=1:cpr.sim.nblk
  //**      if state.iz(i)<>0 then
  //**         with_work(i)=%t
  //**      end
  //**   end
  //** Added Simulation run for Ethercat Initialisation, if driver loaded!
  //** Etherlab codegeneration
  //** not very nice
  //**   ierr=execstr('[state,t]=scicosim(state,0,0,cpr.sim,'+..
  //**                '''run'',scs_m.props.tol)','errcatch')
  //**   ierr=execstr('[state,t]=scicosim(state,0,0,cpr.sim,'+..
  //**                '''finish'',scs_m.props.tol)','errcatch')
  //** end

  //**
  //** retrieve all open Scilab windows with winsid
  //** and close the additional windows open since first
  //** 
 
  //** This code does not cover 100% of the possible situations because the user can 
  //** destroy one or more Scicos wins manually during this intermediate phase
  //** This code is 100% functional if the the user does not close manually any win.
  //** TO BE updated in Scilab 5.0

  //** AfterCG_WinList = winsid();
  
  //** AfterCG_size = size(AfterCG_WinList); //** matrix
  //** AfterCG_size = AfterCG_size(2) ; //** vector lenght 

  //** BeforeCG_size = size(BeforeCG_WinList); //** matrix
  //** BeforeCG_size = BeforeCG_size(2) ; //** vector lenght

  //** if (or(AfterCG_WinList<>BeforeCG_WinList)) & (AfterCG_size>BeforeCG_size) then
     //** means that a scope or other scicos object has created some
     //** output window

  //**    DiffCG_Winlist = AfterCG_WinList<>BeforeCG_WinList ; //** T/F mismatch 
  //**    DiffCG_Index = find(DiffCG_Winlist); //** recover the mismatched indexes only 

  //**    for win_idx = DiffCG_Index
  //**        delete( scf( AfterCG_WinList(win_idx) ) ) ; //** clear the spurious windows   
  //**    end 
     
  //** end  
  //**------------- end of windows cleaning for internal scopes -------------------------------

  cpr.sim.funs=funs_save;
  cpr.sim.funtyp=funtyp_save;

  //** BEWARE: replace this OLD graphics instruction !
  xset('window',%windo) ; 

  ///////////////////
  //** %cpr pointers 
  x = cpr.state.x;
  z = cpr.state.z;
  outtb = cpr.state.outtb;

  // RN
  zcptr = cpr.sim.zcptr;
  ozptr = cpr.sim.ozptr;
  rpptr = cpr.sim.rpptr;
  ipptr = cpr.sim.ipptr;
  opptr = cpr.sim.opptr;
  funs = cpr.sim.funs;
  xptr = cpr.sim.xptr;
  zptr = cpr.sim.zptr;
  inpptr = cpr.sim.inpptr;
  inplnk = cpr.sim.inplnk;
  outptr = cpr.sim.outptr;
  outlnk = cpr.sim.outlnk;
  
  // @l@n lnkptr = cpr.sim.lnkptr; ???
  
  ordclk = cpr.sim.ordclk;
  funtyp = cpr.sim.funtyp;
  cord   = cpr.sim.cord;
  ncord  = size(cord,1);
  nblk   = cpr.sim.nb;
  ztyp   = cpr.sim.ztyp;
  clkptr = cpr.sim.clkptr
  
  // taille totale de z : nztotal
  nztotal = size(z,1);

  //*******************************
  //Checking if superblock is valid
  //*******************************
  msg = []

  for i=1:length(funs)-1

    if funtyp(i)==3 then
      msg = [msg;"Type 3 block''s not allowed"] ;
   
// Bubu 18.03.2008
// comment out for Etherlab Codegeneration
//    elseif funtyp(i)==0 & funs(i)~="bidon"  then
//      msg = [msg;"Type 0 block''s"+" ''"+funs(i)+"''"+" not allowed"] ;
//end
   
    elseif (clkptr(i+1)-clkptr(i))<>0 &funtyp(i)>-1 &funs(i)~="bidon" then

      //Alan // msg=[msg;'Regular block generating activation not allowed yet']

    end

    if msg<>[] then 
         message(msg) ;
         ok = %f ;
         return ; //** 
    end

  end


  //** Find the clock connected to the SuperBlock and retreive
  //** the sampling time
  
  if XX.graphics.pein==[] | XX.graphics.pein(1)==0 then
    sTsamp="0.001"; //** default value is ONE millisecond 
  else
    o_ev = XX.graphics.pein(1);
    o_ev=all_scs_m.objs(o_ev).from(1);

    while (all_scs_m.objs(o_ev).gui~='CLOCK_c' & ...
           all_scs_m.objs(o_ev).gui~='CLOCK_f' & ...
           all_scs_m.objs(o_ev).gui~='SampleCLK')

               o_ev = all_scs_m.objs(o_ev).graphics.pein(1);
               o_ev = all_scs_m.objs(o_ev).from(1);

    end

    if all_scs_m.objs(o_ev).gui=='SampleCLK' then
      sTsamp=all_scs_m.objs(o_ev).model.rpar(1);
      sTsamp=sci2exp(sTsamp);
      Tsamp_delay=all_scs_m.objs(o_ev).model.rpar(2);
      Tsamp_delay=sci2exp(Tsamp_delay);
    else
      sTsamp=all_scs_m.objs(o_ev).model.rpar.objs(2).graphics.exprs(1);
      sTsamp=sci2exp(eval(sTsamp));
      Tsamp_delay=all_scs_m.objs(o_ev).model.rpar.objs(2).graphics.exprs(2);
      Tsamp_delay=sci2exp(eval(Tsamp_delay));
    end

  end

  //***********************************
  // Get the name of the file
  //***********************************
  foo = 3; //** probably this variable is never used ? 
  okk = %f; 
  rdnom='foo'; 
  rpat = getcwd(); 
  archname=''; 
  Tsamp = sci2exp(eval(sTsamp));
  
  template = ''; //** default values for this version 
  
  if XX.model.rpar.props.void3 == [] then
	target = 'etherlab'; //** default compilation chain 
	odefun = 'ode4';  //** default solver 
	odestep = '10';   //** default continous step size 
  else
	target  = XX.model.rpar.props.void3(1); //** user defined parameters 
	odefun  = XX.model.rpar.props.void3(2);
	odestep = XX.model.rpar.props.void3(3);
  end

  libs='';

  //** dialog box default variables 
  label1=[hname;getcwd()+'/'+hname+"_scig";target;template];
  label2=[hname;getcwd()+'/'+hname+"_scig";target;template;odefun;odestep];
  
  ode_x=['ode1';'ode2';'ode4']; //** available continous solver 
  
  //** Open a dialog box 
  while %t do
    ok = %t ;
    if x==[] then
      //** Pure discrete system NO CONTINOUS blocks 
     
      [okk, rdnom, rpat,target,template,label1] = getvalue(..
	            'Embedded Code Generation',..
		        ['New block''s name :';
		         'Created files Path:';
			     'Toolchain: ';
			     'Target Board: '],..
		         list('str',1,'str',1,'str',1,'str',1),label1);
    else
      //** continous blocks are presents
      [okk,rdnom,rpat,target,template,odefun,odestep,label2] = getvalue(..
	            "Embedded Code Generation",..
		        ["New block''s name: "  ;
		         "Created files Path: " ;
			     "Toolchain: "          ;
			     "Target Board: "       ;
		         "ODE solver type: "       ;
		         "ODE solver steps betw. samples: "],..
		         list('str',1,'str',1,'str',1,'str',1,'str',1,'str',1),label2);
    end
  
    if okk==%f then
      ok = %f
      return ; //** EXIT point 
    end
    rpat = stripblanks(rpat);


    //** I put a warning here in order to inform the user
    //** that the name of the superblock will change
    //** because the space char in name isn't allowed.
    if grep(rdnom," ")<>[] then
      message(['Superblock name cannot contains space characters.';
               'space chars will be automatically substituted by ""_"" '])
    end
    rdnom = strsubst(rdnom,' ','_');

    //** Put a warning here in order to inform the user
    //** that the name of the superblock will change
    //** because the "-" char could generate GCC problems
    //** (the C functions contains the name of the superblock).
    if grep(rdnom,"-")<>[] then
      message(['For full C compiler compatibility ';
               'Superblock name cannot contains ""-"" characters';
               '""-"" chars will be automatically substituted by ""_"" '])
    end

    rdnom = strsubst(rdnom,'-','_'); 

    dirinfo = fileinfo(rpat)
    
    if dirinfo==[] then
      [pathrp, fnamerp, extensionrp] = fileparts(rpat); 
      ok = mkdir(pathrp, fnamerp+extensionrp) ; 
      if ~ok then 
        message("Directory '+rpat+' cannot be created");
      end
    elseif filetype(dirinfo(2))<>'Directory' then
      ok = %f;
      message(rpat+" is not a directory");
    end

    if stripblanks(rdnom)==emptystr() then 
      ok = %f;
      message("Sorry: C file name not defined");
    end


    //** This comments will be moved in the documentation 

    //** /contrib/RT_templates/pippo.gen

    //** 1: pippo.mak 
    //** 2: pippo.cmd

    //** pippo.mak : scheletro del Makefile 
    //**             - GNU/Linux : Makefile template
    //**             - Windows/Erika : conf.oil
    //**                               erika.cmd
 
    //** pippo.cmd : sequenza di comandi Scilab 


    //**TARGETDIR = SCI+"/contrib/RTAI/RT_templates";
    TARGETDIR = getenv('ETLPATH')+'/modules/etherlab_codegen/RT_templates';	

    [fd,ierr] = mopen(TARGETDIR+'/'+target+'.gen','r');

    if ierr==0 then
      mclose(fd);
    else
      ok = %f;
      message("Target not valid " + target + ".gen");
    end
    
    if ok then
      target_t = mgetl(TARGETDIR+'/'+target+'.gen');
      makfil = target_t(1);
      cmdfil = target_t(2);

      [fd,ierr]=mopen(TARGETDIR+'/'+makfil,'r');
      if ierr==0 then
        mclose(fd);
      else
        ok = %f ;
        message("Makefile not valid " + makfil);
      end
    end

    if x ~= [] then
      if grep(odefun,ode_x) == [] then
         message("Ode function not valid");
         ok = %f;
      end
    end

    if ok then break,end
  end

  //////////////////////////////////////////////////
  maxnrpar=max(rpptr(2:$)-rpptr(1:$-1))
  maxnipar=max(ipptr(2:$)-ipptr(1:$-1))
  maxnx=max(xptr(2:$)-xptr(1:$-1))
  maxnz=max(zptr(2:$)-zptr(1:$-1))
  maxnin=max(inpptr(2:$)-inpptr(1:$-1))
  maxnout=max(outptr(2:$)-outptr(1:$-1))
  maxdim=[];
  for i=1:lstsize(cpr.state.outtb)
    maxdim=max(size(cpr.state.outtb(i)))
  end
  maxtotal=max([maxnrpar;maxnipar;maxnx;maxnz;maxnin;maxnout;maxdim]);

//------------------ The real code generation is here ------------------------------------

  //************************************************************************
  //generate the call to the blocks and blocs simulation function prototypes
  //************************************************************************
  wfunclist = list();
  nbcap = 0;
  nbact = 0;
  capt  = [];
  actt  = [];
  Protostalone = [];
  Protos       = [];
  dfuns        = [] ;
  


  //** scan the data structure and call the generating functions 
  //** Substitute previous code!!!!
  
  for i=1:length(funs)
    ki= find(funs(i) == dfuns) ; //** 
    dfuns = [dfuns; funs(i)] ; 
      
    if ki==[] then
      Protostalone=[Protostalone;'';BlockProto(i)];
    end
  end


  //***********************************
  // Scilab and C files generation
  //***********************************

  cmdseq = mgetl(TARGETDIR+'/' + cmdfil);
  n_cmd = size(cmdseq,1);

  for i=1:n_cmd
  
    if (cmdseq(i)~="") then
         disp("Executing " + """" +cmdseq(i)+ """" + '...'); 
    end;
    
    execstr(cmdseq(i));
  
  end

  disp("----> Target generation terminated!");

endfunction

//==========================================================================
function t=filetype(m)
  m=int32(m)
  filetypes=['Directory','Character device','Block device',...
             'Regular file','FIFO','Symbolic link','Socket']
  bits=[16384,8192,24576,32768,4096,40960,49152]
  m=int32(m)&int32(61440)
  t=filetypes(find(m==int32(bits)))
endfunction

//==========================================================================
//Generates Code for dynamically linked Fortran and C Blocks

// Original file from Project Metalau - INRIA
// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function [CCode,FCode]=gen_blocks()

  CCode=[]
  FCode=[]

  kdyn=find(funtyp>1000) //dynamically linked blocs
                         //100X : Fortran blocks
                         //200X : C blocks

  if (size(kdyn,'*') >1)
    kfuns=[]; 
    //get the block data structure in the initial scs_m structure
    if size(corinv(kdyn(1)),'*')==1 then
      O=scs_m.objs(corinv(kdyn(1)));
    else
      path=list('objs');
      for l=corinv(kdyn(1))(1:$-1)
        path($+1)=l;
        path($+1)='model';
        path($+1)='rpar';
        path($+1)='objs';
      end
      path($+1)=corinv(kdyn(1))($);
      O=scs_m(path);
    end
    if funtyp(kdyn(1))>2000 then
      //C block
      CCode=[CCode;O.graphics.exprs(2)]
    else
      FCode=[FCode;O.graphics.exprs(2)]
    end
    kfuns=funs(kdyn(1));
    for i=2:size(kdyn,'*')
      //get the block data structure in the initial scs_m structure
      if size(corinv(kdyn(i)),'*')==1 then
        O=scs_m.objs(corinv(kdyn(i)));
      else
        path=list('objs');
         for l=corinv(kdyn(i))(1:$-1)
           path($+1)=l;
           path($+1)='model';
           path($+1)='rpar';
           path($+1)='objs';
        end
        path($+1)=corinv(kdyn(i))($);
        O=scs_m(path);
      end
      if (find(kfuns==funs(kdyn(i))) == [])
        kfuns=[kfuns;funs(kdyn(i))];
        if funtyp(kdyn(i))>2000  then
          //C block
          CCode=[CCode;O.graphics.exprs(2)]
        else
          FCode=[FCode;O.graphics.exprs(2)]
        end
      end
    end
  elseif (size(kdyn,'*')==1)
    //get the block data structure in the initial scs_m structure
    if size(corinv(kdyn),'*')==1 then
      O=scs_m.objs(corinv(kdyn));
    else
      path=list('objs');
      for l=corinv(kdyn)(1:$-1)
        path($+1)=l;
        path($+1)='model';
        path($+1)='rpar';
        path($+1)='objs';
      end
      path($+1)=corinv(kdyn)($);
      O=scs_m(path);
    end
    if funtyp(kdyn)>2000 then
      //C block
      CCode=[CCode;O.graphics.exprs(2)]
    else
      FCode=[FCode;O.graphics.exprs(2)]
    end
  end
  if CCode==[]
    CCode=['void no_ccode()'
           '{'
           '  return;'
           '}']
  end
endfunction

//==========================================================================
//get_blank : return blanks with a length
//            of the given input string
//
//input : str : a string
//
//output : txt : blanks
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [txt] = get_blank(str)
 txt='';
 for i=1:length(str)
     txt=txt+' ';
 end
endfunction

//==========================================================================
// get_comment : return a C comment
//               for generated code
//
//input : typ : a string
//        param : a list
//
//output : a C comment
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [txt]=get_comment(typ,param)
  txt = [];
  select typ
    //** main flag
    case 'flag' then
        select param(1)
          case 0 then
             txt = '/* Continuous state computation */'
          case 1 then
             txt = '/* Output computation */'
          case 2 then
             txt = '/* Discrete state computation */'
          case 3 then
             txt = '/* Output Event computation */'
          case 4 then
             txt = '/* Initialization */'
          case 5 then
             txt = '/* Ending */'
          case 9 then
             txt = '/* Update zero crossing surfaces */'
        end
    //** blocks activated on event number
    case 'ev' then
       txt = '/* Blocks activated on the event number '+string(param(1))+' */'

    //** blk calling sequence
    case 'call_blk' then
        txt = ['/* Call of '''+param(1) + ...
               ''' (type '+string(param(2))+' - blk nb '+...
                    string(param(3))];
        if ztyp(param(3)) then
          txt=txt+' - with zcross) */';
        else
          txt=txt+') */';
        end
    //** proto calling sequence
    case 'proto_blk' then
        txt = ['/* prototype of '''+param(1) + ...
               ''' (type '+string(param(2))];
        if ztyp(param(3)) then
          txt=txt+' - with zcross) */';
        else
          txt=txt+') */';
        end
    //** ifthenelse calling sequence
    case 'ifthenelse_blk' then
        txt = ['/* Call of ''if-then-else'' blk (blk nb '+...
                    string(param(1))+') */']
    //** eventselect calling sequence
    case 'evtselect_blk' then
        txt = ['/* Call of ''event-select'' blk (blk nb '+...
                    string(param(1))+') */']
    //** set block structure
    case 'set_blk' then
        txt = ['/* set blk struc. of '''+param(1) + ...
               ''' (type '+string(param(2))+' - blk nb '+...
                    string(param(3))+') */'];
    //** Update xd vector ptr
    case 'update_xd' then
        txt = ['/* Update xd vector ptr */'];
    //** Update g vector ptr
    case 'update_g' then
        txt = ['/* Update g vector ptr */'];
    else
      break;
  end
endfunction

//==========================================================================
//generates code of the standalone simulation procedure
//
//Copyright INRIA
//
// rmq : La fonction zdoit n'est pas utilisï¿½e pour le moment

// Original file from Project Metalau - INRIA
// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function [Code,Code_common]=make_standalone42()

  x=cpr.state.x;
  modptr=cpr.sim.modptr;
  rpptr=cpr.sim.rpptr;
  ipptr=cpr.sim.ipptr;
  opptr=cpr.sim.opptr;
  rpar=cpr.sim.rpar;
  ipar=cpr.sim.ipar;
  opar=cpr.sim.opar;
  oz=cpr.state.oz;
  ordptr=cpr.sim.ordptr;
  oord=cpr.sim.oord;
  zord=cpr.sim.zord;
  iord=cpr.sim.iord;
  tevts=cpr.state.tevts;
  evtspt=cpr.state.evtspt;
  zptr=cpr.sim.zptr;
  clkptr=cpr.sim.clkptr;
  ordptr=cpr.sim.ordptr;
  pointi=cpr.state.pointi;
  funs=cpr.sim.funs;
  noord=size(cpr.sim.oord,1);
  nzord=size(cpr.sim.zord,1);
  niord=size(cpr.sim.iord,1);

  Indent='  ';
  Indent2=Indent+Indent;
  BigIndent='          ';

  work=zeros(nblk,1)
  Z=[z;zeros(lstsize(outtb),1);work]';
  nX=size(x,'*');
  nztotal=size(z,1);

  stalone = %t;

  Code=['/* Code prototype for standalone use  */'
        '/*     Generated by Code_Generation toolbox of Scicos with '+ ..
        getversion()+' */'
        '/*     date : '+date()+' */'
        ''
        '/* ---- Headers ---- */'
        '#include <stdlib.h>'
        '#include <math.h>'
        '#include <string.h>'
        '#include <memory.h>'
        '#include <scicos_block4.h>'
        '#include <machine.h>'
        ''
	'/*MSR-Registration Header*/'
	'#include '"msr_reg_scicos.h'"'
	''
	//'#ifdef linux'
	'#define __CONST__'
	//'#else'
	//'#define __CONST__ static const'
	//'#endif'
	''
	'double '+rdnom+'_get_tsamp()'
	'{'
	'  return(' + string(Tsamp) + ');'
	'}'
	''
	'double '+rdnom+'_get_tsamp_delay()'
	''
        '{'
	'  return(' + string(Tsamp_delay) + ');'
	'}'
	''
        '/* ---- Internals functions declaration ---- */'
        'int '+rdnom+'_init(void);'
        'int '+rdnom+'_isr(double);'
        'int '+rdnom+'_end(void);'
        Protostalone
        '']

  if x<>[] then
    Code=[Code
          '/* Code prototype for standalone use  */'
          'int C2F('+rdnom+'simblk)(double , double *, double *);'
          'extern  int C2F(dset)();'
          'static int ode1();'
          'static int ode2();'
          'static int ode4();'
          '']
  end

  Code=[Code;
        ''
        '/* Some general static variables */'
        'static double zero=0;'
        'static double w[1];'
        'static int nport = 0;'
	''
        'void **'+rdnom+'_block_outtbptr;'] //** !!

  Code=[Code;
        make_static_standalone42()]

  Code=[Code
        '  /* Initial values */'
        ''
        '  /* Note that z[]=[z_initial_condition;outtbptr;work]'
        cformatline('     z_initial_condition={'+...
          strcat(string(z),",")+'};',70)
        cformatline('     outtbptr={'+...
          strcat(string(zeros(lstsize(outtb),1)),"," )+'};',70)
        cformatline('     work= {'+...
          strcat(string(work),"," )+'};',70)
        '  */'
        ''
        cformatline('  static double z[]={'+strcat(string(Z),',')+'};',70)
       '']

  //** declaration of outtb
  Code_outtb = [];
  for i=1:lstsize(outtb)
    if mat2scs_c_nb(outtb(i)) <> 11 then
      Code_outtb=[Code_outtb;
                  cformatline('  static '+mat2c_typ(outtb(i))+...
                              ' outtb_'+string(i)+'[]={'+...
                              strcat(string(outtb(i)(:)),',')+'};',70)]
    else //** cmplx test
      Code_outtb=[Code_outtb;
                  cformatline('  static '+mat2c_typ(outtb(i))+...
                              ' outtb_'+string(i)+'[]={'+...
                              strcat(string([real(outtb(i)(:));
                                             imag(outtb(i)(:))]),',')+'};',70)]
    end
  end
  Code=[Code;
        Code_outtb;
        '']

// Bubu modified

  Code1 = []

  Code2=[''
        '/*'+part('-',ones(1,40))+'  Initialisation function */'
        'int '+rdnom+'_init()'
        '{'
	'  double t;'
        '  int local_flag;'
	//'#ifdef linux'
	'  int nrd_0,nrd_1;'
        '  double *args[2];'
	//'#endif'
        '']


  if size(z,1) <> 0 then
    for i=1:(length(zptr)-1)
      if zptr(i+1)-zptr(i)>0 then
        if size(corinv(i),'*')==1 then
          OO=scs_m.objs(corinv(i))
        else
          path=list('objs')
          for l=cpr.corinv(i)(1:$-1)
            path($+1)=l;path($+1)='model'
            path($+1)='rpar'
            path($+1)='objs'
          end
          path($+1)=cpr.corinv(i)($)
          OO=scs_m(path)
        end
        aaa=OO.gui
        bbb=emptystr(3,1);
        if and(aaa+bbb~=['INPUTPORTEVTS';'OUTPUTPORTEVTS';'EVTGEN_f']) then
          Code2($+1)='';
          Code2($+1)=' /* Routine name of block: '+strcat(string(cpr.sim.funs(i)));
          Code2($+1)='    Gui name of block: '+strcat(string(OO.gui));
          //Code2($+1)='/* Name block: '+strcat(string(cpr.sim.funs(i)));
          //Code2($+1)='Object number in diagram: '+strcat(string(cpr.corinv(i)));
          Code2($+1)='   Compiled structure index: '+strcat(string(i));
          if stripblanks(OO.model.label)~=emptystr() then
            Code2=[Code2;
                   cformatline('   Label: '+strcat(string(OO.model.label)),70)]
          end
          if stripblanks(OO.graphics.exprs(1))~=emptystr() then
            Code2=[Code2;
                   cformatline('   Exprs: '+strcat(OO.graphics.exprs(1),","),70)]
          end
          if stripblanks(OO.graphics.id)~=emptystr() then
            Code2=[Code2;
                   cformatline('   Identification: '+..
                     strcat(string(OO.graphics.id)),70)]
          end
          Code2=[Code2;
                 cformatline('   z={'+...
                 strcat(string(z(zptr(i):zptr(i+1)-1)),",")+'};',70)]
          Code2($+1)=' */';
        end
      end
    end
  end

  //** declaration of oz
  Code_oz = [];
  for i=1:lstsize(oz)
    if mat2scs_c_nb(oz(i)) <> 11 then
      Code_oz=[Code_oz;
               cformatline('  '+mat2c_typ(oz(i))+...
                           ' oz_'+string(i)+'[]={'+...
                           strcat(string(oz(i)(:)),',')+'};',70)]
    else //** cmplx test
      Code_oz=[Code_oz;
               cformatline('  '+mat2c_typ(oz(i))+...
                           ' oz_'+string(i)+'[]={'+...
                           strcat(string([real(oz(i)(:));
                                          imag(oz(i)(:))]),',')+'};',70)]
    end
  end

  if Code_oz <> [] then
    Code2=[Code2;
           '  /* oz declaration */'
           Code_oz]
  end

  Code2=[Code2
         '  /* Get work ptr of blocks */'
         '  void **work;'
         '  work = (void **)(z+'+string(size(z,'*')+lstsize(outtb))+');'
         '']


  if Code_outtb<>[] then
    Code2=[Code2
           '  /* outtbptr declaration */'
           '  '+rdnom+'_block_outtbptr = (void **)(z+'+string(nztotal)+');'
           '']
  end

  Code_outtbptr=[];
  for i=1:lstsize(outtb)
    Code_outtbptr=[Code_outtbptr;
                   '  '+rdnom+'_block_outtbptr['+...
                    string(i-1)+'] = (void *) outtb_'+string(i)+';'];
  end

  if Code_outtbptr<>[] then
    Code2=[Code2;
           Code_outtbptr
           '']
  end

  for kf=1:nblk
    nx=xptr(kf+1)-xptr(kf);       //** number of continuous state
    nin=inpptr(kf+1)-inpptr(kf);  //** number of input ports
    nout=outptr(kf+1)-outptr(kf); //** number of output ports

    //** add comment
    txt=[get_comment('set_blk',list(funs(kf),funtyp(kf),kf));]

    Code2=[Code2;
           '  '+txt];

    flex_ng     = zcptr(kf+1)-zcptr(kf);
    flex_nz     = zptr(kf+1)-zptr(kf);
    flex_noz    = ozptr(kf+1)-ozptr(kf);
    flex_nin    = inpptr(kf+1)-inpptr(kf);
    flex_nout   = outptr(kf+1)-outptr(kf);
    flex_nevout = clkptr(kf+1)-clkptr(kf);
    flex_nopar  = opptr(kf+1)-opptr(kf);

    Code2=[Code2;
           '  block_'+rdnom+'['+string(kf-1)+'].type   = '+string(funtyp(kf))+';';
           '  block_'+rdnom+'['+string(kf-1)+'].ztyp   = '+string(ztyp(kf))+';';
           '  block_'+rdnom+'['+string(kf-1)+'].ng     = '+string(flex_ng)+';';
           '  block_'+rdnom+'['+string(kf-1)+'].nz     = '+string(flex_nz)+';';
           '  block_'+rdnom+'['+string(kf-1)+'].noz    = '+string(flex_noz)+';';
           '  block_'+rdnom+'['+string(kf-1)+'].nrpar  = '+string(rpptr(kf+1)-rpptr(kf))+';';
           '  block_'+rdnom+'['+string(kf-1)+'].nopar  = '+string(flex_nopar)+';';
           '  block_'+rdnom+'['+string(kf-1)+'].nipar  = '+string(ipptr(kf+1)-ipptr(kf))+';'
           '  block_'+rdnom+'['+string(kf-1)+'].nin    = '+string(flex_nin)+';';
           '  block_'+rdnom+'['+string(kf-1)+'].nout   = '+string(flex_nout)+';';
           '  block_'+rdnom+'['+string(kf-1)+'].nevout = '+string(flex_nevout)+';';
           '  block_'+rdnom+'['+string(kf-1)+'].nmode  = '+string(modptr(kf+1)-modptr(kf))+';';]

    if nx <> 0 then
      Code2=[Code2;
             '  block_'+rdnom+'['+string(kf-1)+'].nx = '+string(nx)+';';
             '  block_'+rdnom+'['+string(kf-1)+'].x  = &(x['+string(xptr(kf)-1)+']);'
             '  block_'+rdnom+'['+string(kf-1)+'].xd = &(xd['+string(xptr(kf)-1)+']);']
    end

    if flex_nevout <> 0 then
      flex_str = rdnom +'_'+string(kf-1)+'_evout'
      Code1=[Code1;
             'double '+flex_str+'['+string(flex_nevout)+'];'
            ]
      Code2=[Code2;
             '  block_'+rdnom+'['+string(kf-1)+'].evout = '+flex_str+';';
            ];
    end

    //***************************** input port *****************************//
    if flex_nin <> 0 then
      flex_str = rdnom +'_'+string(kf-1)+'_inptr'
      Code1=[Code1;
             'double * '+flex_str+'['+string(flex_nin)+'];'
            ]
      Code2=[Code2;
             '  block_'+rdnom+'['+string(kf-1)+'].inptr = '+flex_str+';';
            ];
      flex_str = rdnom +'_'+string(kf-1)+'_insz'

      Code2=[Code2;
             '  block_'+rdnom+'['+string(kf-1)+'].insz = '+flex_str+';';
            ];

      //** inptr **//
      for k=1:nin
         lprt=inplnk(inpptr(kf)-1+k);
         Code2=[Code2
                '  block_'+rdnom+'['+string(kf-1)+'].inptr['+string(k-1)+...
                ']  = '+rdnom+'_block_outtbptr['+string(lprt-1)+'];']
      end

      //** 1st dim **//
      szCode='const int '+flex_str+'['+string(3*flex_nin)+']={'
      for k=1:nin
         lprt=inplnk(inpptr(kf)-1+k);
         szCode=szCode+string(size(outtb(lprt),1))+','
      end

      //** 2dn dim **//
      for k=1:nin
         lprt=inplnk(inpptr(kf)-1+k);
         szCode=szCode+string(size(outtb(lprt),2))+','
      end

      //** typ **//
      for k=1:nin
         lprt=inplnk(inpptr(kf)-1+k);
         szCode=szCode+mat2scs_c_typ(outtb(lprt))+','
      end
      szCode=part(szCode,1:length(szCode)-1)+'};'
      Code1=[Code1;
	     szCode;
	    ]
    end
    
    //***************************** output port *****************************//
    if flex_nout <> 0 then
      flex_str = rdnom +'_'+string(kf-1)+'_outptr'
      Code1=[Code1;
             'double * '+flex_str+'['+string(flex_nout)+'];'
            ]
      Code2=[Code2;
             '  block_'+rdnom+'['+string(kf-1)+'].outptr = '+flex_str+';';
            ];
      flex_str = rdnom +'_'+string(kf-1)+'_outsz'
      Code2=[Code2;
             '  block_'+rdnom+'['+string(kf-1)+'].outsz = '+flex_str+';';
            ];

      //** outptr **//
      for k=1:nout
         lprt=outlnk(outptr(kf)-1+k);
         Code2=[Code2
                '  block_'+rdnom+'['+string(kf-1)+'].outptr['+string(k-1)+...
                '] = '+rdnom+'_block_outtbptr['+string(lprt-1)+'];']
      end

      //** 1st dim **//
      szCode='const int '+flex_str+'['+string(3*flex_nout)+']={'
      for k=1:nout
         lprt=outlnk(outptr(kf)-1+k);
         szCode=szCode+string(size(outtb(lprt),1))+','
      end

      //** 2dn dim **//
      for k=1:nout
         lprt=outlnk(outptr(kf)-1+k);
         szCode=szCode+string(size(outtb(lprt),2))+','
      end

      //** typ **//
      for k=1:nout
         lprt=outlnk(outptr(kf)-1+k);
         szCode=szCode+mat2scs_c_typ(outtb(lprt))+','
      end
      szCode=part(szCode,1:length(szCode)-1)+'};'
      Code1=[Code1;
	     szCode;
	    ]    
    end

    //**********************************************************************//
    Code2=[Code2
           '  block_'+rdnom+'['+string(kf-1)+...
           '].z = &(z['+string(zptr(kf)-1)+']);']

    if (part(funs(kf),1:7) ~= 'capteur' &...
        part(funs(kf),1:10) ~= 'actionneur' &...
        funs(kf) ~= 'bidon') then
      //** rpar **//
      if (rpptr(kf+1)-rpptr(kf)>0) then
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+...
               '].rpar=&(RPAR['+string(rpptr(kf)-1)+']);']
      end
      //** ipar **//
      if (ipptr(kf+1)-ipptr(kf)>0) then
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+...
               '].ipar=&(IPAR['+string(ipptr(kf)-1)+']);']
      end

      //**********************************************************************//
      //** opar **//

      if flex_nopar<> 0 then
        flex_str = rdnom +'_'+string(kf-1)+'_oparptr'
        Code1=[Code1;
               'void * '+flex_str+'['+string(flex_nopar)+'];'
            ]
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+'].oparptr = '+flex_str+';';
              ];

        flex_str = rdnom +'_'+string(kf-1)+'_oparsz'
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+'].oparsz = '+flex_str+';';
              ];

        flex_str = rdnom +'_'+string(kf-1)+'_opartyp'
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+'].opartyp = '+flex_str+';';
              ];

        nopar = flex_nopar;
        //** oparptr **//
        for k=1:nopar
          Code2=[Code2;
                 '  block_'+rdnom+'['+string(kf-1)+'].oparptr['+string(k-1)+...
                 ']   = (void *) OPAR_'+string(opptr(kf)-1+k)+';'];
        end
        //** 1st dim **//
	szCode='const int '+rdnom+'_'+string(kf-1)+'_oparsz['+string(2*flex_nopar)+']={';
        for k=1:nopar
           szCode=szCode+string(size(opar(opptr(kf)-1+k),1))+',';
        end
        //** 2dn dim **//
        for k=1:nopar
           szCode=szCode+string(size(opar(opptr(kf)-1+k),2))+',';
        end
        szCode=part(szCode,1:length(szCode)-1)+'};'
        Code1=[Code1;
	       szCode;
	      ]    

        //** typ **//
	szCode='const int '+rdnom+'_'+string(kf-1)+'_opartyp['+string(flex_nopar)+']={';
        for k=1:nopar
           szCode=szCode+mat2scs_c_typ(opar(opptr(kf)-1+k))+',';
        end
        szCode=part(szCode,1:length(szCode)-1)+'};'
        Code1=[Code1;
	       szCode;
	      ]    
      end

      //**********************************************************************//
      //** oz **//
      if flex_noz>0 then
        noz = flex_noz;
        flex_str = rdnom +'_'+string(kf-1)+'_ozptr'
        Code1=[Code1;
               'void * '+flex_str+'['+string(flex_noz)+'];'
            ]
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+'].ozptr = '+flex_str+';';
              ];

        flex_str = rdnom +'_'+string(kf-1)+'_ozsz'
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+'].ozsz = '+flex_str+';';
              ];

        flex_str = rdnom +'_'+string(kf-1)+'_oztyp'
        Code2=[Code2;
               '  block_'+rdnom+'['+string(kf-1)+'].oztyp = '+flex_str+';';
              ];

        //** ozptr **//
        for k=1:noz
          Code2=[Code2;
                 '  block_'+rdnom+'['+string(kf-1)+'].ozptr['+string(k-1)+...
                 ']   = (void *) oz_'+string(ozptr(kf)-1+k)+';'];
        end
        //** 1st dim **//
	szCode='const int '+rdnom+'_'+string(kf-1)+'_ozsz['+string(2*flex_noz)+']={';
        for k=1:noz
           szCode=szCode+string(size(oz(ozptr(kf)-1+k),1))+',';
        end
        //** 2dn dim **//
        for k=1:noz
           szCode=szCode+string(size(oz(ozptr(kf)-1+k),2))+',';
        end
        szCode=part(szCode,1:length(szCode)-1)+'};'
        Code1=[Code1;
	       szCode;
	      ]    

        //** typ **//
	szCode='int '+rdnom+'_'+string(kf-1)+'_oztyp['+string(flex_noz)+']={';
        for k=1:noz
           szCode=szCode+mat2scs_c_typ(oz(ozptr(kf)-1+k))+',';
        end
        szCode=part(szCode,1:length(szCode)-1)+'};'
        Code1=[Code1;
	       szCode;
	      ]    
      end
    end
    Code2=[Code2;
           '  block_'+rdnom+'['+string(kf-1)+'].work = '+...
           '(void **)(((double *)work)+'+string(kf-1)+');']
  end

  //** init
  Code=[Code;
        Code1;
        Code2;
        '   '+get_comment('flag',list(4))]

  for kf=1:nblk
//    if or(kf==act) | or(kf==cap) then
//        txt = call_block42(kf,0,4);
//        if txt <> [] then
//          Code=[Code;
//                '';
//                '  '+txt];
//        end
//    else
      txt = call_block42(kf,0,4);
      if txt <> [] then
        Code=[Code;
              '';
              '  '+txt];
      end
//    end
  end

  //** cst blocks and it's dep
  txt=write_code_idoit()

  if txt<>[] then
    Code=[Code;
          ''
          '    /* Initial blocks must be called with flag 1 */'
          txt]
  end
  Code=[Code;
	''
	'  /*-----------------------------*/'
	'  /* Etherlab Codegeneration     */'
	'  /* Call MSR Block Registration */'
	'  model_register_blocks();'
	'  /*-----------------------------*/'
	''
	'  return(local_flag);'
	'}'];

  Code=[Code;
        ''
        '/*'+part('-',ones(1,40))+'  ISR function */'
        'int '+rdnom+'_isr(double t)'
        '{'
//        '  int nevprt=1;'
        '  int local_flag;'
	'  int i;'
	//'#ifdef linux'
	'  int nrd_0,nrd_1;'
        '  double *args[2];'
	//'#endif'
       ]

  if (x <> []) then
    Code=[Code
          '  double tout, dt, he, h;'
          '']
  end

  //** find source activation number
  blks=find(funtyp>-1);
  evs=[];

  for blk=blks
    for ev=clkptr(blk):clkptr(blk+1)-1
      if funs(blk)=='bidon' then
        if ev > clkptr(howclk) -1
         evs=[evs,ev];
        end
      end
    end
  end

  //** flag 1,2,3
  for flag=[1,2,3]

    txt3=[]

    //** continuous time blocks must be activated
    //** for flag 1
    if flag==1 then
      txt = write_code_cdoit(flag);

      if txt <> [] then
        txt3=[''
              '  '+get_comment('ev',list(0))
              txt;
             ];
      end
    end

    //** blocks with input discrete event must be activated
    //** for flag 1, 2 and 3
    if size(evs,2)>=1 then
      txt4=[]
      //**
      for ev=evs
        txt2=write_code_doit(ev,flag);
        if txt2<>[] then
          //** adjust event number because of bidon block
          new_ev=ev-(clkptr(howclk)-1)
          //**
          txt4=[txt4;
//                Indent+['case '+string(new_ev)+' : '+...
//                get_comment('ev',list(new_ev))
                Indent+[get_comment('ev',list(new_ev))
                txt2];
//                '    break;';
		'']
        end
      end

      //**
      if txt4 <> [] then
        txt3=[txt3;
              Indent+'/* Discrete activations */'
//              Indent+'switch (nevprt) {'
              txt4
//              '  }'
             ];
      end
    end

    //**
    if txt3<>[] then
      Code=[Code;
            '  '+get_comment('flag',list(flag))
            txt3];
    end
  end

  if x<>[] then
    Code=[Code
          ''
          '  tout=t;'
	  '  dt='+rdnom+'_get_tsamp();'
          '  h=dt/'+odestep+';' 
          '  while (tout+h<t+dt){'
          '    '+odefun+'(C2F('+rdnom+'simblk),tout,h);'
          '     tout=tout+h;'
          '  }'
          ''
          '  he=t+dt-tout;'
          '  '+odefun+'(C2F('+rdnom+'simblk),tout,he);'
          '']
  end

  //** fix bug provided by Roberto Bucher
  //** Alan, 13/10/07
  if nX <> 0 then
    Code=[Code;
          ''
          '    /* update ptrs of continuous array */']
    for kf=1:nblk
      nx=xptr(kf+1)-xptr(kf);  //** number of continuous state
      if nx <> 0 then
        Code=[Code;
              '  block_'+rdnom+'['+string(kf-1)+'].nx = '+...
               string(nx)+';';
              '  block_'+rdnom+'['+string(kf-1)+'].x  = '+...
               '&(x['+string(xptr(kf)-1)+']);'
              '  block_'+rdnom+'['+string(kf-1)+'].xd = '+...
               '&(xd['+string(xptr(kf)-1)+']);']
      end
    end
  end

  Code=[Code
	''
	'  return 0;'
        '}']

  //** flag 5

  Code=[Code
        '/*'+part('-',ones(1,40))+'  Termination function */'
        'int '+rdnom+'_end()'
        '{'
	'  double t;'
        '  int local_flag;'
	//'#ifdef linux'
	'  int nrd_0,nrd_1;'
        '  double *args[2];'
	//'#endif'
        '']

  Code=[Code;
        '  '+get_comment('flag',list(5))]

  for kf=1:nblk
//    if or(kf==act) | or(kf==cap) then
//        txt = call_block42(kf,0,5);
//        if txt <> [] then
//          Code=[Code;
//                '';
//                '  '+txt];
//        end
//    else
      txt = call_block42(kf,0,5);
      if txt <> [] then
        Code=[Code;
              '';
              '  '+txt];
      end
//    end
  end

  Code=[Code
        '  return 0;'
        '}'
	'']

  Code_common=['/* Code prototype for common use  */'
               '/*     Generated by Code_Generation toolbox of Scicos with '+ ..
                getversion()+' */'
               '/*     date : '+date()+' */'
               ''
               '/* ---- Headers ---- */'
               '#include <memory.h>'
               '#include <stdlib.h>'
               '#include <scicos_block4.h>';
               '#include '"machine.h'"'
               '']

	       if(isempty(grep(SCI,'5.1.1'))) then
	       Code_common=[Code_common
	       '/*'+part('-',ones(1,40))+'  Lapack messag function */';
               'void C2F(xerbla)(SRNAME,INFO,L)'
               '     char *SRNAME;'
               '     int *INFO;'
               '     long int L;'
               '{}'
	       '']
	       end

               Code_common=[Code_common
	       'void set_block_error(int err)'
               '{'
               '  return;'
               '}'
               ''
               'int get_phase_simulation()'
               '{'
               '  return 1;'
               '}'
               ''
               'void * scicos_malloc(size_t size)'
               '{'
               '  return malloc(size);'
               '}'
               ''
               'void scicos_free(void *p)'
               '{'
               '  free(p);'
               '}'
               ''
               'void do_cold_restart()'
               '{'
               '  return;'
               '}'
               ''
               'void sciprint (char *fmt)'
               '{'
               '  return;'
               '}'
               '']

  if (x <> []) then
    Code=[Code;
          'int C2F('+rdnom+'simblk)(t, xc, xdc)'
          ''
          '   double t, *xc, *xdc;'
          ''
          '     /*'
          '      *  !purpose'
          '      *  compute state derivative of the continuous part'
          '      *  !calling sequence'
          '      *  neq   : integer the size of the  continuous state'
          '      *  t     : current time'
          '      *  xc    : double precision vector whose contains the continuous state'
          '      *  xdc   : double precision vector, contain the computed derivative'
          '      *  of the state'
          '      */'
          '{'
          '  int phase=2;'
          '  int local_flag;'
          '  int nport;'
//          '  int nevprt=1;'
	  //'#ifdef linux'	
          '  double *args[2];'
	  '  int nrd_0, nrd_1;'
	  //'#endif'
          '  C2F(dset)(&neq, &c_b14,xd , &c__1);'
          '']

    Code=[Code;
          '    '+get_comment('update_xd',list())]

    for kf=1:nblk
      if (xptr(kf+1)-xptr(kf)) > 0 then
        Code=[Code;
              '    block_'+rdnom+'['+string(kf-1)+'].x='+...
                '&(xc['+string(xptr(kf)-1)+']);'
              '    block_'+rdnom+'['+string(kf-1)+'].xd='+...
                '&(xdc['+string(xptr(kf)-1)+']);']
      end
    end

    Code=[Code;
          ''
          write_code_odoit(1)
          write_code_odoit(0)
         ]

    for kf=1:nblk
      if (xptr(kf+1)-xptr(kf)) > 0 then
        Code=[Code;
              '    block_'+rdnom+'['+string(kf-1)+'].x='+...
                '&(x['+string(xptr(kf)-1)+']);'
              '    block_'+rdnom+'['+string(kf-1)+'].xd='+...
                '&(xd['+string(xptr(kf)-1)+']);']
      end
    end

    Code=[Code
          ''
          '  return 0;'
          '}'
          ''
          '/* Euler''s Method */'
          'static int ode1(f,t,h)'
          '  int (*f) ();'
          '  double t, h;'
          '{'
          '  int i;'
          ''
          '  /**/'
          '  (*f)(t,x, xd);'
          ''
          '  for (i=0;i<neq;i++) {'
          '   x[i]=x[i]+h*xd[i];'
          '  }'
          ''
          '  return 0;'
          '}'
          ''
          '/* Heun''s Method */'
          'static int ode2(f,t,h)'
          '  int (*f) ();'
          '  double t, h;'
          '{'
          '  int i;'
          '  double y['+string(nX)+'],yh['+string(nX)+'],temp,f0['+string(nX)+'],th;'
          ''
          '  /**/'
          '  memcpy(y,x,neq*sizeof(double));'
          '  memcpy(f0,xd,neq*sizeof(double));'
          ''
          '  /**/'
          '  (*f)(t,y, f0);'
          ''
          '  /**/'
          '  for (i=0;i<neq;i++) {'
          '    x[i]=y[i]+h*f0[i];'
          '  }'
          '  th=t+h;'
          '  for (i=0;i<neq;i++) {'
          '    yh[i]=y[i]+h*f0[i];'
          '  }'
          '  (*f)(th,yh, xd);'
          ''
          '  /**/'
          '  temp=0.5*h;'
          '  for (i=0;i<neq;i++) {'
          '    x[i]=y[i]+temp*(f0[i]+xd[i]);'
          '  }'
          ''
          '  return 0;'
          '}'
          ''
          '/* Fourth-Order Runge-Kutta (RK4) Formula */'
          'static int ode4(f,t,h)'
          '  int (*f) ();'
          '  double t, h;'
          '{'
          '  int i;'
          '  double y['+string(nX)+'],yh['+string(nX)+'],'+...
            'temp,f0['+string(nX)+'],th,th2,'+...
            'f1['+string(nX)+'],f2['+string(nX)+'];'
          ''
          '  /**/'
          '  memcpy(y,x,neq*sizeof(double));'
          '  memcpy(f0,xd,neq*sizeof(double));'
          ''
          '  /**/'
          '  (*f)(t,y, f0);'
          ''
          '  /**/'
          '  for (i=0;i<neq;i++) {'
          '    x[i]=y[i]+h*f0[i];'
          '  }'
          '  th2=t+h/2;'
          '  for (i=0;i<neq;i++) {'
          '    yh[i]=y[i]+(h/2)*f0[i];'
          '  }'
          '  (*f)(th2,yh, f1);'
          ''
          '  /**/'
          '  temp=0.5*h;'
          '  for (i=0;i<neq;i++) {'
          '    x[i]=y[i]+temp*f1[i];'
          '  }'
          '  for (i=0;i<neq;i++) {'
          '    yh[i]=y[i]+(h/2)*f1[i];'
          '  }'
          '  (*f)(th2,yh, f2);'
          ''
          '  /**/'
          '  for (i=0;i<neq;i++) {'
          '    x[i]=y[i]+h*f2[i];'
          '  }'
          '  th=t+h;'
          '  for (i=0;i<neq;i++) {'
          '    yh[i]=y[i]+h*f2[i];'
          '  }'
          '  (*f)(th2,yh, xd);'
          ''
          '  /**/'
          '  temp=h/6;'
          '  for (i=0;i<neq;i++) {'
          '    x[i]=y[i]+temp*(f0[i]+2.0*f1[i]+2.0*f2[i]+xd[i]);'
          '  }'
          ''
          'return 0;'
          '}']
  end
endfunction

//==========================================================================
//generates  static table definitions
//
//Author : Rachid Djenidi, Alan Layec
//Copyright INRIA

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function txt=make_static_standalone42()

  txt=[''];

  //*** Continuous state ***//
  if x <> [] then
   txt=[txt;
        '/* def continuous state */'
        cformatline('static double x[]={'+strcat(string(x),',')+'};',70)
        cformatline('static double xd[]={'+strcat(string(x),',')+'};',70)
        'static int c__1 = 1;'
        'static double c_b14 = 0.;'
        'static int neq='+string(nX)+';'
        '']
  end
  //************************//

  txt=[txt;
       'scicos_block block_'+rdnom+'['+string(nblk)+'];'
       ''];

  //*** Real parameters ***//
  nbrpa=0;strRCode='';lenRCode=[];ntot_r=0;
  if size(rpar,1) <> 0 then
    txt=[txt;
	 '/* def real parameters */'
         '__CONST__ double RPAR[ ] = {'];

    for i=1:(length(rpptr)-1)
      if rpptr(i+1)-rpptr(i)>0  then

        if size(corinv(i),'*')==1 then
          OO=scs_m.objs(corinv(i));
        else
          path=list('objs');
          for l=cpr.corinv(i)(1:$-1)
            path($+1)=l;
            path($+1)='model';
            path($+1)='rpar';
            path($+1)='objs';
          end
          path($+1)=cpr.corinv(i)($);
          OO=scs_m(path);
        end

        //** Add comments **//
	nbrpa=nbrpa+1;
	ntot_r = ntot_r + (rpptr(i+1)-rpptr(i));
        txt($+1)='/* Routine name of block: '+strcat(string(cpr.sim.funs(i)));
        txt($+1)=' * Gui name of block: '+strcat(string(OO.gui));
        txt($+1)=' * Compiled structure index: '+strcat(string(i));

        if stripblanks(OO.model.label)~=emptystr() then
          txt=[txt;cformatline(' * Label: '+strcat(string(OO.model.label)),70)];
        end
        if stripblanks(OO.graphics.exprs(1))~=emptystr() then
          txt=[txt;cformatline(' * Exprs: '+strcat(OO.graphics.exprs(1),","),70)];
        end
	if stripblanks(OO.graphics.id)~=emptystr() then
	  str_id = string(OO.graphics.id);
        else
	  str_id = 'RPARAM[' + string(nbrpa) +']';
	end
        txt=[txt;
             cformatline(' * Identification: '+strcat(string(OO.graphics.id)),70)];
	txt=[txt;cformatline('rpar= {'+strcat(string(rpar(rpptr(i):rpptr(i+1)-1)),",")+'};',70)];
	txt($+1)='*/';
                //******************//

        txt=[txt;
             cformatline(strcat(msprintf('%.16g,\n',rpar(rpptr(i):rpptr(i+1)-1))),70);
             '']
	strRCode = strRCode + '""' + str_id + '"",';
	lenRCode = lenRCode + string(rpptr(i+1)-rpptr(i)) + ',';

      end
    end
    txt=[txt;
           '};']
  else
    txt($+1)='double RPAR[1];';
  end

  //txt = [txt;
  //       '';
  //       '#ifdef linux';
  //      ]
  //txt($+1) = 'int NRPAR = '+string(nbrpa)+';';
  //txt($+1) = 'int NTOTRPAR = '+string(ntot_r)+';';
  //  
  //strRCode = 'char * strRPAR[' + string(nbrpa) + '] = {' + ..
  //           part(strRCode,[1:length(strRCode)-1]) + '};';
  //
  //if nbrpa <> 0 then
  //  txt($+1) = strRCode;
  //  lenRCode = 'int lenRPAR[' + string(nbrpa) + '] = {' + ..
  //             part(lenRCode,[1:length(lenRCode)-1]) + '};';
  //else
  //   txt($+1) = 'char * strRPAR;'
  //   lenRCode = 'int lenRPAR[1] = {0};'
  //end
  //txt($+1) = lenRCode;
  //txt = [txt;
  //       '#endif';
  //       '';
  //      ]

  //***********************//

  //*** Integer parameters ***//
  nbipa=0;strICode='';lenICode=[];ntot_i=0;
  if size(ipar,1) <> 0 then
    txt=[txt;
           '/* def integer parameters */'
           '__CONST__ int IPAR[ ] = {'];

    for i=1:(length(ipptr)-1)
      if ipptr(i+1)-ipptr(i)>0  then
        if size(corinv(i),'*')==1 then
          OO=scs_m.objs(corinv(i));
        else
          path=list('objs');
          for l=cpr.corinv(i)(1:$-1)
            path($+1)=l
            path($+1)='model'
            path($+1)='rpar'
            path($+1)='objs'
          end
          path($+1)=cpr.corinv(i)($);
          OO=scs_m(path);
        end

        //** Add comments **//
        nbipa=nbipa+1;
	ntot_i = ntot_i + (ipptr(i+1)-ipptr(i));
        txt($+1)='/* Routine name of block: '+strcat(string(cpr.sim.funs(i)));
        txt($+1)=' * Gui name of block: '+strcat(string(OO.gui));
        txt($+1)=' * Compiled structure index: '+strcat(string(i));
        if stripblanks(OO.model.label)~=emptystr() then
          txt=[txt;cformatline(' * Label: '+strcat(string(OO.model.label)),70)];
        end

        if stripblanks(OO.graphics.exprs(1))~=emptystr() then
          txt=[txt;
               cformatline(' * Exprs: '+strcat(OO.graphics.exprs(1),","),70)];
        end

	if stripblanks(OO.graphics.id)~=emptystr() then
	  str_id = string(OO.graphics.id);
        else
	  str_id = 'IPARAM[' + string(nbipa) +']';
	end

        txt=[txt;
               cformatline(' * Identification: '+strcat(string(OO.graphics.id)),70)];
	txt=[txt;cformatline('ipar= {'+strcat(string(ipar(ipptr(i):ipptr(i+1)-1)),",")+'};',70)];
	txt($+1)='*/';

        //******************//

        txt=[txt;cformatline(strcat(string(ipar(ipptr(i):ipptr(i+1)-1))+','),70)];
	strICode = strICode + '""' + str_id + '"",';
	lenICode = lenICode + string(ipptr(i+1)-ipptr(i)) + ',';
      end
    end
    txt=[txt;
         '};']
  else
    txt($+1)='int IPAR[1];';
  end

//  txt = [txt;
//         '';
//         '#ifdef linux';
//        ]
//  txt($+1) = 'int NIPAR = '+string(nbipa)+';';
//  txt($+1) = 'int NTOTIPAR = '+string(ntot_i)+';';
//
//  strICode = 'char * strIPAR[' + string(nbipa) + '] = {' + ..
//             part(strICode,[1:length(strICode)-1]) + '};';
//
//  if nbipa <> 0 then
//     txt($+1) = strICode;
//     lenICode = 'int lenIPAR[' + string(nbipa) + '] = {' + ..
//                part(lenICode,[1:length(lenICode)-1]) + '};';
//  else
//     txt($+1) = 'char * strIPAR;'
//     lenICode = 'int lenIPAR[1] = {0};'
//  end
//  txt($+1) = lenICode;
//  txt = [txt;
//         '#endif';
//         '';
//        ]

  //**************************//

  //Alan added opar (27/06/07)
  //*** Object parameters ***//
  if lstsize(opar)<>0 then
    txt=[txt;
          '/* def object parameters */']
    for i=1:(length(opptr)-1)
      if opptr(i+1)-opptr(i)>0  then

        if size(corinv(i),'*')==1 then
          OO=scs_m.objs(corinv(i));
        else
          path=list('objs');
          for l=cpr.corinv(i)(1:$-1)
            path($+1)=l;
            path($+1)='model';
            path($+1)='rpar';
            path($+1)='objs';
          end
          path($+1)=cpr.corinv(i)($);
          OO=scs_m(path);
        end

        //** Add comments **//
        txt($+1)='';
        txt($+1)='/* Routine name of block: '+strcat(string(cpr.sim.funs(i)));
        txt($+1)=' * Gui name of block: '+strcat(string(OO.gui));
        txt($+1)=' * Compiled structure index: '+strcat(string(i));
        if stripblanks(OO.model.label)~=emptystr() then
          txt=[txt;cformatline(' * Label: '+strcat(string(OO.model.label)),70)];
        end
        if stripblanks(OO.graphics.id)~=emptystr() then
          txt=[txt;
               cformatline(' * Identification: '+strcat(string(OO.graphics.id)),70)];
        end
        txt($+1)=' */';
        //******************//

        for j=1:opptr(i+1)-opptr(i)
          txt =[txt;
                cformatline('static __CONST__ '+mat2c_typ(opar(opptr(i)+j-1)) +...
                            ' OPAR_'+string(opptr(i)+j-1) + '[] = {'+...
                            strcat(string(opar(opptr(i)+j-1)),',')+'};',70)]
        end
      end
    end
  end
  //*************************//

  txt=[txt;
       '']
endfunction

//==========================================================================
//mat2c_typ : matrix to C type
//sci2c_ttyp : get the C string of a scicos type
//
//input : outtb : a matrix
//
//output : txt : the string of the C scicos type
//               of the data of outtb
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [txt]=mat2c_typ(outtb)
 select type(outtb)
   //real matrix
   case 1 then
      if isreal(outtb) then
        txt = "double"
      else
        txt = "double"
      end
   //integer matrix
   case 8 then
      select typeof(outtb)
         case 'int32' then
           txt = "long"
         case 'int16' then
           txt = "short"
         case 'int8' then
           txt = "char"
         case 'uint32' then
           txt = "unsigned long"
         case 'uint16' then
           txt = "unsigned short"
         case 'uint8' then
           txt = "unsigned char"
      end
   else
     break;
 end
endfunction


//==========================================================================
//mat2scs_c_nb  matrix to scicos C number (sci2sci_n)
//
//input : outtb : a matrix
//
//output : c_nb : the scicos C number
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [c_nb]=mat2scs_c_nb(outtb)
 select type(outtb)
   //real matrix
   case 1 then
      if isreal(outtb) then
        c_nb = 10
      else
        c_nb = 11
      end
   //integer matrix
   case 8 then
      select typeof(outtb)
         case 'int32' then
           c_nb = 84
         case 'int16' then
           c_nb = 82
         case 'int8' then
           c_nb = 81
         case 'uint32' then
           c_nb = 814
         case 'uint16' then
           c_nb = 812
         case 'uint8' then
           c_nb = 811
      end
   else
     break;
 end
endfunction

//==========================================================================
//mat2scs_c_ptr matrix to scicos C ptr (sci2c_typ)
//
//input : outtb : a matrix
//
//output : txt : the string of the C scicos type
//               of the data of outtb
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [txt]=mat2scs_c_ptr(outtb)
 select type(outtb)
   //real matrix
   case 1 then
      if isreal(outtb) then
        txt = "SCSREAL_COP"
      else
        txt = "SCSCOMPLEX_COP"
      end
   //integer matrix
   case 8 then
      select typeof(outtb)
         case 'int32' then
           txt = "SCSINT32_COP"
         case 'int16' then
           txt = "SCSINT16_COP"
         case 'int8' then
           txt = "SCSINT8_COP"
         case 'uint32' then
           txt = "SCSUINT32_COP"
         case 'uint16' then
           txt = "SCSUINT16_COP"
         case 'uint8' then
           txt = "SCSUINT8_COP"
      end
   else
     break;
 end
endfunction

//==========================================================================
//mat2scs_c_typ matrix to scicos C type
//
//input : outtb : a matrix
//
//output : txt : the string of the C scicos type
//               of the data of outtb
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [txt]=mat2scs_c_typ(outtb)
 select type(outtb)
   //real matrix
   case 1 then
      if isreal(outtb) then
        txt = "SCSREAL_N"
      else
        txt = "SCSCOMPLEX_N"
      end
   //integer matrix
   case 8 then
      select typeof(outtb)
         case 'int32' then
           txt = "SCSINT32_N"
         case 'int16' then
           txt = "SCSINT16_N"
         case 'int8' then
           txt = "SCSINT8_N"
         case 'uint32' then
           txt = "SCSUINT32_N"
         case 'uint16' then
           txt = "SCSUINT16_N"
         case 'uint8' then
           txt = "SCSUINT8_N"
      end
   else
     break;
 end
endfunction

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function rename(folder,newname,ext)
  oldname=folder+'/Makefile';
  newname=folder+'/'+newname;
  T=mgetl(oldname);
  T=strsubst(T,'.obj','.o');
  T=strsubst(T,'.o',ext);
  T=strsubst(T,SCI,WSCI);
  mputl(T,newname);
  mdelete(oldname);
endfunction

//==========================================================================

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function Makename=rt_gen_make(name,files,libs)

  Makename=rpat+'/Makefile';

  T=mgetl(TARGETDIR+'/'+makfil);
  T=strsubst(T,'$$MODEL$$',name);
  T=strsubst(T,'$$OBJ$$',strcat(files+'.o',' '));
  T=strsubst(T,'$$SCILAB_DIR$$',SCI);
  T=strsubst(T,'$$ETLPATH$$',getenv('ETLPATH'));
  mputl(T,Makename)

endfunction



//==========================================================================
//scs_c_n2c_fmt : scicos C number to C format
//
//input : c_nb : a C scicos type
//
//output : txt : the string of the C format string
//               of the data of outtb
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [txt]=scs_c_n2c_fmt(c_nb)
 select c_nb
   //real matrix
   case 10 then
     txt = '%f';
   //complex matrix
   case 11 then
     txt = '%f,%f';
   //int8 matrix
   case 81 then
     txt = '%d';
   //int16 matrix
   case 82 then
     txt = '%d';
   //int32 matrix
   case 84 then
     txt = '%d';
   //uint8 matrix
   case 811 then
     txt = '%d';
   //uint16 matrix
   case 812 then
     txt = '%d';
   //uint32 matrix
   case 814 then
     txt = '%d';
   else
     txt='%f'
     break;
 end
endfunction

//==========================================================================
//scs_c_n2c_typ scicos C number to C type
//
//input : c_nb : a C scicos number
//
//output : txt : the string of the C format string
//               of the data of outtb
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [txt]=scs_c_n2c_typ(c_nb)
 select c_nb
   //real matrix
   case 10 then
     txt = 'double';
   //complex matrix
   case 11 then
     txt = 'double';
   //int8 matrix
   case 81 then
     txt = 'char';
   //int16 matrix
   case 82 then
     txt = 'short';
   //int32 matrix
   case 84 then
     txt = 'long';
   //uint8 matrix
   case 811 then
     txt = 'unsigned char';
   //uint16 matrix
   case 812 then
     txt = 'unsigned short';
   //uint32 matrix
   case 814 then
     txt = 'unsigned long';
   else
     txt='double'
     break;
 end
endfunction

//==========================================================================
//scs_c_nb2scs_nb : scicos C number to scicos number
//
//input : c_nb  : the scicos C number type
//
//output : scs_nb : the scilab number type
//
//16/06/07 Author : A.Layec
//Copyright INRIA
function [scs_nb]=scs_c_nb2scs_nb(c_nb)
 scs_nb=zeros(size(c_nb,1),size(c_nb,2));
 for i=1:size(c_nb,1)
   for j=1:size(c_nb,2)
     select (c_nb(i,j))
       case 10 then
         scs_nb(i,j) = 1
       case 11 then
         scs_nb(i,j) = 2
       case 81 then
         scs_nb(i,j) = 5
       case 82 then
         scs_nb(i,j) = 4
       case 84 then
         scs_nb(i,j) = 3
       case 811 then
         scs_nb(i,j) = 8
       case 812 then
         scs_nb(i,j) = 7
       case 814 then
         scs_nb(i,j) = 6
       else
         scs_nb(i,j) = 1
     end
   end
 end
endfunction

//==========================================================================
//write_code_cdoit : generate body of the code for
//                   for all time dependant blocks
//
//input : flag : flag number for block's call
//
//output : txt for cord blocks
//
//12/07/07 Alan Layec
//Copyright INRIA

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function [txt]=write_code_cdoit(flag)

  txt=[];

  for j=1:ncord
    bk=cord(j,1);
    pt=cord(j,2);
    //** blk
    if funtyp(bk)>-1 then
//      if or(bk==act) | or(bk==cap) then
//        if stalone then
//          txt2=call_block42(bk,pt,flag);
//          if txt2<>[] then
//            txt=[txt;
//                 '    '+txt2
//                 ''];
//          end
//        end
//      else
        txt2=call_block42(bk,pt,flag);
        if txt2<>[] then
          txt=[txt;
               '  '+txt2
               ''];
        end
//      end
    //** ifthenelse blk
    elseif funtyp(bk)==-1 then
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      thentxt=write_code_doit(clkptr(bk),flag);
      elsetxt=write_code_doit(clkptr(bk)+1,flag);
      if thentxt<>[] | elsetxt<>[] then
        txt=[txt;
             '  '+get_comment('ifthenelse_blk',list(bk));]
        //** C **//
        tmp_='*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        txt=[txt;
             '  if('+tmp_+'>0) {']
        //*******//
        txt=[txt;
             Indent+thentxt];
        if elsetxt<>[] then
          //** C **//
          txt=[txt;
               '  }';
               '  else {';]
          //*******//
          txt=[txt;
               Indent+elsetxt];
        end
        //** C **//
        txt=[txt;
             '  }']
        //*******//
      end
    //** eventselect blk
    elseif funtyp(bk)==-2 then
      Noutport=clkptr(bk+1)-clkptr(bk);
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      II=[];
      switchtxt=list()
      for i=1: Noutport
        switchtxt(i)=write_code_doit(clkptr(bk)+i-1,flag);
        if switchtxt(i)<>[] then II=[II i];end
      end
      if II<>[] then
        txt=[txt;
             '  '+get_comment('evtselect_blk',list(bk));]
        //** C **//
        tmp_='*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        txt=[txt;
             '  i=max(min((int) '+...
              tmp_+',block_'+rdnom+'['+string(bk-1)+'].evout),1);'
             '  switch(i)'
             '  {']
        //*******//
        for i=II
         //** C **//
         txt=[txt;
              '   case '+string(i)+' :';]
         //*******//
         txt=[txt;
              BigIndent+write_code_doit(clkptr(bk)+i-1,flag);]
         //** C **//
         txt=[txt;
              BigIndent+'break;']
         //*******//
        end
        //** C **//
        txt=[txt;
             '  }'];
        //*******//
      end
    //** Unknown block
    else
      error('Unknown block type '+string(bk));
    end
  end

endfunction

//==========================================================================
//write_code_doit : generate body of the code for
//                  ordering calls of blocks during
//                  flag 1,2 & flag 3
//
//input : ev  : evt number for block's call
//       flag : flag number for block's call
//
//output : txt for flag 1 or 2, or flag 3
//
//12/07/07 Alan Layec

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function [txt]=write_code_doit(ev,flag)

  txt=[];

  for j=ordptr(ev):ordptr(ev+1)-1
    bk=ordclk(j,1);
    pt=ordclk(j,2);
    //** blk
    if funtyp(bk)>-1 then
//      if or(bk==act) | or(bk==cap) then
//        if stalone then
//          txt2=call_block42(bk,pt,flag);
//          if txt2<>[] then
//            txt=[txt;
//                 '    '+txt2
//                 ''];
//          end
//        end
//      else
        txt2=call_block42(bk,pt,flag);
        if txt2<>[] then
          txt=[txt;
               '    '+txt2
               ''];
        end
//      end
    //** ifthenelse blk
    elseif funtyp(bk)==-1 then
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      thentxt=write_code_doit(clkptr(bk),flag);
      elsetxt=write_code_doit(clkptr(bk)+1,flag);
      if thentxt<>[] | elsetxt<>[] then
        txt=[txt;
             '    '+get_comment('ifthenelse_blk',list(bk));]
        //** C **//
        tmp_ = '*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        txt=[txt;
             '    if('+tmp_+'>0) {']
        //*******//
        txt=[txt;
             Indent+thentxt]
        if elsetxt<>[] then
           //** C **//
           txt=[txt;
                '    }';
                '    else {';]
           //*******//
           txt=[txt;
                Indent+elsetxt];
        end
        //** C **//
        txt=[txt;
             '    }']
        //*******//
      end
    //** eventselect blk
    elseif funtyp(bk)==-2 then
      Noutport=clkptr(bk+1)-clkptr(bk);
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      II=[];
      switchtxt=list()
      for i=1: Noutport
        switchtxt(i)=write_code_doit(clkptr(bk)+i-1,flag);
        if switchtxt(i)<>[] then II=[II i];end
      end
      if II<>[] then
        txt=[txt;
             '    '+get_comment('evtselect_blk',list(bk));]
        tmp_='*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        //** C **//
        txt=[txt;
             '    i=max(min((int) '+...
              tmp_+',block_'+rdnom+'['+string(bk-1)+'].evout),1);'
             '    switch(i)'
             '    {']
        //*******//
        for i=II
          //** C **//
          txt=[txt;
               '     case '+string(i)+' :';]
          //*******//
          txt=[txt;
               BigIndent+write_code_doit(clkptr(bk)+i-1,flag);]
          //** C **//
          txt=[txt;
               BigIndent+'break;']
          //*******//
        end
        //** C **//
        txt=[txt;
             '    }']
        //*******//
      end
    //** Unknown block
    else
      error('Unknown block type '+string(bk));
    end
  end

endfunction

//==========================================================================
//write_code_idoit : generate body of the code for
//                   ordering calls of initial
//                   called blocks
//
//input : nothing (blocks are called with flag 1)
//
//output : txt for iord
//
//15/07/07 Alan Layec
//Copyright INRIA

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

function [txt]=write_code_idoit()

  txt=[];

  for j=1:niord
    bk=iord(j,1);
    pt=iord(j,2);
    //** blk
    if funtyp(bk)>-1 then
//      if or(bk==act) then
//        if stalone then
//          txt2=call_block42(bk,pt,1);
//          if txt2<>[] then
//            txt=[txt;
//                 '  '+txt2
//                 ''];
//          end
//        end
//      else
        txt2=call_block42(bk,pt,1);
        if txt2<>[] then
          txt=[txt;
               '  '+txt2
               ''];
        end
//      end
    //** ifthenelse blk
    elseif funtyp(bk)==-1 then
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      thentxt=write_code_doit(clkptr(bk),1);
      elsetxt=write_code_doit(clkptr(bk)+1,1);
      if thentxt<>[] | elsetxt<>[] then
        txt=[txt;
             '  '+get_comment('ifthenelse_blk',list(bk));]
        //** C **//
        tmp_ = '*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        txt=[txt;
             '  if('+tmp_+'>0) {']
        //*******//
        txt=[txt;
             Indent+thentxt];
        if elsetxt<>[] then
           //** C **//
           txt=[txt;
                '  }';
                '  else {';]
           //*******//
           txt=[txt;
                Indent+elsetxt];
        end
        //** C **//
        txt=[txt;
             '  }']
        //*******//
      end
    //** eventselect blk
    elseif funtyp(bk)==-2 then
      Noutport=clkptr(bk+1)-clkptr(bk);
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      II=[];
      switchtxt=list()
      for i=1: Noutport
        switchtxt(i)=write_code_doit(clkptr(bk)+i-1,1);
        if switchtxt(i)<>[] then II=[II i];end
      end
      if II<>[] then
        txt=[txt;
             '  '+get_comment('evtselect_blk',list(bk));]
        //** C **//
        tmp_='*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        txt=[txt;
             '  i=max(min((int) '+...
              tmp_+',block_'+rdnom+'['+string(bk-1)+'].evout),1);']
        txt=[txt;
             '  switch(i)'
             '  {']
        //*******//
        for i=II
          //** C **//
          txt=[txt;
               '   case '+string(i)+' :';]
          //*******//
          txt=[txt;
               BigIndent+write_code_doit(clkptr(bk)+i-1,1);]
          //** C **//
          txt=[txt;
               BigIndent+'break;']
          //*******//
        end
        //** C **//
        txt=[txt;
             '  }'];
        //*******//
      end
    //** Unknown block
    else
      error('Unknown block type '+string(bk));
    end
  end

endfunction

//==========================================================================
//write_code_odoit : generate body of the code for
//                   ordering calls of blocks before
//                   continuous time integration
//
//input : flag : flag number for block's call
//
//output : txt for flag 0
//
//12/07/07 Alan Layec
//Copyright INRIA

// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch


function [txt]=write_code_odoit(flag)
 
  txt=[];

  for j=1:noord
    bk=oord(j,1);
    pt=oord(j,2);
    //** blk
    if funtyp(bk)>-1 then
      txt2=call_block42(bk,pt,flag);
      if txt2<>[] then
        txt=[txt;
             '    '+txt2
             ''];
      end
    //** ifthenelse blk
    elseif funtyp(bk)==-1 then
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      thentxt=write_code_ozdoit(clkptr(bk),flag);
      elsetxt=write_code_ozdoit(clkptr(bk)+1,flag);
      if thentxt<>[] | elsetxt<>[] then
        txt=[txt;
             '    '+get_comment('ifthenelse_blk',list(bk));]
        //** C **//
        tmp_='*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        txt=[txt;
             '    if ((block_'+rdnom+'['+string(bk-1)+'].nmode<0'+...
              ' && '+tmp_+'>0)'+...
              ' || \'
             '        (block_'+rdnom+'['+string(bk-1)+'].nmode>0'+...
              ' && block_'+rdnom+'['+string(bk-1)+'].mode[0]==1)) {']
        //*******//
        txt=[txt;
             Indent+thentxt]
        //** C **//
        txt=[txt;
             '    }'];
        //*******//
        if elsetxt<>[] then
          //** C **//
          txt=[txt;
               '    else if  ((block_'+rdnom+'['+string(bk-1)+'].nmode<0'+...
                ' && '+tmp_+'<=0)'+...
                ' || \'
               '              (block_'+rdnom+'['+string(bk-1)+'].nmode>0'+...
                ' && block_'+rdnom+'['+string(bk-1)+'].mode[0]==2)) {';]
          //*******//
          txt=[txt;
               Indent+elsetxt]
          //** C **//
          txt=[txt;
               '    }'];
          //*******//
        end
      end
    //** eventselect blk
    elseif funtyp(bk)==-2 then
      Noutport=clkptr(bk+1)-clkptr(bk);
      ix=-1+inplnk(inpptr(bk));
      TYPE=mat2c_typ(outtb(ix+1)); //** scilab index start from 1
      II=[];
      switchtxt=list()
      for i=1: Noutport
        switchtxt(i)=write_code_ozdoit(clkptr(bk)+i-1,flag);
        if switchtxt(i)<>[] then II=[II i];end
      end
      if II<>[] then
        txt=[txt;
             '    '+get_comment('evtselect_blk',list(bk));]
        //** C **//
        tmp_='*(('+TYPE+' *)'+rdnom+'_block_outtbptr['+string(ix)+'])'
        txt=[txt;
             '    if (block_'+rdnom+'['+string(bk-1)+'].nmode<0) {';
             '      i=max(min((int) '+...
                tmp_+',block_'+rdnom+'['+string(bk-1)+'].evout),1);'
             '    }'
             '    else {'
             '      i=block_'+rdnom+'['+string(bk-1)+'].mode[0];'
             '    }']
        txt=[txt;
             '    switch(i)'
             '    {'];
        //*******//
        for i=II
          //** C **//
          txt=[txt;
               '     case '+string(i)+' :';]
          //*******//
          txt=[txt;
               BigIndent+write_code_ozdoit(clkptr(bk)+i-1,flag);]
          //** C **//
          txt=[txt;
               BigIndent+'break;']
          //*******//
        end
        //** C **//
        txt=[txt;
             '    }'];
        //*******//
      end
    //** Unknown block
    else
      error('Unknown block type '+string(bk));
    end
  end

endfunction

function [files]=write_code(Code,CCode,FCode,Code_common)

// Original file from Project Metalau - INRIA
// Modified for RT purposes by Roberto Bucher - RTAI Team
// roberto.bucher@supsi.ch

 ierr=execstr('mputl(Code,rpat+''/''+rdnom+''.c'')','errcatch')
  if ierr<>0 then
    message(lasterror())
    ok=%f
    return
  end

 ierr=execstr('mputl(Code_common,rpat+''/common.c'')','errcatch')
  if ierr<>0 then
    message(lasterror())
    ok=%f
    return
  end

  if FCode<>[] then
    ierr=execstr('mputl(FCode,rpat+''/''+rdnom+''f.f'')','errcatch')
    if ierr<>0 then
      message(lasterror())
      ok=%f
      return
    end
  end

  if CCode<>[] then
    CCode = [
          '#include <math.h>';
          '#include <stdlib.h>';
          '#include <scicos_block4.h>';
	  '';
	  CCode];
    ierr=execstr('mputl(CCode,rpat+''/''+rdnom+''_Cblocks.c'')','errcatch')
    if ierr<>0 then
      message(lasterror())
      ok=%f
      return
    end
  end

  files=[]
  [fd,ierr]=mopen(rpat+'/'+rdnom+'f.f','r')
  if ierr==0 then mclose(fd),files=[files,rdnom+'f'],end
  [fd,ierr]=mopen(rpat+'/'+rdnom+'_Cblocks.c','r')
  if ierr==0 then mclose(fd),files=[files,rdnom+'_Cblocks'],end

endfunction

//==========================================================================


//==========================================================================
//Create  MSR-Interface
//==========================================================================
//Author : Andreas Stewering-Bone
//Copyright IgH Essen
function [Code] = build_msr_interface(InCode)
  mynblk=cpr.sim.nb;
  funs = cpr.sim.funs;
  
  Code=[InCode;
	'/* Create MSR-Interface */'
       	'int model_register_blocks(void)'
       	'{'
        '  int i;'
        '  int rvalue;'
	];
  for i=1:(mynblk) 
  	if check_fun_blacklist(funs(i))<0 then 
  		Code=[Code;
			''
			'/* Skipping register Block '+get_block_name(i)+' */'	
			''
		];
	else
  		Code=[Code;
		''
      		'  rvalue  = msr_register_blocks(""'+rdnom+'"",""'+get_block_name(i)+'"",&block_'+rdnom+'['+string(i-1)+']);'
       	 	'  if(rvalue != 0)'
		'     return rvalue;'
		''
		];
	end
  end
	
  Code=[Code;
	' /*MSR Registration finished*/'
        '  return 0;'
	'}'];
endfunction

//==========================================================================
//Search  MSR- and C-Names
//==========================================================================
//Author : Andreas Stewering-Bone
//Copyright IgH Essen
function blockpath = get_block_name(index)

  mycorinv=cpr.corinv;		

  if size(mycorinv(index),'*')==1 then
    OO=scs_m.objs(mycorinv(index));
    blockpath=[get_block_string(OO,index)];
  else
   blockpath=[''];	
   path=list('objs');
   for l=mycorinv(index)(1:$-1)
      path($+1)=l;
      OO=scs_m(path);
      blockpath=[blockpath+'/'+get_block_string(OO,index)];
      path($+1)='model';
      path($+1)='rpar';
      path($+1)='objs';
    end
    path($+1)=mycorinv(index)($);
    OO=scs_m(path);
    blockpath=[blockpath+'/'+get_block_string(OO,index)];
  end
endfunction


//==========================================================================
//Get block Identification or Label or Functionname
//==========================================================================
//Author : Andreas Stewering-Bone
//Copyright IgH Essen

function blockstring = get_block_string(blockobject,index)
	if stripblanks(blockobject.graphics.id)~=emptystr() then
         	blockstring=blockobject.graphics.id;
       	else
                blockstring=[blockobject.gui+'_'+string(index)];
       	end
	tmpstring = blockstring;
	blockstring = strsubst(tmpstring,''"','\'"'); 
endfunction




//==========================================================================
//Check for Gui depended functions
//==========================================================================
//Author : Andreas Stewering-Bone
//Copyright IgH Essen

function rvalue = check_fun_blacklist(funame)
	rvalue = 1;
	blacklist = ['affich2';'cmscope';'canimxy';'cevscpe';'cfscope';'cmat3d';'cmatview';'cscope';'cscopxy';'cscopxy3d';'tows_c';'writef';'writeau';'cevscpe';'writec'];
 	if ~isempty(grep(blacklist,funame)) then
 	    rvalue=-1;
 	end
endfunction
