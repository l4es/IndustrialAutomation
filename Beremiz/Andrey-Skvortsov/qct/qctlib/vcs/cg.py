# Cogito VCS back-end code for qct
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

from qctlib.utils import runProgram, isBinary
from tempfile import mkstemp
import os

class qctVcsCg:
    def initRepo(self, argv):
        '''Initialize your revision control system, open repository'''
        self.stateNames = { 'M' : 'modified',
                'R' : 'removed',
                '!' : 'missing',
                '?' : 'unknown' }
        self.repoRoot = self.detect_root()
        if not self.repoRoot:
            print 'No Git/Cogito repository found'
            return -1
        return 0

    def detect_root(self):
        path = os.getcwd()
        while path != '/':
            if os.path.exists(os.path.join(path, ".git")):
                return path
            path = os.path.dirname(path)
        return None

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        return ('ignore', 'progressbar')

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        runProgram(['cg', 'restore', '-f', workingFile])

    def addIgnoreMask(self, newIgnoreMask):
        '''The user has provided a new ignoreMask to be added to revision control'''
        try:
            f = open(os.path.join(self.repoRoot, ".gitignore"), 'a')
            f.write(newIgnoreMask)
            f.write('\n')
            f.close()
            print "Added '%s' to ignore mask" % newIgnoreMask
        except IOError, e:
            print "Unable to add '%s' to ignore mask" % newIgnoreMask
            print e

    def getLogTemplate(self):
        '''Request default log message template from VCS'''
        logFileName = self.repoRoot + "/.commit.template"
        try:
            f = open(logFileName)
            text = f.read()
            f.close()
            return text
        except IOError:
            return ''

    def getAutoSelectTypes(self):
        '''Return annotations of file types which should be automatically
           selected when a new commit is started'''
        return ['A', 'M', 'R']

    def dirtyCache(self, fileName):
        '''The GUI would like us to forget cached data for this file'''
        for itemName in self.diffCache.keys():
            if itemName[2:] == fileName:
                del self.diffCache[itemName]

    def scanFiles(self, showIgnored, pb = None):
        '''Request scan for all commitable files from VCS'''
        # Called at startup and when 'Refresh' button is pressed
        self.diffCache = {}
        itemList = []

        if pb: pb.setValue(1)
        if showIgnored:  extra = ['-x']
        else:            extra = []
        recs = runProgram(['cg', 'status'] + extra).split(os.linesep)
        recs.pop() # remove eoln
        if pb: pb.setValue(2)

        # Skip header lines by looking for blank line
        lookingForBlank = True
        for line in recs:
            if lookingForBlank:
                if not line: lookingForBlank = False
                continue
            status = line[0]
            fname = line[2:]
            if status == 'M':  # modified
                itemList.append('M ' + fname)
            elif status == 'A':  # added
                itemList.append('A ' + fname)
            elif status == '?':    # unknown
                itemList.append('? ' + fname)
            elif status == '!':  # missing
                itemList.append('! ' + fname)
            elif status == 'D':  # deleted
                itemList.append('R ' + fname)
            elif status in ('m'):
                # Skip these files, they are not commitable
                pass
            else:
                print "Cogito returned unexpected status %s" % status
        if pb: pb.setValue(3)
        return itemList


    def __getWorkingDirChanges(self, fileName, type):
        if self.diffCache.has_key(fileName):
            return self.diffCache[fileName]

        # For revisioned files, we use cg diff
        if type in ['A', 'M', 'R']:
            os.environ['PAGER'] = 'cat'
            text = runProgram(['cg', 'diff', fileName], expectedexits=[0,1])
            self.diffCache[fileName] = text
            return text
        elif type in ['!']:  # Missing files can be retrieved with cat
            text = runProgram(['cg', 'admin-cat', fileName], expectedexits=[0,1])
            self.diffCache[fileName] = text
            return text
        elif type in ['?', 'I']:
            # For unrevisioned files, we return file contents
            if os.path.isdir(fileName):
                text = " <Unrevisioned Directory>"
                fnames = os.listdir(fileName)
                text += os.linesep + ' ' + '\n '.join(fnames)
            elif isBinary(fileName):
                text = " <Binary File>"
            else:
                f = open(fileName)
                text = f.read()
                f.close()
            self.diffCache[fileName] = text
            return text
        else:
            return "Unknown file type " + type


    def getFileStatus(self, itemName):
        '''Request file deltas from VCS'''
        type = itemName[0]
        fileName = itemName[2:]

        text = self.__getWorkingDirChanges(fileName, type)

        # Useful shorthand vars.  Leading lines beginning with % are treated as RTF
        bFileName = "%<b>" + fileName + "</b>"
        noteLineSep = os.linesep + '%'

        if type == 'A':
            note = bFileName + " has been added to git, but has never been commited."
            return note + os.linesep + text
        elif type == 'M':
            note = bFileName + " has been modified in your working directory."
            return note + os.linesep + text
        elif type == '?':
            note = bFileName + " is not currently tracked. If commited, it will be added to revision control."
            return note + os.linesep + "= Unrevisioned File Contents" + os.linesep + text
        elif type == 'I':
            note = bFileName + " is usually ignored, but will be added to revision control if commited"
            return note + os.linesep + text
        elif type == 'R':
            note = bFileName + " has been marked for deletion, or renamed, but has not yet been commited"
            note += noteLineSep + "The file can be recovered by reverting it to it's last revisioned state."
            return note + os.linesep + "= Removed File Diffs" + os.linesep + text
        elif type == '!':
            note = bFileName + " was tracked but is now missing. If commited, it will be marked as removed in git."
            note += noteLineSep + "The file can be recovered by reverting it to it's last revisioned state."
            return note + os.linesep + "= Contents of Missing File" + os.linesep + text
        else:
            return "Unknown file type " + type

    def commitFiles(self, selectedFileList, logMsgText):
        '''Commit selected files'''
        # Files in list are annotated (A, M, etc) so this function can
        # mark files for add or delete as necessary before instigating the commit.

        commitFileNames = []
        renameFiles = []
        for f in selectedFileList:
            annotation = f[0]
            fileName = f[2:]
            commitFileNames.append(fileName)
            if annotation in ('?', 'I'):
                runProgram(['cg', 'add', fileName])
            elif type == '!': removeFileList.append(fileName)

        (fd, filename) = mkstemp()
        file = os.fdopen(fd, "w+b")
        file.write(logMsgText)
        file.close()
        runProgram(['cg', 'commit', '-M', filename] + commitFileNames)
        print "%d file(s) commited: %s" % (len(selectedFileList), ', '.join(commitFileNames))
        return

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        runProgram(['cg', 'add'] + selectedFileList)

    def revertFiles(self, selectedFileList):
        '''Revert selected files to last revisioned state'''
        revertFileNames = []
        for f in selectedFileList:
            annotation = f[0]
            fileName = f[2:]
            if annotation in ['R', '!']:
                print "deleted %s recovered from revision control" % fileName
                revertFileNames.append(fileName)
            elif annotation == '>':
                print "rename %s recovered from revision control" % fileName
                renameFiles = fileName.split(' => ')
                revertFileNames.append(renameFiles[0])
            elif annotation == 'A':
                print "added %s forgot from revision control" % fileName
                revertFileNames.append(fileName)
            elif annotation == 'M':
                print "modifications to %s reverted" % fileName
                revertFileNames.append(fileName)
            else:
                print "File %s not reverted" % fileName

        if len(revertFileNames):
            runProgram(['cg', 'restore', '-f'] + revertFileNames)
            print "%d file(s) reverted: %s" % (len(revertFileNames), ', '.join(revertFileNames))
        else:
            print "No revertable files"
        return

# vim: tw=120
