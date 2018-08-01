# Qct commit tool extension for Mercurial
#
# Copyright 2006 Steve Borho <steve@borho.org>
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

'''Qt commit tool'''

from mercurial import hg, commands, dispatch

import os

# every command must take a ui and and repo as arguments.
# opts is a dict where you can find other command line flags
#
# Other parameters are taken in order from items on the command line that
# don't start with a dash.  If no default value is given in the parameter list,
# they are required.
def launch_qct(ui, repo, *extras, **opts):
    """start qct commit tool

       If '.' is given as an argument the tool will operate out of the
       current directory, else it will operate repository-wide.
    
       This command will open a window from which you can browse all of
       the commitable changes you have made to your working directory.
       You can then enter a log message and commit your changes to the
       repository.  You can remove files from the commit list by
       de-selecting them in the file list.

       Keyboard Shortcuts:

       CTRL-O  - Commit selected files
       CTRL-R  - Refresh file list
       CTRL-N  - View diffs of next file in list
       CTRL-[] - Page up/down through file diffs
       CTRL-U  - Unselect all files
       CTRL-F  - Clear file filter text
       ESC     - Abort and exit
    """
    rundir = repo.root
    if '.' in extras:
        rundir = '.'
    os.chdir(rundir)

    # If this user has a username validation hook enabled,
    # it could conflict with Qct because both will try to
    # allocate a QApplication, and PyQt doesn't deal well
    # with two app instances running under the same context.
    # To prevent this, we run the hook early before Qct
    # allocates the app
    try:
        from hgconf.uname import hook
        hook(ui, repo)
    except ImportError:
        pass

    try:
        import qctlib
    except ImportError:
        import sys
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
        
    try:
        from PyQt4 import QtGui
        from qctlib.gui_logic import CommitTool
        from qctlib.vcs.hg import qctVcsHg
    except ImportError:
        # If we're unable to import Qt4 and qctlib, try to
        # run the application directly
        # You can specificy it's location in ~/.hgrc via
        #   [qct]
        #   path=
        try:    udata = " -u %s" % opts['user'][-1]
        except: udata = ''
        cmd = ui.config("qct", "path", "qct") + udata + " --hg"
        os.system(cmd)
    else:
        import sys
        vcs = qctVcsHg()
        if hasattr(commands, "dispatch"): # 0.9.4 and below
            if vcs.initRepo(None, commands) != 0:
                sys.exit()
        else:
            if vcs.initRepo(None, dispatch) != 0:
                sys.exit()

        # Pass along -I/-X and --user options to Mercurial back-end
        vcs.pluginOptions(opts)

        app = QtGui.QApplication([])
        dialog = CommitTool(vcs)
        dialog.show()
        app.exec_()


cmdtable = {
    "^qct|commit-tool": (launch_qct,
        [('I', 'include', [], 'include names matching the given patterns'),
        ('X', 'exclude', [], 'exclude names matching the given patterns'),
        ('u', 'user', [],    'record user as committer')],
        "hg qct [options] [.]")
}
