# gui_logic.py - Commit Tool
#
# Copyright 2006 Steve Borho
#
# This software may be used and distributed according to the terms
# of the GNU General Public License, incorporated herein by reference.

from PyQt4 import QtCore, QtGui
from qctlib.ui_dialog import Ui_commitToolDialog
from qctlib.ui_preferences import Ui_prefDialog
from qctlib.utils import formatPatchRichText, runProgram
from qctlib.version import qct_version
from qctlib.changeselect import ChangeDialog
import os, sys, shutil, shlex

class CommitTool(QtGui.QWidget):
    '''QCT commit tool GUI logic'''
    def __init__(self, vcs):
        '''Initialize the dialog window, fill with initial data'''
        QtGui.QWidget.__init__(self)

        self.maxHistCount = 8
        self.vcs = vcs
        self.autoSelectTypes = self.vcs.getAutoSelectTypes()
        self.patchColors = {'std': 'black', 'new': '#009600', 'remove': '#C80000', 'head': '#C800C8'}
        self.fileCheckState = {}
        self.logHistory = []
        self.changeSelectedFiles = []
        self.showIgnored = False
        self.wrapList = False
        self.itemChangeEntered = False
        self.logTemplate = None

        self.ui = Ui_commitToolDialog()
        self.ui.setupUi(self)
        self.setWindowFlags(self.windowFlags() | QtCore.Qt.WindowContextHelpButtonHint)
        self.setWindowTitle("Qct Commit Tool %s" % qct_version)
        self.ui.histButton = None

        # Support for Mercurial Queues, and Stacked Git.  If a patch is
        # applied, then the user is refreshing that patch, not commiting
        # to the underlying repository.
        self.patchRefreshMode = False
        self.commitButtonToolTip = "Commit selected (checked) files"
        if 'patchqueue' in self.vcs.capabilities() and self.vcs.isPatchQueue():
            self.patchRefreshMode = True
            self.ui.commitMsgBox.setTitle(QtGui.QApplication.translate("commitToolDialog",
                "Patch Description: " + self.vcs.topPatchName(),
                None, QtGui.QApplication.UnicodeUTF8))
            self.ui.commitTextEntry.setToolTip(QtGui.QApplication.translate("commitToolDialog",
                "Description of current top patch " + self.vcs.topPatchName(),
                None, QtGui.QApplication.UnicodeUTF8))
            self.ui.commitPushButton.setText(QtGui.QApplication.translate("commitToolDialog",
                "Refresh Patch", None, QtGui.QApplication.UnicodeUTF8))
            self.commitButtonToolTip = "Refresh selected files in patch " + self.vcs.topPatchName()
            self.ui.cancelPushButton.setText("Exit")
        # Support for merge commits (multiple parents for working directory)
        elif 'merge' in self.vcs.capabilities() and self.vcs.isMerge():
            self.ui.commitMsgBox.setTitle(QtGui.QApplication.translate("commitToolDialog",
                "Merge Description", None, QtGui.QApplication.UnicodeUTF8))
            self.ui.commitPushButton.setText(QtGui.QApplication.translate("commitToolDialog",
                "Commit Merge Results", None, QtGui.QApplication.UnicodeUTF8))
            self.ui.selectAllPushButton.setEnabled(False)
            self.ui.fileListBox.setTitle(QtGui.QApplication.translate("commitToolDialog", 
                "File list cannot be filtered when committing merge results",
                None, QtGui.QApplication.UnicodeUTF8))

        # Recover persistent data
        settings = QtCore.QSettings('vcs', 'qct')
        settings.beginGroup('mainwindow')

        self.sortby = settings.value('sortby').toInt()
        # I don't understand why this check is required, seems like a
        # PyQt bug.  It's returning a tuple (int, bool)
        if type(self.sortby) is not int:
            self.sortby = self.sortby[0]

        # size and pos loading is for backward compatibility
        if settings.contains('size'):
            self.resize(settings.value('size').toSize())
            settings.remove('size')
        if settings.contains('pos'):
            self.move(settings.value('pos').toPoint())
            settings.remove('pos')
        # windowgeometry supersedes size, pos, and what was missing (such as
        # maximized state)
        if settings.contains('windowgeometry'):
            self.restoreGeometry(settings.value('windowgeometry').toByteArray())
        if settings.contains('3waysplitter'):
            self.ui.splitter.restoreState(settings.value('3waysplitter').toByteArray())
        settings.endGroup()
        settings.beginGroup('commitLog')
        size = settings.beginReadArray('history')
        for i in xrange(0, size):
            settings.setArrayIndex(i)
            self.logHistory.append(settings.value('text').toString())
        settings.endArray()
        settings.endGroup()

        self.ui.sortComboBox.setCurrentIndex(self.sortby)

        self.connect(self.ui.fileListWidget,
                QtCore.SIGNAL("customContextMenuRequested(const QPoint &)"),
                self.__contextMenu)

        self.__fillLogHistCombo()

        # Prepare for simple syntax highlighting
        self.ui.diffBrowser.setAcceptRichText(True)

        # Setup ESC to exit
        self.actionEsc = QtGui.QAction(self)
        self.actionEsc.setShortcut(QtGui.QKeySequence(self.tr("ESC")))
        self.ui.commitTextEntry.addAction(self.actionEsc)
        self.connect(self.actionEsc, QtCore.SIGNAL("triggered()"), self.close)

        # Setup CTRL-O to trigger commit
        self.actionCtrlO = QtGui.QAction(self)
        self.actionCtrlO.setShortcut(QtGui.QKeySequence(self.tr("Ctrl+O")))
        self.ui.commitTextEntry.addAction(self.actionCtrlO)
        self.connect(self.actionCtrlO, QtCore.SIGNAL("triggered()"), self.commitSelected)

        # Setup CTRL-Enter to trigger commit
        self.actionCtrlEnter = QtGui.QAction(self)
        self.actionCtrlEnter.setShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_Return ) )
        self.ui.commitTextEntry.addAction(self.actionCtrlEnter)
        self.connect(self.actionCtrlEnter, QtCore.SIGNAL("triggered()"), self.commitSelected)

        # Setup CTRL-R to trigger refresh
        self.actionCtrlR = QtGui.QAction(self)
        self.actionCtrlR.setShortcut(QtGui.QKeySequence(self.tr("Ctrl+R")))
        self.ui.commitTextEntry.addAction(self.actionCtrlR)
        self.connect(self.actionCtrlR, QtCore.SIGNAL("triggered()"), self.on_refreshPushButton_pressed)

        # Setup CTRL-N to display next file
        self.actionCtrlN = QtGui.QAction(self)
        self.actionCtrlN.setShortcut(QtGui.QKeySequence(self.tr("Ctrl+N")))
        self.ui.commitTextEntry.addAction(self.actionCtrlN)
        self.connect(self.actionCtrlN, QtCore.SIGNAL("triggered()"), self.displayNextFile)

        # Setup CTRL-U to select next file
        self.actionCtrlU = QtGui.QAction(self)
        self.actionCtrlU.setShortcut(QtGui.QKeySequence(self.tr("Ctrl+U")))
        self.ui.commitTextEntry.addAction(self.actionCtrlU)
        self.connect(self.actionCtrlU, QtCore.SIGNAL("triggered()"), self.unselectAll)

        # Setup CTRL-] to scroll browser window
        self.actionPageDown = QtGui.QAction(self)
        self.actionPageDown.setShortcut(QtGui.QKeySequence(self.tr("CTRL+]")))
        self.ui.commitTextEntry.addAction(self.actionPageDown)
        self.connect(self.actionPageDown, QtCore.SIGNAL("triggered()"), self.__pageDownBrowser)

        # Setup CTRL-[ to scroll browser window
        self.actionPageUp = QtGui.QAction(self)
        self.actionPageUp.setShortcut(QtGui.QKeySequence(self.tr("CTRL+[")))
        self.ui.commitTextEntry.addAction(self.actionPageUp)
        self.connect(self.actionPageUp, QtCore.SIGNAL("triggered()"), self.__pageUpBrowser)

        # Setup CTRL-F to clear filter
        self.actionCtrlF = QtGui.QAction(self)
        self.actionCtrlF.setShortcut(QtGui.QKeySequence(self.tr("CTRL+F")))
        self.ui.commitTextEntry.addAction(self.actionCtrlF)
        self.connect(self.actionCtrlF, QtCore.SIGNAL("triggered()"), self.on_clearFilterButton_pressed)

        self.connect(self.ui.commitPushButton, QtCore.SIGNAL("clicked()"), self.commitSelected)
        self.ui.fileListWidget.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)

        self.__retrieveConfigurables()
        self.__rescanFiles()
        if not self.itemList:
            print "No uncommited changes"
            sys.exit()
            return
        self.__refreshFileList(True)
        self.__updateCommitButtonState()

    def reject(self):
        '''User has pushed the cancel button'''
        self.close()

    def __retrieveConfigurables(self):
        '''Run at startup and after the preferences dialog exits'''
        settings = QtCore.QSettings('vcs', 'qct')
        self.signoff = settings.value('signoff', QtCore.QVariant('')).toString()
        settings.beginGroup('fileList')
        self.showIgnored = settings.value('showIgnored', QtCore.QVariant(False)).toBool()
        self.wrapList = settings.value('wrapping', QtCore.QVariant(False)).toBool()
        settings.endGroup()

        settings.beginGroup('tools')
        self.histTool = str(settings.value('histTool', QtCore.QVariant('')).toString())
        self.diffTool = str(settings.value('diffTool', QtCore.QVariant('')).toString())
        self.editTool = str(settings.value('editTool', QtCore.QVariant('')).toString())
        self.twowayTool = str(settings.value('twowayTool', QtCore.QVariant('')).toString())
        settings.endGroup()

        # Disable the 'show ignored' feature if VCS does not support it (perforce)
        if 'ignore' not in self.vcs.capabilities():
            self.showIgnored = False

        if self.ui.histButton and not self.histTool:
            self.ui.hboxlayout2.removeWidget(self.ui.histButton)
            self.ui.histButton = None
        elif self.histTool and not self.ui.histButton:
            self.ui.histButton = QtGui.QPushButton(self)
            self.ui.histButton.setObjectName("histButton")
            self.ui.histButton.setText("History")
            self.ui.hboxlayout2.insertWidget(1, self.ui.histButton)
            self.connect(self.ui.histButton, QtCore.SIGNAL("clicked()"), self.__history)

        if self.wrapList:
            self.ui.fileListWidget.setWrapping( True )
            self.ui.fileListWidget.setFlow( QtGui.QListView.LeftToRight )
            # self.ui.fileListWidget.setUniformItemSizes( True )
        else:
            self.ui.fileListWidget.setWrapping( False )
            self.ui.fileListWidget.setFlow( QtGui.QListView.TopToBottom )
            # self.ui.fileListWidget.setUniformItemSizes( False )

    def __history(self):
        '''User has clicked the visual history button'''
        if self.histTool:
            runProgram(self.histTool.split(' '), expectedexits=[0,1,255])

    def __rescanFiles(self):
        '''Helper function which wraps progress bar functionality around
           the call to vcs.scanFiles()
        '''
        if 'progressbar' in self.vcs.capabilities():
            pb = QtGui.QProgressDialog()
            pb.setWindowTitle('Repository Scan')
            pb.setLabelText('Progress of repository scan')
            pb.setMinimum(0)
            pb.setMaximum(4)
            pb.setModal(True)
            pb.forceShow()
            pb.setValue(0)
            self.itemList = self.vcs.scanFiles(self.showIgnored, pb)
        else:
            self.itemList = self.vcs.scanFiles(self.showIgnored)
        self.__applyFilter()
        self.__sortList()

    def __applyFilter(self):
        filter = str(self.ui.filterLineEdit.text())
        if filter:
            self.filteredList = []
            for _item in self.itemList:
                if filter in _item[2:]:
                    self.filteredList.append(_item)
        else:
            self.filteredList = self.itemList

    def on_clearFilterButton_pressed(self):
        self.ui.filterLineEdit.clear()

    def on_filterLineEdit_textChanged(self):
        self.__applyFilter()
        self.__refreshFileList(False)

    def __sortList(self):
        if self.sortby == 1:    # filename
            self.filteredList.sort(lambda x, y: cmp(x[2:], y[2:]))
        elif self.sortby == 2:  # status
            self.filteredList.sort()
        elif self.sortby == 3:  # status
            self.filteredList.sort()
            self.filteredList.reverse()
        elif self.sortby == 4:  # extension
            self.filteredList.sort(lambda x, y:
                    cmp(os.path.splitext(x[2:])[1], os.path.splitext(y[2:])[1]))

    def __safeQMessageBox(self, title, text):
        try:
            ret = QtGui.QMessageBox.warning(self, title, text,
                    QtGui.QMessageBox.Ok | QtGui.QMessageBox.Cancel)
        except TypeError:
            # Older Qt versions (<4.1) have slightly different arguments
            ret = QtGui.QMessageBox.warning(self, title, text,
                    QtGui.QMessageBox.Ok, QtGui.QMessageBox.Cancel)
        return ret

    def __contextMenu(self, Point):
        '''User has right clicked inside the file list window
           (or pressed the windows menu key).  We determine which
           item is under the mouse and then present options.
        '''
        item = self.ui.fileListWidget.itemAt(Point)
        if not item: return
        menuPos = self.ui.fileListWidget.mapToGlobal(Point)

        # Multi-selection context menu
        selectedItemList = self.ui.fileListWidget.selectedItems()
        if len(selectedItemList) > 1:
            allUnknowns = True
            for item in selectedItemList:
                itemName = str(item.text())
                if itemName[0] not in ['?', 'I']:
                    allUnknowns = False
                    break
            if allUnknowns:
                menu = QtGui.QMenu()
                menu.addAction("Add all selected files to version control")
                menu.addAction("Delete all selected files")
                a = menu.exec_(menuPos)
                if a is None: return
                actionText = str(a.text())
                if actionText.startswith('Delete all'):
                    if self.__safeQMessageBox("File Deletion Warning",
                            "Are you sure you want to delete all selected files?") != QtGui.QMessageBox.Ok:
                        return
                    for item in selectedItemList:
                        itemName = str(item.text())
                        if os.path.isdir(itemName[2:]):
                            shutil.rmtree(itemName[2:])
                        else:
                            os.unlink(itemName[2:])
                    self.__rescanFiles()
                    self.__refreshFileList(False)
                elif actionText.startswith('Add all'):
                    selectedFileList = []
                    for item in selectedItemList:
                        itemName = str(item.text())
                        selectedFileList.append(itemName[2:])
                    self.vcs.addFiles(selectedFileList)
                    self.__rescanFiles()
                    self.__refreshFileList(False)
                return
            else:
                menu = QtGui.QMenu()
                menu.addAction("Revert all selected files")
                if self.diffTool:
                    menu.addAction("Visual diff")
                    if self.patchRefreshMode:
                        menu.addAction("Visual diff of all patch changes")
                a = menu.exec_(menuPos)
                if not a: return
                actionText = str(a.text())
                if actionText.startswith('Revert'):
                    if self.__safeQMessageBox("Revert Warning",
                            "Are you sure you want to revert all selected files?") != QtGui.QMessageBox.Ok:
                        return
                    fileList = []
                    for item in selectedItemList:
                        itemName = str(item.text())
                        fileList.append(itemName)
                    self.vcs.revertFiles(fileList)
                    self.__rescanFiles()
                    self.__refreshFileList(False)
                elif actionText.startswith('Visual diff'):
                    fileList = []
                    # Hack to get visual diff of all changes in the patch, not
                    # just those in the working directory
                    if actionText.endswith('all patch changes'): fileList = ['--rev', '-2' ]
                    for item in selectedItemList:
                        itemName = str(item.text())
                        fileList.append(itemName[2:])
                    runProgram(self.diffTool.split(' ') + fileList, expectedexits=[0,1,255])
                    self.__rescanFiles()
                    self.__refreshFileList(False)
                return

        itemName = str(item.text())
        targetType = itemName[0]
        targetFile = itemName[2:]

        # Context menu for unknown files (ignore masks or copy detection)
        if targetType in ['?', 'I']:
            menu = QtGui.QMenu()
            if targetType == '?' and 'ignore' in self.vcs.capabilities():
                basename = os.path.basename(targetFile) # baz.ext
                dirname = os.path.dirname(targetFile)   # foo/bar else ''
                ext = os.path.splitext(basename)[1]     # .ext else ''
                menu.addAction("Add Ignore: %s" % targetFile)
                if dirname and ext: menu.addAction("Add Ignore: %s/*%s" % (dirname, ext))
                if dirname:         menu.addAction("Add Ignore: %s" % basename)
                if ext:             menu.addAction("Add Ignore: *%s" % ext)
            if 'copy' in self.vcs.capabilities():
                menu.addAction("%s is a copy of a revisioned file" % targetFile)
            if self.editTool:
                menu.addAction("Open in %s" % os.path.basename(self.editTool))
            menu.addAction("Add to version control")
            menu.addAction("Delete %s" % targetFile)
            a = menu.exec_(menuPos)
            if a is not None:
                actionText = str(a.text())
                if actionText.startswith('Add to'):
                    self.vcs.addFiles([targetFile])
                    self.__rescanFiles()
                    self.__refreshFileList(False)
                elif actionText.startswith('Add Ignore: '):
                    self.vcs.addIgnoreMask(actionText[12:])
                elif actionText.startswith('Open '):
                    runProgram([self.editTool, targetFile], expectedexits=[0,1,255])
                    self.vcs.dirtyCache(targetFile)
                    self.__refreshFileList(False)
                elif actionText.startswith('Delete '):
                    if self.__safeQMessageBox("File Deletion Warning",
                            "Are you sure you want to delete %s?" % targetFile) != QtGui.QMessageBox.Ok:
                        return
                    if os.path.isdir(targetFile):
                        shutil.rmtree(targetFile)
                    else:
                        os.unlink(targetFile)
                else:
                    self.__detectFileCopySource(targetFile)
                self.__rescanFiles()
                self.__refreshFileList(False)
            return

        # Context menu for rename events
        if targetType == '>':
            menu = QtGui.QMenu()
            menu.addAction("Revert rename back to %s" % targetFile)
            a = menu.exec_(menuPos)
            if not a: return
            self.__revertFile(itemName)
            self.__rescanFiles()
            self.__refreshFileList(False)
            return

        # Context menu for missing files (detect renames)
        if targetType == '!':
            menu = QtGui.QMenu()

            # Present unknown files as possible rename/move targets
            if self.unknownFileList and 'rename' in self.vcs.capabilities():
                for u in self.unknownFileList:
                    menu.addAction("%s was moved/renamed to %s" % (targetFile, u))
            menu.addAction("Recover %s from revision control" % targetFile)
            a = menu.exec_(menuPos)
            if not a: return

            actionText = str(a.text())
            if actionText.startswith('Recover '):
                self.__revertFile(itemName)
            else:
                l = len(targetFile) + 22
                renameTarget = actionText[l:]
                self.vcs.fileMoveDetected(targetFile, renameTarget)
            self.__rescanFiles()
            self.__refreshFileList(False)

        # Context menu for files with merge conflicts
        if targetType == 'C':
            menu = QtGui.QMenu()

            menu.addAction("Revert %s to last revisioned state" % targetFile)
            if self.editTool:
                menu.addAction("Open in %s" % os.path.basename(self.editTool))
            if self.diffTool:
                menu.addAction("Visual diff")
            a = menu.exec_(menuPos)
            if not a: return

            actionText = str(a.text())
            if actionText.startswith('Open '):
                runProgram([self.editTool, targetFile], expectedexits=[0,1,255])
                self.vcs.dirtyCache(targetFile)
            elif actionText.startswith('Revert '):
                self.__revertFile(itemName)
                self.__rescanFiles()
            elif actionText.startswith('Visual diff'):
                runProgram(self.diffTool.split(' ') + [ targetFile ], expectedexits=[0,1,255])
                self.vcs.dirtyCache(targetFile)
            self.__refreshFileList(False)

        # Context menu for 'A' 'M' and 'R' (and 'a', 'm', 'r')
        if targetType in self.autoSelectTypes:
            menu = QtGui.QMenu()
            if targetFile in self.changeSelectedFiles:
                menu.addAction("Reset selection of changes")
            elif targetType == 'M':
                menu.addAction("Select changes to commit")
            if targetType not in ['a', 'm', 'r']:
                menu.addAction("Revert %s to last revisioned state" % targetFile)
            if targetType not in ['R', 'r'] and self.editTool:
                menu.addAction("Open in %s" % os.path.basename(self.editTool))
            if targetType in ['M', 'A'] and self.diffTool:
                menu.addAction("Visual diff")
                if self.patchRefreshMode:
                    menu.addAction("Visual diff of all patch changes")
            elif targetType in ['m', 'a'] and self.diffTool:
                menu.addAction("Visual diff of all patch changes")
            a = menu.exec_(menuPos)
            if not a: return

            actionText = str(a.text())
            if actionText.startswith('Open '):
                runProgram([self.editTool, targetFile], expectedexits=[0,1,255])
                self.vcs.dirtyCache(targetFile)
            elif actionText.startswith('Revert '):
                self.__revertFile(itemName)
                self.__rescanFiles()
            elif actionText.startswith('Visual diff'):
                if actionText.endswith('all patch changes'):
                    args = ['--rev', '-2', targetFile ]
                else:
                    args = [ targetFile ]
                runProgram(self.diffTool.split(' ') + args, expectedexits=[0,1,255])
                self.vcs.dirtyCache(targetFile)
            elif actionText.startswith('Select '):
                self.__selectChanges(targetFile)
            elif actionText.startswith('Reset '):
                self.__resetChangeSelection(targetFile)
            self.__refreshFileList(False)

    def __selectChanges(self, workingFile):
        '''User would like to select changes made to this file for commit,
           unselected changes are left in working directory after commit or
           at exit.
        '''
        self.vcs.dirtyCache(workingFile)
        workingCopy = '.qct/' + workingFile + '.orig'
        try:
            path = os.path.dirname(workingFile)
            os.makedirs('.qct/' + path)
        except OSError:
            pass
        try:
            os.remove(workingCopy)
        except OSError:
            pass
        try:
            os.rename(workingFile, workingCopy)
        except:
            return
        self.changeSelectedFiles.append(workingFile)
        try:
            self.vcs.generateParentFile(workingFile)
            if not self.twowayTool:
                dialog = ChangeDialog(workingCopy, workingFile)
                dialog.show()
                dialog.exec_()
                if not dialog.accepted:
                    raise Exception
                # Copy permissions, times back to workingFile
                shutil.copystat(workingCopy, workingFile)
                return

            cmd = self.twowayTool
            if '%o' in cmd and '%m' in cmd:
                cmd = cmd.replace('%o', workingFile)
                cmd = cmd.replace('%m', workingCopy)
                cmd = cmd.replace('\\', '/')
                runProgram(shlex.split(cmd), expectedexits=[0,1,255])
            else:
                runProgram([self.twowayTool, workingCopy, workingFile])
            # Copy permissions, times back to workingFile
            shutil.copystat(workingCopy, workingFile)
        except:
            print "Change selection failed, returning working file"
            self.__resetChangeSelection(workingFile)

    def __resetChangeSelection(self, workingFile, deleteindex=True):	    
        '''Restore original working copy, clean up .qct/'''
        if deleteindex:
            i = self.changeSelectedFiles.index(workingFile)
            del self.changeSelectedFiles[i] 
        self.vcs.dirtyCache(workingFile)
        workingCopy = '.qct/' + workingFile + '.orig'
        try:
            os.remove(workingFile)
        except OSError:
            pass
        os.rename(workingCopy, workingFile)
        try:
            path = os.path.dirname(workingFile)
            if path: os.removedirs('.qct/' + path)
            os.removedirs('.qct/')
        except OSError:
            pass

    def __updateCommitButtonState(self):
        '''Only enable the commit button if a valid log message exists
           and one or more files are selected
        '''
        logMessage = self.ui.commitTextEntry.toPlainText()
        if (logMessage != self.logTemplate or self.patchRefreshMode) and self.getCheckedFiles():
            self.ui.commitPushButton.setEnabled(True)
            self.ui.commitPushButton.setToolTip(QtGui.QApplication.translate("commitToolDialog",
                self.commitButtonToolTip, None, QtGui.QApplication.UnicodeUTF8))
        else:
            self.ui.commitPushButton.setEnabled(False)
            self.ui.commitPushButton.setToolTip(QtGui.QApplication.translate("commitToolDialog",
                'Disabled until file(s) are selected and a log message is entered',
                None, QtGui.QApplication.UnicodeUTF8))

    def __revertFile(self, fileName):
        if self.__safeQMessageBox("Revert Warning",
                "Are you sure you want to revert %s?" % fileName[2:]) != QtGui.QMessageBox.Ok:
            return
        self.vcs.revertFiles([ fileName ])

    def __detectFileCopySource(self, targetFile):
        '''The user has identified an unknown file as a copy of a revisioned
           file.  Allow the user to select the copy source by opening a file
           selection dialog
        '''
        ext = os.path.splitext(targetFile)[1]
        if ext:
            searchExtensions = '%s Files (*%s);;All Files (*)' % (ext[1:].capitalize(), ext)
        else:
            searchExtensions = 'All Files (*)'

        fileName = QtGui.QFileDialog.getOpenFileName(self,
                                         "Select copy source of %s" % targetFile,
                                         targetFile, searchExtensions)
        if not fileName.isEmpty():
            self.vcs.fileCopyDetected(str(fileName), targetFile)


    def __saveLogMessage(self, logMessage):
        '''A new commit (or abort) has occurred, try to save the log message.
           If the message is a duplicate of a message already in the history,
           then move it to the top of the stack
        '''
        if logMessage != self.logTemplate:
            if logMessage in self.logHistory:
                self.logHistory.remove(logMessage)
            self.logHistory.append(logMessage)
            if len(self.logHistory) > self.maxHistCount:
                del self.logHistory[0]

    def __fillLogHistCombo(self):
        '''Fill the log history drop-down box with the last N messages'''
        for log in self.logHistory:
            topLine = log.split('\n')[0]
            self.ui.logHistComboBox.insertItem(0, topLine)
        self.ui.logHistComboBox.setCurrentIndex(0)

    def on_prefPushButton_pressed(self):
        '''Preferences Dialog'''
        prefDialog = PrefDialog()
        prefDialog.show()
        prefDialog.exec_()

        oldIgnored = self.showIgnored
        self.__retrieveConfigurables()
        if self.showIgnored != oldIgnored:
            self.__rescanFiles()
            self.__refreshFileList(False)
            self.__updateCommitButtonState()

    @QtCore.pyqtSignature("int")
    def on_sortComboBox_activated(self, row):
        self.sortby = row
        self.__sortList()
        self.__refreshFileList(False)

    @QtCore.pyqtSignature("int")
    def on_logHistComboBox_activated(self, row):
        '''The user has selected a log entry from the history drop-down'''
        index = len(self.logHistory) - row - 1
        self.ui.commitTextEntry.clear()
        self.ui.commitTextEntry.setFocus()
        self.ui.commitTextEntry.setPlainText(self.logHistory[index])

    def closeEvent(self, e = None):
        '''Dialog is closing, save persistent state'''

        # Recover working directory first, priorities...
        for targetFile in self.changeSelectedFiles:
            self.__resetChangeSelection(targetFile,False)
        self.changeSelectedFiles = []

        # Save off any aborted log message
        logMessage = self.ui.commitTextEntry.toPlainText()
        self.__saveLogMessage(logMessage)

        settings = QtCore.QSettings('vcs', 'qct')
        settings.beginGroup('mainwindow')
        settings.setValue('windowgeometry', QtCore.QVariant(self.saveGeometry()))
        settings.setValue("3waysplitter", QtCore.QVariant(self.ui.splitter.saveState()))
        settings.setValue("sortby", QtCore.QVariant(self.sortby))
        settings.endGroup()
        settings.beginGroup('commitLog')
        settings.beginWriteArray('history')
        for i, log in enumerate(self.logHistory):
            settings.setArrayIndex(i)
            settings.setValue("text", QtCore.QVariant(log))
        settings.endArray()
        settings.endGroup()

        settings.sync()
        if e is not None:
            e.accept()

    def __pageDownBrowser(self):
        '''Page Up the diff browser (Ctrl-])'''
        vs = self.ui.diffBrowser.verticalScrollBar()
        vs.triggerAction(QtGui.QAbstractSlider.SliderPageStepAdd)

    def __pageUpBrowser(self):
        '''Page Up the diff browser (Ctrl-[)'''
        vs = self.ui.diffBrowser.verticalScrollBar()
        vs.triggerAction(QtGui.QAbstractSlider.SliderPageStepSub)

    def __displaySelectedFile(self):
        '''Show status of currently selected file'''
        if not self.filteredList: return  # Filtered list could be empty
        item = self.filteredList[ self.displayedRow ]
        deltaText = self.vcs.getFileStatus(item)
        #self.ui.diffBrowser.setPlainText(deltaText)
        self.ui.diffBrowser.setHtml(formatPatchRichText(deltaText, self.patchColors))
        self.ui.diffBrowserBox.setTitle(item[2:] + " file status")

    def __refreshFileList(self, newCommitFlag):
        '''Refresh the file list, display status of first file'''
        if not self.itemList:
            print "No remaining uncommited changes"
            self.close()
            return

        if newCommitFlag:
            self.fileCheckState = {}

        if 'merge' in self.vcs.capabilities() and self.vcs.isMerge():
            merge = True
        else:
            merge = False

        self.ui.fileListWidget.clear()
        self.unknownFileList = []
        for itemName in self.filteredList:
            listItem = QtGui.QListWidgetItem(itemName)
            status = itemName[0]
            fileName = itemName[2:]
            if status == '?':
                self.unknownFileList.append(fileName)
            if newCommitFlag and status in self.autoSelectTypes:
                listItem.setCheckState(QtCore.Qt.Checked)
                self.fileCheckState[ fileName ] = True
            elif self.fileCheckState.has_key(fileName) and self.fileCheckState[ fileName ] == True:
                listItem.setCheckState(QtCore.Qt.Checked)
            else:
                listItem.setCheckState(QtCore.Qt.Unchecked)
                self.fileCheckState[ fileName ] = False
            if merge: listItem.setFlags(QtCore.Qt.ItemIsSelectable)
            self.ui.fileListWidget.addItem(listItem)

        # Display status (diff) of first item in list, and select it
        self.displayedRow = 0
        self.__displaySelectedFile()
        item = self.ui.fileListWidget.item(0)
        self.ui.fileListWidget.setItemSelected(item, True)
        self.ui.fileListWidget.setCurrentItem(item)

        # Refresh log template if necessary
        if newCommitFlag or self.patchRefreshMode:
            self.logTemplate = self.vcs.getLogTemplate()

        # Prepare for new commit message
        if newCommitFlag:
            self.ui.commitTextEntry.clear()
            self.ui.commitTextEntry.setFocus()
            self.ui.commitTextEntry.setPlainText(self.logTemplate)
        self.__updateCommitButtonState()

    def on_commitTextEntry_textChanged(self):
        '''User has typed something in the commit text window'''
        self.__updateCommitButtonState()

    def unselectAll(self):
        '''Reset checked state of all files (Ctrl-U)'''
        self.fileCheckState = {}
        self.__refreshFileList(False)

    def getCheckedFiles(self):
        '''Helper function to build list of checked (selected) files'''
        checkedItemList = []
        for item in self.itemList:
            fileName = item[2:]
            if self.fileCheckState[ fileName ] == True:
                checkedItemList.append(item)
        return checkedItemList

    def displayNextFile(self):
        '''User has hit CTRL-N'''
        self.displayedRow += 1
        if self.displayedRow >= len(self.filteredList):
            self.displayedRow = 0

        item = self.ui.fileListWidget.item(self.displayedRow)
        self.ui.fileListWidget.setCurrentItem(item)
        self.ui.fileListWidget.setItemSelected(item, True)
        self.ui.fileListWidget.scrollToItem(item)

        selectedItemList = self.ui.fileListWidget.selectedItems()
        for i in selectedItemList:
            if i is item:
                continue
            self.ui.fileListWidget.setItemSelected(i, False)

        self.__displaySelectedFile()

    def commitSelected(self):
        '''Commit selected files, then refresh the dialog for next commit'''
        checkedItemList = self.getCheckedFiles()
        if not checkedItemList:
            self.__safeQMessageBox("Commit Warning", "No files are selected, nothing to commit")
            self.ui.fileListWidget.setFocus()
            return

        logMessage = self.ui.commitTextEntry.toPlainText()
        if logMessage == self.logTemplate and not self.patchRefreshMode:
            self.__safeQMessageBox("Commit Warning", "No log message specified, aborting commit")
            self.ui.commitTextEntry.setFocus()
            return

        if self.signoff:
            logMessage += os.linesep + self.signoff
        msg = logMessage.toLocal8Bit()

        self.vcs.commitFiles(checkedItemList, msg)
        self.__saveLogMessage(logMessage)
        self.__fillLogHistCombo()

        # Put back unselected changes (original working copies) and
        # clean up .qct/ directory
        for targetFile in self.changeSelectedFiles:
            self.__resetChangeSelection(targetFile,False)
        self.changeSelectedFiles = []

        self.__rescanFiles()
        self.__refreshFileList(True)

    def on_selectAllPushButton_pressed(self):
        '''(Un)Select All button has been pressed'''
        # Try to select all items
        changedFileState = False
        for item in self.filteredList:
            f = item[2:]
            if self.fileCheckState[ f ] == False:
                self.fileCheckState[ f ] = True
                changedFileState = True
        # If there were no un-selected items, toggle unselect them all
        if changedFileState == False:
            self.fileCheckState = { }
        self.__refreshFileList(False)

    def on_refreshPushButton_pressed(self):
        '''Refresh button pressed slot handler'''
        oldSelectState = self.fileCheckState
        self.fileCheckState = {}
        self.__rescanFiles()
        for item in self.filteredList:
            f = item[2:]
            if oldSelectState.has_key(f) and oldSelectState[ f ] == True:
                self.fileCheckState[ f ] = True
            else:
                self.fileCheckState[ f ] = False
        self.__refreshFileList(False)
        self.__updateCommitButtonState()

    def on_fileListWidget_itemActivated(self, item):
        '''The user has activated a list item, we toggle its check state'''
        # These will trigger cell change signals
        if item.checkState() == QtCore.Qt.Checked:
            item.setCheckState(QtCore.Qt.Unchecked)
        else:
            item.setCheckState(QtCore.Qt.Checked)

    def on_fileListWidget_itemChanged(self, item):
        '''The user has modified the check state of an item,
           If the item was part of a select group we set them all to the
           checked state of the modified item.'''
        if self.itemChangeEntered: return
        self.itemChangeEntered = True

        if item.checkState() == QtCore.Qt.Checked:
            selectedItemList = self.ui.fileListWidget.selectedItems()
            if item in selectedItemList:
                for si in selectedItemList: 
                    fileName = str(si.text())[2:]
                    si.setCheckState(QtCore.Qt.Checked)
                    self.fileCheckState[ fileName ] = True
            else:
                fileName = str(item.text())[2:]
                item.setCheckState(QtCore.Qt.Checked)
                self.fileCheckState[ fileName ] = True
        else:
            selectedItemList = self.ui.fileListWidget.selectedItems()
            if item in selectedItemList:
                for si in selectedItemList: 
                    fileName = str(si.text())[2:]
                    si.setCheckState(QtCore.Qt.Unchecked)
                    self.fileCheckState[ fileName ] = False
            else:
                fileName = str(item.text())[2:]
                item.setCheckState(QtCore.Qt.Unchecked)
                self.fileCheckState[ fileName ] = False

        self.__updateCommitButtonState()
        self.itemChangeEntered = False

    def on_fileListWidget_itemClicked(self, item):
        '''The user has clicked on a list item'''
        row = self.ui.fileListWidget.row(item)
        if row != -1 and self.filteredList and row != self.displayedRow:
            self.displayedRow = row
            self.__displaySelectedFile()
        self.__updateCommitButtonState()

    def on_fileListWidget_itemSelectionChanged(self):
        '''The user has selected a list item'''
        row = self.ui.fileListWidget.currentRow()
        if row != -1 and self.filteredList and row != self.displayedRow:
            if row >= len(self.filteredList):
                row = 0
            self.displayedRow = row
            self.__displaySelectedFile()

class PrefDialog(QtGui.QDialog):
    '''QCT Preferences Dialog'''
    def __init__(self):
        QtGui.QDialog.__init__(self)

        self.ui = Ui_prefDialog()
        self.ui.setupUi(self)

        settings = QtCore.QSettings('vcs', 'qct')

        self.signoff = settings.value('signoff', QtCore.QVariant('')).toString()

        settings.beginGroup('fileList')
        self.showIgnored = settings.value('showIgnored', QtCore.QVariant(False)).toBool()
        self.wrapList = settings.value('wrapping', QtCore.QVariant(False)).toBool()
        settings.endGroup()
        
        settings.beginGroup('tools')
        self.diffTool = settings.value('diffTool', QtCore.QVariant('')).toString()
        self.histTool = settings.value('histTool', QtCore.QVariant('')).toString()
        self.editTool = settings.value('editTool', QtCore.QVariant('')).toString()
        self.twowayTool = settings.value('twowayTool', QtCore.QVariant('')).toString()
        settings.endGroup()

        # Disable wrap feature for Qt < 4.2
        try:
            from PyQt4 import pyqtconfig
        except ImportError:
            # The Windows installed PyQt4 does not support pyqtconfig, but
            # does support wrapping, etc.  So we will leave this feature
            # enabled if we fail to import pyqtconfig.
            # self.ui.wrapListCheckBox.setEnabled(False)
            pass
        else:
            pyqtconfig = pyqtconfig.Configuration()
            if pyqtconfig.qt_version < 0x40200:
                self.wrapList = False
                self.ui.wrapListCheckBox.setEnabled(False)
                self.ui.wrapListCheckBox.setToolTip(QtGui.QApplication.translate("wrapListCheckBox",
                    "This feature requires Qt >= 4.2", None, QtGui.QApplication.UnicodeUTF8))

        self.ui.ignoredButton.setChecked(self.showIgnored)
        self.ui.wrapListCheckBox.setChecked(self.wrapList)
        self.ui.diffToolEdit.setText(self.diffTool)
        self.ui.histToolEdit.setText(self.histTool)
        self.ui.mergeToolEdit.setText(self.twowayTool)
        self.ui.editToolEdit.setText(self.editTool)
        self.ui.signoffTextEdit.setPlainText(self.signoff)

    def on_aboutPushButton_pressed(self):
        QtGui.QMessageBox.about(self, 'Qct Commit Tool', 
"<qt><center><h1>Qct " + qct_version + """</h1></center>\n
<center>Copyright &copy; 2007 Steve Borho &lt;steve@borho.org&gt;</center>\n<p>
This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.</p></qt>""")

    def accept(self):
        self.histTool = str(self.ui.histToolEdit.text())
        self.diffTool = str(self.ui.diffToolEdit.text())
        self.twowayTool = str(self.ui.mergeToolEdit.text())
        self.editTool = str(self.ui.editToolEdit.text())
        self.wrapList = self.ui.wrapListCheckBox.isChecked()
        self.showIgnored = self.ui.ignoredButton.isChecked()
        settings = QtCore.QSettings('vcs', 'qct')
        settings.setValue('signoff',
                QtCore.QVariant(self.ui.signoffTextEdit.toPlainText()))
        settings.beginGroup('fileList')
        settings.setValue('showIgnored', QtCore.QVariant(self.ui.ignoredButton.isChecked()))
        settings.setValue('wrapping', QtCore.QVariant(self.ui.wrapListCheckBox.isChecked()))
        settings.endGroup()
        settings.beginGroup('tools')
        settings.setValue('histTool', QtCore.QVariant(self.histTool))
        settings.setValue('diffTool', QtCore.QVariant(self.diffTool))
        settings.setValue('editTool', QtCore.QVariant(self.editTool))
        settings.setValue('twowayTool', QtCore.QVariant(self.twowayTool))
        settings.endGroup()
        settings.sync()
        self.close()
