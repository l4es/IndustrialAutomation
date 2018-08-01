# Mercurial VCS back-end code for qct
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

import os, sys
from stat import *
from qctlib.utils import *
from tempfile import mkstemp
from StringIO import StringIO
from mercurial.extensions import enabled

HAS_COLOR = "color" in enabled()[0]

def findHg():
    path = os.environ["PATH"].split(os.pathsep)
    for d in path:
        if os.name == "nt":
            pathexts = os.environ["PATHEXT"].split(os.pathsep)
            for ext in pathexts:
                exepath = os.path.join(d, 'hg' + ext)
                if os.access(exepath, os.X_OK):
                    try:
                        runProgram([exepath, 'version'])
                        return exepath
                    except:
                        pass
        else:
            exepath = os.path.join(d, 'hg')
            if os.access(exepath, os.X_OK):
                try:
                    runProgram([exepath, 'version'])
                    return exepath
                except:
                    pass
    return None

class qctVcsHg:
    def initRepo(self, argv, hgdispatch=None, username=None):
        '''Initialize your revision control system, open repository'''

        self.commitname = username

        # If we're running from the qct.py extension, we can call directly into mercurial
        if hgdispatch:
            self.commands = hgdispatch
        else:
            self.commands = None
            self.hg_exe = findHg()
            if not self.hg_exe:
                print "Unable to find hg (.exe, .bat, .cmd) in your path"
                return -1

        # Verify we have a valid repository
        (out, err) = self.hgcmd(['root'], okresults=[0,255])
        self.repoRoot = out.split('\n')[0]
        if err.startswith('abort'):
            print err
            return -1

        self.parents = self.hgcmd(['parents', '-q'])[0].splitlines()

        if os.getcwd() == self.repoRoot:
            self.runningFromRoot = True
        else:
            self.runningFromRoot = False

        self.stateNames = { 'M' : 'modified',
                'R' : 'removed',
                '!' : 'missing',
                '?' : 'unknown' }

        self.capList = [
                'ignore',      # VCS supports ignore masks (addIgnoreMask)
                'copy',        # VCS supports revisioned copying of files (fileMoveDetected)
                'rename',      # VCS supports revisioned renames (fileMoveDetected)
                'symlink',     # VCS supports symlinks
                'patchqueue',  # VCS supports patch queue (isPatchQueue, topPatchName)
                'progressbar', # back-end supports a progress bar
                'merge']       # VCS supports merges (isMerge)

        self.cmdLineOptions = []

        # To enable an auto-matic sign-off message:
        # [qct]
        # signoff = Sign-Off: Steve Borho
        try:
            self.signOff = self.hgcmd(['showconfig', 'qct.signoff'])[0]
        except ProgramError:
            self.signOff = None
            pass
        
        # Determine if this repository has any applied Mercurial Queue patches
        (output, err) = self.hgcmd(['qheader'], okresults=[0,1,255])
        if err and "unknown command" in err:
            self.isPatchQ = False
            return 0
        if output and 'o patches applied' in output:
            self.isPatchQ = False
            return 0

        # Patches only make sense from repository root
        if not self.runningFromRoot:
            print "Changing context to repository root: " + self.repoRoot
            os.chdir(self.repoRoot)
            self.runningFromRoot = True
        self.isPatchQ = True
        return 0

    NO_COLOR = set(["status", "diff", "qseries"])
    
    def hgcmd(self, args, okresults=[0], binary=False):
        if self.commands:
            if HAS_COLOR and args[0] in self.NO_COLOR:
                args.append("--color=never")
            # Use mercurial library directly
            ostream = StringIO()
            errstream = StringIO()
            ret = None
            try:
                sys.stdout = ostream
                sys.stderr = errstream
                sys.stdin = StringIO()
                os.environ['HGPLAIN'] = '1'
                ret = self.commands.dispatch(args)
            finally:
                sys.stdin  = sys.__stdin__
                sys.stdout = sys.__stdout__
                sys.stderr = sys.__stderr__
            if ret and ret not in okresults:
                ret = ret % 256
                if ret not in okresults:
                    print "Cmd: hg", ' '.join(args), 'failed with code', ret
            outstreams = (cleanansi(ostream.getvalue()), cleanansi(errstream.getvalue()))
            ostream.close()
            errstream.close()
            
            return outstreams
        else:
            (out, err) = runProgramStderr([self.hg_exe] + args, expectedexits=okresults)
            if os.linesep != '\n' and not binary:
                out = out.replace(os.linesep, '\n')
                err = err.replace(os.linesep, '\n')
            return (out, err)

    def pluginOptions(self, opts):
        '''The Mercurial extension is passing along -I/-X command line options'''
        for epath in opts['exclude']:
            self.cmdLineOptions += ['-X', epath]
        for ipath in opts['include']:
            self.cmdLineOptions += ['-I', ipath]
        if 'user' in opts:
            for name in opts['user']:
                self.commitname = name

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        return self.capList

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        self.hgcmd(['revert', '--no-backup', workingFile])

    def addIgnoreMask(self, newIgnoreMask):
        '''The user has provided a new ignoreMask to be added to revision control'''
        # Read existing .hgignore (possibly empty)
        globString = 'syntax: glob\n'
        try:
            f = open(os.path.join(self.repoRoot, '.hgignore'), 'rb')
            iLines = f.readlines()
        except IOError:
            iLines = []
        else:
            f.close()

        # fixup eoln to unix style for search and insert
        if iLines and iLines[0].endswith('\r\n'):
            iLines = [line[:-2] + '\n' for line in iLines]
            doseoln = True
        else:
            doseoln = False

        # Find 'syntax: glob' line, add at end if not found
        if globString in iLines:
            line = iLines.index(globString)
        else:
            iLines.append(globString)
            line = len(iLines) - 1
            if line == 0: 
                iLines.append('')

        # Insert new mask after 'syntax: glob' line
        iLines.insert(line + 1, newIgnoreMask + '\n')

        # replace original eoln
        if doseoln:
            iLines = [line[:-1] + '\r\n' for line in iLines]
        try:
            f = open(os.path.join(self.repoRoot, '.hgignore'), 'wb')
            f.writelines(iLines)
            f.close()
            print "Added '%s' to ignore mask" % newIgnoreMask
        except IOError, e:
            print "Unable to add '%s' to ignore mask" % newIgnoreMask
            print e
        shell_notify(os.path.join(self.repoRoot, '.hgignore'))

    def fileMoveDetected(self, origFileName, newFileName):
        '''User has associated an unknown file with a missing file, describing
           a move/rename which occurred outside of revision control'''
        self.hgcmd(['mv', '--after', origFileName, newFileName])
        print "Recording move of %s to %s" % (origFileName, newFileName)

    def fileCopyDetected(self, origFileName, newFileName):
        '''User has associated an unknown file with an existing file, describing
           a copy which occurred outside of revision control'''
        self.hgcmd(['cp', '--after', origFileName, newFileName])
        print "Recording copy of %s to %s" % (origFileName, newFileName)

    def getLogTemplate(self):
        '''Request default log message template from VCS'''
        # If this repository has a patch queue with applied patches, then the
        # user is not commiting a changeset. they are refreshing the top patch.
        # So we put the current patch's description in the edit window.
        if self.isPatchQ:
            qheader = self.hgcmd(['qheader'], okresults=[0,1,255])[0]
            return qheader[:-1]

        try:
            f = open(os.path.join(self.repoRoot, '.commit.template'), 'r')
            text = f.read()
            f.close()
        except IOError:
            text = ''
        return text

    def getAutoSelectTypes(self):
        '''Return annotations of file types which should be automatically
           selected when a new commit is started'''
        if self.isPatchQ:
            return ['A', 'M', 'R', 'a', 'm', 'r']
        else:
            return ['A', 'M', 'R']

    def isPatchQueue(self):
        '''Return true if Mercurial Queue patches are applied'''
        return self.isPatchQ

    def isMerge(self):
        '''Return true if working directory has two parents'''
        return len(self.parents) > 1

    def topPatchName(self):
        '''Return name of top patch (being refreshed)'''
        output = self.hgcmd(['qtop'], okresults=[0,1])[0]
        return output[:-1]

    def dirtyCache(self, fileName):
        '''The GUI would like us to forget cached data for this file'''
        if self.wdDiffCache.has_key(fileName):
            del self.wdDiffCache[fileName]
        if self.patchDiffCache.has_key(fileName):
            del self.patchDiffCache[fileName]

    def scanFiles(self, showIgnored, pb = None):
        '''Request scan for all commitable files from VCS, with optional
           progress bar
        '''
        # Called at startup, when 'Refresh' button is pressed, or when showIgnored toggled.
        self.patchDiffCache = {}
        self.wdDiffCache = {}

        # Cache changes in the working directory (parse and store hg diff).  The paths reported
        # by diff are always relative to the repo root, so if we're running outside of the root
        # directory there is no point in trying to pre-cache diffs.
        if self.runningFromRoot and len(self.parents) == 1:
            diff = self.hgcmd(['diff', '--show-function'] + self.cmdLineOptions)[0]
            (addedList, removedList, modifiedList, self.wdDiffCache) = scanDiffOutput(diff)

        if pb: pb.setValue(1)

        # Provides ARM, same as diff, plus unknown ? and missing !
        statusOutput = self.hgcmd(['status'] + self.cmdLineOptions + ['.'])[0]
        recs = statusOutput.split('\n')
        recs.pop() # remove last entry (which is '')

        if pb: pb.setValue(2)

        if showIgnored:
            statusOutput = self.hgcmd(['status', '-i'] + self.cmdLineOptions + ['.'])[0]
            recs += statusOutput.split('\n')
            recs.pop() # remove last entry (which is '')

        if pb: pb.setValue(3)

        annotatedFileList = [ ]
        workingDirList = [ ]
        for fileName in recs:
            workingDirList.append(fileName[2:])
            annotatedFileList.append(fileName)

        if pb: pb.setValue(4)

        if self.isPatchQ:
            # Capture changes in the current patch (parse and store hg tip)
            modifiedPList = self.hgcmd(['tip', '--debug', '--template', "{files}"])[0].split()
            addedPList = self.hgcmd(['tip', '--debug', '--template', "{file_adds}"])[0].split()
            removedPList = self.hgcmd(['tip', '--debug', '--template', "{file_dels}"])[0].split()

            self.filesinpatch = modifiedPList + addedPList + removedPList

            # Add patch files which did not show up in `hg status`
            for f in addedPList:
                if f not in workingDirList:
                    annotatedFileList.append('a ' + f)
            for f in removedPList:
                if f not in workingDirList:
                    annotatedFileList.append('r ' + f)
            for f in modifiedPList:
                if f in removedPList: continue
                if f in addedPList:   continue
                if f not in workingDirList:
                    annotatedFileList.append('m ' + f)

        return annotatedFileList


    def __getPatchChanges(self, filename, type):
        if filename not in self.filesinpatch:
            hgpathname = '/'.join(filename.split(os.sep))
            if hgpathname not in self.filesinpatch:
                return '--- Not yet included in patch ---'
        if self.patchDiffCache.has_key(filename):
            return self.patchDiffCache[filename]
        if type in ['A', 'a', 'M', 'm']:
            self.patchDiffCache[filename] = self.hgcmd(['qdiff', filename])[0]
            return self.patchDiffCache[filename]
        if type in ['R', 'r']:
            self.patchDiffCache[filename] = self.hgcmd(['cat', '-r', '-2', filename])[0]
            return self.patchDiffCache[filename]
        return 'unknown patch state!'


    def __getWorkingDirChanges(self, fileName, type):
        if self.wdDiffCache.has_key(fileName):
            return self.wdDiffCache[fileName]

        # For symlinks, we return the link data
        if type not in ['R', '!']:
            lmode = os.lstat(fileName)[ST_MODE]
            if S_ISLNK(lmode):
                text = "Symlink: %s -> %s" % (fileName, os.readlink(fileName))
                self.wdDiffCache[fileName] = text
                return text

        # For revisioned files, we use hg diff
        if type in ['A', 'M', 'R']:
            if len(self.parents) > 1:
                text = "\n= Diff to first parent %s\n" % self.parents[0]
                text += self.hgcmd(['diff', '--show-function', fileName])[0]
                otherparentrev = self.parents[1].split(':')[0]
                text += "\n= Diff to second parent %s\n" % self.parents[1]
                text += self.hgcmd(['diff', '--show-function', '--rev', otherparentrev, fileName])[0]
            else:
                text = self.hgcmd(['diff', '--show-function', fileName])[0]
            self.wdDiffCache[fileName] = text
            return text

        # For unrevisioned files, we return file contents
        if type in ['?', 'I']:
            if isBinary(fileName):
                text = " <Binary File>"
            else:
                f = open(fileName)
                text = f.read()
                f.close()
            self.wdDiffCache[fileName] = text
            return text

        # For missing files, we use hg cat
        if type == '!':
            text = self.hgcmd(['cat', fileName], binary=True)[0]
            if not text: text = " <empty file>"
            elif '\0' in text: text = " <Binary File of %d KBytes>" % (len(text) / 1024)
            self.wdDiffCache[fileName] = text
            return text
        else:
            return "Unknown file type " + type


    def getFileStatus(self, itemName):
        '''Request file deltas from VCS'''
        if self.isPatchQ:
            return self._getPatchFileStatus(itemName)

        type = itemName[0]
        fileName = itemName[2:]
        text = self.__getWorkingDirChanges(fileName, type)

        linesep = '\n'

        # Useful shorthand vars.  Leading lines beginning with % are treated as RTF
        bFileName = "%<b>" + fileName + "</b>"
        noteLineSep = linesep + '%'

        if type == 'A':
            note = bFileName + " has been added to revision control or is a rename target, but has never been commited."
            return note + linesep + text
        elif type == 'M':
            if len(self.parents) > 1:
                note = bFileName + " has been merged in your working directory."
            else:
                note = bFileName + " has been modified in your working directory."
            return note + linesep + text
        elif type == '?':
            note = bFileName + " is not currently tracked. If commited, it will be added to revision control."
            return note + linesep + "= Unrevisioned File Contents" + linesep + text
        elif type == 'I':
            note = bFileName + " is usually ignored, but will be added to revision control if commited"
            return note + linesep + text
        elif type == 'R':
            note = bFileName + " has been marked for deletion, or renamed, but has not yet been commited"
            note += noteLineSep + "The file can be recovered by reverting it to it's last revisioned state."
            return note + linesep + "= Removed File Diffs" + linesep + text
        elif type == '!':
            note = bFileName + " was tracked but is now missing. If commited, it will be marked as removed in revision control."
            note += noteLineSep + "The file can be recovered by reverting it to it's last revisioned state."
            return note + linesep + "= Contents of Missing File" + linesep + text
        else:
            return "Unknown file type " + type


    def _getPatchFileStatus(self, itemName):
        '''Get status of a file, which may have patch diffs, and may have working directory diffs'''

        type = itemName[0]
        fileName = itemName[2:]

        # Useful shorthand vars.  Leading lines beginning with % are treated as RTF
        bFileName = "%<b>" + fileName + "</b>"
        linesep = '\n'
        noteLineSep = linesep + '%'

        if type == 'A':
            note = bFileName + " has been added to the working directory, but has not been included in this patch."
            note += noteLineSep + "If reverted, this file will return to an unrevisioned state."
            wtext = self.__getWorkingDirChanges(fileName, type)
            return note + linesep + "= Added File Diffs" + linesep + wtext
        elif type == 'a':
            note = bFileName + " is a new file provided by this patch.  "
            note += noteLineSep + "Reverting this file has no effect, it must be removed from the patch first."
            ptext = self.__getPatchChanges(fileName, type)
            return note + linesep + "= Added File Diffs" + linesep + ptext
        elif type == '?':
            note = bFileName + " is not currently tracked. If commited, it will appear to be provided by this patch.  "
            note += noteLineSep + "Reverting this file has no effect."
            wtext = self.__getWorkingDirChanges(fileName, type)
            return note + linesep + "= Unrevisioned File Contents" + linesep + wtext
        elif type == 'I':
            note = bFileName + " is usually ignored, but will be recorded as provided by this patch if commited.  "
            note += noteLineSep + "Reverting this file has no effect."
            wtext = self.__getWorkingDirChanges(fileName, type)
            return note + linesep + "= Unrevisioned File Contents" + linesep + wtext
        elif type == '!':
            note = bFileName + " was tracked but is now missing, will be marked as removed by this patch if commited.  "
            note += noteLineSep + "If reverted, this file will be recovered to last revisioned state."
            wtext = self.__getWorkingDirChanges(fileName, type)
            return note + linesep + "= Contents of Missing File" + linesep + wtext
        elif type == 'R':
            note = bFileName + " has been marked for deletion in your working directory, but has not yet been commited.  "
            note += noteLineSep + "If reverted, this file will be recovered to it's last revisioned state."
            wtext = self.__getWorkingDirChanges(fileName, type)
            return note + linesep + "= Removed File Diffs" + linesep + wtext
        elif type == 'r':
            note = bFileName + " is deleted by this patch"
            note += noteLineSep + "If you remove this file from the patch, it will appear removed in your working dir, "
            note += noteLineSep + "at which point you can revert it to it's last revisioned state."
            ptext = self.__getPatchChanges(fileName, type)
            return note + linesep + "= Removed File Diffs" + linesep + ptext
        elif type == 'M':
            wtext = self.__getWorkingDirChanges(fileName, type)
            ptext = self.__getPatchChanges(fileName, type)
            if ptext:
                note = bFileName + " has changes recorded in the patch, and further changes in the working directory "
                note += noteLineSep + "If reverted, only the working directory changes will be removed.  "
                note += noteLineSep + "If you refresh without this file, all changes will be left in your working directory."
                status = note + linesep + "= Working Directory Diffs" + linesep + wtext
                status += linesep + "= Patch Diffs" + linesep + ptext
            else:
                note = bFileName + " has changes in the working directory that are not yet included in this patch."
                note += noteLineSep + "If reverted, the working directory diffs will be removed."
                status = note + linesep + "= Working Directory Diffs" + linesep + wtext
            return status
        elif type == 'm':
            note = bFileName + " is modified by this patch.  There are no further changes in the working directory so "
            note += noteLineSep + "reverting this file will have no effect.  If you remove this file from the patch "
            note += noteLineSep + "these modifications will be left in the working directory."
            ptext = self.__getPatchChanges(fileName, type)
            return note + linesep + "= Patch Diffs" + linesep + ptext
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
            self.hgcmd(['add'] + addFileList)
            print "Added %d file(s) to revision control" % len(addFileList)

        if removeFileList:
            self.hgcmd(['rm'] + removeFileList)
            print "Removed %d file(s) from revision control" % len(removeFileList)

        if self.signOff:
            logMsgText += '\n' + self.signOff

        (fd, filename) = mkstemp()
        file = os.fdopen(fd, "w+b")
        file.write(logMsgText)
        file.close()

        if self.isPatchQ:
            self.hgcmd(['qrefresh', '-l', filename] + commitFileNames)
            print self.topPatchName() + " refreshed with %d file(s)" % len(commitFileNames)
        elif len(self.parents) > 1:
            if self.commitname:
                self.hgcmd(['commit', '-u', self.commitname, '-l', filename])
            else:
                self.hgcmd(['commit', '-l', filename])
            print "Merge results commited" 
        else:
            if self.commitname:
                self.hgcmd(['commit', '-u', self.commitname, '-l', filename] + commitFileNames)
            else:
                self.hgcmd(['commit', '-l', filename] + commitFileNames)
            print "%d file(s) commited" % len(commitFileNames)

        os.unlink(filename)
        shell_notify(commitFileNames)

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        self.hgcmd(['add'] + selectedFileList)
        shell_notify(selectedFileList)

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
            self.hgcmd(['revert'] + revertFileNames)
            shell_notify(revertFileNames)
        else:
            print "No revertable files"


def shell_notify(paths):
    '''
    Refresh Windows shell when file states change.  This allows
    the explorer to refresh their icons based on their new state
    '''
    if not os.name == 'nt':
        return
    try:
        from win32com.shell import shell, shellcon
        for path in paths:
            abspath = os.path.abspath(path)
            pidl, ignore = shell.SHILCreateFromPath(abspath, 0)
            shell.SHChangeNotify(shellcon.SHCNE_UPDATEITEM, 
                                 shellcon.SHCNF_IDLIST | shellcon.SHCNF_FLUSHNOWAIT,
                                 pidl, None)
    except ImportError:
        pass
# vim: tw=120
