import sys
import prj_source
import vs_builder
import nsis_builder

def copy_sources(source_location, target):
    source_folders = [    "installator",
                          "source",
                          "bin\\opc",
                          "bin\\redist",
                          "documentation"]
    
    exclude_list = [    "vcproj.*.user",
                        "*.pch",
                        "*.pchi",
                        "*.obj",
                        "*.ncb",
                        "*.suo",
                        "*/Debug/*",
                        "*/Release/*",
                        "*/.svn/*",
                        ".svn/*"]
    project = prj_source.prj_source(project_source, source_folders, exclude_list)
    project.copy_to('c:\\build\\free-scada')

    
project_source = '\\\\michael\\hdd\\Prog\\Projects\\free-scada\\trunk'
local_source = 'c:\\build\\free-scada'
copy_sources(project_source, local_source)

if vs_builder.build_project(local_source+'\\source\\allprojects.sln') == False:
    print "Build failed"
    sys.exit(-1)

#######################################################
version="0.08b"
#######################################################

if nsis_builder.build_package(local_source+"\\installator",version) == False:
    print "Build failed"
    sys.exit(-1)
    
print "Build succesed!"