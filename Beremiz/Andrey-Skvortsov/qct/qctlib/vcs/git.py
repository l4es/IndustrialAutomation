# GIT VCS back-end code for qct
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

import sys, os, re
from qctlib.utils import runProgram

class qctVcsGit:
    def initRepo(self, argv):
        '''Initialize your revision control system, open repository'''
        if not os.environ.has_key('GIT_DIR'):
            os.environ['GIT_DIR'] = '.git'

        if not os.environ.has_key('GIT_OBJECT_DIRECTORY'):
            os.environ['GIT_OBJECT_DIRECTORY'] = os.environ['GIT_DIR'] + '/objects'

        if not (os.path.exists(os.environ['GIT_DIR']) and
                os.path.exists(os.environ['GIT_DIR'] + '/refs') and
                os.path.exists(os.environ['GIT_OBJECT_DIRECTORY'])):
            print "Git archive not found."
            print "Make sure that the current working directory contains a '.git' directory, or\nthat GIT_DIR is set appropriately."
            return -1
        return 0

    def capabilities(self):
        '''Return a list of optional capabilities supported by this VCS'''
        # Git does not support explicit renames and moves, AFAIK
        return ('ignore', 'patchqueue')

    def addIgnoreMask(self, newIgnoreMask):
        '''The user has provided a new ignoreMask to be added to revision control'''
        # TODO (add to .gitignore?)

    def getLogTemplate(self):
        '''Request default log message template from VCS'''
        return ''

    def getAutoSelectTypes(self):
        '''Return annotations of file types which should be automatically
           selected when a new commit is started'''
        return ['A', 'M', 'R']

    def isPatchQueue(self):
        '''Return true if Stacked Git Queue patches are applied'''
        return False

    def generateParentFile(self, workingFile):
        '''The GUI needs this file's parent revision in place so the user
           can select individual changes for commit (basically a revert)
        '''
        # TODO: Verify this gets the parent revision
        runProgram(['git', 'cat', workingFile])

    def dirtyCache(self, fileName):
        '''The GUI would like us to forget cached data for this file'''
        pass

    def scanFiles(self, showIgnored):
        '''Request scan for all commitable files from VCS'''
        # Called at startup, when 'Refresh' button is pressed, or when
        # showIgnored toggled.
        list = runProgram(['git-diff-files', '--name-status', '-z']).split('\0')
        list.pop()
        fileList = []
        nameList = []
        while len(list):
            name = list.pop()
            nameList.append(name)
            type = list.pop()
            fileList.append(type + " " + name)
        runXargsStyle(['git-update-index', '--remove', '--'], nameList)
        # The returned file list will be annotated by type
        return fileList

    def getFileStatus(self, itemName):
        '''Request file deltas from VCS'''
        annotation = itemName[0]
        fileName = itemName[2:]
        if annotation == 'A':
            return fileName + " has been added to VCS or is a rename target, but has never been commited"
        elif annotation == '?':
            return fileName + " is not currently tracked, will be added to VCS if commited"
        elif annotation == '!':
            return fileName + " was tracked but is now missing, will be removed from VCS if commited"
        elif annotation == 'I':
            return fileName + " is usually ignored, but will be added to VCS if commited"
        elif annotation == 'R':
            return fileName + " has been marked for deletion, or renamed, but has not yet been commited"
        elif annotation == 'M':
            return runProgram(['git-diff-cache', '-p', '-M', '--cached', 'HEAD', fileName])
        else:
            return "Unknown file type " + annotation

    def commitFiles(self, selectedFileList, logMsgText):
        '''Commit selected files'''
        # Files in list are annotated (A, M, etc) so this function can
        # mark files for add or delete as necessary before instigating the commit.
        commitFileNames = []
        for f in selectedFileList:
            annotation = f[0]
            fileName = f[2:]
            if annotation == '?' or annotation == 'I':
                print "Adding %s to revision control" % fileName
                #runProgram(['hg', 'add', fileName])
            elif annotation == '!':
                print "Removing %s from revision control" % fileName
                #runProgram(['hg', 'rm', fileName])
            commitFileNames.append(fileName)

        #runProgram(['hg', 'commit', '-l', '-'] + commitFileNames, logMsgText)
        print "%d file(s) commited: %s" % (len(selectedFileList), ', '.join(commitFileNames))
        # You could call sys.exit(), if this is running inside a plugin/extension
        return

    def addFiles(self, selectedFileList):
        '''Add selected files to version control'''
        runProgram(['git', 'add'] + selectedFileList)

    def revertFiles(self, selectedFileList):
        print "Git revert is currently unsupported"
        return


def runXargsStyle(origProg, args):
    steps = range(10, len(args), 10)
    prog = origProg[:]
    prev = 0
    for i in steps:
        prog.extend(args[prev:i])
        runProgram(prog)
        prog = origProg[:]
        prev = i

    prog.extend(args[prev:])
    runProgram(prog)

# vim: tw=120
