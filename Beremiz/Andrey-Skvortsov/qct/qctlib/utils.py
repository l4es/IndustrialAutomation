# Helper classes for VCS back-end code
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

import os, sys
from PyQt4 import QtCore, QtGui
from string import split
import re
import errno

def findInSystemPath(filename):
    '''Search for an executable in the system path'''
    paths = split(os.environ['PATH'], os.path.pathsep)
    for path in paths:
        fullName = os.path.join(path, filename)
        if os.path.exists(fullName):
            return os.path.abspath(os.path.join(path, filename))
    return None

def isBinary(filename):
    """Return true iff the given filename is binary.

    Raises an EnvironmentError if the file does not exist or cannot be
    accessed.
    """
    fin = open(filename, 'rb')
    try:
        CHUNKSIZE = 1024
        while 1:
            chunk = fin.read(CHUNKSIZE)
            if '\0' in chunk: # found null byte
                return 1
            if len(chunk) < CHUNKSIZE:
                break # done
    finally:
        fin.close()
    return 0

def scanDiffOutput(diffStream):
    '''Scan output of diff, collect lists and patch hash'''
    patchHash = { }
    addedList = [ ]
    removedList = [ ]
    modifiedList = [ ]

    # A typical diff header looks like this:
    # diff -r 52b82ffc6695 -r 52b82ffc6695 qctlib/dialog.ui
    # --- a/qctlib/dialog.ui  Wed Dec 27 14:02:36 2006 -0600
    # +++ b/qctlib/dialog.ui  Wed Dec 27 08:39:45 2006 -0600

    fileName = ''
    patchStarted = False
    patchContents = [ ]
    lines = diffStream.split('\n')
    for line in lines:
        if line.startswith('diff '):
            # Start of next patch
            if patchStarted and patchContents:
                patchHash[fileName] = '\n'.join(patchContents)
                patchContents = [ ]
            patchStarted = True
            maybe_git, fileName = tuple(line.split(' ')[-3 : -1])
            # Convert / to \ on Windows, diff always reports /
            if "--git" in maybe_git:
              fileName = os.sep.join(fileName.split('/')[1:])
            else:
              fileName = os.sep.join(fileName.split('/'))
        elif line.startswith('Binary file '):
            patchStarted = True
            fileName = line.split(' ')[-3]
            # Convert / to \ on Windows, diff always reports /
            fileName = os.sep.join(fileName.split('/'))
        elif line.startswith('--- '):
            words = line.split(' ')
            fname = words[1].split('\t')[0]
            if fname == "/dev/null": addedList.append(fileName)
        elif line.startswith('+++ '):
            words = line.split(' ')
            fname = words[1].split('\t')[0]
            if fname == "/dev/null": removedList.append(fileName)
            elif fileName not in addedList:
                modifiedList.append(fileName)
        if patchStarted: patchContents.append(line)

    if fileName and patchContents:
        patchHash[fileName] = '\n'.join(patchContents)
    return (addedList, removedList, modifiedList, patchHash)

import subprocess

# Thank you, hgct authors
class ProgramError(Exception):
    def __init__(self, progStr, error):
        self.progStr = progStr
        self.error = error.rstrip()

    def __str__(self):
        return self.progStr + ': ' + self.error

def runProgram(prog, input=None, expectedexits=[0]):
    if type(prog) is str:
        progStr = prog
    else:
        progStr = ' '.join(prog)
    
    try:
        pop = subprocess.Popen(prog,
                               shell = type(prog) is str,
                               stderr=subprocess.STDOUT,
                               stdout=subprocess.PIPE,
                               stdin=subprocess.PIPE)
    except OSError, e:
        raise ProgramError(progStr, e.strerror)

    if input != None:
        pop.stdin.write(input)
    pop.stdin.close()
    
    try:
        out = pop.stdout.read()
    except IOError:
        out = ''
        pass

    while True:
        try:
            code = pop.wait()
        except OSError, e:
            if e.errno == errno.EINTR: 
                continue 
            else: 
                raise e
        else:
            break
        
    if code not in expectedexits:
        # Windows does not use uchar return values
        code = code % 256
        if code not in expectedexits:
            print >>sys.stderr, "Error code %d not expected" % code
            raise ProgramError(progStr, out)
    return out

def runProgramStderr(prog, input=None, expectedexits=[0]):
    if type(prog) is str:
        progStr = prog
    else:
        progStr = ' '.join(prog)
    
    try:
        pop = subprocess.Popen(prog,
                               shell = type(prog) is str,
                               stderr=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stdin=subprocess.PIPE)
    except OSError, e:
        raise ProgramError(progStr, e.strerror)

    if input != None:
        pop.stdin.write(input)
    pop.stdin.close()
    
    try:
        out = pop.stdout.read()
    except IOError:
        out = ''

    try:
        err = pop.stderr.read()
    except IOError:
        err = ''

    while True:
        try:
            code = pop.wait()
        except OSError, e:
            if e.errno == errno.EINTR: 
                continue 
            else: 
                raise e
        else:
            break

    if code not in expectedexits:
        # Windows does not use uchar return values
        code = code % 256
        if code not in expectedexits:
            print >>sys.stderr, "Error code %d not expected" % code
            raise ProgramError(progStr, out)
    return (out, err)

def formatPatchRichText(patch, colors):
    '''Syntax highlight patches based on first character of each line'''

    ret = ['<qt><pre><font face="Sans Serif"; color="', colors['std'], '">']
    prev = 'header'
    for l in patch.split('\n'):
        if l: c = l[0]
        else: c = ' '

        # Allow VCS code to insert RichText in the diff header by
        # preceding RichText lines with %.
        if prev == 'header':
            if c == '%':
                ret.extend([str(l[1:]), os.linesep])
                continue
            else:
                prev = ' '

        if l.startswith('= '):
            ret.extend(['</font><em>' + l[2:] + '</em><br>' +
                '<font face="Sans Serif"; color="', colors['std'], '">'])
            prev = ' '
            continue
        elif c != prev:
            if   c == '+': style = 'new'
            elif c == '-': style = 'remove'
            elif c == '@': style = 'head'
            else:          style = 'std'
            ret.extend(['</font><font face="Mono"; color="', colors[style], '">'])
            prev = c

        # Escape patch text, make it HTML safe
        try:
            line = QtCore.Qt.escape(QtCore.QString.fromLocal8Bit(l))
        except UnicodeEncodeError:
            line = '!Unicode Encoding Error!'
        ret.extend([line, os.linesep])

    ret.append('</pre></qt>')
    retstring = QtCore.QString()
    for s in ret:
        retstring += s
    return retstring

ansire = re.compile('(\x1b[[]([0-9]{0,3};?)+[a-zA-Z])')
def cleanansi(txt):
    """
    Remove color annotation (which may be inserted by hgext.color extension)
    """
    return ansire.sub('', txt)
    
