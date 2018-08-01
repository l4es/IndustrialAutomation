# Bazaar VCS back-end code for qct
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

from qctlib.utils import runProgram
from tempfile import mkstemp
import os

class qctVcsBzr:
    def initRepo(self, argv):
        '''Initialize your revision control system, open repository'''
        # Verify we have a valid repository
        output = runProgram(['bzr', 'root'], expectedexits=[0,3])
        words = output.split(' ')
        if len(words) == 1:
            self.repoRoot = output[:-len(os.linesep)]
            return 0
        else:
            print output
            return -1

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        # bazaar does not seem to support file copies or marking moves
        # which were initially done outside of revision detection (using mv)
        return ('ignore', 'progressbar')

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        runProgram(['bzr', 'revert', '--no-backup', workingFile])

    def addIgnoreMask(self, newIgnoreMask):
        '''The user has provided a new ignoreMask to be added to revision control'''
        runProgram(['bzr', 'ignore', newIgnoreMask])
        print "Added '%s' to ignore mask" % newIgnoreMask

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
        return ['A', 'M', 'R', '>']

    def dirtyCache(self, fileName):
        '''The GUI would like us to forget cached data for this file'''
        for itemName in self.diffCache.keys():
            if itemName[2:] == fileName:
                del self.diffCache[itemName]

    def scanFiles(self, showIgnored, pb = None):
        '''Request scan for all commitable files from VCS'''
        # Called at startup, when 'Refresh' button is pressed, or when
        # showIgnored toggled.
        self.diffCache = { }

        statusOutput = runProgram(['bzr', 'status'])

        if pb: pb.setValue(1)

        recs = statusOutput.split(os.linesep)
        recs.pop() # remove last entry (which is '')
        itemList = []
        fileList = []
        type = ''
        for line in recs:
            if   line == 'added:':   type = 'A'
            elif line == 'modified:': type = 'M'
            elif line == 'unknown:':  type = '?'
            elif line == 'removed:':  type = 'R'
            elif line == 'renamed:':  type = '>'
            elif line[-1] == '/':
                line = '' # nop, just skipping this directory
            else:
                # Prevent files from showing up w/ two status
                if line[2:] not in fileList:
                    itemList.append(type + " " + line[2:])
                    fileList.append(line[2:])

        if pb: pb.setValue(2)

        if showIgnored:
            statusOutput = runProgram(['bzr', 'ls', '--ignored'])
            recs = statusOutput.split(os.linesep)
            recs.pop() # remove last entry (which is '')
            for fileName in recs:
                itemList.append("I " + fileName)

        if pb: pb.setValue(3)
        return itemList

    def getFileStatus(self, itemName):
        '''Request file deltas from VCS'''
        annotation = itemName[0]
        fileName = itemName[2:]
        bFileName = "%<b>" + fileName + "</b>"
        if annotation == 'A':
            note = bFileName + " has been added to VCS, but has never been commited"
            if self.diffCache.has_key(itemName):
                text = self.diffCache[itemName]
            else:
                text = runProgram(['bzr', 'diff', fileName], expectedexits=[0,1])[:-len(os.linesep)]
                self.diffCache[itemName] = text
            return note + os.linesep + text
        elif annotation == 'R':
            note = bFileName + " has been marked for deletion, but has not yet been commited"
            if self.diffCache.has_key(itemName):
                text = self.diffCache[itemName]
            else:
                text = runProgram(['bzr', 'diff', fileName], expectedexits=[0,1])[:-len(os.linesep)]
                self.diffCache[itemName] = text
            return note + os.linesep + text
        elif annotation == 'M':
            note = bFileName + " has been modified in your working directory"
            if self.diffCache.has_key(itemName):
                text = self.diffCache[itemName]
            else:
                text = runProgram(['bzr', 'diff', fileName], expectedexits=[0,1])[:-len(os.linesep)]
                self.diffCache[itemName] = text
            return note + os.linesep + text
        elif annotation == '?':
            note = bFileName + " is not currently revisioned, will be added to VCS if commited"
            if self.diffCache.has_key(itemName):
                text = self.diffCache[itemName]
            else:
                text = runProgram(['diff', '-u', '/dev/null', fileName], expectedexits=[0,1,2])
                if not text: text = "<empty file>"
                self.diffCache[itemName] = text
            return note + os.linesep + text
        elif annotation == 'I':
            note = bFileName + " is usually ignored, but will be added to VCS if commited" + os.linesep
            if self.diffCache.has_key(itemName):
                text = self.diffCache[itemName]
            else:
                text = runProgram(['diff', '-u', '/dev/null', fileName], expectedexits=[0,1,2])
                if not text: text = "<empty file>"
                self.diffCache[itemName] = text
            return note + os.linesep + text
        elif annotation == '>':
            return "%<b>Rename event:</b> " + fileName
        else:
            return "Unknown file type " + annotation

    def commitFiles(self, selectedFileList, logMsgText):
        '''Commit selected files'''
        # Files in list are annotated (A, M, etc) so this function can
        # mark files for add or delete as necessary before instigating the commit.

        commitFileNames = []
        renameFiles = []
        for f in selectedFileList:
            annotation = f[0]
            fileName = f[2:]
            if annotation == '?' or annotation == 'I':
                print "Adding %s to revision control" % fileName
                runProgram(['bzr', 'add', fileName])
                commitFileNames.append(fileName)
            elif annotation == '>':
                print "Commit rename: %s" % fileName
                renameFiles += fileName.split(' => ')
            else:
                commitFileNames.append(fileName)

        # Renamed files may be on the modified list as well, so we add
        # them at the end to prevent duplicates
        for f in renameFiles:
            if f not in commitFileNames:
                commitFileNames.append(f)

        (fd, filename) = mkstemp()
        file = os.fdopen(fd, "w+b")
        file.write(logMsgText)
        file.close()

        runProgram(['bzr', 'commit', '-F', filename] + commitFileNames)
        print "%d file(s) commited: %s" % (len(selectedFileList), ', '.join(commitFileNames))

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        runProgram(['bzr', 'add'] + selectedFileList)

    def revertFiles(self, selectedFileList):
        '''Revert selected files to last revisioned state'''
        revertFileNames = []
        for f in selectedFileList:
            annotation = f[0]
            fileName = f[2:]
            if annotation == 'R':
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
            runProgram(['bzr', 'revert'] + revertFileNames)
            print "%d file(s) reverted: %s" % (len(revertFileNames), ', '.join(revertFileNames))
        else:
            print "No revertable files"

# vim: tw=120
