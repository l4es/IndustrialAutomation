function toolbox_creator(name,target_dir)
// A tool for quickly setting up a new toolbox.
// Calling Sequence
//  toolbox_creator() 
//  toolbox_creator(name,target_dir)
//
// Parameters
//  name: the name of the toolbox 
//  target_dir: target directory where the new toolbox will be created
//
// Description
// A user friendly frontend for the Scilab toolbox template. 
//
// Select which sub directories should be included in the toolbox, 
// and define names and target directory.
//
// After the toobox template directory has been created and the relevant 
// files have been place in the macro directory, use 
// help_from_sci for automatic generation of xml help files.
//
// See also
//  help_from_sci
//
// Authors
//  T. Pettersen ; torbjorn.pettersen@broadpark.no
// Bibliography
//   Cornett, A., Lipp, S. ; SCI/contrib/toolbox_skeleton

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

if argn(2)<2 then target_dir=pwd(); end
if argn(2)<1 then name='test'; end;

pg=list();
pg($+1)=list(list('text','Name of toolbox'),list(2,'edit',name));
pg($+1)=list(list('pushbutton','Target directory','callback','set(findobj(''''tag'''',''''TDir''''),''''string'''',uigetdir(""""'+target_dir+'""""))'),list(2,'edit',target_dir,'tag','TDir'));
pg($+1)=list(list('text','Help chapter heading'),list(2,'edit',name));
pg($+1)=list(list('text','Template directories'),list(2,'text','Description'))
pg($+1)=list(list('checkbox','macros','value','1'),list(2,'text','Scilab macros (.sci files)'));
pg($+1)=list(list('checkbox','src'),list(2,'text','source code (all .c and .f files)'));
pg($+1)=list(list('checkbox','sci_gateway'),list(2,'text','interface programs'));
pg($+1)=list(list('checkbox','help','value','1'),list(2,'text','the .xml help files'));
pg($+1)=list(list('checkbox','tests'),list(2,'text','scripts to test the toolbox'));
pg($+1)=list(list('checkbox','demos'),list(2,'text','examples to illustrate the toolbox'));
pg($+1)=list(list('pushbutton','OK','Horizontalalignment','center','callback','OK=%T'),list('pushbutton','Cancel','Horizontalalignment','center','callback','CANCEL=%T'));
[name,target_dir,help_name,c_macros,c_src,c_sci_gw,c_help,c_tests,c_demos]=guimaker(pg,list("toolbox_creator",650));

if isempty(name) then disp('Terminated by user...nothing changed.'); return; end

printf('Creating a toolbox template...\n');
printf(' ...in the target directory: %s\n',target_dir);
printf(' ...for the toolbox: %s\n',name);

chdir(target_dir);
status=mkdir(name)
if status==2 then
  answ=buttondialog(target_dir+'\'+name+' already exists',"Overwrite|Cancel");
  if answ==2 then return; end
  printf(' %s already exists - overwriting files...\n',name);
end
chdir(name);

builder_txt=[
'mode(-1);'
'lines(0);'
'// ===================================================================='
'// Created using toolbox_creator()'
'// ===================================================================='
'try'
' getversion(''scilab'');'
'catch'
' error(gettext(''Scilab 5.0 or more is required.''));  '
'end;'
'// ===================================================================='
'if ~with_module(''development_tools'') then'
'  error(msprintf(gettext(''%s module not installed.""),''development_tools''));'
'end'
'// ===================================================================='
];
builder_txt=[builder_txt;'TOOLBOX_NAME = '''+name+''';'];
builder_txt=[builder_txt;'TOOLBOX_TITLE = '''+help_name+''';'];
builder_txt=[builder_txt;..
'// ===================================================================='
'toolbox_dir = get_absolute_file_path(''builder.sce'');'
''];
if c_macros then builder_txt=[builder_txt;'tbx_builder_macros(toolbox_dir);']; end
if c_src then builder_txt=[builder_txt;'tbx_builder_src(toolbox_dir);']; end
if c_sci_gw then builder_txt=[builder_txt;'tbx_builder_gateway(toolbox_dir);']; end
if c_help then builder_txt=[builder_txt;'tbx_builder_help(toolbox_dir);']; end
builder_txt=[builder_txt;'tbx_build_loader(TOOLBOX_NAME, toolbox_dir);'];
builder_txt=[builder_txt;..
''
'clear toolbox_dir TOOLBOX_NAME TOOLBOX_TITLE;'
'// ===================================================================='
];
f=mopen('builder.sce','w'); mfprintf(f,'%s\n',builder_txt); mclose(f);
copyfile(SCI+'/contrib/toolbox_skeleton/changelog.txt','.');
txt=[
'License for the toolbox '+name
''
'Please update this file with the license terms (Copy and paste the license agreement e.g).'
''
'    Copyright (C) YEAR  Authors name'
''
' This program is free software; you can redistribute it and/or modify'
' it under the terms of the GNU General Public License as published by'
' the Free Software Foundation; either version 2 of the License, or'
' (at your option) any later version.'
' '
' This program is distributed in the hope that it will be useful,'
' but WITHOUT ANY WARRANTY; without even the implied warranty of'
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the'
' GNU General Public License for more details.'
' '
' You should have received a copy of the GNU General Public License'
' along with this program; if not, write to the Free Software'
' Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA'
''
'Please note that Scilab is released under the terms of the CeCILL license :'
'http://www.cecill.info/index.en.html'
''
];
f=mopen('license.txt','w'); mfprintf(f,'%s\n',txt); mclose(f);

txt=[
'README file for the toolbox '+name
''
'Please update this file to provide information about your toolbox:'
' * what it is doing'
' * the author'
' * a few word about the license'
''
'Add relevant source code (.sci files) in the directory '+name+'/macros'
'Document the source code using the templates provided by help_from_sci.'
'See help help_from_sci for more information.'
'Create .xml help files using the command (run from directory '+name+')'
'   help_from_sci(''macros'',''help/en_US'')'
'Build the toolbox using the command:'
'   exec builder.sce'
'Finally load the toolbox using the command:'
'   exec loader.sce'
];
f=mopen('readme.txt','w'); mfprintf(f,'%s\n',txt); mclose(f);

if c_macros then
  mkdir('macros');
  copyfile(SCI+'/contrib/toolbox_skeleton/macros/buildmacros.sce','macros');
  printf(' ..creating %s/macros for Scilab function definitions (.sci files)\n',name);
end

if c_src then
  mkdir('src');
  copyfile(SCI+'/contrib/toolbox_skeleton/src/builder_src.sce','src');
  mkdir('src/c');
  printf(' ..creating %s/src/c for .c source code\n',name);
  txt=[
  '// Created by toolbox_creator'
  '// cfiles and ofiles may be also be manually defined'
  '// see SCI/contrib/toolbox_skeleton/src/c/builder_c.sce'
  'files=dir(''*.c'');' 
  'cfiles=strsubst(files.name,''.c'','''');'
  'ofiles=strsubst(files.name,''.c'',''.o'');'
  'tbx_build_src(cfiles, ofiles, ''c'',get_absolute_file_path(''builder_c.sce''));'
  'clear tbx_build_src, files, cfiles, ofiles;'
  ''];
  f=mopen('src/c/builder_c.sce','w'); mfprintf(f,'%s\n',txt); mclose(f);

  mkdir('src/fortran');
  printf(' ..creating %s/src/fortran for .f source code\n',name);
  txt=[
  '// Created by toolbox_creator'
  '// ffiles and ofiles may be also be manually defined'
  '// see SCI/contrib/toolbox_skeleton/src/fortran/builder_fortran.sce'
  'files=dir(''*.f'');' 
  'ffiles=strsubst(files.name,''.f'','''');'
  'ofiles=strsubst(files.name,''.f'',''.o'');'
  'tbx_build_src(ffiles, ofiles, ''f'',get_absolute_file_path(''builder_fortran.sce''));'
  'clear tbx_build_src, files, ffiles, ofiles;'
  ''];
  f=mopen('src/fortran/builder_fortran.sce','w'); mfprintf(f,'%s\n',txt); mclose(f);
end

if c_sci_gw then
  mkdir('sci_gateway');
  copyfile(SCI+'/contrib/toolbox_skeleton/sci_gateway/builder_gateway.sce','sci_gateway');
  
  mkdir('sci_gateway/c');
  copyfile(SCI+'/contrib/toolbox_skeleton/sci_gateway/c/builder_gateway_c.sce','sci_gateway/c');
  printf(' ..creating %s/sci_gateway/c for .c source code\n',name);

  mkdir('sci_gateway/cpp');
  copyfile(SCI+'/contrib/toolbox_skeleton/sci_gateway/cpp/builder_gateway_cpp.sce','sci_gateway/cpp');
  printf(' ..creating %s/sci_gateway/cpp for .cpp source code\n',name);

  mkdir('sci_gateway/fortran');
  copyfile(SCI+'/contrib/toolbox_skeleton/sci_gateway/fortran/builder_gateway_fortran.sce','sci_gateway/fortran');
  printf(' ..creating %s/sci_gateway/fortran for .f source code\n',name);
end

if c_help then
  mkdir('help');
  txt=[
  'help_dir = get_absolute_file_path(''builder_help.sce'');'
  'xml=dir(help_dir+''/en_US/*.xml''); xml=xml.name;'
  'if ~isempty(xml) then tbx_builder_help_lang(""en_US"", help_dir); end'
  'xml=dir(help_dir+''/en_US/*.xml''); xml=xml.name;'
  'if ~isempty(xml) then tbx_builder_help_lang(""fr_FR"", help_dir); end'
  ''
  'clear help_dir;'
  ]
  f=mopen('help/builder_help.sce','w'); mfprintf(f,'%s\n',txt); mclose(f);
  mkdir('help/en_US');
  printf(' ..creating %s/help/en_US for english .xml help files\n',name);
  txt=[
  'help_lang_dir = get_absolute_file_path(''build_help.sce'');'
  'f=dir(''*.xml'');'
  'if ~isempty(f.name) then tbx_build_help(TOOLBOX_TITLE, help_lang_dir); end'
  'clear help_lang_dir;'
  ]
  f=mopen('help/en_US/build_help.sce','w'); mfprintf(f,'%s\n',txt); mclose(f);
  mkdir('help/fr_FR');
  printf(' ..creating %s/help/en_US for french .xml help files\n',name);
  f=mopen('help/fr_FR/build_help.sce','w'); mfprintf(f,'%s\n',txt); mclose(f);
end

mkdir('etc');
printf(' ..creating %s/etc\n',name);
start=[
'mode(-1);'
'lines(0);'
'// ===================================================================='
'// Created by toolbox_creator'
'// ===================================================================='
'warning_mode = warning(''query''); warning(''off'');'
''];
start=[start;'disp(''Start '+name+''');'];

start=[start;'etc_tlbx = get_absolute_file_path('''+name+'.start'');'];
start=[start;
'etc_tlbx = getshortpathname(etc_tlbx);'
'root_tlbx = strncpy( etc_tlbx, length(etc_tlbx)-length(''\etc\'') );'
'pathmacros = pathconvert( root_tlbx ) + ''macros''+ filesep();'
''
'disp(''Load macros'');'
'//Load  functions library'
name+'lib = lib(pathmacros);'
''];
if c_sci_gw then
  start=[start;
  '// load gateways'
  'disp(''Load gateways'');'
  ''
  '// load gateways'
  'exec( root_tlbx + ''/sci_gateway/loader_gateway.sce'' );'
  ];
end

if c_help then
  start=[start;
  'disp(''Load help'');'
  ''
  '//add help chapter'
  'path_addchapter = root_tlbx + ""/jar/"";'
  'if ( fileinfo(path_addchapter) <> [] ) then'
  '  add_help_chapter('''+help_name+''', path_addchapter, %F);'
  '  clear add_help_chapter;'
  'end'];
end

if c_demos then
  start=[start;  
  '// ===================================================================='
  '// add demos'
  'pathdemos = pathconvert(root_tlbx+""/demos/'+name+'.dem.gateway.sce"",%f,%t);'
  'add_demo(""'+help_name+'"",pathdemos);'
  'clear pathdemos ;'
  ];
end
start=[start;  
'warning(warning_mode);'
'// ===================================================================='
'clear warning_mode;'
'if exists(''path_addchapter'') then clear path_addchapter; end'
'clear root_tlbx;'
'clear etc_tlbx;'
'if exists(''pathmacros'') then clear pathmacros; end'
'if exists(''pathconvert'') then clear pathconvert; end'
'// ===================================================================='
];
f=mopen('etc/'+name+'.start','w'); mfprintf(f,'%s\n',start); mclose(f);


if c_tests then
  mkdir('tests');
  printf(' ..creating %s/tests for unit test files (.sce)\n',name);
end

if c_demos then
  mkdir('demos');
  printf(' ..creating %s/demos for demo files (*.dem.sce)\n',name);
  txt=[
  '// ===================================================================='
  '// Created by toolbox_creator'
  '// See SCI/contrib/toolbox_skeleton/demos/toolbox_skeleton.dem.gateway.sce'
  '// for an example on how to define subdemolist manually.'
  '// ===================================================================='
  'demopath = get_absolute_file_path(""'+name+'.dem.gateway.sce"");'
  ''
  'f=dir(''*.sce''); f=f.name;' 
  'i=find(f<>name+''.dem.gateway.sce''; f=f(i);'
  'subdemolist=[''demo ''+strsubst(f,''.dem.sce'','''') f];'
  'subdemolist(:,2) = demopath + subdemolist(:,2);'
  '// ===================================================================='
  ];
  f=mopen('demos/'+name+'.dem.gateway.sce','w'); mfprintf(f,'%s\n',txt); mclose(f);  
end
chdir('..\');

if c_help then
  printf('Run help_from_sci() to create a scilab function template with\n');
  printf('documentation in the .sci source files to be saved in %s/macros.\n',name);
  printf('See help help_from_sci for more information.\n');
  printf('Run help_from_sci(''%s/macros'',''help/en_US'') to generate .xml help files\n',name);
  printf('from the .sci source code.\n');
end
printf('Run exec builder.sce from the directory %s, to build the toolbox.\n',name);
printf('Run exec loader.sce from the directory %s, to load the toolbox.\n',name);
endfunction

