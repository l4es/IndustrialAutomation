# Subversion back-end code for qct
#
# Copyright 2007 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

# Usage Notes:
#
# * Assumes you have a working svn command line tool
# * Assumes there is no passphrase required for rsh/ssh access

from qctlib.utils import runProgram, isBinary
from tempfile import mkstemp
import os

class qctVcsSvn:
    def initRepo(self, argv):
        '''Initialize your revision control system, open repository'''
        if not os.path.exists('.svn/'):
            print "No Subversion repository found"
            return -1

        self.svn_exe = 'svn'

        self.stateNames = { 'M' : 'modified',
                'R' : 'removed',
                '!' : 'missing',
                '?' : 'unknown' }
        return 0

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        return ( 'ignore', 'progressbar' )

    def addIgnoreMask(self, newIgnoreMask):
        '''The user has provided a new ignoreMask to be added to revision control'''
        existingIgnores = runProgram([self.svn_exe, 'propget', 'svn:ignore', '.']).split(os.linesep)
        existingIgnores.pop()
        existingIgnores.append(newIgnoreMask)
        (fd, filename) = mkstemp()
        file = os.fdopen(fd, "w+b")
        file.write(os.linesep.join(existingIgnores))
        file.close()
        runProgram([self.svn_exe, 'propset', '--file', filename, 'svn:ignore', '.'])

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        runProgram([self.svn_exe, 'revert', workingFile])

    def getLogTemplate(self):
        '''Request default log message template from VCS'''
        logFileName = os.path.expanduser('~/.commit.template')
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
        if self.wdDiffCache.has_key(fileName):
            del self.wdDiffCache[fileName]

    def scanFiles(self, showIgnored, pb = None):
        '''Request scan for all commitable files from VCS, with optional
           progress bar
        '''
        # Called at startup and when 'Refresh' button is pressed
        self.wdDiffCache = {}
        self.fileStatus = {}
        itemList = []

        if pb: pb.setValue(1)

        if showIgnored:  extra = ['--no-ignore']
        else:            extra = []
        versionOutput = runProgram([self.svn_exe, '--version', '--quiet'])
        version = [int(s) for s in versionOutput.split(os.linesep)[0].split('.')]
        statusOutput = runProgram([self.svn_exe, '--ignore-externals'] + extra + ['status'])
        recs = statusOutput.split(os.linesep)
        recs.pop() # remove last entry (which is '')

        if pb: pb.setValue(2)

        for line in recs:
            # a 7th column was added in svn 1.6, see http://subversion.tigris.org/svn_1.6_releasenotes.html#svn-status
            if version[0]>1 or version[0]==1 and version[1]>=6:
                if len(line) < 8:
                    continue
                status = line[0]
                fname = line[8:]
                self.fileStatus[ fname ] = line[0:7]
            else:
                if len(line) < 7:
                    continue
                status = line[0]
                fname = line[7:]
                self.fileStatus[ fname ] = line[0:6]
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
            elif status == 'C':  # conflict (allow edit)
                itemList.append('C ' + fname)
            elif status in ('~', 'X', 'R'):
                # Skip these files, they are not commitable
                pass

        if pb: pb.setValue(3)

        return itemList


    def __getWorkingDirChanges(self, fileName, type):
        if self.wdDiffCache.has_key(fileName):
            return self.wdDiffCache[fileName]

        # For revisioned files, we use cvs diff
        if type in ['A', 'M', 'R']:
            text = runProgram([self.svn_exe, 'diff', fileName], expectedexits=[0,1])
            self.wdDiffCache[fileName] = text
            return text
        elif type in ['!']:  # Missing files can be retrieved with cat
            text = runProgram([self.svn_exe, 'cat', fileName], expectedexits=[0,1])
            self.wdDiffCache[fileName] = text
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
            self.wdDiffCache[fileName] = text
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
            note = bFileName + " has been added to cvs, but has never been commited."
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
            note = bFileName + " was tracked but is now missing. If commited, it will be marked as removed in cvs."
            note += noteLineSep + "The file can be recovered by reverting it to it's last revisioned state."
            return note + os.linesep + "= Contents of Missing File" + os.linesep + text
        else:
            return "Unknown file type " + type


    def commitFiles(self, selectedFileList, logMsgText):
        '''Commit selected files'''
        # Files in list are annotated (A, M, etc) so this function can
        # mark files for add or delete as necessary before instigating the commit.
        commitFileNames = []
        dirList = []
        addFileList = []
        binaryAddFileList = []
        removeFileList = []
        for f in selectedFileList:
            type = f[0]
            fileName = f[2:]
            commitFileNames.append(fileName)
            if type in ['?', 'I']:
                if os.path.isdir(fileName):
                    dirList.append(fileName)
                elif isBinary(fileName):
                    binaryAddFileList.append(fileName)
                else:
                    addFileList.append(fileName)
            elif type == '!':
                removeFileList.append(fileName)

        if dirList:
            # Sort added directories by name length, to avoid recursion problems
            dirList.sort(lambda x,y: cmp(len(x), len(y)))
            runProgram([self.svn_exe, 'add', '--non-recursive'] + dirList)
            print "Added %d directory(s) to revision control: %s" % (len(dirList), ', '.join(dirList))

        if binaryAddFileList:
            runProgram([self.svn_exe, 'add'] + addFileList)
            print "Added %d binary file(s) to revision control: %s" % (len(addFileList), ', '.join(addFileList))

        if addFileList:
            runProgram([self.svn_exe, 'add'] + addFileList)
            print "Added %d file(s) to revision control: %s" % (len(addFileList), ', '.join(addFileList))

        if removeFileList:
            runProgram([self.svn_exe, 'delete'] + removeFileList)
            print "Removed %d file(s) from revision control: %s" % (len(removeFileList), ', '.join(removeFileList))

        (fd, filename) = mkstemp()
        file = os.fdopen(fd, "w+b")
        file.write(logMsgText)
        file.close()
        runProgram([self.svn_exe, 'commit', '-F', filename] + commitFileNames)
        print "%d file(s) commited: %s" % (len(commitFileNames), ', '.join(commitFileNames))

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        runProgram([self.svn_exe, 'add'] + selectedFileList)

    def revertFiles(self, selectedFileList):
        '''Revert selected files to last revisioned state'''
        revertFileNames = []
        for f in selectedFileList:
            type = f[0]
            fileName = f[2:]
            if type in ['R', '!', 'M']:
                prevState = self.stateNames[type]
                print "%s recovered to last revisioned state (was %s)" % (fileName, prevState)
                revertFileNames.append(fileName)
            elif type == 'A':
                print "%s removed from revision control (was added)" % fileName
                revertFileNames.append(fileName)
            else:
                print "File %s not reverted" % fileName

        if revertFileNames:
            runProgram([self.svn_exe, 'revert'] + revertFileNames)
        else:
            print "No revertable files"

# vim: tw=120
