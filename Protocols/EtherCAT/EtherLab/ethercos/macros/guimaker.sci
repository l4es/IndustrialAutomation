function varargout=guimaker(page,guiProp,DefProp,Flag)
  // A command line based tool for building graphical user interfaces.
  // Calling Sequence
  //  [val1,val2,...]=guiMaker(page,[guiProp],[DefProp],[Flag]) // return values from objects in the gui
  //  links=guiMaker(page,[guiProp],[DefProp],2)        // return handles to gui objects
  //  txt=guiMaker(page,[guiProp],[DefProp],1)         // return source code for the gui
  //
  // Parameters
  //  page : list of lines making up the gui. Lines are counted from top to bottom of the gui.
  //  lines : list of objects in each line. Empty elements are interpreted as objects not present.
  //  objects : list([wh],Style,String,Property1,Value1,Property2,Value2,...) or [] to indicate a vacant position in the line.
  //  wh : OPTIONAL vector where wh(1) defines the object width relative to the width of the gui. wh(2) is the object height in number of lines.
  //  Style : string with the object type {pushbutton | radiobutton | checkbox | edit | text | slider | frame |listbox | popupmenu }.
  //  String : string containing the 'string' value of the object.
  //  Property : string with legal object properties { BackgroundColor | callback | fontangle | fontsize | fontunits | fontname | ForegroundColor | Horizontalalignment | ListboxTop | Max | Min | SliderStep | String | Tag | Units | Userdata | Value | Verticalalignment }
  //  Value : value of the previous object property, see uicontrol for more details.
  //  guiProp : OPTIONAL list(Title,width,height,space,Backgroundcolor) defining gui window properties.
  //  Title : OPTIONAL string with name of the gui figure (default is 'Scilab - guimaker()').
  //  width : OPTIONAL width (points) of the gui window (default is 450).
  //  heigth : OPTIONAL height (points) of a line (default is 20).
  //  space : OPTIONAL space (points) between lines and objects on lines (default is 5)
  //  Backgroundcolor : OPTIONAL vector [R,G,B] with background color codes (default is [.9 .9 .9])
  //  DefProp : OPTIONAL list(Property1,Value1,...) with default properties for objects used in the gui.
  //  Flag : OPTIONAL =1: return Scilab code; =2: return list of object handles; ="filename": read/store input/output values in TMPDIR.
  //  links : vector of object handles to each object in the gui (starting with the gui window and proceeds from the top left to bottom right position).
  //  val1,val2,... : results {string | value} from objects, except { pushbutton | text | frame }, in the order from top left to bottom right position in the gui. If only val1 is present the results are returned as a list.
  //  
  // Description
  //  guimaker is a tool for lazy programmers for creating graphical user interfaces in Scilab.
  //  
  //  guimaker can be used in three modes: 
  //
  //  Interactive retrieval of input: 
  //  Called with more than one output argument ([val1,val2,...]=guimaker(...))
  //  the output arguments wil be assigned values from the "string" or "value" fields 
  //  of objects present in the gui. The objects value or string are assigned to output 
  //  variables according to the order the objects occure in the gui, counting objects 
  //  from top left to the bottom right of the gui. 
  //  The values of pushbutton's and text fields are not assigned to output variables.
  //  The gui is kept open until the variabel OK is set to %T by executing a callback 
  //  function 'OK=%T;' from one of the objects in the GUI. 
  //  If the local variable CANCEL is set to %T (or the gui window is killed) all output 
  //  variables are set to an empty matrix. 
  //  The user must make sure to define an object (pushbutton) with callback 'OK=%t'
  //  and optionally also an object with 'callback' set to 'CANCEL=%t'. 
  //  See example section below.
  //
  //  Interactive creation of a gui:
  //  Called with only one output variable guimaker and the fourth input parameter 
  //  set to 2 it will create the gui and return a vector of handles. 
  //  links(1) is the handle to the gui figure, while link(2:$) are handles to 
  //  each object present in the gui (the handles are listed counting 
  //  from top left to bottom right in the gui). 
  //
  //  Source code generation: 
  //  Called with one output parameter and the fourth input parameter set to 1
  //  guimaker will return the source code for the gui as a string matrix. 
  //  The resulting code can be used and modified independently of guimaker.
  //
  //  The graphical user interface is defined by the argument page, which is a list 
  //  variable. Each element in the page list represents lines, counting from the 
  //  top to the bottom of the gui. The lines are list variables.
  //  Each element of the line list represents objects present in that line,
  //  counting from left to right in the line. 
  //  Finally each object is defined by a list where the elements represent
  //  object type, string value and relevant properties. See uicontrol  
  //  for further details on object properties and values. 
  //  The width and height of each object may be defined using the optional 
  //  parameter wh (as the first element in the object list). 
  //  If present, wh(1) represents the width of the object, relative to the width of 
  //  the gui window. The default width weighting factor for an object is 1. 
  //  wh(2) give the height of the object in number of lines. The default is
  //  a height of one line for each object.
  //
  // Examples
  //  // Interactive retrieval of input:
  //  // first example from http://wiki.scilab.org/howto/guicontrol
  //    page=list();
  //    page($+1)=list(list('text','general properties','Horizontalalignment','center','Backgroundcolor',[1 1 1]));
  //    page($+1)=list(list('text','particle diameter (nm)'),list('edit','1000'));
  //    page($+1)=list(list('text','particle density (g/cm3)'),list('edit','1'));
  //    page($+1)=list(list('text','gas viscosity (°0C) u (N*sec/m2)'),list('edit','0.0000172'));
  //    page($+1)=list(list('text','gas density (0°C), 1 atm (kg/m3)'),list('edit','1.29'));
  //    page($+1)=list(list('text','mean free path (0°C), 1 atm (nm)'),list('edit','67'));
  //    page($+1)=list(list('text','gamma, Cv/Cp'),list('edit','1.4'));
  //    page($+1)=list(list('text','Sutherlands temperature, K'),list('edit','110.4'));
  //    page($+1)=list(list('text','Temperature, K'),list('edit','300'));
  //    page($+1)=list(list('text','volume flow rate (standard l/min)'),list('edit','0.5'));
  //    page($+1)=list(list('text','telescope properties','Horizontalalignment','center','Backgroundcolor',[1 1 1]));
  //    page($+1)=list(list('text','orifice diameters (mm)'),list('edit','1,1,1,1,1'));
  //    page($+1)=list(list('text','tube diameters (mm)'),list('edit','100,100,100,100,100'));
  //    page($+1)=list(list('text','input diameters (mm)'),list('edit','201325'));
  //    page($+1)=list(list(2),list('pushbutton','Stop','callback','OK=%t'),list(2));
  //    [dp,rhop,mu0,rhog0,lambda0,CvCp,S,T,mdot,df,ds,pin]=guimaker(page,list('Aerodynamic Lens Design',[],[],[],[]))
  //
  //  // second example from http://wiki.scilab.org/howto/guicontrol
  //    pg=list();
  //    pg($+1)=list(list('text','exposure time','Horizontalalignment','center'));
  //    pg($+1)=list(list('slider','','Min',0,'Max',100,'Value',5));
  //    pg($+1)=list(list('text','arevscale','Horizontalalignment','center'));
  //    pg($+1)=list(list('slider','','Min',0,'Max',100,'Value',50));
  //    pg($+1)=list(list([1 2],'pushbutton','STOP','callback','OK=%t'),list(2,'radiobutton','bin x2'));
  //    pg($+1)=list(list(1),list(2,'radiobutton','free/trig'));
  //    [hexp,hbri,hbin,htrig]=guimaker(pg,list('objfigure1',220,[],10,[]))
  //    
  //  // Interactive creation of gui:
  //    line1=list(list('text','Description in text field'),list('edit','input in edit field'));
  //    line2=list(list('radiobutton','radiobutton'),list('checkbox','checkbox'));
  //    line3=list(list('slider','','Min',0,'Max',100,'SliderStep',[1 10],'Value',35),list('popupmenu','option one|two|three|four|five'));
  //    line4=list(list(2),list('pushbutton','OK','callback','OK=%t'),list(2));
  //    page=list(line1,line2,line3,line4);
  //    h=guimaker(page,[],[],2)  // return vector of object handles
  //
  //  // Source code generation:
  //    source_code = guimaker(page,[],[],1) 
  //
  // See also
  //  uicontrol
  // Bibliography
  //  http://wiki.scilab.org/howto/guicontrol ; provides more information on gui's in Scilab.
  // Authors
  // T. Pettersen ; torbjorn.pettersen@broadpark.no

  //    Copyright (C) 2008  Torbjørn Pettersen
  //
  // This program is free software; you can redistribute it and/or modify
  // it under the terms of the GNU General Public License as published by
  // the Free Software Foundation; either version 2 of the License, or
  // (at your option) any later version.
  // 
  // This program is distributed in the hope that it will be useful,
  // but WITHOUT ANY WARRANTY; without even the implied warranty of
  // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  // GNU General Public License for more details.
  // 
  // You should have received a copy of the GNU General Public License
  // along with this program; if not, write to the Free Software
  // Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  // Check input arguments...
  if argn(2)<4 | isempty(Flag) then Flag=[]; ShowCode=%f; ReturnValues=%t; FileName=[]; Values=[]; end
  if argn(2)<3 | isempty(DefProp) then DefProp=list('Backgroundcolor',.9*[1 1 1],'Horizontalalignment','left'); end
  if argn(2)<2 | isempty(guiProp) then guiProp=list('Scilab guimaker()',450,20,5,[1 1 1]*.9); end
  if argn(2)<1 | isempty(page) then // start demo...
    page=list();
    page($+1)=list(list('text','Enter input'),list('edit','in this field'));
    page($+1)=list(list('radiobutton','radiobutton'),list('checkbox','checkbox'));
    page($+1)=list(list('slider','','Min',0,'Max',100,'SliderStep',[1 10],'Value',35),...
               list('popupmenu','option one|two|three|four|five'));
    page($+1)=list(list(1),list('pushbutton','OK','callback','OK=%T'),..
               list(1),list('pushbutton','Cancel','callback','CANCEL=%T'),list(1));
    help guimaker
  end

  if length(guiProp)<1 | isempty(guiProp(1)) then Title='Scilab guimaker()'; else Title=guiProp(1); end
  if length(guiProp)<2 | isempty(guiProp(2)) then W=450; else W=guiProp(2); end
  if length(guiProp)<3 | isempty(guiProp(3)) then h=20; else h=guiProp(3); end
  if length(guiProp)<4 | isempty(guiProp(4)) then dh=5; else dh=guiProp(4); end
  if length(guiProp)<5 | isempty(guiProp(5)) then Backgroundcolor=[1 1 1]*.9; else Backgroundcolor=guiProp(5); end

  if type(Flag)==10 then 
    FileName=Flag; ShowCode=%f; ReturnValues=%t;
    if ~isempty(fileinfo(TMPDIR+'\'+FileName)) then
      load(TMPDIR+'\'+FileName,'Values');
    else
      Values=[];
    end
  elseif Flag==1 then
    ShowCode=%t; ReturnValues=%f; Values=[];
  elseif Flag==2 then
    ShowCode=%f; ReturnValues=%f; Values=[];
  end

  // Check output arguments and figure out what to do...
  if ReturnValues then   // ... need to return values from input fields
    if isempty(FileName) then
      h=guimaker(page,guiProp,DefProp,2); // create gui and get hold of the handles
    else
      h=guimaker(page,guiProp,DefProp,2,FileName); 
    end
    guiTag=sprintf('guimaker%f',rand(1)); set(h(1),'Tag',guiTag); // make a unique tag
    OK=%F; CANCEL=%F; 
    out=list(); for i=1:argn(1), out(i)=[]; end; // prepare for an empty return
    while ~OK & ~CANCEL
      sleep(50); 
      gui=findobj('Tag',guiTag);
//      if isempty(gui) then error('GUI was killed ...'); end // return if the figure is killed
    end
    if OK then
      OutputObj=list();               // which objects contain output values
      for i=2:length(h),      
        ObjType=get(h(i),'Style');
        if ObjType~='pushbutton' & ObjType~='text' & ObjType~='frame' then
          OutputObj($+1)=h(i);
        end
      end
      for i=1:length(OutputObj),
        ObjType=get(OutputObj(i),'style');
        if ObjType=='edit' then
          str=get(OutputObj(i),'string');
          execstr('out(i)='+str,'errorcatch');
          if ~isempty(lasterror()) then out(i)=str; end        
        else
          out(i)=get(OutputObj(i),'value');
        end
      end
    end
    if ~isempty(FileName) then 
      Values=out; 
      save(TMPDIR+'\'+FileName,Values);
    end
    if argn(1)>1 then varargout=out; else varargout=list(); varargout(1)=out; end;
    close(h(1)); // close figure window
  else                // ... or just create the gui and return handles to objects
    Offset=100; // offset due to changed figure format in Scilab 5.0
    nlines=length(page);  // # lines
    H=Offset+nlines*(h+dh)+2*dh; // height in points of the gui windows
    Y=(H-Offset)-(dh+h); // Y location for the first line in the gui

    links=[];   // list with handles to objects that will be created
    figNo=max(winsid())+1; // next available figure number
    if ShowCode then
      source=sprintf('fig=figure(%i,''figure_name'',''%s'',''position'',[0 0 %i %i],''Backgroundcolor'',[%f %f %f])',figNo,Title,W,H,Backgroundcolor(1),Backgroundcolor(2),Backgroundcolor(3)); 
      source=[source;'toolbar(fig.figure_id,""off"");']; 
      source=[source;'delmenu(fig.figure_id,''File''); delmenu(fig.figure_id,''Tools'');'];  
      source=[source;'delmenu(fig.figure_id,''Edit''); delmenu(fig.figure_id,''?'');'];
      itt=1;
    else
      fig=figure(figNo,'figure_name',Title,'position',[0 0 W H],'BackgroundColor',Backgroundcolor);
      links(1)=fig; 
      toolbar(fig.figure_id,"off"); // hide the toolbar
      delmenu(fig.figure_id,'File'); delmenu(fig.figure_id,'Tools');  // get rid of the
      delmenu(fig.figure_id,'Edit'); delmenu(fig.figure_id,'?');      // default menus
    end
    ValuesCounter=1;        // counter used in case Values is defined.
    nValues=length(Values); // number of Values...
    for i=1:nlines        // for every line in a page
      line=page(i);       // get current line
      nobj=length(line);  // count # of objects in current line
      x=ones(1,nobj); nl=ones(1,nobj);
      for k=1:nobj,                                   // for every object in the line
        if typeof(line(k)(1))=='constant' then        // figure out if we have special 
          if size(line(k)(1),'*')==2 then             // width requirements for 
            x(k)=line(k)(1)(1); nl(k)=line(k)(1)(2);  // each object in the line
          else
            x(k)=line(k)(1);
          end
          if length(line(k))>1 then line(k)=list(line(k)(2:$)); else line(k)=list(); end
        end
      end

      X=dh; w=W/nobj-nobj*dh;
      w = (W-(nobj+2)*dh)*x/sum(x); 
      for j=1:nobj        // for every object in current line
        obj=line(j);      // pick the object
        if ~isempty(obj) then // and create it if it exists
          if isempty(Values) | (obj(1)=='text' | obj(1)=='pushbutton' | obj(1)=='frame') then
            cmd=putguiobj(obj(1),obj(2),[X Y-(h+dh)*(nl(j)-1) w(j) h*nl(j)],list(obj(3:$)),DefProp);
          else
            if ValuesCounter>nValues then 
              warning('Too few Values stored in '+FileName+'... check code'); 
              ValuesCounter=nValues; 
            end
            cmd=putguiobj(obj(1),obj(2),[X Y-(h+dh)*(nl(j)-1) w(j) h*nl(j)],list(obj(3:$)),DefProp,Values(ValuesCounter));
            ValuesCounter=ValuesCounter+1;
          end
          if ShowCode then
            source=[source;sprintf('h(%i)=%s;',itt,cmd)]; itt=itt+1;
          else
            links($+1)=evstr(cmd);
          end
        end
        X=X+w(j)+dh;         // advance to the next x position
      end
      Y=Y-(dh+h);
    end
    if ShowCode then
      varargout=list(source);
    else
      varargout=list(links);
    end
  end
endfunction
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
function txt=putguiobj(Style,String,XYwh,Prop,DefProp,DefValue)
  // Style : string - type of uicontrol
  // String : string - the 'string' value of the object
  // XYwh : 4x1 vector - position coordinates
  // Prop : optional list(Prop1,Val1,Prop2,Val2...) - properties for uicontrol which overrides DefProp
  // DefProp : optional list(Prop1,Val1,Prop2,Val2) - default properties for objects in this gui
  // DefValue : optional value 'string' or 'value' overriding any in Prop.
  // txt : text string with uicontrol calling sequence to create the object.

  if argn(2)<6 then DefValue=[]; end
  if argn(2)<5 then DefProp=[]; end
  if argn(2)<4 then Prop=[]; end;

  SetValue=%t;
  if ~isempty(DefValue) then
    if Style=='edit' then
      String=string(DefValue);
    elseif Style=='radiobutton' | Style=='checkbox' | Style=='slider' | Style=='listbox' | Style=='popupmenu' then
      for i=1:2:length(Prop),
        if convstr(Prop(i),'l')=='value' then Prop(i+1)=DefValue; SetValue=%f; end
      end
      if SetValue then Prop($+1)='value'; Prop($+1)=DefValue; end
    end
  end
  
  String=strsubst(String,'[','\['); String=strsubst(String,']','\]');
  txt='uicontrol(fig,""Position"",'+sprintf('[%i %i %i %i]',XYwh)+',""Style"",""'+Style+'""';
  if ~isempty(String) then txt=txt+',""string"",""'+String+'""'; end

  Props=[]; for i=1:2:length(Prop), Props=[Props,Prop(i)]; end
  for i=1:2:length(DefProp),                        // for every DefProp
    if isempty(find(Props==DefProp(i))) then        // which is not in Prop
      Prop($+1)=DefProp(i); Prop($+1)=DefProp(i+1); // add it to Prop
    end
  end

  for k=1:2:length(Prop), 
    txt=txt+',""'+Prop(k)+'""';
    if typeof(Prop(k+1))=='string' then
      Prop(k+1)=strsubst(Prop(k+1),'\','\\'); // in 'string' parameters.
      Prop(k+1)=strsubst(Prop(k+1),'[','\['); // to avoid tcl error message
      Prop(k+1)=strsubst(Prop(k+1),']','\]'); // in case [] or \ is used
      txt=txt+',""'+Prop(k+1)+'""';
    elseif length(Prop(k+1))>1 then
      txt=txt+',['+sprintf('%f ',Prop(k+1)(:))+']';
    else
      txt=txt+','+string(Prop(k+1));
    end
  end
  txt=txt+')'; 
endfunction

