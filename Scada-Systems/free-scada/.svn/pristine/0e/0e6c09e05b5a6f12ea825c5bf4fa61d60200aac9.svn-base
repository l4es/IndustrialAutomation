import os
import os.path
import fnmatch
import shutil

class prj_source:
    m_source_dir = ""
    m_folders = []
    m_excludes = []
    def __init__(self, source_dir, folders, excludes):
        self.m_source_dir = source_dir
        self.m_folders = folders
        self.m_excludes = excludes
        
    def copy_to(self, target):
        # Prepare file list
        print "Searching..."
        file_list=[]
        for fldr in self.m_folders:
            search_dir = os.path.normpath(os.path.join(self.m_source_dir, fldr))
            print "Current folder: ",search_dir
            file_list += self.search_files(search_dir,fldr)
        print "\nSearch completed. Copying..."
        
        shutil.rmtree(target, True)
        for fldr in file_list:
            src_file = os.path.normpath(os.path.join(self.m_source_dir, fldr))
            dst_file = os.path.normpath(os.path.join(target, fldr))
            (target_dir, tmp) = os.path.split(dst_file)
            if not os.path.isdir(target_dir):
                os.makedirs(target_dir)
            print dst_file
            shutil.copy2(src_file, dst_file)
            os.chmod(dst_file, 0666)
        
        print "\nFiles copying completed."
            

    def search_files(self, top, prefix):
        top = os.path.normpath(top)
   
        file_list=[]
        for root, dirs, files in os.walk(top, topdown=False):
            for name in files:
                filename = os.path.normpath(os.path.join(root,name))
                filename = os.path.normpath(os.path.join(prefix,filename.replace(top,'').lstrip(os.sep)))
                if not self.its_exclude(filename):
                    file_list.append(filename)
        return file_list

    def its_exclude(self, filename):
        result = False;
        for pattern in self.m_excludes:
            if fnmatch.fnmatch(filename,pattern):
                return True
        return False;
