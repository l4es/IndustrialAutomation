# Qct commit tool plugin for bazaar (bzr)
#
# Copyright 2006 Steve Borho <steve@borho.org>
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.
#
# To install, copy this file into ~/.bazaar/plugins

from bzrlib.commands import Command, register_command

class cmd_qct(Command):
    """Qt-based GUI Commit Tool
    
       This command will open a window from which you
       can browse all of the pending changes you have made
       to your branch.  You can then type in a commit log
       message and commit your changes to the repository.
       You can remove files from the commit list by de-selecting
       them in the file list.

       Keyboard Shortcuts:

       CTRL-O  - Commit selected files
       CTRL-R  - Refresh file list
       CTRL-N  - View diffs of next file in list
       CTRL-[] - Page up/down through file diffs
       CTRL-U  - Unselect all files
       CTRL-F  - Clear file filter text
       ESC     - Abort and exit
    """

    def run(self):
        from bzrlib.branch import Branch
        from bzrlib.errors import NoWorkingTree
        from bzrlib.workingtree import WorkingTree
        from bzrlib import urlutils

        import os
        from PyQt4 import QtCore, QtGui
        from qctlib.gui_logic import CommitTool
        from qctlib.vcs.bzr import qctVcsBzr

        def local_path(path):
            if path.startswith("file://"):
                return urlutils.local_path_from_url(path)
            else:
                return urlutils.unescape(path)

        try:
            branch = WorkingTree.open_containing(u'.')[0].branch
        except NoWorkingTree:
            branch = Branch.open_containing(u'.')[0]

        branch_root = branch.bzrdir.root_transport.base
        # print "Branch root at " + branch_root
        os.chdir(local_path(branch_root))
        vcs = qctVcsBzr()
        if vcs.initRepo(None) != 0:
            return

        try:
            app = QtGui.QApplication([])
            dialog = CommitTool(vcs)
            dialog.show()
            app.exec_()
        except SystemExit:
            pass

register_command(cmd_qct)
