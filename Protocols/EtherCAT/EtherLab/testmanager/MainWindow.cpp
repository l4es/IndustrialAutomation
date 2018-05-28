/*****************************************************************************
 *
 * Testmanager - Graphical Automation and Visualisation Tool
 *
 * Copyright (C) 2018  Florian Pose <fp@igh.de>
 *
 * This file is part of Testmanager.
 *
 * Testmanager is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Testmanager is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Testmanager. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <Python.h> // must be first!

#include "MainWindow.h"
#include "ConnectDialog.h"
#include "DataModel.h"
#include "DataSource.h"
#include "PropertyModel.h"
#include "PropertyDelegate.h"
#include "Property.h"
#include "TabDialog.h"
#include "PythonShell.h"
#include "StyleDialog.h"
#include "AboutDialog.h"

// manually load plugins
#include "plugins/DialPlugin.h"
#include "plugins/DigitalPlugin.h"
#include "plugins/DoubleSpinBoxPlugin.h"
#include "plugins/GraphPlugin.h"
#include "plugins/PushButtonPlugin.h"
#include "plugins/QLabelPlugin.h"
#include "plugins/XYGraphPlugin.h"

#include "TabPage.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QSettings>

#include <stdexcept>

#include <sys/types.h> // socketpair()
#include <sys/socket.h>

#ifdef TM_MODEL_TEST
#include "modeltest.h"
#endif

/****************************************************************************/

int multiply(int a, int b)
{
    return a * b;
}

/****************************************************************************/

MainWindow *MainWindow::singleton = NULL;

/****************************************************************************/

MainWindow::MainWindow(
        const QString &fileName,
        bool newView,
        QWidget *parent):
    QMainWindow(parent),
    propertyDelegate(new PropertyDelegate()),
    programName(NULL),
    editMode(false),
    gridStep(10),
    restore(false),
    propertyNode(NULL),
    pythonStdOutNotifier(NULL),
    pythonStdErrNotifier(NULL),
    pythonHistoryIndex(0)
{
    if (singleton) {
        throw(std::runtime_error("Only one instance allowed!"));
    }
    singleton = this;

    setupUi(this);

    setWindowIcon(QPixmap(":/images/testmanager.svg"));

    dataModel = new DataModel();
#ifdef TM_MODEL_TEST
    qDebug() << "DataModel test enabled.";
    new ModelTest(dataModel, this);
#endif
    sourceTree->setModel(dataModel);
    sourceTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    sourceTree->header()->resizeSection(1, 100);
    sourceTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(sourceTree, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(sourceTreeCustomContextMenu(const QPoint &)));
    connect(dataModel, SIGNAL(connectionEstablished()),
            this, SLOT(connectDataSlots()));
    connect(dataModel,
            SIGNAL(connectionStateChanged(DataModel::ConnectionState)),
            this, SLOT(connectionStateChanged(DataModel::ConnectionState)));
    connect(dataModel, SIGNAL(statsChanged()),
            this, SLOT(statsChanged()));
    connectionStateChanged(DataModel::NoSources);

    propertyModel = new PropertyModel();
#ifdef TM_MODEL_TEST
    qDebug() << "PropertyModel test enabled.";
    new ModelTest(propertyModel, this);
#endif
    propertyTree->setModel(propertyModel);
    propertyTree->header()->resizeSection(0, 192);
    propertyTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(propertyTree, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(propertyTreeCustomContextMenu(const QPoint &)));

    connect(propertyModel,
            SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SLOT(expandProperties()));

    propertyTree->setItemDelegateForColumn(1, propertyDelegate);

    labelIn = new QLabel(this);
    statusBar()->addWidget(labelIn);
    labelOut = new QLabel(this);
    statusBar()->addWidget(labelOut);

    initPython();

    // fill list with available widgets
    Plugin *plugin;
    plugin = new DialPlugin();
    pluginMap.insert(plugin->type(), plugin);
    plugin = new DigitalPlugin();
    pluginMap.insert(plugin->type(), plugin);
    plugin = new GraphPlugin();
    pluginMap.insert(plugin->type(), plugin);
    plugin = new DoubleSpinBoxPlugin();
    pluginMap.insert(plugin->type(), plugin);
    plugin = new PushButtonPlugin();
    pluginMap.insert(plugin->type(), plugin);
    plugin = new QLabelPlugin();
    pluginMap.insert(plugin->type(), plugin);
    plugin = new XYGraphPlugin();
    pluginMap.insert(plugin->type(), plugin);

    on_actionNew_triggered();

    updateWindowTitle();

#if QT_VERSION >= 0x050600
    QList<QDockWidget *> docks;
    docks
        << dockWidgetSources
        << dockWidgetProperties;
    QList<int> sizes;
    sizes << 256 << 350;
    resizeDocks(docks, sizes, Qt::Horizontal);
#endif

    QSettings settings;
    restore = settings.value("restore", restore).toBool();
    recentFiles = settings.value("recentFiles").toStringList();
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
        menuRecentFiles->addAction(recentFileActions[i]);
    }

    updateRecentFileActions();

    menuWindows->addAction(dockWidgetSources->toggleViewAction());
    menuWindows->addAction(dockWidgetProperties->toggleViewAction());
    menuWindows->addAction(dockWidgetPython->toggleViewAction());

#if 0
    pythonBenchmark();
#endif

    QString fileToLoad;

    if (newView) {
        // pass
    }
    else if (!fileName.isEmpty()) {
        fileToLoad = fileName;
    }
    else if (restore && recentFiles.size() > 0) {
        fileToLoad = recentFiles.front();
    }

    if (!fileToLoad.isEmpty()) {
        loadLayout(fileToLoad);
    }

    updateEditMode();

    connect(tabWidget->tabBar(), SIGNAL(tabCloseRequested(int)),
                this, SLOT(tabCloseRequested(int)));

    lineEditPython->installEventFilter(this);
}

/****************************************************************************/

MainWindow::~MainWindow()
{
    tabWidget->clear();

    sourceTree->setModel(NULL);
    delete dataModel;

    delete propertyDelegate;

    Py_Finalize();

    delete [] programName;
}

/****************************************************************************/

PyObject *MainWindow::pythonMethodTabCount(PyObject *, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":tabCount")) {
        return NULL;
    }
    return PyLong_FromLong(singleton->tabWidget->count());
}

/****************************************************************************/

PyObject *MainWindow::pythonMethodMaximize(PyObject *, PyObject *args)
{
    bool max(true);

    if (!PyArg_ParseTuple(args, "|b:maximize", &max)) {
        return NULL;
    }
    Qt::WindowStates states(singleton->windowState());
    if (max) {
        states |= Qt::WindowMaximized;
    }
    else {
        states &= ~Qt::WindowMaximized;
    }
    singleton->setWindowState(states);
    return Py_None;
}

/****************************************************************************/

void MainWindow::signalRaised(int signum)
{
    switch (signum) {
        case SIGINT:
        case SIGTERM:
            qDebug() << "Closing application.";
            close();
            break;
    }
}

/****************************************************************************/

static PyMethodDef pythonModuleMethods[] = {
    {
        "tabCount", // ml_name
        MainWindow::pythonMethodTabCount, // ml_meth
        METH_VARARGS, // ml_flags
        "Return the number of tab pages." // ml_doc
    },
    {
        "maximize", // ml_name
        MainWindow::pythonMethodMaximize, // ml_meth
        METH_VARARGS, // ml_flags
        "Maximize the window." // ml_doc
    },
    {NULL, NULL, 0, NULL}
};

/****************************************************************************/

static PyModuleDef pythonModuleDef = {
    PyModuleDef_HEAD_INIT, // m_base
    "tm", // m_name
    NULL, // m_doc
    -1, // m_size: global state
    pythonModuleMethods, // methods
    NULL, // slots
    NULL, // m_traverse
    NULL, // m_clear
    NULL // m_free
};

/****************************************************************************/

static PyObject *initPythonModule(void)
{
    return PyModule_Create(&pythonModuleDef);
}

/****************************************************************************/

void MainWindow::initPython()
{
    programName = new wchar_t[32];
    size_t ret = mbstowcs(programName, "testmanager", sizeof(programName));
    if (ret == (size_t) -1) {
        qWarning() << "Failed to convert program name.";
    }
    else {
        Py_SetProgramName(programName);
    }

    PyImport_AppendInittab("tm", &initPythonModule);

    Py_Initialize();

    PyObject *sys = PyImport_ImportModule("sys");
    if (!sys) {
        qFatal("Failed to import sys module.");
        return;
    }

    // ----- redirect stdout -----

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, pythonStdOut)) {
        qWarning() << "Couldn't create stdout socket pair:"
            << strerror(errno);
        return;
    }

    pythonStdOutNotifier =
        new QSocketNotifier(pythonStdOut[1], QSocketNotifier::Read, this);
    connect(pythonStdOutNotifier, SIGNAL(activated(int)),
            this, SLOT(pythonStdOutDataAvailable(int)));

    PyObject *out = PyFile_FromFd(pythonStdOut[0], NULL, "w", 1, NULL, NULL,
            NULL, 1);
    if (!out) {
        qFatal("Failed to create python file from stdout socket.");
        return;
    }

    if (PyObject_SetAttrString(sys, "stdout", out)) {
        qFatal("Failed to replace python stdout.");
        return;
    }

    // ----- redirect stderr -----

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, pythonStdErr)) {
        qWarning() << "Couldn't create stderr socket pair:"
            << strerror(errno);
        return;
    }

    pythonStdErrNotifier =
        new QSocketNotifier(pythonStdErr[1], QSocketNotifier::Read, this);
    connect(pythonStdErrNotifier, SIGNAL(activated(int)),
            this, SLOT(pythonStdErrDataAvailable(int)));

    PyObject *err = PyFile_FromFd(pythonStdErr[0], NULL, "w", 1, NULL, NULL,
            NULL, 1);
    if (!err) {
        qFatal("Failed to create python file from stderr socket.");
        return;
    }

    if (PyObject_SetAttrString(sys, "stderr", err)) {
        qFatal("Failed to replace python stderr.");
        return;
    }

    QString banner(QString("Python %1 on %2\n")
            .arg(Py_GetVersion())
            .arg(Py_GetPlatform()));
    textEditPython->setTextColor(QColor(Qt::blue).darker(150));
    textEditPython->append(banner);

#if 0
    ::write(pythonStdOut[0], "stdout\n", 7);
    ::write(pythonStdErr[0], "stderr\n", 7);
#endif

    on_actionTest_triggered();
}

/****************************************************************************/

void MainWindow::loadLayout(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open " << path;
        return;
    }

    QByteArray ba = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc(QJsonDocument::fromJson(ba, &err));

    if (err.error != QJsonParseError::NoError) {
        qCritical() << err.error << err.errorString();
        return;
    }

    filePath = path;
    QFileInfo fi(filePath);
    QString searchPath(fi.absoluteDir().path());
    QDir::setSearchPaths("layout", QStringList(searchPath));

    QJsonObject layoutObject(doc.object());
    QJsonArray tabArray(layoutObject["tabs"].toArray());

    tabWidget->clear();

    foreach (QJsonValue tabValue, tabArray) {
        QJsonObject tabObject(tabValue.toObject());
        QString label(tabObject["name"].toString());
        TabPage *tab = new TabPage(this);
        tab->read(tabObject);
        tabWidget->addTab(tab, label);
    }

    if (layoutObject.contains("styleSheet")) {
        tabWidget->setStyleSheet(layoutObject["styleSheet"].toString());
    }

    updateWindowTitle();
    addRecentFile(path);

    connectDataSlots();
}

/****************************************************************************/

void MainWindow::saveLayout()
{
    QJsonArray tabArray;

    for (int i = 0; i < tabWidget->count(); i++) {
        TabPage *tab = static_cast<TabPage *>(tabWidget->widget(i));
        QJsonObject tabObject;
        tabObject["name"] = tabWidget->tabText(i);
        tab->write(tabObject);
        tabArray.append(tabObject);
    }

    QJsonObject layoutObject;
    layoutObject["version"] = 1;
    layoutObject["tabs"] = tabArray;

    if (!tabWidget->styleSheet().isEmpty()) {
        layoutObject["styleSheet"] = tabWidget->styleSheet();
    }

    QJsonDocument saveDoc(layoutObject);

    QFile saveFile(filePath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        statusBar()->showMessage(tr("Failed to open %1")
                .arg(filePath), 2000);
        return;
    }

    saveFile.write(saveDoc.toJson());
    addRecentFile(filePath);
}

/****************************************************************************/

void MainWindow::updateWindowTitle()
{
    if (filePath.isEmpty()) {
        setWindowFilePath("");
        setWindowTitle(QCoreApplication::applicationName());
    }
    else {
        setWindowTitle("");
        setWindowFilePath(filePath);
    }
}

/****************************************************************************/

void MainWindow::pythonBenchmark()
{
    // add local directory to python path
    PyObject *sysPath = PySys_GetObject("path");
    PyObject *dirString = PyUnicode_FromString(".");
    PyList_Append(sysPath, dirString);

    PyObject *module = PyImport_ImportModule("mult");
    if (!module) {
        PyErr_Print();
        qWarning() << "Failed to import module.";
        return;
    }

    PyObject *func = PyObject_GetAttrString(module, "multiply");
    if (!func || !PyCallable_Check(func)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        qWarning() << "Failed to find func.";
        Py_DECREF(module);
        return;
    }

    PyObject *args = PyTuple_New(2);
    for (int i = 0; i < 2; ++i) {
        PyObject *value = PyLong_FromLong(i + 3);
        if (!value) {
            Py_DECREF(args);
            Py_DECREF(func);
            Py_DECREF(module);
            qWarning() << "Cannot convert argument.";
            return;
        }
        /* pValue reference
         * stolen here: */
        PyTuple_SetItem(args, i, value);
        Py_DECREF(value);
    }


    PyObject *call = NULL;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 1000000; i++) {
        if (call) {
            Py_DECREF(call);
        }
        call = PyObject_CallObject(func, args);
    }

    qDebug() << "The slow operation took"
        << timer.elapsed() << "milliseconds";

    Py_DECREF(args);

    if (call) {
        qDebug()  << "Result of call:" << PyLong_AsLong(call);
        Py_DECREF(call);
    }
    else {
        Py_DECREF(func);
        Py_DECREF(module);
        PyErr_Print();
        qWarning() << "Call failed";
        return;
    }

    Py_DECREF(func);
    Py_DECREF(module);

    timer.start();

    int (*ref)(int, int) = &multiply;

    for (int i = 0; i < 1000000; i++) {
        int res = ref(3, 4);
        res = res;
    }

    qDebug() << "The fast operation took"
        << timer.elapsed() << "milliseconds";
}

/****************************************************************************/

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Up) {
            pythonHistoryStep(-1);
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Down) {
            pythonHistoryStep(1);
            return true;
        }
    }

    return false;
}

/****************************************************************************/

void MainWindow::pythonHistoryStep(int step)
{
    if (pythonHistory.empty()) {
        return;
    }

    int idx = pythonHistoryIndex + step;

    if (idx < 0 || idx >= pythonHistory.size()) {
        return;
    }

    pythonHistoryIndex = idx;
    lineEditPython->setText(pythonHistory[pythonHistoryIndex]);
}

/********************** HACK: QTBUG-16507 workaround ************************/

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    QString filePath = windowFilePath();
    if (!filePath.isEmpty()) {
        setWindowFilePath(filePath + "x");
        setWindowFilePath(filePath);
    }
}

/****************************************************************************/

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;

    settings.setValue("restore", restore);
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    event->accept();
}

/****************************************************************************/

void MainWindow::addRecentFile(const QString &path)
{
    QDir cur = QDir::currentPath();
    QString absPath = cur.absoluteFilePath(path);
    absPath = QDir::cleanPath(absPath);
    recentFiles.removeAll(absPath);
    recentFiles.prepend(absPath);
    updateRecentFileActions();
}

/****************************************************************************/

void MainWindow::updateRecentFileActions()
{
    int numRecentFiles = qMin(recentFiles.size(), (int) MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = QString("&%1 %2").arg(i + 1).arg(recentFiles[i]);
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(recentFiles[i]);
        recentFileActions[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j) {
        recentFileActions[j]->setVisible(false);
    }

    menuRecentFiles->setEnabled(numRecentFiles > 0);
}

/****************************************************************************/

void MainWindow::expandChildren(
        const QModelIndex &index,
        QTreeView *view,
        int depth
        )
{
    if (!depth or !view->model()) {
        return;
    }

    if (!view->isExpanded(index)) {
        view->expand(index);
    }

    if (depth > 0) {
        depth--;
    }

    int rowCount = 0;

    rowCount = view->model()->rowCount(index);

    for (int i = 0; i < rowCount; i++) {
        const QModelIndex &child = view->model()->index(i, 0, index);
        expandChildren(child, view, depth);
    }
}

/****************************************************************************/

void MainWindow::updateEditMode()
{
    dockWidgetProperties->setVisible(editMode);
    tabWidget->setTabsClosable(editMode);
    tabWidget->setMovable(editMode);
    actionAddTab->setEnabled(editMode);

    foreach (TabPage *tab, tabWidget->findChildren<TabPage *>()) {
        tab->editModeChanged();
    }
}

/*****************************************************************************
 * private slots
 ****************************************************************************/

void MainWindow::on_actionNew_triggered()
{
    filePath = QString();
    QDir::setSearchPaths("layout", QStringList());
    updateWindowTitle();

    tabWidget->clear();

    on_actionAddTab_triggered();
}

/****************************************************************************/

void MainWindow::on_actionLoad_triggered()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList filters;
    filters
        << tr("Testmanager Layouts (*.tml)")
        << tr("Any files (*)");

    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix("tml");

    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    QString path = dialog.selectedFiles()[0];
    loadLayout(path);
}

/****************************************************************************/

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) {
        return;
    }

    QString path = action->data().toString();
    loadLayout(path);
}

/****************************************************************************/

void MainWindow::on_actionSave_triggered()
{
    if (filePath.isEmpty()) {
        on_actionSaveAs_triggered();
        return;
    }

    saveLayout();
}

/****************************************************************************/

void MainWindow::on_actionSaveAs_triggered()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    QStringList filters;
    filters
        << "Testmanager Layouts (*.tml)"
        << "Any files (*)";

    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix("tml");

    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    filePath = dialog.selectedFiles()[0];
    QFileInfo fi(filePath);
    QString searchPath(fi.absoluteDir().path());
    QDir::setSearchPaths("layout", QStringList(searchPath));
    updateWindowTitle();

    saveLayout();
}

/****************************************************************************/

void MainWindow::on_actionClose_triggered()
{
    close();
}

/****************************************************************************/

void MainWindow::on_actionConnectAll_triggered()
{
    dataModel->connectAll();
}

/****************************************************************************/

void MainWindow::on_actionConnect_triggered()
{
    ConnectDialog dialog(dataModel);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    DataSource *source = dialog.adoptSource();
    dataModel->append(source);
    connectDataSlots();
}

/****************************************************************************/

void MainWindow::on_actionEditMode_toggled(bool checked)
{
    editMode = checked;
    updateEditMode();
}

/****************************************************************************/

void MainWindow::on_actionGlobalStyleSheet_triggered()
{
    StyleDialog dialog(tabWidget);
    dialog.exec();
}

/****************************************************************************/

void MainWindow::on_actionAddTab_triggered()
{
    TabPage *tab = new TabPage(this);
    tabWidget->addTab(tab, tr("New Tab"));
}

/****************************************************************************/

void MainWindow::on_actionTest_triggered()
{
    PyRun_SimpleString("from time import time, ctime\n"
            "print('Today is', ctime(time()))\n");
}

/****************************************************************************/

void MainWindow::on_actionAboutTestManager_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}

/****************************************************************************/

void MainWindow::on_tabWidget_tabBarDoubleClicked(int index)
{
    if (!editMode) {
        return;
    }

    TabDialog dialog(tabWidget, index, this);
    dialog.exec();
}

/****************************************************************************/

void MainWindow::on_lineEditFilter_textChanged(const QString &text)
{
    if (text.isEmpty()) {
        dataModel->filter(QRegExp());
        return;
    }

    QRegExp re(text, Qt::CaseInsensitive);

    if (re.isValid()) {
        lineEditFilter->setStyleSheet(QString());
        lineEditFilter->setToolTip(QString());
        dataModel->filter(re);
    }
    else {
        lineEditFilter->setStyleSheet("color: red;");
        lineEditFilter->setToolTip(re.errorString());
    }
}

/****************************************************************************/

void MainWindow::on_toolButtonClearFilter_clicked()
{
    lineEditFilter->setText(QString());
}

/****************************************************************************/

void MainWindow::sourceTreeCustomContextMenu(const QPoint &point)
{
    QMenu *menu = new QMenu;
    dataIndex = sourceTree->indexAt(point);
    int rows(dataModel->rowCount(dataIndex));
    DataSource *dataSource(dataModel->getDataSource(dataIndex));

    QAction *action;
    if (dataSource) {
        action = menu->addAction(tr("Connect"),
                this, SLOT(connectDataSource()));
        action->setEnabled(!dataSource->isConnected());
        action = menu->addAction(tr("Disconnect"),
                this, SLOT(disconnectDataSource()));
        action->setEnabled(dataSource->isConnected());
        action = menu->addAction(tr("Remove datasource"),
                this, SLOT(removeDataSource()));
        menu->addSeparator();
    }

    action = menu->addAction(tr("Add datasource..."),
            this, SLOT(on_actionConnect_triggered()));

    if (dataIndex.isValid()) {
        menu->addSeparator();
        action = menu->addAction(tr("Expand complete subtree"),
                this, SLOT(expandData()));
        action->setEnabled(rows > 0);
    }

    menu->exec(sourceTree->viewport()->mapToGlobal(point));
}

/****************************************************************************/

void MainWindow::connectDataSource()
{
    DataSource *dataSource(dataModel->getDataSource(dataIndex));
    if (!dataSource) {
        return;
    }

    if (!dataSource->isConnected()) {
        dataSource->connectToHost();
    }
}

/****************************************************************************/

void MainWindow::disconnectDataSource()
{
    DataSource *dataSource(dataModel->getDataSource(dataIndex));
    if (!dataSource) {
        return;
    }

    if (dataSource->isConnected()) {
        dataSource->disconnectFromHost();
    }
}

/****************************************************************************/

void MainWindow::removeDataSource()
{
    dataModel->removeDataSource(dataIndex);
}

/****************************************************************************/

void MainWindow::expandData()
{
    expandChildren(dataIndex, sourceTree, -1);
}

/****************************************************************************/

void MainWindow::expandProperties()
{
    expandChildren(propertyTree->rootIndex(), propertyTree, 2);
}

/****************************************************************************/

void MainWindow::propertyTreeCustomContextMenu(const QPoint &point)
{
    QMenu *menu = new QMenu;
    QModelIndex index = propertyTree->indexAt(point);

    if (index.isValid()) {
        propertyNode = (PropertyNode *) index.internalPointer();
    }
    else {
        propertyNode = NULL;
    }

    QAction *a = menu->addAction(tr("Reset"), this, SLOT(resetProperty()));
    //a->setIcon(QIcon(":/images/view-refresh.svg"));
    Property *property = dynamic_cast<Property *>(propertyNode);
    if (property) {
        a->setEnabled(property->isSet(propertyModel));
    }
    else {
        a->setEnabled(false);
    }

    menu->exec(propertyTree->viewport()->mapToGlobal(point));
}

/****************************************************************************/

void MainWindow::resetProperty()
{
    if (!propertyNode) {
        return;
    }

    Property *property = dynamic_cast<Property *>(propertyNode);
    if (property) {
        property->reset(propertyModel);
    }
}

/****************************************************************************/

void MainWindow::tabCloseRequested(int index)
{
    tabWidget->removeTab(index);
}

/****************************************************************************/

void MainWindow::pythonStdOutDataAvailable(int)
{
    pythonStdOutNotifier->setEnabled(false);

    QByteArray data;
    data.reserve(1024);
    int ret = ::read(pythonStdOut[1], data.data(), data.capacity());
    if (ret == -1) {
        qWarning() << "Failed to read from python stdout:" << strerror(errno);
        return;
    }
    data.resize(ret);

    textEditPython->moveCursor(QTextCursor::End);
    textEditPython->setTextColor(QColor(Qt::blue).darker(150));
    textEditPython->insertPlainText(data);
    textEditPython->moveCursor(QTextCursor::End);

    pythonStdOutNotifier->setEnabled(true);
}

/****************************************************************************/

void MainWindow::pythonStdErrDataAvailable(int)
{
    pythonStdErrNotifier->setEnabled(false);

    QByteArray data;
    data.reserve(1024);
    int ret = ::read(pythonStdErr[1], data.data(), data.capacity());
    if (ret == -1) {
        qWarning() << "Failed to read from python stderr:" << strerror(errno);
        return;
    }
    data.resize(ret);

    QColor color(textEditPython->textColor());
    textEditPython->moveCursor(QTextCursor::End);
    textEditPython->setTextColor(QColor(Qt::red).darker(150));
    textEditPython->insertPlainText(data);
    textEditPython->moveCursor(QTextCursor::End);

    pythonStdErrNotifier->setEnabled(true);
}

/****************************************************************************/

void MainWindow::on_lineEditPython_returnPressed()
{
    QString text = lineEditPython->text();
    lineEditPython->clear();

    text += "\n";

    textEditPython->moveCursor(QTextCursor::End);
    textEditPython->setTextColor(Qt::black);
    textEditPython->insertPlainText(">>> " + text);
    textEditPython->moveCursor(QTextCursor::End);

    PyRun_InteractiveOneObject(text.toLocal8Bit());

    QString histItem(text.trimmed());
    if (pythonHistory.empty() || pythonHistory.last() != histItem) {
        pythonHistory.append(histItem);
    }
    pythonHistoryIndex = pythonHistory.size();
}

/****************************************************************************/

void MainWindow::connectionStateChanged(DataModel::ConnectionState state)
{
    actionConnectAll->setEnabled(state == DataModel::NoneConnected ||
            state == DataModel::SomeConnected);
}

/****************************************************************************/

void MainWindow::connectDataSlots()
{
    foreach (TabPage *tab, tabWidget->findChildren<TabPage *>()) {
        tab->connectDataSlots();
    }
}

/****************************************************************************/

void MainWindow::statsChanged()
{
    QLocale loc;
    QString num;

    num = loc.toString(dataModel->getInRate() / 1024.0, 'f', 1);
    labelIn->setText(tr("%1 KB/s").arg(num));
    num = loc.toString(dataModel->getOutRate() / 1024.0, 'f', 1);
    labelOut->setText(tr("%1 KB/s").arg(num));
}

/****************************************************************************/
