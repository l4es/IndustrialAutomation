#!/usr/bin/env python
# Change Selection Dialog
#
# Copyright 2007 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from qctlib.ui_select import Ui_ChangeDialog
from qctlib.utils import formatPatchRichText
from qctlib.patches import iter_patched, hunk_from_header
import difflib

patchColors = {
    'std': 'black',
    'new': '#009600',
    'remove': '#C80000',
    'head': '#C800C8'}

class ChangeDialog(QDialog):
    '''QCT commit tool GUI logic'''
    def __init__(self, changefile, origfile):
        '''Initialize the dialog window, calculate diff hunks'''
        QDialog.__init__(self)

        self.changefile = changefile
        self.origfile = origfile
        self.accepted = False
        self.ui = Ui_ChangeDialog()
        self.ui.setupUi(self)
        self.ui.buttonBox.setStandardButtons(QDialogButtonBox.Cancel)
        settings = QSettings('vcs', 'qct')
        settings.beginGroup('changetool')
        if settings.contains('size'):
            self.resize(settings.value('size').toSize())
            self.move(settings.value('pos').toPoint())
        settings.endGroup()

        # Calculate unified diffs, split into hunks
        self.hunklist = []
        hunk = None
        self.origtext = open(origfile, 'rb').readlines()
        changetext = open(changefile, 'rb').readlines()
        for line in difflib.unified_diff(self.origtext, changetext):
            if line.startswith('@@'):
                if hunk: self.hunklist.append(hunk)
                hunk = [line]
            elif hunk:
                hunk.append(line)
        if hunk: self.hunklist.append(hunk)

        self.curhunk = 0
        self.showhunk()

    def showhunk(self):
        text = ''.join(self.hunklist[self.curhunk])
        h = formatPatchRichText(text, patchColors)
        self.ui.textEdit.setHtml(h)

    def on_keepButton_pressed(self):
        '''User has pushed the keep button'''
        self.curhunk += 1
        if self.curhunk < len(self.hunklist):
            self.showhunk()
        else:
            self.ui.keepButton.setEnabled(False)
            self.ui.shelveButton.setEnabled(False)
            self.ui.buttonBox.setStandardButtons( \
                    QDialogButtonBox.Cancel | QDialogButtonBox.Ok)

    def on_shelveButton_pressed(self):
        del self.hunklist[self.curhunk]
        if self.curhunk < len(self.hunklist):
            self.showhunk()
        else:
            self.ui.keepButton.setEnabled(False)
            self.ui.shelveButton.setEnabled(False)
            self.ui.buttonBox.setStandardButtons( \
                    QDialogButtonBox.Cancel | QDialogButtonBox.Ok)

    def reject(self):
        '''User has pushed the cancel button'''
        self.close()

    def closeEvent(self, e = None):
        '''Dialog is closing, save persistent state'''
        settings = QSettings('vcs', 'qct')
        settings.beginGroup('changetool')
        settings.setValue("size", QVariant(self.size()))
        settings.setValue("pos", QVariant(self.pos()))
        settings.endGroup()
        settings.sync()
        if e is not None:
            e.accept()

    def accept(self):
        '''User has pushed the accept button'''
        # Make sure at least one hunk was selected
        if not self.hunklist:
            self.close()
            return

        # Determine lines we expect from iter_patched to replace
        lasthunk = self.hunklist[-1]
        lasth = hunk_from_header(lasthunk[0])
                
        # Apply selected hunks to origfile
        try:
            try:
                patchlines = ['--- orig\n', '+++ changed\n']
                for p in self.hunklist:
                    patchlines += p
                replace = []
                for line in iter_patched(self.origtext, patchlines):
                    replace.append(line)
                if replace:
                    self.origtext[0:lasth.orig_pos + lasth.orig_range - 1] = \
                            replace
                f = open(self.origfile, 'wb')
                f.writelines(self.origtext)
                f.close()
                self.accepted = True
            except Exception, e:
                print str(e)
        finally:
            self.close()

# Test operation by running directly
if __name__ == "__main__":
    import sys
    app = QApplication(sys.argv)
    try:
        (changefile, origfile) = sys.argv[1:3]
    except ValueError:
        print sys.argv[0], 'changed-file orig-file'
        sys.exit(1)
    dialog = ChangeDialog(changefile, origfile)
    dialog.show()
    app.exec_()
    if dialog.accepted:
        sys.exit(0)
    else:
        print 'changes not selected'
        sys.exit(1)
