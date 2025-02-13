﻿#include "MainWindow.h"
#include "Helper.h"
#include "TagManager.h"
#include "DrawListUtils.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QFileInfo>
#include <QRect>
#include <QGraphicsView>
#include <QFileDialog>
#include <QScrollArea>
#include <QToolBar>
#include <QDebug>


MainWindow::MainWindow(const QString &szProjPath,
                       const QString &szProjName,
                       const QString &szGraphPageName,
                       QWidget *parent) :
    QMainWindow(parent),
    currentGraphPage_(nullptr),
    currentView_(nullptr),
    gridVisible_(true),
    currentGraphPageIndex_(0),
    szProjPath_(szProjPath),
    szProjName_(szProjName),
    graphPageName_(szGraphPageName)
{
    setupUi(this);

    undoGroup_ = new QUndoGroup(this);
    createActions();
    createMenus();
    createToolbars();
    initView();
    slotUpdateActions();

    setWindowState(Qt::WindowMaximized);
    setWindowTitle(tr("画面编辑器"));
    setWindowIcon(QIcon(":/images/application.png"));

    connect(graphPageTabWidget_, SIGNAL(currentChanged(int)), SLOT(slotChangeGraphPage(int)));

    QDesktopWidget * pDesktopWidget = QApplication::desktop();
    QRect rect = pDesktopWidget->screenGeometry();
    int screenWidth = rect.width();
    int screenHeight = rect.height();
    this->resize(screenWidth*3/4, screenHeight*3/4);

    Helper::WidgetMoveCenter(this);

    TagManager::setProjectPath(szProjPath_);
    DrawListUtils::setProjectPath(szProjPath_);
}


MainWindow::~MainWindow()
{

}


void MainWindow::initView()
{
    graphPageTabWidget_ = new QTabWidget(this);
    graphPageTabWidget_->installEventFilter(this);
    this->scrollArea->setWidget(graphPageTabWidget_);

    elementWidget_ = new ElementLibraryWidget();
    this->ElemetsLayout->addWidget(elementWidget_);

    propertyModel_ = new PropertyModel();
    propertyView_ = new PropertyTableView(propertyModel_, false);
    this->PropertyLayout->addWidget(propertyView_);

    objTree_ = new ObjectsTreeView();
    this->ObjectTreeLayout->addWidget(objTree_);
    slotShowTreeObj(false);
}

void MainWindow::createActions()
{

    actionShowGraphObj_ = new QAction(tr("图形元素"), this);
    actionShowGraphObj_->setCheckable(true);
    actionShowGraphObj_->setChecked(true);
    connect(actionShowGraphObj_, SIGNAL(triggered(bool)), SLOT(slotShowGraphObj(bool)));

    actionShowTreeObj_ = new QAction(tr("对象树"), this);
    actionShowTreeObj_->setCheckable(true);
    actionShowTreeObj_->setChecked(false);
    connect(actionShowTreeObj_, SIGNAL(triggered(bool)), SLOT(slotShowTreeObj(bool)));

    actionShowPropEditor_ = new QAction(tr("属性编辑器"), this);
    actionShowPropEditor_->setCheckable(true);
    actionShowPropEditor_->setChecked(true);
    connect(actionShowPropEditor_, SIGNAL(triggered(bool)), SLOT(slotShowPropEditor(bool)));

    actionNew = new QAction(QIcon(":/images/filenew.png"), tr("新建"), this);
    actionNew->setShortcut(QString("Ctrl+N"));
    connect(actionNew, SIGNAL(triggered()), SLOT(slotEditNew()));

    actionOpen = new QAction(QIcon(":/images/fileopen.png"), tr("打开"), this);
    actionOpen->setShortcut(QString("Ctrl+O"));
    connect(actionOpen, SIGNAL(triggered()), SLOT(slotEditOpen()));

    actionSaveGraphPage_ = new QAction(QIcon(":/images/saveproject.png"), tr("保存"), this);
    actionSaveGraphPage_->setShortcut(QKeySequence::Save);
    connect(actionSaveGraphPage_, SIGNAL(triggered()), SLOT(slotSaveGraphPage()));

    actionCloseGraphPage_ = new QAction(tr("关闭"), this);
    connect(actionCloseGraphPage_, SIGNAL(triggered()), SLOT(slotCloseGraphPage()));

    actionCloseAll_ = new QAction(tr("关闭所有"), this);
    connect(actionCloseAll_, SIGNAL(triggered()), SLOT(slotCloseAll()));

    actionExit_ = new QAction(tr("退出"),this);
    actionExit_->setShortcut(QKeySequence::Quit);
    connect(actionExit_,SIGNAL(triggered()),SLOT(slotExit()));

    actionShowGrid = new QAction(QIcon(":/images/showgrid.png"), tr("显示栅格"), this);
    actionShowGrid->setCheckable(true);
    actionShowGrid->setChecked(false);
    connect(actionShowGrid, SIGNAL(triggered(bool)), SLOT(slotShowGrid(bool)));

    actionShowLinear_ = new QAction(QIcon(":/images/ruler.png"), tr("显示线条"), this);
    actionShowLinear_->setCheckable(true);
    actionShowLinear_->setChecked(false);
    connect(actionShowLinear_, SIGNAL(triggered(bool)), SLOT(slotShowLinear(bool)));

    actionZoomIn_ = new QAction(QIcon(":/images/zoom-in.png"), tr("放大"), this);
    connect(actionZoomIn_, SIGNAL(triggered()), SLOT(slotZoomIn()));

    actionZoomOut_ = new QAction(QIcon(":/images/zoom-out.png"), tr("缩小"), this);
    connect(actionZoomOut_, SIGNAL(triggered()), SLOT(slotZoomOut()));

    actionUndo_ = undoGroup_->createUndoAction(this);
    actionUndo_->setIcon(QIcon(":/images/undo.png"));
    actionUndo_->setText(tr("撤销"));
    actionUndo_->setShortcut(QKeySequence::Undo);

    actionRedo_ = undoGroup_->createRedoAction(this);
    actionRedo_->setText(tr("重做"));
    actionRedo_->setIcon(QIcon(":/images/redo.png"));
    actionRedo_->setShortcut(QKeySequence::Redo);
}

void MainWindow::createMenus()
{
    QMenu *filemenu = new QMenu(tr("文件"), this);
#if 0  // for test we need this
    filemenu->addAction(actionNew);
    filemenu->addAction(actionOpen);
#endif
    filemenu->addAction(actionSaveGraphPage_);
    filemenu->addSeparator();
    filemenu->addAction(actionCloseGraphPage_);
    filemenu->addAction(actionCloseAll_);
    filemenu->addSeparator();
    filemenu->addAction(actionExit_);

    QMenu *windowMenu = new QMenu(tr("窗口"), this);
    windowMenu->addAction(actionShowGraphObj_);
    windowMenu->addAction(actionShowTreeObj_);
    windowMenu->addAction(actionShowPropEditor_);

    QMainWindow::menuBar()->addMenu(filemenu);
    QMainWindow::menuBar()->addMenu(windowMenu);
}

void MainWindow::createToolbars()
{

    toolBar->addAction(actionSaveGraphPage_);
    toolBar->addSeparator();
    toolBar->addAction(actionShowGrid);
    //toolBar->addAction(actionShowLinear);
    toolBar->addAction(actionZoomOut_);
    toolBar->addAction(actionZoomIn_);
    toolBar->addSeparator();
    toolBar->addAction(actionUndo_);
    toolBar->addAction(actionRedo_);
    toolBar->addSeparator();
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    bool unsaved = false;

    QListIterator<GraphPage*> it(GraphPageManager::getInstance()->getGraphPageList());

    while (it.hasNext()) {
        GraphPage *graphPage = it.next();
        if (!graphPage->undoStack()->isClean() || graphPage->getUnsavedFlag()) {
            unsaved = true;
        }
    }

    if (unsaved) {
        int ret = exitResponse();

        if (ret == QMessageBox::Yes) {
            slotSaveGraphPage();
            event->accept();
        } else if (ret == QMessageBox::No) {
            event->accept();
        }
    } else {
        event->accept();
    }
}


/**
 * @brief openGraphPage
 * @details 打开画面
 * @param pagePath 画面路径
 * @param pagePath 画面名称
 */
void MainWindow::openGraphPage(const QString &szProjPath,
                               const QString &szProjName,
                               const QString &szPageName)
{
    QString fileName = szProjPath + "/" + szPageName + ".drw";

    if (fileName.toLower().endsWith(".drw")) {

        QGraphicsView *view = createTabView();

        if (graphPageTabWidget_->indexOf(view) != -1) {
            delete view;
            return;
        }

        GraphPage *graphPage = new GraphPage(QRectF());
        if (!createDocument(graphPage, view, fileName)) {
            return;
        }
        graphPage->setProjectPath(szProjPath);
        graphPage->setProjectName(szProjName);
        graphPage->loadAsXML(fileName);
        view->setFixedSize(graphPage->getGraphPageWidth(), graphPage->getGraphPageHeight());
        graphPage->fillGraphPagePropertyModel();
        graphPage->setFileName(szPageName + ".drw");
        graphPage->setGraphPageId(szPageName);
    }
}


void MainWindow::slotEditNew()
{

    addNewGraphPage();
}

QString MainWindow::fixedWindowTitle(const QGraphicsView *viewGraphPage) const
{

    QString title = currentGraphPage_->getGraphPageId();

    if (title.isEmpty()) {
        title = "Untitled";
    }
    else {
        title = QFileInfo(title).fileName();
    }

    QString result;

    for (int i = 0; ;++i) {
        result = title;

        if (i > 0) {
            result += QString::number(i);
        }

        bool unique = true;

        for (int j = 0; j < graphPageTabWidget_->count(); ++j) {
            const QWidget *widget = graphPageTabWidget_->widget(j);

            if (widget == viewGraphPage) {
                continue;
            }

            if (result == graphPageTabWidget_->tabText(j)) {
                unique = false;
                break;
            }
        }

        if (unique) {
            break;
        }
    }

    return result;
}

bool MainWindow::isGraphPageOpen(const QString &filename)
{
    QListIterator <GraphPage*> it(GraphPageManager::getInstance()->getGraphPageList());

    while (it.hasNext()) {
        if (filename == it.next()->getFileName()) {
            return true;
        }
    }

    return false;
}

void MainWindow::addNewGraphPage()
{
    QGraphicsView *view = createTabView();

    if (graphPageTabWidget_->indexOf(view) != -1) {
        delete view;
        return;
    }

    GraphPage *graphPage = new GraphPage(QRectF());
    graphPage->setProjectPath(szProjPath_);
    graphPage->setProjectName(szProjName_);
    graphPage->setGridVisible(gridVisible_);
    currentGraphPage_ = graphPage;
    view->setScene(graphPage);
    view->setFixedSize(graphPage->getGraphPageWidth(), graphPage->getGraphPageHeight());
    currentView_ = view;
    QString title = fixedWindowTitle(view);
    graphPage->setFileName(title + ".drw");
    graphPage->setGraphPageId(title);
    graphPage->setPropertyModel(propertyModel_);
    graphPageTabWidget_->addTab(currentView_, title);
    graphPageTabWidget_->setCurrentWidget(currentView_);
    GraphPageManager::getInstance()->addGraphPage(graphPage);

    undoGroup_->addStack(graphPage->undoStack());
    undoGroup_->setActiveStack(graphPage->undoStack());

    connectGraphPage(graphPage);
}

void MainWindow::connectGraphPage(GraphPage *graphPage)
{
    connect(graphPage->undoStack(), SIGNAL(indexChanged(int)), SLOT(slotUpdateActions()));
    connect(graphPage->undoStack(), SIGNAL(cleanChanged(bool)), SLOT(slotUpdateActions()));
    connect(graphPage, SIGNAL(newElementAdded()), SLOT(slotNewElementAdded()));
    connect(graphPage, SIGNAL(elementsDeleted()), SLOT(slotElementsDeleted()));
    connect(graphPage, SIGNAL(elementIdChanged()), SLOT(slotElementIdChanged()));
    connect(graphPage, SIGNAL(changeGraphPageName()), SLOT(slotChangeGraphPageName()));
    connect(graphPage, SIGNAL(selectionChanged()), SLOT(slotUpdateActions()));
    connect(graphPage, SIGNAL(elementPropertyChanged()), SLOT(slotUpdateActions()));
    connect(graphPage, SIGNAL(GraphPagePropertyChanged()), SLOT(slotUpdateActions()));
    connect(graphPage, SIGNAL(GraphPageSaved()), SLOT(slotUpdateActions()));
}

void MainWindow::disconnectGraphPage(GraphPage *graphPage)
{
    disconnect(graphPage->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(slotUpdateActions()));
    disconnect(graphPage->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(slotUpdateActions()));
    disconnect(graphPage, SIGNAL(newElementAdded()), this, SLOT(slotNewElementAdded()));
    disconnect(graphPage, SIGNAL(elementsDeleted()), this, SLOT(slotElementsDeleted()));
    disconnect(graphPage, SIGNAL(elementIdChanged()), this, SLOT(slotElementIdChanged()));
    disconnect(graphPage, SIGNAL(changeGraphPageName()), this, SLOT(slotChangeGraphPageName()));
    disconnect(graphPage, SIGNAL(selectionChanged()), this, SLOT(slotUpdateActions()));
    disconnect(graphPage, SIGNAL(elementPropertyChanged()), this, SLOT(slotUpdateActions()));
    disconnect(graphPage, SIGNAL(GraphPagePropertyChanged()), this, SLOT(slotUpdateActions()));
    disconnect(graphPage, SIGNAL(GraphPageSaved()), this, SLOT(slotUpdateActions()));
}

QGraphicsView *MainWindow::createTabView()
{
    QGraphicsView *view = new QGraphicsView;
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    return view;
}

void MainWindow::slotUpdateActions()
{
    static const QIcon unsaved(":/images/filesave.png");

    for (int i = 0; i < graphPageTabWidget_->count(); i++) {
        QGraphicsView *view = (QGraphicsView*)graphPageTabWidget_->widget(i);
        GraphPage *scene = dynamic_cast<GraphPage *>(view->scene());
        view->setFixedSize(scene->getGraphPageWidth(), scene->getGraphPageHeight());
        if (!((GraphPage*)view->scene())->undoStack()->isClean() ||
            ((GraphPage*)view->scene())->getUnsavedFlag()) {
            graphPageTabWidget_->setTabIcon(graphPageTabWidget_->indexOf(view),unsaved);
        } else {
            graphPageTabWidget_->setTabIcon(graphPageTabWidget_->indexOf(view),QIcon());
        }
    }

    actionZoomIn_->setEnabled(graphPageTabWidget_->count() ? true : false);
    actionZoomOut_->setEnabled(graphPageTabWidget_->count() ? true : false);
    actionShowGrid->setEnabled(graphPageTabWidget_->count() ? true : false);

    if (!currentGraphPage_) {
        return;
    }

    undoGroup_->setActiveStack(currentGraphPage_->undoStack());

    if (!currentGraphPage_->undoStack()->isClean() || currentGraphPage_->getUnsavedFlag()) {
        actionSaveGraphPage_->setEnabled(true);
    } else {
        actionSaveGraphPage_->setEnabled(false);
    }
}

void MainWindow::slotChangeGraphPage(int GraphPageNum)
{
    if (GraphPageNum == -1) {
        objTree_->clearModel();
        propertyModel_->resetModel();
        return;
    }

    for (int i = 0; i < graphPageTabWidget_->count(); i++) {
        QGraphicsView *view = (QGraphicsView*)graphPageTabWidget_->widget(i);
        ((GraphPage*)view->scene())->setActive(false);
    }

    currentView_ = (QGraphicsView*)graphPageTabWidget_->widget(GraphPageNum);
    currentGraphPage_ = (GraphPage*)currentView_->scene();
    currentGraphPage_->setActive(true);
    currentGraphPage_->fillGraphPagePropertyModel();
    objTree_->setContentList(currentGraphPage_->items());
    objTree_->updateContent();
    currentGraphPageIndex_ = GraphPageNum;
    slotUpdateActions();
}

void MainWindow::slotChangeGraphPageName()
{
    graphPageTabWidget_->setTabText(currentGraphPageIndex_, currentGraphPage_->getGraphPageId());
    int index = GraphPageManager::getInstance()->getIndexByGraphPage(currentGraphPage_);
}

void MainWindow::updateObjectTree()
{
    objTree_->setContentList(currentGraphPage_->items());
    objTree_->updateContent();
}

void MainWindow::slotElementIdChanged()
{
    updateObjectTree();
}

void MainWindow::slotElementPropertyChanged()
{

}

void MainWindow::slotGraphPagePropertyChanged()
{

}

void MainWindow::slotNewElementAdded()
{
    updateObjectTree();
}

void MainWindow::slotElementsDeleted()
{
    updateObjectTree();
}

void MainWindow::slotShowGrid(bool on)
{
    QListIterator <GraphPage*> iter(GraphPageManager::getInstance()->getGraphPageList());

    while (iter.hasNext()) {
        iter.next()->setGridVisible(on);
    }

    gridVisible_ = on;
}

void MainWindow::slotShowGraphObj(bool on)
{
    on ? this->dockElemets->show() : this->dockElemets->hide();
}

void MainWindow::slotShowTreeObj(bool on)
{
    on ? this->dockObjectTree->show() : this->dockObjectTree->hide();
}

void MainWindow::slotShowPropEditor(bool on)
{
    on ? this->dockProperty->show() : this->dockProperty->hide();
}


void MainWindow::slotCloseAll()
{
    while (graphPageTabWidget_->count()) {
        QGraphicsView *view = static_cast<QGraphicsView*>(graphPageTabWidget_->widget(graphPageTabWidget_->currentIndex()));
        removeGraphPage(view);
        delete view;
    }

    currentView_ = 0;
    currentGraphPage_ = 0;
    slotUpdateActions();
}

void MainWindow::removeGraphPage(QGraphicsView *view)
{
    int index = graphPageTabWidget_->indexOf(view);
    GraphPage *graphPage = static_cast<GraphPage*>(view->scene());

    if (index == -1)
        return;

    if (!graphPage->undoStack()->isClean()) {
        int ret = exitResponse();

        if (ret == QMessageBox::Yes) {
            slotSaveGraphPage();
        }
    }

    graphPageTabWidget_->removeTab(index);
    undoGroup_->removeStack(graphPage->undoStack());
    GraphPageManager::getInstance()->removeGraphPage(graphPage);
    disconnectGraphPage(graphPage);
    delete graphPage;
}

void MainWindow::slotCloseGraphPage()
{
    QGraphicsView *view = currentView_;
    removeGraphPage(view);
    delete view;

    if (graphPageTabWidget_->count() == 0) {
        currentGraphPage_ = 0;
        currentView_ = 0;
    }

    slotUpdateActions();
}

void MainWindow::slotEditOpen()
{
    const QString &filename = QFileDialog::getOpenFileName(this,
                                                           tr("Open"),
                                                           ".",
                                                           tr("GraphPage (*.drw)"));
    if (filename.isEmpty())
        return;
#if 0
    if (filename.toLower().endsWith(".drwb")) {

        QGraphicsView *view = createTabView();

        if (graphPageTabWidget_->indexOf(view) != -1) {
            delete view;
            return;
        }

        GraphPage *graphPage = new GraphPage(QRectF());
        if (!createDocument(graphPage,view,filename)) {
            return;
        }
        graphPage->loadAsBinary(filename);
    }
#endif
    if (filename.toLower().endsWith(".drw")) {

        QGraphicsView *view = createTabView();

        if (graphPageTabWidget_->indexOf(view) != -1) {
            delete view;
            return;
        }

        GraphPage *graphPage = new GraphPage(QRectF());
        if (!createDocument(graphPage, view, filename)) {
            return;
        } 
        graphPage->setProjectPath(szProjPath_);
        graphPage->setProjectName(szProjName_);
        graphPage->loadAsXML(filename);
        int pos = filename.lastIndexOf("/");
        QString pageFileName = "";
        if (pos != -1) {
            pageFileName = filename.right(filename.length() - pos - 1);
        }
        graphPage->setFileName(pageFileName);
        graphPage->setGraphPageId(pageFileName.left(pageFileName.length() - 4));

    }
}

bool MainWindow::createDocument(GraphPage *graphPage,
                                QGraphicsView *view,
                                const QString &filename)
{
    if (isGraphPageOpen(filename)) {
        QMessageBox::information(this,
                                 tr("打开文件错误"),
                                 tr("文件已打开"),
                                 QMessageBox::Ok);
        delete graphPage;
        delete view;
        return false;
    }

    graphPage->setGridVisible(gridVisible_);
    currentGraphPage_ = graphPage;
    view->setScene(graphPage);
    view->setFixedSize(graphPage->getGraphPageWidth(), graphPage->getGraphPageHeight());
    currentView_ = view;
    graphPage->setPropertyModel(propertyModel_);
    graphPageTabWidget_->addTab(currentView_, graphPage->getGraphPageId());
    graphPageTabWidget_->setCurrentWidget(view);
    GraphPageManager::getInstance()->addGraphPage(graphPage);

    undoGroup_->addStack(graphPage->undoStack());
    undoGroup_->setActiveStack(graphPage->undoStack());

    connectGraphPage(graphPage);

    graphPage->undoStack()->setClean();

    return true;
}


QString MainWindow::getFileName()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save as"),
                                                    QString("./%1").arg(currentGraphPage_->getGraphPageId()),
                                                    tr("GraphPage(*.drw)"));
    return filename;
}

void MainWindow::updateGraphPageViewInfo(const QString &fileName)
{
    int index = graphPageTabWidget_->indexOf(currentView_);
    QFileInfo file(fileName);
    currentGraphPage_->setGraphPageId(file.baseName());
    graphPageTabWidget_->setTabText(index,file.baseName());
    slotChangeGraphPageName();
}

void MainWindow::slotSaveGraphPage()
{
    if (!currentGraphPage_) {
        return;
    }

    for (;;) {
        QString fileName = currentGraphPage_->getFileName();

        if (fileName.isEmpty())
            fileName = getFileName();

        if (fileName.isEmpty())
            break;

        currentGraphPage_->setFileName(fileName);
        updateGraphPageViewInfo(fileName);
        currentGraphPage_->saveAsXML(szProjPath_ + "/" + fileName);
#if 0
        if (fileName.toLower().endsWith(".drw")) {
            QString binaryFileName = fileName.toLower()+ "b"; // ".drw"==>".drwb"
            currentGraphPage->saveAsBinary(szProjPath_ + "/" + binaryFileName);
        }
#endif
        break;

    }
}

void MainWindow::slotExit()
{
    if (graphPageTabWidget_->count() == 0) {
        QApplication::quit();
        return;
    }

    bool unsaved = false;

    QListIterator<GraphPage*> it(GraphPageManager::getInstance()->getGraphPageList());

    while (it.hasNext()) {
        if (!it.next()->undoStack()->isClean()) {
            unsaved = true;
        }
    }

    if (unsaved) {
        int ret = exitResponse();
        if (ret == QMessageBox::Yes) {
            slotSaveGraphPage();
            QApplication::quit();
        } else if (ret == QMessageBox::No) {
            QApplication::quit();
        }
    }

    QApplication::quit();
}

int MainWindow::exitResponse()
{
    int ret = QMessageBox::information(this,
                                       tr("退出程序"),
                                       tr("文件已修改。是否保存?"),
                                       QMessageBox::Yes | QMessageBox::No);
    return ret;
}

void MainWindow::slotShowLinear(bool on) {
    Q_UNUSED(on)
}

void MainWindow::slotZoomIn()
{
    if(currentGraphPage_ != nullptr) {
        int width = currentGraphPage_->getGraphPageWidth();
        int height = currentGraphPage_->getGraphPageHeight();
        currentGraphPage_->setGraphPageWidth(width * 1.25);
        currentGraphPage_->setGraphPageHeight(height * 1.25);
        currentGraphPage_->setGridVisible(currentGraphPage_->isGridVisible());
    }
    if (currentView_ != nullptr) {
        currentView_->scale(1.25, 1.25);
        currentView_->setFixedSize(currentGraphPage_->getGraphPageWidth(), currentGraphPage_->getGraphPageHeight());
    }
}

void MainWindow::slotZoomOut()
{
    if(currentGraphPage_ != nullptr) {
        int width = currentGraphPage_->getGraphPageWidth();
        int height = currentGraphPage_->getGraphPageHeight();
        currentGraphPage_->setGraphPageWidth(width * 1/1.25);
        currentGraphPage_->setGraphPageHeight(height * 1/1.25);
        currentGraphPage_->setGridVisible(currentGraphPage_->isGridVisible());
    }
    if (currentView_ != nullptr) {
        currentView_->scale(1/1.25, 1/1.25);
        currentView_->setFixedSize(currentGraphPage_->getGraphPageWidth(), currentGraphPage_->getGraphPageHeight());
    }
}









