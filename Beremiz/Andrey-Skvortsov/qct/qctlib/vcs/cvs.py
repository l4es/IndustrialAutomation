# CVS back-end code for qct
#
# Copyright 2007 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

# Usage Notes:
#
# * Assumes you have a working cvs command line tool
# * Assumes you have CVSROOT setup appropriately
# * Assumes there is no passphrase required for rsh/ssh access
# * Does not support listing ignored files (cvs seems broken in this regard)

from qctlib.utils import runProgram, isBinary
from tempfile import mkstemp
import os

class qctVcsCvs:
    def initRepo(self, argv):
        '''Initialize your revision control system, open repository'''
        if not os.path.exists('CVS/'):
            print "No CVS repository found"
            return -1

        self.stateNames = { 'M' : 'modified',
                'R' : 'removed',
                '!' : 'missing',
                '?' : 'unknown' }
        return 0

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        return [ 'ignore', 'progressbar' ]

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        runProgram(['cvs', 'update', '-C', workingFile])

    def addIgnoreMask(self, newIgnoreMask):
        '''The user has provided a new ignoreMask to be added to revision control'''
        try:
            f = open('.cvsignore', 'a')
            f.write(newIgnoreMask)
            f.write('\n')
            f.close()
            print "Added '%s' to ignore mask" % newIgnoreMask
        except IOError, e:
            print "Unable to add '%s' to ignore mask" % newIgnoreMask
            print e

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
        itemList = []

        if pb: pb.setValue(1)

        # Provides ARM, same as diff, plus unknown ? and missing !
        statusOutput = runProgram(['cvs', '-qn', 'update'])
        recs = statusOutput.split(os.linesep)
        recs.pop() # remove last entry (which is '')

        if pb: pb.setValue(2)

        nextFileMissing = False
        for line in recs:
            status = line[0]
            fname = line[2:]
            if line.endswith("' was lost"):
                nextFileMissing = True
            elif nextFileMissing:
                itemList.append('! ' + fname)
                nextFileMissing = False
            elif status not in ['U', 'D', 'P']:
                itemList.append(line)

        if pb: pb.setValue(3)

        return itemList


    def __getWorkingDirChanges(self, fileName, type):
        if self.wdDiffCache.has_key(fileName):
            return self.wdDiffCache[fileName]

        # For revisioned files, we use cvs diff
        if type in ['A', 'M']:
            text = runProgram(['cvs', 'diff', '-du', fileName], expectedexits=[0,1])
            self.wdDiffCache[fileName] = text
            return text

        if type in ['R', '!']:
            text = 'Deleted file, unable to retrieve contents' # TODO
            self.wdDiffCache[fileName] = text
            return text

        # For unrevisioned files, we return file contents
        if type in ['?', 'I']:
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
        addFileList = []
        binaryAddFileList = []
        removeFileList = []
        for f in selectedFileList:
            type = f[0]
            fileName = f[2:]
            commitFileNames.append(fileName)
            if type in ['?', 'I']:
                if isBinary(fileName):
                    binaryAddFileList.append(fileName)
                else:
                    addFileList.append(fileName)
            elif type == '!': removeFileList.append(fileName)

        if binaryAddFileList:
            runProgram(['cvs', 'add', '-kb'] + addFileList)
            print "Added %d binary file(s) to revision control: %s" % (len(addFileList), ', '.join(addFileList))

        if addFileList:
            runProgram(['cvs', 'add'] + addFileList)
            print "Added %d file(s) to revision control: %s" % (len(addFileList), ', '.join(addFileList))

        if removeFileList:
            runProgram(['cvs', 'delete'] + removeFileList)
            print "Removed %d file(s) from revision control: %s" % (len(removeFileList), ', '.join(removeFileList))

        (fd, filename) = mkstemp()
        file = os.fdopen(fd, "w+b")
        file.write(logMsgText)
        file.close()
        runProgram(['cvs', 'commit', '-F', filename] + commitFileNames)
        print "%d file(s) commited: %s" % (len(commitFileNames), ', '.join(commitFileNames))

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        runProgram(['cvs', 'add'] + selectedFileList)

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
            runProgram(['cvs', 'update', '-C'] + revertFileNames)
        else:
            print "No revertable files"

# vim: tw=120
