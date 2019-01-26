/** \example test_user_interaction.cpp
 * JKQTPlotter: Tutorial: User Interaction in JKQTPlotter
 *
 * \ref JKQTPlotterUserInteraction
 * \see \ref JKQTPLOTTER_USERINTERACTION
 */

#include "test_user_interaction.h"
#include <QMenuBar>
#include <QMessageBox>

TestUserInteraction::TestUserInteraction(QWidget *parent) :
    QMainWindow(parent)
{   
    QWidget* w=new QWidget(this);
    setCentralWidget(w);

    // setup layouts for form
    layout=new QGridLayout;
    layForm=new QFormLayout;
    layChk=new QHBoxLayout;
    layout->addLayout(layChk,0,0);
    layout->addLayout(layForm,1,0);

    // generate a JKQTPlotter and initialize some plot data
    plot=new JKQTPlotter(this);
    layout->addWidget(plot,2,0);
    initPlot();

    // add some of the default QActions from the JKQTPlotter to the window menu
    // Some of the are also available in the context menu and toolbar of the JKQTPlotter
    // others are not
    QMenu* menuPlot=menuBar()->addMenu("Plot-Menu");
    menuPlot->addAction(plot->getPlotter()->getActionPrint());
    QMenu* menuPlotS=menuPlot->addMenu("Save ...");
    menuPlotS->addAction(plot->getPlotter()->getActionSaveData());
    menuPlotS->addAction(plot->getPlotter()->getActionSavePDF()); // not available from JKQTPlotter by default
    menuPlotS->addAction(plot->getPlotter()->getActionSavePlot());
    QMenu* menuPlotZ=menuPlot->addMenu("Zoom ...");
    menuPlotZ->addAction(plot->getPlotter()->getActionZoomAll());
    menuPlotZ->addAction(plot->getPlotter()->getActionZoomIn());
    menuPlotZ->addAction(plot->getPlotter()->getActionZoomOut());

    // add a checkbox to show and hide the position display label in the plot
    chkPositionDisplay=new QCheckBox(tr("show mouse cursor position"), this);
    chkPositionDisplay->setChecked(plot->isMousePositionShown());
    connect(chkPositionDisplay, SIGNAL(toggled(bool)), plot, SLOT(setMousePositionShown(bool)));
    layChk->addWidget(chkPositionDisplay);

    // add a checkbox to en-/disable the toolbar of the JKQTPlotter
    chkShowToolbar=new QCheckBox(tr("enable toolbar"), this);
    chkShowToolbar->setChecked(plot->isToolbarVisible());
    connect(chkShowToolbar, SIGNAL(toggled(bool)), plot, SLOT(setToolbarVisible(bool)));
    layChk->addWidget(chkShowToolbar);

    // add a checkbox to switch the toolbar between always visible and the hiding mode,
    // where it only appears, when the mouse hovers over its designated area
    chkToolbarAlwaysOn=new QCheckBox(tr("toolbar 'always on'"), this);
    chkToolbarAlwaysOn->setChecked(plot->isToolbarAlwaysOn());
    connect(chkToolbarAlwaysOn, SIGNAL(toggled(bool)), plot, SLOT(setToolbarAlwaysOn(bool)));
    layChk->addWidget(chkToolbarAlwaysOn);

    // add a checkbox to switch the grid on and off
    chkGrid=new QCheckBox(tr("show grid in plot"), this);
    chkGrid->setChecked(true);
    connect(chkGrid, SIGNAL(toggled(bool)), plot, SLOT(setGrid(bool)));
    layChk->addWidget(chkGrid);

    // add a QComboBox that allows to set the left mouse button action for the JKQTPlotter
    cmbMouseAction=new QComboBox(this);
    layForm->addRow("mouse action:", cmbMouseAction);
    cmbMouseAction->addItem("NoMouseAction");
    cmbMouseAction->addItem("PanPlot=DragPlotWindow");
    cmbMouseAction->addItem("ZoomRectangle");
    cmbMouseAction->addItem("RectangleEvents");
    cmbMouseAction->addItem("CircleEvents");
    cmbMouseAction->addItem("EllipseEvents");
    cmbMouseAction->addItem("LineEvents");
    cmbMouseAction->addItem("ScribbleEvents");
    cmbMouseAction->addItem("ClickEvents");
    connect(cmbMouseAction, SIGNAL(currentIndexChanged(int)), this, SLOT(setLeftMouseAction(int)));
    setLeftMouseAction(cmbMouseAction->currentIndex());
    // and add a QLabel to show the different events of the JKQTPlotter:
    labMouseMoved=new QLabel(this);
    layForm->addRow("last mouse moved:", labMouseMoved);
    labMouseClicked=new QLabel(this);
    layForm->addRow("last mouse clicked:", labMouseClicked);
    labMouseAction=new QLabel(this);
    layForm->addRow("last plotter signal:", labMouseAction);
    connect(plot, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(plot, SIGNAL(plotMouseClicked(double, double, Qt::KeyboardModifiers , Qt::MouseButton)), this, SLOT(plotMouseClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton)));
    connect(plot, SIGNAL(plotMouseDoubleClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton)), this, SLOT(plotMouseDoubleClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton)));
    connect(plot, SIGNAL(plotNewZoomRectangle(double, double, double, double, Qt::KeyboardModifiers)), this, SLOT(plotNewZoomRectangle(double, double, double, double, Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(contextMenuOpened(double, double, QMenu*)), this, SLOT(contextMenuOpened(double, double, QMenu*)));
    connect(plot, SIGNAL(zoomChangedLocally(double, double, double, double, JKQTPlotter*)), this, SLOT(zoomChangedLocally(double, double, double, double, JKQTPlotter*)));
    connect(plot, SIGNAL(userClickFinished(double, double, Qt::KeyboardModifiers)), this, SLOT(userClickFinished(double, double, Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userScribbleClick(double, double, Qt::KeyboardModifiers, bool, bool)), this, SLOT(userScribbleClick(double, double, Qt::KeyboardModifiers, bool, bool)));
    connect(plot, SIGNAL(userRectangleFinished(double, double, double, double, Qt::KeyboardModifiers)), this, SLOT(userRectangleFinished(double, double, double, double, Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userLineFinished(double, double, double, double, Qt::KeyboardModifiers)), this, SLOT(userLineFinished(double, double, double, double, Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userCircleFinished(double, double, double, Qt::KeyboardModifiers)), this, SLOT(userCircleFinished(double, double, double, Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userEllipseFinished(double, double, double, double, Qt::KeyboardModifiers)), this, SLOT(userEllipseFinished(double, double, double, double, Qt::KeyboardModifiers)));


    w->setLayout(layout);
    resize(800,600);
}

void TestUserInteraction::setLeftMouseAction(int index)
{
    plot->setMouseActionMode(static_cast<JKQTPlotter::MouseActionModes>(index));
}

void TestUserInteraction::plotMouseMove(double x, double y)
{
    labMouseMoved->setText(QString("plotMouseMove(%1, %2)").arg(x).arg(y));
}

void TestUserInteraction::plotMouseClicked(double x, double y, Qt::KeyboardModifiers modifiers, Qt::MouseButton button)
{
    labMouseClicked->setText(QString("plotMouseClicked(%1, %2, modifiers=%3, button=%4)").arg(x).arg(y).arg(KeyboradMod2String(modifiers)).arg(MouseButton2String(button)));
}

void TestUserInteraction::plotMouseDoubleClicked(double x, double y, Qt::KeyboardModifiers modifiers, Qt::MouseButton button)
{
    labMouseClicked->setText(QString("plotMouseDoubleClicked(%1, %2, modifiers=%3, button=%4)").arg(x).arg(y).arg(KeyboradMod2String(modifiers)).arg(MouseButton2String(button)));
}

void TestUserInteraction::plotNewZoomRectangle(double mouseDragRectXStart, double mouseDragRectXEnd, double mouseDragRectYStart, double mouseDragRectYEnd, Qt::KeyboardModifiers modifiers)
{
    labMouseAction->setText(QString("plotNewZoomRectangle(x=%1..%2, y=%3..%4, modifiers=%5)").arg(mouseDragRectXStart).arg(mouseDragRectXEnd).arg(mouseDragRectYStart).arg(mouseDragRectYEnd).arg(KeyboradMod2String(modifiers)));
}

void TestUserInteraction::contextMenuOpened(double x, double y, QMenu *contextMenu)
{
    contextMenu->addSeparator();
    QAction* act=contextMenu->addMenu(QString("contextMenuOpened(x=%1, y=%2)").arg(x).arg(y))->addAction("user-added action");
    connect(act, &QAction::triggered, [x,y]() { QMessageBox::warning(nullptr, tr("Plot Context Menu"),
                                                                     tr("Context Menu was opened at x/y=%1/%2!").arg(x).arg(y),
                                                                     QMessageBox::Ok,
                                                                     QMessageBox::Ok); });
    labMouseAction->setText(QString("contextMenuOpened(x=%1, y=%2)").arg(x).arg(y));
}

void TestUserInteraction::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQTPlotter */*sender*/)
{
    labMouseAction->setText(QString("zoomChangedLocally(x=%1..%2, y=%3..%4)").arg(newxmin).arg(newxmax).arg(newymin).arg(newymax));
}

void TestUserInteraction::userClickFinished(double x, double y, Qt::KeyboardModifiers modifiers)
{
    labMouseAction->setText(QString("userClickFinished(%1, %2, modifiers=%3)").arg(x).arg(y).arg(KeyboradMod2String(modifiers)));
}

void TestUserInteraction::userScribbleClick(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last)
{
    static int counter=0;
    if (first) counter=0;
    else counter++;
    labMouseAction->setText(QString("userClickFinished(%1, %2, modifiers=%3, first=%4, last=%5) -> scribble-points so far: %6").arg(x).arg(y).arg(KeyboradMod2String(modifiers)).arg(first).arg(last).arg(counter));
}

void TestUserInteraction::userRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers)
{
    labMouseAction->setText(QString("userRectangleFinished(x=%1, y=%2, width=%3, height=%4, modifiers=%5)").arg(x).arg(y).arg(width).arg(height).arg(KeyboradMod2String(modifiers)));
}

void TestUserInteraction::userLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers)
{
    labMouseAction->setText(QString("userLineFinished(x1=%1/%2, x2=%3/%4, modifiers=%5)").arg(x1).arg(y1).arg(x2).arg(y2).arg(KeyboradMod2String(modifiers)));
}

void TestUserInteraction::userCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers)
{
    labMouseAction->setText(QString("userRectangleFinished(x=%1, y=%2, radius=%3, modifiers=%4)").arg(x).arg(y).arg(radius).arg(KeyboradMod2String(modifiers)));
}

void TestUserInteraction::userEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers)
{
    labMouseAction->setText(QString("userRectangleFinished(x=%1, y=%2, radiusX=%3, radiusY=%4, modifiers=%5)").arg(x).arg(y).arg(radiusX).arg(radiusY).arg(KeyboradMod2String(modifiers)));
}

QString TestUserInteraction::KeyboradMod2String(Qt::KeyboardModifiers modifiers) {
    QString mod="";
    if ((modifiers & Qt::ShiftModifier) != 0) mod+="SHIFT ";
    if ((modifiers & Qt::ControlModifier) != 0) mod+="CTRL ";
    if ((modifiers & Qt::AltModifier) != 0) mod+="ALT ";
    if ((modifiers & Qt::MetaModifier) != 0) mod+="META ";
    if ((modifiers & Qt::KeypadModifier) != 0) mod+="KEYPAD ";
    return mod;
}

QString TestUserInteraction::MouseButton2String(Qt::MouseButton button) {
    QString btn="";
    if ((button & Qt::LeftButton) != 0) btn+="LEFT ";
    if ((button & Qt::RightButton) != 0) btn+="RIGHT ";
    if ((button & Qt::MiddleButton) != 0) btn+="MID ";
    if ((button & Qt::BackButton) != 0) btn+="BACK ";
    if ((button & Qt::ForwardButton) != 0) btn+="FORWARD ";
    return btn;
}


void TestUserInteraction::initPlot()
{
    // 1. create a plotter window and get a pointer to the internal datastore (for convenience)
    JKQTPDatastore* ds=plot->getDatastore();

    // 2. now we create data for a simple plot (a sine curve)
    QVector<double> X, Y1, Y2;
    const int Ndata=100;
    for (int i=0; i<Ndata; i++) {
        const double x=double(i)/double(Ndata)*8.0*M_PI;
        X<<x;
        Y1<<sin(x);
        Y2<<sin(x)*exp(-0.2*x);
    }

    // 3. make data available to JKQTPlotter by adding it to the internal datastore.
    //    Note: In this step the data is copied (of not specified otherwise), so you can
    //          reuse X and Y afterwards!
    //    the variables columnX and columnY will contain the internal column ID of the newly
    //    created columns with names "x" and "y" and the (copied) data from X and Y.
    size_t columnX=ds->addCopiedColumn(X, "x");
    size_t columnY1=ds->addCopiedColumn(Y1, "y1");
    size_t columnY2=ds->addCopiedColumn(Y2, "y2");

    // 4. create a graph in the plot, which plots the dataset X/Y:
    JKQTPXYLineGraph* graph1=new JKQTPXYLineGraph(plot);
    graph1->setXColumn(columnX);
    graph1->setYColumn(columnY1);
    graph1->setTitle(QObject::tr("sine graph"));
    plot->addGraph(graph1);

    JKQTPXYLineGraph* graph2=new JKQTPXYLineGraph(plot);
    graph2->setXColumn(columnX);
    graph2->setYColumn(columnY2);
    graph2->setTitle(QObject::tr("sine*exp graph"));
    plot->addGraph(graph2);

    // 6. autoscale the plot so the graph is contained
    plot->zoomToFit();

    plot->getPlotter()->setPlotLabel("Sine Plots - User-Interaction Example");
}
