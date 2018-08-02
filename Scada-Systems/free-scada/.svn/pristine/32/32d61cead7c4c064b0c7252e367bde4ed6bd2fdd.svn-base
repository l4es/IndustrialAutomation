import os
import os.path
import subprocess
import sys

def SysCall (cmdStr):
    currentDir = os.getcwd ()
    print currentDir + '> ' + cmdStr
    sys.stdout.flush ()
    try: 
        proc = subprocess.Popen (cmdStr, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
        while (proc.poll () == None):
            sys.stdout.write (proc.stdout.readline ())
        sys.stdout.write (proc.stdout.read ())
        print ''
        ret = proc.returncode
        if ret:
            print 'Error: The command "' + cmdStr + '" returned ' + str (ret)
            return False
        else:
            return True
    except:
        print 'Error: exception encountered', sys.exc_type, sys.exc_value
        return False


def build_project(vs_solution_file):
    VSINSTALLDIR = ""
    if 'VSINSTALLDIR' in os.environ:
        VSINSTALLDIR = os.environ['VSINSTALLDIR']
    else:
        VSINSTALLDIR="C:\\Program Files\\Microsoft Visual Studio 8"
    
    SDK="C:\\build\\sdk"
    PSDK=SDK+"\\psdk"
    
    path = VSINSTALLDIR+"\\Common7\\IDE" +";"+ \
           VSINSTALLDIR+"\\VC\\BIN" +";"+ \
           VSINSTALLDIR+"\\Common7\\Tools" +";"+ \
           VSINSTALLDIR+"\\VC\\VCPackages" +";"+ \
           PSDK+"\\bin"
    if 'PATH' in os.environ:    path += ";"+os.environ['PATH']
    os.environ['PATH']=path;
    
    include =   VSINSTALLDIR+"\\VC\\INCLUDE" +";"+ \
                SDK+"\\boost_1_33_1" +";"+ \
                SDK+"\\atl80\\include" +";"+ \
                PSDK+"\\include"
    if 'INCLUDE' in os.environ:    path += ";"+os.environ['INCLUDE']
    os.environ['INCLUDE']=include;
    
    lib =    VSINSTALLDIR+"\\VC\\LIB" +";"+ \
             PSDK+"\\lib"  +";"+ \
             SDK+"\\atl80\\lib"
    if 'LIB' in os.environ:    path += ";"+os.environ['LIB']
    os.environ['LIB']=lib;

    sub_projects = ["ArchiverService",
                    "Designer",
                    "MultyConfigurator",
                    "ReportGenerator",
                    "Visualizator"]
    
    log = "C:\\build\\free-scada\\logs"
    if not os.path.isdir(log): os.makedirs(log)
    log += "\\release_build.txt"
    
    (solution_dir, tmp) = os.path.split(vs_solution_file);
    cmd_string = "VCExpress "+vs_solution_file+" /Rebuild Release "
#    cmd_string = "devenv "+vs_solution_file+" /Rebuild Release "
    cmd_string +="/Out \""+log+"\" /useenv"
    
    return SysCall(cmd_string)
