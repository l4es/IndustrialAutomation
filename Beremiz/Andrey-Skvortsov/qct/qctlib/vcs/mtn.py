# Monotone VCS back-end code for qct
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

# Usage Notes:
#
# User must specify a get_passphrase() in their monotonerc in order for
# this back-end to commit files.

from qctlib.utils import runProgram, findInSystemPath, scanDiffOutput, isBinary
from tempfile import mkstemp
from stat import *
import os

class qctVcsMtn:
    def initRepo(self, argv):
        '''Initialize your revision control system, open repository'''

        self.mtn_exe = 'mtn'
        output = runProgram([self.mtn_exe, 'ls', 'unknown'], expectedexits=[0,1])
        if output.startswith('mtn: misuse: workspace required'):
            print "No Monotone repository found"
            return -1

        # Verify we have a valid repository
        self.stateNames = { 'M' : 'modified',
                'R' : 'removed',
                '!' : 'missing',
                '?' : 'unknown' }

        self.capList = [
                'ignore',      # VCS supports ignore masks (addIgnoreMask)
                'rename',      # VCS supports revisioned renames (fileMoveDetected)
                'progressbar'] # back-end supports a progress bar

        return 0

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        return self.capList

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        runProgram([self.mtn_exe, 'revert', workingFile])

    def addIgnoreMask(self, newIgnoreMask):
        '''The user has provided a new ignoreMask to be added to revision control
           Requires 'ignore' capability.
        '''
        # (probably too simple) Glob-to-Regexp
        regexp = '^'
        for char in newIgnoreMask:
            if char == '*':
                regexp += '.*'
            elif char == '.':
                regexp += '\.'
            else:
                regexp += char
        regexp += '$'

        if not os.path.exists('.mtn-ignore'):
            print 'No ignore file found, unable to add %s' % regexp
            return

        try:
            f = open('.mtn-ignore', 'a')
            f.write(regexp)
            f.write('\n')
            f.close()
            print "Added regexp '%s' to ignore mask" % regexp
        except IOError, e:
            print "Unable to add '%s' to ignore mask" % regexp
            print e
        pass

    def fileMoveDetected(self, origFileName, newFileName):
        '''User has associated an unknown file with a missing file, describing
           a move/rename which occurred outside of revision control
           Requires 'rename' capability
        '''
        runProgram([self.mtn_exe, 'rename', origFileName, newFileName])

    def getLogTemplate(self):
        '''Request default log message template from VCS'''
        return ''

    def getAutoSelectTypes(self):
        '''Return annotations of file types which should be automatically
           selected when a new commit is started'''
        return ['A', 'M', 'R', '>']

    def dirtyCache(self, fileName):
        '''The GUI would like us to forget cached data for this file'''
        if self.wdDiffCache.has_key(fileName):
            del self.wdDiffCache[fileName]

    def scanFiles(self, showIgnored, pb = None):
        '''Request scan for all commitable files from VCS, with optional
           progress bar
        '''
        # Called at startup, when 'Refresh' button is pressed, or when showIgnored toggled.
        self.wdDiffCache = {}

        inventory = runProgram([self.mtn_exe, 'automate', 'inventory']).split(os.linesep)[:-1]
        if pb: pb.setValue(1)

        self.fileState = {}
        itemList = []
        renameSrc = {}
        renameDest = {}
        listedFiles = []
        for line in inventory:
            state = line[0:3]
            srcRename = line[4]
            destRename = line[6]
            fileName = line[8:]

            if state == '   ':
                # Skip unchanged files
                continue

            self.fileState[fileName] = state

            if srcRename != '0':
                renameSrc[srcRename] = fileName
            if destRename != '0':
                renameDest[destRename] = fileName

            if state[2] == 'I':      # Ignored by lua-hook
                if showIgnored:
                    itemList.append('I ' + fileName)
                    listedFiles.append(fileName)
            elif state[2] == 'P':    # patched (modified)
                itemList.append('M ' + fileName)
                listedFiles.append(fileName)
            elif state[2] == 'U':    # Unknown
                if state[0] == 'D':  # Dropped
                    itemList.append('R ' + fileName)
                    listedFiles.append(fileName)
                else:
                    itemList.append('? ' + fileName)
                    listedFiles.append(fileName)
            elif state[2] == 'M':    # Missing
                itemList.append('! ' + fileName)
                listedFiles.append(fileName)
            elif state[2] == ' ':
                if state[1] == 'A':  # Added
                    itemList.append('A ' + fileName)
                    listedFiles.append(fileName)
                elif state[0] == 'D':
                    itemList.append('R ' + fileName)
                    listedFiles.append(fileName)
            elif state[0] == 'R' or state[1] == 'R':
                pass
            else:
                print '%s [%s] is uncharacterized!' % (fileName, state)

        if pb: pb.setValue(2)

        # Find rename pairs
        self.renameTarget = {}
        for k in renameSrc.keys():
            src = renameSrc[k]
            tgt = renameDest[k]
            self.renameTarget[src] = tgt
            if src not in listedFiles:
                itemList.append('> ' + src)
                listedFiles.append(src)

        if pb: pb.setValue(3)

        return itemList


    def __getWorkingDirChanges(self, fileName, type):
        if self.wdDiffCache.has_key(fileName):
            return self.wdDiffCache[fileName]

        # For symlinks, we return the link data
        if type not in ['R', '!', '>']:
            lmode = os.lstat(fileName)[ST_MODE]
            if S_ISLNK(lmode):
                text = "Symlink: %s -> %s" % (fileName, os.readlink(fileName))
                self.wdDiffCache[fileName] = text
                return text

        # For revisioned files, we use hg diff
        if type in ['A', 'M', 'R']:
            text = runProgram([self.mtn_exe, 'diff', fileName])
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

        if type == '>':
            target = self.renameTarget[fileName]
            text = 'Rename event: %s [%s] -> %s [%s]' % (fileName, self.fileState[fileName],
                            target, self.fileState[target])
            self.wdDiffCache[fileName] = text
            return text

        # For missing files, we use mtn cat
        if type == '!':
            if self.fileState[fileName][1] == 'A':
                text = " <Missing file was never revisioned>"
                self.wdDiffCache[fileName] = text
                return text
            text = runProgram([self.mtn_exe, 'cat', fileName])
            if not text: text = " <empty file>"
            elif '\0' in text: text = " <Binary File of %d KBytes>" % (len(text) / 1024)
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
        bFileName = '%' + '<b>%s</b> [%s]' % (fileName, self.fileState[fileName])
        noteLineSep = os.linesep + '%'

        if type == 'A':
            note = bFileName + " has been added to revision control, but has never been commited."
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
            note = bFileName + " has been marked for deletion, but has not yet been commited"
            note += noteLineSep + "The file can be recovered by reverting it to it's last revisioned state."
            return note + os.linesep + "= Removed File Diffs" + os.linesep + text
        elif type == '!':
            note = bFileName + " was tracked but is now missing. If commited, it will be marked as dropped."
            note += noteLineSep + "The file can be recovered by reverting it to it's last revisioned state."
            return note + os.linesep + "= Contents of Missing File" + os.linesep + text
        elif type == '>':
            return text
        else:
            return "Unknown file type " + type


    def commitFiles(self, selectedFileList, logMsgText):
        '''Commit selected files'''
        # Files in list are annotated (A, M, etc) so this function can
        # mark files for add or delete as necessary before instigating the commit.
        commitFileNames = []
        addFileList = []
        removeFileList = []
        for f in selectedFileList:
            type = f[0]
            fileName = f[2:]
            commitFileNames.append(fileName)
            if type in ['?', 'I']: addFileList.append(fileName)
            elif type == '!': removeFileList.append(fileName)

        if addFileList:
            runProgram([self.mtn_exe, 'add'] + addFileList)
            print "Added %d file(s) to revision control: %s" % (len(addFileList), ', '.join(addFileList))

        if removeFileList:
            runProgram([self.mtn_exe, 'drop'] + removeFileList)
            print "Removed %d file(s) from revision control: %s" % (len(removeFileList), ', '.join(removeFileList))

        (fd, filename) = mkstemp()
        file = os.fdopen(fd, "w+b")
        file.write(logMsgText)
        file.close()
        runProgram([self.mtn_exe, 'commit', '--message-file', filename] + commitFileNames)
        print "%d file(s) commited: %s" % (len(commitFileNames), ', '.join(commitFileNames))

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        runProgram([self.mtn_exe, 'add'] + selectedFileList)

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
            elif type == '>':
                runProgram([self.mtn_exe, 'revert', fileName])
                targetName = self.renameTarget[ fileName ]
                os.unlink(targetName)
                print "Rename of %s reverted, %s removed" % (fileName, targetName)
            else:
                print "File %s not reverted" % fileName

        if revertFileNames:
            runProgram([self.mtn_exe, 'revert'] + revertFileNames)

# vim: tw=120
