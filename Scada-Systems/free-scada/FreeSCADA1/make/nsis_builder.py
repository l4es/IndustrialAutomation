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


def build_package(script_folder, version):
    target_dir = os.path.normpath(os.path.join(script_folder,"..\\bin\\distributive"))
    if not os.path.isdir(target_dir): os.makedirs(target_dir)
    
    cmd_string = "cd \""+script_folder+"\" & "
    cmd_string += "makensis.exe /V3 /X\"SetCompressor /solid lzma\" !Main.nsi"
    if SysCall(cmd_string) == False:
        return False
    
    cmd_string = "cd \""+script_folder+"\" & "
    cmd_string += "winrar a ..\\bin\\distributive\\free-scada-src @src_add.lst -x@src_excl.lst -m5 -ed -dh -s -mc63:128t+ -t"
    if SysCall(cmd_string) == False:
        return False

    cmd_string = "cd \""+script_folder+"\" & "
    cmd_string += "winrar a ..\\bin\\distributive\\free-scada-pdb @src_add_pdb.lst -x@src_excl_pdb.lst -m5 -ed -dh -s -t"
    if SysCall(cmd_string) == False:
        return False
    
    target_file = os.path.join(target_dir,"free-scada_"+version+".exe")
    os.rename(os.path.join(target_dir,"free-scada.exe"),target_file)
    
    target_file = os.path.join(target_dir,"free-scada-src_"+version+".rar")
    os.rename(os.path.join(target_dir,"free-scada-src.rar"),target_file)

    target_file = os.path.join(target_dir,"free-scada-pdb_"+version+".rar")
    os.rename(os.path.join(target_dir,"free-scada-pdb.rar"),target_file)

    return True
