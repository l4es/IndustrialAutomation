# Perfoce VCS back-end code for qct
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

# Usage Notes:
#   o  Assumes P4CLIENT, P4PORT, P4USER are properly set
#   o  Will execute out of current directory, but all paths will be listed globally
#   o  Will always use the default changelist

from qctlib.utils import runProgram
import os

class qctVcsP4:
    def initRepo(self, argv):
        '''Initialize your revision control system, open repository'''
        # Verify we have a valid repository
        output = runProgram(['p4', 'fstat', '.'], expectedexits=[0,1])
        if output.startswith("Path "):
            print output
            return -1
        else:
            return 0

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        # Perforce support is pretty sparse
        return ( )

    def getLogTemplate(self):
        '''Request default log message template from VCS'''
        if not os.environ.has_key('P4_LOG_TEMPLATE'):
            return ''
        logFileName = os.environ['P4_LOG_TEMPLATE']
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

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        runProgram(['p4', 'revert', self.clientToDepotMap[workingFile] ])
        runProgram(['p4', 'edit', self.clientToDepotMap[workingFile] ])

    def dirtyCache(self, fileName):
        '''The GUI would like us to forget cached data for this file'''
        if self.wdCache.has_key(fileName):
            del self.wdCache[fileName]

    def scanFiles(self, showIgnored):
        '''Request scan for all commitable files from VCS'''
        # Called at startup, when 'Refresh' button is pressed, or when
        # showIgnored toggled.
        fileList = []
        self.wdCache = { }
        self.clientToDepotMap = { }

        # Find open files, this command returns depot paths
        statusOutput = runProgram(['p4', 'opened', '...'])
        recs = statusOutput.split(os.linesep)
        recs.pop() # remove last entry (which is '')
        for line in recs:
            if line.startswith('... '): break
            fileName = line.split('#')[0]  # remove rev number and status
            results = getP4FileStatus(fileName)
            fileList.append(results[2] + ' ' + results[1])
            self.clientToDepotMap[results[1]] = results[0]

        # Find missing files, this command returns client paths
        statusOutput = runProgram(['p4', 'diff', '-sd', '...'])
        recs = statusOutput.split(os.linesep)
        recs.pop() # remove last entry (which is '')
        for fileName in recs:
            # current directory may not be revisioned yet
            if fileName.startswith('... '): break
            results = getP4FileStatus(fileName)
            fileList.append('!' + ' ' + results[1])
            self.clientToDepotMap[results[1]] = results[0]

        return fileList

    def getFileStatus(self, itemName):
        '''Request file deltas from VCS'''
        status = itemName[0]
        fileName = itemName[2:]
        bFileName = "%<b>" + fileName + "</b> "

        if status == 'M':
            if self.wdCache.has_key(fileName):
                return self.wdCache[fileName]
            else:
                text = runProgram(['p4', 'diff', '-du', self.clientToDepotMap[fileName] ])
                self.wdCache[fileName] = text[:-len(os.linesep)]
                return text
        elif status == 'A':
            return bFileName + "has been added to perforce, but not yet commited"
        elif status == 'R':
            return bFileName + "has been opened for delete, but has not yet been commited"
        elif status == '!':
            return bFileName + "was tracked but is now missing, will be removed from perforce if commited"
        elif status == '*':
            return bFileName + "has no recorded open reason with perforce (what's going on?)"
        else:
            return "Unknown file type " + status

    def commitFiles(self, selectedFileList, logMsgText):
        '''Commit selected files'''
        # Files in list are annotated (A, M, etc) so this function can
        # mark files for add or delete as necessary before instigating the commit.
        depotFileNames = []
        for f in selectedFileList:
            status = f[0]
            fileName = f[2:]
            if status == '?' or status == 'I':
                print "Adding %s to revision control" % fileName
                runProgram(['p4', 'add', fileName])
            elif status == '!':
                print "Removing %s from revision control" % fileName
                runProgram(['p4', 'delete', self.clientToDepotMap[fileName] ])
            if self.clientToDepotMap.has_key(fileName):
                depotFileNames.append(self.clientToDepotMap[fileName])
            else:
                print "Unable to find depot name of " + fileName

        # Perforce doesn't allow you to specify a message on the command line.
        # What you have to do is pretend to do the commit and ask for the form it would
        # create.  You then insert the log message and actual file list into that file and
        # then submit that with -i
        iform = runProgram(['p4', 'change', '-o'])
        recs = iform.split(os.linesep)
        oform = ''
        for line in recs:
            if line == '\t<enter description here>':
                logMsgLines = logMsgText.split(os.linesep)
                for l in logMsgLines:
                    oform += '\t' + l + os.linesep
            elif line == 'Files:':
                oform += line + os.linesep
                for i in depotFileNames:
                    oform += '\t' + i + os.linesep
                break
            else:
                oform += line + os.linesep

        # print "Output form that would be submitted to perforce: " + oform
        runProgram(['p4', 'submit', '-i'], oform)
        print "%d file(s) commited: %s" % (len(selectedFileList), ', '.join(depotFileNames))

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        runProgram(['p4', 'add'] + selectedFileList)

    def revertFiles(self, selectedFileList):
        '''Revert selected files to last revisioned state'''
        revertFileNames = []
        for f in selectedFileList:
            status = f[0]
            fileName = f[2:]
            if status == 'R':
                print "deleted %s recovered from revision control" % fileName
                revertFileNames.append(self.clientToDepotMap[fileName])
            elif status == '!':
                print "missing %s recovered from revision control" % fileName
                runProgram(['p4', 'sync', '-f', self.clientToDepotMap[fileName] ])
            elif status == 'A':
                print "added %s forgot from revision control" % fileName
                revertFileNames.append(self.clientToDepotMap[fileName])
            elif status == 'M':
                print "modifications to %s reverted" % fileName
                revertFileNames.append(self.clientToDepotMap[fileName])
            else:
                print "File %s not reverted" % fileName

        if len(revertFileNames):
            runProgram(['p4', 'revert'] + revertFileNames)
        else:
            print "No revertable files"

def  getP4FileStatus(fileName):
    '''Helper function which determines a file's open reason'''
    fstatOut = runProgram(['p4', 'fstat', fileName])
    status = '*'  # No open action
    # normalize case of pwd and client path, to have a good chance of this actually
    # working on windows
    cwd = os.path.normcase(os.getcwd())
    afterCwd = len(cwd) + 1
    recs = fstatOut.split(os.linesep)
    for line in recs:
        words = line.split(' ')
        if len(words) < 2: continue
        if words[1] == 'depotFile':
            depotName = words[2]
        elif words[1] == 'clientFile':
            clientName = os.path.normcase(words[2])
            # Prune current working directory from client name (make relative)
            if clientName.startswith(cwd):
                clientName = clientName[afterCwd:]
        elif words[1] == 'action':
            if   words[2] == 'add': status = 'A'
            elif words[2] == 'edit': status = 'M'
            elif words[2] == 'delete': status = 'R'
    return (depotName, clientName, status)

# vim: tw=120
