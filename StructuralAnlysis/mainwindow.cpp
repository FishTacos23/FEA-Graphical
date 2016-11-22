#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "tokenize.h"
#include <fstream>
#include "analyze.h"
#include <iostream>
#include <QGraphicsLineItem>
#include <QPointF>
#include <QGraphicsView>
#include <QGraphicsPolygonItem>
#include <QWheelEvent>
#include <QToolBar>
#include <QLineEdit>
#include <QLayout>
#include <QRadioButton>
#include <QPicture>
#include <QMessageBox>

// to DO //////////////
// user select joints
// zoom and pan
// stop leaking memory
// property numbers
// if already exists
// when you go back to a previous section it no longer shows the toolbar
// move text for positive y and rz text right
// ////////////////////

// global variables
static Analyze myStructure;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::showMaximized();

    solved = false;
    displace = false;
    constraint = false;
    force = false;
    jToolBarActive = false;
    mToolBarActive = false;
    cToolBarActive = false;
    fToolBarActive = false;
    pToolBarActive = false;

    // set up graphics scene
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->setBackgroundBrush(Qt::black);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{

    // get file name
    QString fileNameQ = QFileDialog::getOpenFileName(this,"Open Shape File", "","*.txt");
    std::string fileName = fileNameQ.toStdString();

    // check file for errors

    try
    {
        readFile(fileName);

        zoom = 1;

        drawMembers();
        drawJoint();
    }
    catch(std::invalid_argument)
    {
        QMessageBox noGood;

        noGood.setText("File Formatt is Incorrect");
        noGood.setInformativeText("please follow example file");
        noGood.setStandardButtons(QMessageBox::Ok);
        noGood.setDefaultButton(QMessageBox::Ok);
        noGood.setWindowTitle("WARNING");
        noGood.exec();
    }
}

void MainWindow::on_actionSave_Results_triggered()
{
    // write results to file
}

void MainWindow::on_actionClear_triggered()
{

    QMessageBox sure;

    sure.setText("Are you sure you want to clear?");
    sure.setInformativeText("You may not undo this action.");
    sure.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    sure.setDefaultButton(QMessageBox::Cancel);
    sure.setWindowTitle("WARNING");
    int ret = sure.exec();

    switch (ret) {
    case QMessageBox::Ok:

        // clear the screan
        scene->clear();

        clearToolbars();
        drawnMembers.clear();
        drawnJoints.clear();

        // clear variables
        myStructure.xstruct.clear();
        myStructure.dxstruct.clear();
        myStructure.conn.clear();
        myStructure.constMat.clear();
        myStructure.loadMat.clear();
        myStructure.properties.clear();
        myStructure.SDOF.clear();
        myStructure.lenRot.clear();
        myStructure.clearStructVar();
        myStructure.compMtoS.clear();
        myStructure.rmem.clear();

        solved = false;
        displace = false;
        constraint = false;
        force = false;
        jToolBarActive = false;
        mToolBarActive = false;
        cToolBarActive = false;
        fToolBarActive = false;
        pToolBarActive = false;

        ui->pushButton_Disp->setEnabled(false);
        ui->pushButton_Stress->setEnabled(false);
        ui->pushButton_solve->setEnabled(false);

        break;
    case QMessageBox::Cancel:
        break;

    default:
        break;
    }

}

void MainWindow::on_actionJoints_triggered()
{
    clearToolbars();

    jToolBarActive = true;

    // formatting
    addJoint.setText("ADD");
    addJoint.setDefault(true);
    Title.setTextFormat(Qt::RichText);
    Title.setText("<b><u> Insert Joints");
    XText.setText("X:");
    YText.setText("Y:");
    addXText.setMaximumWidth(60);
    addYText.setMaximumWidth(60);
    jointToolBar.setMaximumWidth(500);
    jointToolBar.isMovable();

    // set layouts
    xLay.addWidget(&XText);
    xLay.addWidget(&addXText);
    yLay.addWidget(&YText);
    yLay.addWidget(&addYText);
    jointWidgX.setLayout(&xLay);
    jointWidgY.setLayout(&yLay);

    // add widgets to toolbar
    jointToolBar.addWidget(&Title);
    jointToolBar.addSeparator();
    jointToolBar.addWidget(&jointWidgX);
    jointToolBar.addSeparator();
    jointToolBar.addWidget(&jointWidgY);
    jointToolBar.addSeparator();
    jointToolBar.addWidget(&addJoint);

    // add toolbar
    this->addToolBar(&jointToolBar);

    // set signal and slot
    connect(&addJoint,SIGNAL(clicked()),this,SLOT(pushButton_addJoint()));
}

void MainWindow::on_actionMembers_triggered()
{
    clearToolbars();

    mToolBarActive = true;

    // formatting
    addMember.setText("ADD");
    addMember.setDefault(true);
    memberToolBar.isMovable();
    Title.setTextFormat(Qt::RichText);
    Title.setText("<b><u> Insert Members  ");
    XText.setText("1st:");
    YText.setText("2nd:");
    addXText.setMaximumWidth(30);
    addYText.setMaximumWidth(30);
    XText.setMaximumWidth(30);
    YText.setMaximumWidth(30);
    memberToolBar.setMaximumWidth(500);

    // add layouts
    xLay.addWidget(&XText);
    xLay.addWidget(&addXText);
    yLay.addWidget(&YText);
    yLay.addWidget(&addYText);
    jointWidgX.setLayout(&xLay);
    jointWidgY.setLayout(&yLay);

    // add widgets to toolbar
    memberToolBar.addWidget(&Title);
    memberToolBar.addSeparator();
    memberToolBar.addWidget(&jointWidgX);
    memberToolBar.addWidget(&jointWidgY);
    memberToolBar.addWidget(&addMember);

    // add toolbar
    this->addToolBar(&memberToolBar);

    // connect signals and slots
    connect(&addMember,SIGNAL(clicked()),this,SLOT(pushButton_addmember()));
}

void MainWindow::on_actionConstraints_triggered()
{

    clearToolbars();

    cToolBarActive = true;

    // formatting
    addConstraint.setText("ADD");
    constraintToolBar.isMovable();
    Title.setTextFormat(Qt::RichText);
    Title.setText("<b><u> Set Constraints  ");
    addXText.setMaximumWidth(30);
    JText.setText("Joint:");
    XText.setText("X:");
    YText.setText("Y:");
    RzText.setText("Rz:");
    JText.setMaximumWidth(45);
    addConstraint.setDefault(true);
    constraintToolBar.setMaximumWidth(600);

    // add layouts
    jLay.
    jLay.addWidget(&JText);
    jLay.addWidget(&addXText);
    xLay.addWidget(&XText);
    xLay.addWidget(&constX);
    yLay.addWidget(&YText);
    yLay.addWidget(&constY);
    zLay.addWidget(&RzText);
    zLay.addWidget(&constRz);
    jointWidgJ.setLayout(&jLay);
    jointWidgX.setLayout(&xLay);
    jointWidgY.setLayout(&yLay);
    jointWidgRz.setLayout(&zLay);

    // add widgets to tool bar
    constraintToolBar.addWidget(&Title);
    constraintToolBar.addSeparator();
    constraintToolBar.addWidget(&jointWidgJ);
    constraintToolBar.addWidget(&jointWidgX);
    constraintToolBar.addWidget(&jointWidgY);
    constraintToolBar.addWidget(&jointWidgRz);
    constraintToolBar.addWidget(&addConstraint);

    // toggle constraints
    ui->checkBox_const->setChecked(true);

    // add toolbar
    this->addToolBar(&constraintToolBar);

    // connect signals and slot
    connect(&addConstraint,SIGNAL(clicked()),this,SLOT(pushButton_addconstraint()));
}

void MainWindow::on_actionForces_triggered()
{
    clearToolbars();

    fToolBarActive = true;

    // formatting
    addForce.setText("ADD");
    Title.setTextFormat(Qt::RichText);
    Title.setText("<b><u> Place Forces  ");
    JText.setText("Joint:");
    MText.setText("Magnitude:");
    XText.setText("X:");
    YText.setText("Y:");
    RzText.setText("Rz:");
    forceToolBar.isMovable();
    addXText.setMaximumWidth(30);
    addYText.setMaximumWidth(60);
    JText.setMaximumWidth(40);
    MText.setMaximumWidth(80);
    addForce.setDefault(true);
    forceToolBar.setMaximumWidth(800);

    // add layouts
    jLay.addWidget(&JText);
    jLay.addWidget(&addXText);
    mLay.addWidget(&MText);
    mLay.addWidget(&addYText);
    xLay.addWidget(&XText);
    xLay.addWidget(&constX);
    yLay.addWidget(&YText);
    yLay.addWidget(&constY);
    zLay.addWidget(&MText);
    zLay.addWidget(&constRz);
    jointWidgJ.setLayout(&jLay);
    jointWidgMag.setLayout(&mLay);
    jointWidgX.setLayout(&xLay);
    jointWidgY.setLayout(&yLay);
    jointWidgRz.setLayout(&zLay);

    // add widgets to tool bar
    forceToolBar.addWidget(&Title);
    forceToolBar.addSeparator();
    forceToolBar.addWidget(&jointWidgJ);
    forceToolBar.addWidget(&jointWidgMag);
    forceToolBar.addWidget(&jointWidgX);
    forceToolBar.addWidget(&jointWidgY);
    forceToolBar.addWidget(&jointWidgRz);
    forceToolBar.addWidget(&addForce);

    // toggle forces
    ui->checkBox_Force->setChecked(true);

    // add tool bar
    this->addToolBar(&forceToolBar);

    // connect signals and slot
    connect(&addForce,SIGNAL(clicked()),this,SLOT(pushButton_addforce()));
}

void MainWindow::on_actionProperties_triggered()
{
    clearToolbars();

    pToolBarActive = true;

    // formatting
    setProps.setText("SET");
    Title.setTextFormat(Qt::RichText);
    Title.setText("<b><u> Structural Properties");
    XText.setText("E: ");
    YText.setText("I:  ");
    JText.setText("A ");
    MText.setText("e1:");
    RzText.setText("e2:");

    addE.setMaximumWidth(100);
    addA.setMaximumWidth(100);
    addI.setMaximumWidth(100);
    adde1.setMaximumWidth(100);
    adde2.setMaximumWidth(100);
    setProps.setDefault(true);

    // add layouts
    xLay.addSpacing(25);
    xLay.addWidget(&XText);
    xLay.addWidget(&addE);
    xLay.addSpacing(25);
    yLay.addSpacing(25);
    yLay.addWidget(&JText);
    yLay.addWidget(&addA);
    yLay.addSpacing(25);
    zLay.addSpacing(25);
    zLay.addWidget(&YText);
    zLay.addWidget(&addI);
    zLay.addSpacing(25);
    mLay.addSpacing(25);
    mLay.addWidget(&MText);
    mLay.addWidget(&adde1);
    mLay.addSpacing(25);
    jLay.addSpacing(25);
    jLay.addWidget(&RzText);
    jLay.addWidget(&adde2);
    jLay.addSpacing(25);
    pLay.addSpacing(15);
    pLay.addWidget(&setProps);
    pLay.addSpacing(15);
    propLay.addWidget(&Title);
    propLay.addSpacing(3);
    propLay.addLayout(&xLay);
    propLay.addLayout(&yLay);
    propLay.addLayout(&zLay);
    propLay.addLayout(&mLay);
    propLay.addLayout(&jLay);
    propLay.addLayout(&pLay);

    propWidg.setLayout(&propLay);

    // add widgets to toolbar
    propToolBar.addWidget(&propWidg);

    // add toolbar
    this->addToolBar(Qt::LeftToolBarArea,&propToolBar);

    // add signals and slots
    connect(&setProps,SIGNAL(clicked()),this,SLOT(pushButton_setDProperties()));
}

void MainWindow::on_pushButton_solve_released()
{
    if(solved == false)
    {
        undoList.clear();

        myStructure.preprocessing();
        myStructure.AssembleStructStiff();
        myStructure.Triangularization();
        myStructure.AssembleStructForce();
        myStructure.BackSub();
        myStructure.postprocessing();

        dDeform = 1;

        ui->pushButton_solve->setEnabled(false);
        ui->pushButton_Disp->setEnabled(true);
        ui->pushButton_Stress->setEnabled(true);

        solved = true;
    }
}

void MainWindow::on_pushButton_Disp_released()
{
    if(solved==true)
    {
        if(displace == false)
        {
            displace = true;

            if(constraint == true)
            {
                drawConstraints();
            }

            drawDStructure();

            if(force == true)
            {
                drawForces();
            }
        }
        else if(displace == true)
        {
            displace = false;

            scene->clear();         

            if(constraint == true)
            {
                drawConstraints();
            }

            drawMembers();
            drawJoint();

            if(force == true)
            {
                drawForces();
            }
        }
    }
}

void MainWindow::on_horizontalSlider_scaleDisp_sliderMoved(int position)
{
    if(solved==true)
    {
        dDeform = 1;
        dDeform*=position;

        //drawStructure();
        if(displace==true)
        {
            if(constraint==true)
            {
                drawConstraints();
            }
            drawDStructure();
            if(force==true)
            {
                drawForces();
            }

        }
    }
}

void MainWindow::on_pushButton_Stress_released()
{

}

void MainWindow::on_checkBox_const_toggled(bool checked)
{
    if(checked==true)
    {
        constraint = true;

        scene->clear();
        drawConstraints();

        // redraw other options if there
        if(displace == true)
        {
            drawDStructure();
        }
        else
        {
           drawMembers();
           drawJoint();
        }
        if(force == true)
        {
            drawForces();
        }
    }
    else
    {
        constraint = false;
        scene->clear();

        // redraw other options if there
        if(displace == true)
        {
            drawDStructure();
        }
        else
        {
           drawMembers();
           drawJoint();
        }
        if(force == true)
        {
            drawForces();
        }
    }
}

void MainWindow::on_checkBox_Force_toggled(bool checked)
{

    if(checked==true)
    {
        force = true;
        drawForces();
    }
    else
    {
        force = false;
        scene->clear();

        // redraw other options if there
        if(constraint == true)
        {
            drawConstraints();
        }
        if(displace == true)
        {
            drawDStructure();
        }
        else
        {
            drawMembers();
            drawJoint();
        }
    }
}

void MainWindow::drawJoint()
{
    QBrush jgrey(Qt::gray);
    QPen jPen(Qt::white);

    for(int i = 0; i < myStructure.xstruct.size(); i++)
    {
        QGraphicsEllipseItem *newJoint = new QGraphicsEllipseItem;
        drawnJoints.push_back(newJoint);
        drawnJoints[i] = scene->addEllipse(myStructure.xstruct[i][0]-5,-myStructure.xstruct[i][1]-5,10,10,jPen,jgrey);
    }
}

void MainWindow::drawMembers()
{
    QPen mPen;

    if(displace==true)
    {
        mPen.setColor(Qt::gray);
        mPen.setWidth(3);
        mPen.setStyle(Qt::DashLine);
    }
    else
    {
        mPen.setColor(Qt::white);
        mPen.setWidth(5);
    }

    for(int i = 0; i < myStructure.conn.size(); i++)
    {
        QGraphicsLineItem *newMember = new QGraphicsLineItem;
        drawnMembers.push_back(newMember);

        int One = myStructure.conn[i][0];
        int Two = myStructure.conn[i][1];

        double x1 = myStructure.xstruct[One-1][0];
        double y1 = -myStructure.xstruct[One-1][1];
        double x2 = myStructure.xstruct[Two-1][0];
        double y2 = -myStructure.xstruct[Two-1][1];

        drawnMembers[i] = scene->addLine(x1,y1,x2,y2,mPen);
    }
}

void MainWindow::drawConstraints()
{
    QBrush constBrush(Qt::blue);
    QPen constPen(Qt::blue);
    constPen.setWidth(2);

    // loop through constraint matrix
    for(uint i = 0; i < myStructure.constMat.size(); i++)
    {
        int m = myStructure.constMat[i][0]-1;
        int dir = myStructure.constMat[i][1];

        double x1 = myStructure.xstruct[m][0];
        double y1 = -myStructure.xstruct[m][1];

        if(dir==1)
        {
            noDrawnTransShape.clear();
            noDrawnTransShape << QPointF(x1,y1) << QPointF(x1-30,y1-15) << QPointF(x1-30,y1+15);
            noDrawnTrans = scene->addPolygon(noDrawnTransShape,constPen,constBrush);
        }
        else if(dir==2)
        {
            noDrawnTransShape.clear();
            noDrawnTransShape << QPointF(x1,y1) << QPointF(x1-15,y1+30) << QPointF(x1+15,y1+30);
            noDrawnTrans = scene->addPolygon(noDrawnTransShape,constPen,constBrush);
        }
        else
        {
            noDrawnRot = scene->addRect(x1-10,y1-10,20,20,constPen,constBrush);
        }

    }
}

void MainWindow::drawForces()
{
    QBrush momBrush(Qt::transparent);
    QBrush momBrush2(Qt::yellow);
    QBrush loadBrush(Qt::green);
    QPen loadPen(Qt::green);
    QPen momPen(Qt::yellow);
    loadPen.setWidth(1);
    momPen.setWidth(3);
    bool negative = false;

    // loop through constraint matrix
    for(uint i = 0; i < myStructure.loadMat.size(); i++)
    {
        int m = (int)myStructure.loadMat[i][0]-1;
        int dir = (int)myStructure.loadMat[i][1];
        std::string forceMag = std::to_string(myStructure.loadMat[i][2]);

        // get rid of trailing zeros
        std::string point = ".";
        int posdot = forceMag.find(point);

        int strSize = forceMag.size();

        bool nonZero = false;

        if(posdot < strSize)
        {
            for(int j = strSize; j >= posdot-1; j--)
            {
                if(forceMag.substr(j-1,1)=="0" || forceMag.substr(j,1)==".")
                {
                    if(nonZero==false)
                    {
                        forceMag.erase(j);
                    }
                }
                else
                {
                    nonZero = true;
                }
            }
        }

        QString forceT = QString::fromStdString(forceMag);
        if(forceT.at(0)=='-')
        {
            forceT.remove(0,1);
            negative = true;
        }

        double x1 = myStructure.xstruct[m][0];
        double y1 = -myStructure.xstruct[m][1];

        myText = new QGraphicsTextItem(forceT);
        myText->setDefaultTextColor(Qt::white);

        if(dir==1)
        {
            // draw main line
            myStrucLine = scene->addLine(x1,y1,x1-50,y1,loadPen);

            font.setPixelSize(30);
            font.setBold(false);
            font.setFamily("Calibri");
            noDrawnTransShape.clear();

            // draw triangle
            if(negative==true)
            {
                noDrawnTransShape << QPointF(x1-50,y1) << QPointF(x1-30,y1+5) << QPointF(x1-30,y1-5);
                noDrawnTrans = scene->addPolygon(noDrawnTransShape,loadPen,loadBrush);

                myText->setX(x1-80);
                myText->setY(y1);

                // reset negative
                negative = false;
            }
            else
            {
                noDrawnTransShape << QPointF(x1,y1) << QPointF(x1-20,y1+5) << QPointF(x1-20,y1-5);
                noDrawnTrans = scene->addPolygon(noDrawnTransShape,loadPen,loadBrush);

                myText->setX(x1-80);
                myText->setY(y1);
            }

            scene->addItem(myText);

        }
        else if(dir==2)
        {
            // draw main line
            myStrucLine = scene->addLine(x1,y1,x1,y1-50,loadPen);

            font.setPixelSize(30);
            font.setBold(false);
            font.setFamily("Calibri");
            noDrawnTransShape.clear();

            if(negative == true)
            {
                // draw triangle
                noDrawnTransShape << QPointF(x1,y1) << QPointF(x1-5,y1-20) << QPointF(x1+5,y1-20);
                noDrawnTrans = scene->addPolygon(noDrawnTransShape,loadPen,loadBrush);

                myText->setX(x1+5);
                myText->setY(y1-50);

                // reset negative
                negative = false;
            }
            else
            {
                // draw triangle
                noDrawnTransShape << QPointF(x1,y1-50) << QPointF(x1-5,y1-30) << QPointF(x1+5,y1-30);
                noDrawnTrans = scene->addPolygon(noDrawnTransShape,loadPen,loadBrush);

                myText->setX(x1+5);
                myText->setY(y1-25);
            }

            scene->addItem(myText);

        }
        else
        {
            myStructCirc = scene->addEllipse(x1-20, y1-20, 40, 40, momPen, momBrush);

            font.setPixelSize(30);
            font.setBold(false);
            font.setFamily("Calibri");
            noDrawnTransShape.clear();

            if(negative == true)
            {
                // draw triangle
                noDrawnTransShape << QPointF(x1+10,y1-20) << QPointF(x1-10,y1-10) << QPointF(x1-10,y1-30);
                noDrawnTrans = scene->addPolygon(noDrawnTransShape,momPen,momBrush2);

                myText->setX(x1-25);
                myText->setY(y1-60);

                // reset negative
                negative = false;
            }
            else
            {
                // draw triangle
                noDrawnTransShape << QPointF(x1-10,y1-20) << QPointF(x1+10,y1-10) << QPointF(x1+10,y1-30);
                noDrawnTrans = scene->addPolygon(noDrawnTransShape,momPen,momBrush2);

                myText->setX(x1-25);
                myText->setY(y1-60);
            }

            scene->addItem(myText);
        }
    }
}

void MainWindow::drawDStructure()
{
    // clear gview
    scene->clear();

    if(constraint == true)
    {
        drawConstraints();
    }

    drawMembers();
    drawJoint();

    double x1;
    double y1;
    double x2;
    double y2;

    for(uint i = 0; i < myStructure.conn.size();i++)
    {

        // draw with delta
        int m = myStructure.conn[i][0]-1;
        int n = myStructure.conn[i][1]-1;

        x1 = (myStructure.xstruct[m][0]+myStructure.dxstruct[m][0]*dDeform);
        y1 = -(myStructure.xstruct[m][1]+myStructure.dxstruct[m][1]*dDeform);
        x2 = (myStructure.xstruct[n][0]+myStructure.dxstruct[n][0]*dDeform);
        y2 = -(myStructure.xstruct[n][1]+myStructure.dxstruct[n][1]*dDeform);

        QPen linePen(Qt::red);
        linePen.setWidth(5);

        myStrucLine = scene->addLine(x1,y1,x2,y2,linePen);
    }
}

void MainWindow::pushButton_addJoint()
{
    try
    {
        std::vector<double> jPoints;

        QString myX = addXText.text();
        QString myY = addYText.text();

        double x = myX.toDouble();
        double y = myY.toDouble();

        jPoints.push_back(x);
        jPoints.push_back(y);

        myStructure.xstruct.push_back(jPoints);

        drawJoint();

        std::vector<std::string> oneLine;
        oneLine.push_back("joint");
        oneLine.push_back("1");
        undoList.push_back(oneLine);
    }
    catch(std::invalid_argument)
    {
        QMessageBox noGood;

        noGood.setText("Inputs Incorrect");
        noGood.setInformativeText("please follow example");
        noGood.setStandardButtons(QMessageBox::Ok);
        noGood.setDefaultButton(QMessageBox::Ok);
        noGood.setWindowTitle("WARNING");
        noGood.exec();
    }
}

void MainWindow::pushButton_addmember()
{
    try
    {
        std::vector<int> mPoints;

       QString myOne = addXText.text();
       QString myTwo = addYText.text();

       int One = myOne.toInt();
       int Two = myTwo.toInt();

       mPoints.push_back(One);
       mPoints.push_back(Two);

       myStructure.conn.push_back(mPoints);

       drawJoint();
       drawMembers();

       std::vector<std::string> oneLine;
       oneLine.push_back("member");
       oneLine.push_back("1");
       undoList.push_back(oneLine);
    }
    catch(std::invalid_argument)
    {
        QMessageBox noGood;

        noGood.setText("Inputs Incorrect");
        noGood.setInformativeText("please follow example");
        noGood.setStandardButtons(QMessageBox::Ok);
        noGood.setDefaultButton(QMessageBox::Ok);
        noGood.setWindowTitle("WARNING");
        noGood.exec();
    }

}

void MainWindow::pushButton_addconstraint()
{

    try
    {
        int numOneClick = 0;

        std::vector<int> constPoint;

        QString constJoint = addXText.text();

        int constJointNum = constJoint.toInt();

        if(constX.isChecked())
        {
            constPoint.push_back(constJointNum);
            constPoint.push_back(1);
            myStructure.constMat.push_back(constPoint);
            constPoint.clear();

            numOneClick++;
        }
        if(constY.isChecked())
        {
            constPoint.push_back(constJointNum);
            constPoint.push_back(2);
            myStructure.constMat.push_back(constPoint);
            constPoint.clear();

            numOneClick++;
        }
        if(constRz.isChecked())
        {
            constPoint.push_back(constJointNum);
            constPoint.push_back(3);
            myStructure.constMat.push_back(constPoint);
            constPoint.clear();

            numOneClick++;
        }

        std::string NumConst;
        NumConst = std::to_string(numOneClick);
        std::vector<std::string> oneLine;
        oneLine.push_back("constraint");
        oneLine.push_back(NumConst);
        undoList.push_back(oneLine);

        drawConstraints();
        drawMembers();
        drawJoint();
    }
    catch(std::invalid_argument)
    {
        QMessageBox noGood;

        noGood.setText("Inputs Incorrect");
        noGood.setInformativeText("please follow example");
        noGood.setStandardButtons(QMessageBox::Ok);
        noGood.setDefaultButton(QMessageBox::Ok);
        noGood.setWindowTitle("WARNING");
        noGood.exec();
    }
}

void MainWindow::pushButton_addforce()
{

    try
    {
        int numOneClick = 0;

        std::vector<double> forcePoint;

        QString forceJoint = addXText.text();
        QString forceMag = addYText.text();

        double forceJointNum = forceJoint.toInt();
        double forceDoubleMag = forceMag.toDouble();

        if(constX.isChecked())
        {
            forcePoint.push_back(forceJointNum);
            forcePoint.push_back(1);
            forcePoint.push_back(forceDoubleMag);
            myStructure.loadMat.push_back(forcePoint);
            forcePoint.clear();

            numOneClick++;
        }
        if(constY.isChecked())
        {
            forcePoint.push_back(forceJointNum);
            forcePoint.push_back(2);
            forcePoint.push_back(forceDoubleMag);
            myStructure.loadMat.push_back(forcePoint);
            forcePoint.clear();

            numOneClick++;
        }
        if(constRz.isChecked())
        {
            forcePoint.push_back(forceJointNum);
            forcePoint.push_back(3);
            forcePoint.push_back(forceDoubleMag);
            myStructure.loadMat.push_back(forcePoint);
            forcePoint.clear();

            numOneClick++;
        }

        std::string NumForce;
        NumForce = std::to_string(numOneClick);
        std::vector<std::string> oneLine;
        oneLine.push_back("force");
        oneLine.push_back(NumForce);
        undoList.push_back(oneLine);

        drawConstraints();
        drawMembers();
        drawJoint();
        drawForces();
    }
    catch(std::invalid_argument)
    {
        QMessageBox noGood;

        noGood.setText("Inputs Incorrect");
        noGood.setInformativeText("please follow example");
        noGood.setStandardButtons(QMessageBox::Ok);
        noGood.setDefaultButton(QMessageBox::Ok);
        noGood.setWindowTitle("WARNING");
        noGood.exec();
    }
}

void MainWindow::pushButton_setDProperties()
{
    try
    {
        myStructure.properties.clear();

        QString Estring = addE.text();
        QString Istring = addI.text();
        QString Astring = addA.text();
        QString e1string = adde1.text();
        QString e2string = adde2.text();

        myStructure.properties.push_back(Estring.toDouble());
        myStructure.properties.push_back(Astring.toDouble());
        myStructure.properties.push_back(Istring.toDouble());
        myStructure.properties.push_back(e1string.toDouble());
        myStructure.properties.push_back(e2string.toDouble());

        clearToolbars();
        solveReady();
    }
    catch(std::invalid_argument)
    {
        QMessageBox noGood;

        noGood.setText("Inputs Incorrect");
        noGood.setInformativeText("please follow example");
        noGood.setStandardButtons(QMessageBox::Ok);
        noGood.setDefaultButton(QMessageBox::Ok);
        noGood.setWindowTitle("WARNING");
        noGood.exec();
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    // test
    std::cout << "You Scrolled the Wheel ";

    QPoint numDegrees = event->angleDelta() / 8;
    QPoint numSteps = numDegrees / 15;
    QPoint mousePos = event->pos();

    double myZoomY = numSteps.y();
    double myPosX = mousePos.x();
    double myPosY = mousePos.y();

    std::cout << "to Position: y = "<< myZoomY << std::endl;
    std::cout << "The mouse position is: x = " << myPosX << " y = " << myPosY << std::endl;

    // zoom depending on sign of y
    if(myZoomY < 0)
    {
        zoomOut();
    }
    else
    {
        zoomIn();
    }

    event->accept();
}

void MainWindow::zoomIn()
{
    if(zoom >= 1)
    {   // clear the scene of the lines
        scene->clear();

        // set new zoom factor
        zoom++;

        // draw the shapes again
        if(displace == true)
        {
            if(constraint == true)
            {
                drawConstraints();
            }

            drawDStructure();

            if(force == true)
            {
                drawForces();
            }
        }
        else
        {
            if(constraint == true)
            {
                drawConstraints();
            }

            drawMembers();
            drawJoint();

            if(force == true)
            {
                drawForces();
            }
        }
    }
    else if(zoom < 1)
    {
        // clear the scene of the lines
        scene->clear();

        zoom = zoom + 0.1;

        // draw the shapes again
        if(displace == true)
        {
            if(constraint == true)
            {
                drawConstraints();
            }

            drawDStructure();

            if(force == true)
            {
                drawForces();
            }
        }
        else
        {
            if(constraint == true)
            {
                drawConstraints();
            }

            drawMembers();
            drawJoint();

            if(force == true)
            {
                drawForces();
            }
        }
    }
}

void MainWindow::zoomOut()
{
    if(zoom > 1)
    {
        // clear the scene of the lines
        scene->clear();

        // set new zoom factor
        zoom--;

        // draw the shapes again
        if(displace == true)
        {
            if(constraint == true)
            {
                drawConstraints();
            }

            drawDStructure();

            if(force == true)
            {
                drawForces();
            }
        }
        else
        {
            if(constraint == true)
            {
                drawConstraints();
            }

            drawMembers();
            drawJoint();

            if(force == true)
            {
                drawForces();
            }
        }
    }
    else if(zoom <= 1)
    {
        if(zoom > .1)
        {
            // clear the scene of the lines
            scene->clear();

            zoom = zoom - 0.1;

            // draw the shapes again
            if(displace == true)
            {
                if(constraint == true)
                {
                    drawConstraints();
                }

                drawDStructure();

                if(force == true)
                {
                    drawForces();
                }
            }
            else
            {
                if(constraint == true)
                {
                    drawConstraints();
                }

                drawMembers();
                drawJoint();

                if(force == true)
                {
                    drawForces();
                }
            }
        }
    }
}

void MainWindow::readFile(std::string fileName)
{
    // read file
    int numTotalObj = 0;

    std::string line;
    std::ifstream infile (fileName);
    int sections = 0;

    while (std::getline(infile,line, '\n'))
    {
        // make sure line isn't blank
        if(line!="")
        {
           // parse the line for the data

                // define vector of strings
                std::vector<std::string> tokens;

                // define delimiters
                std::string delimiters(", ;\t");

                // call tokenize
                tokenize(line, tokens, delimiters);

                if(sections == 0)
                {
                    if(tokens[0] != "joints")  //make sure you haven't reached the next section
                    {
                        // look at what kind of structure is being used
                        myStructure.StructType = tokens[0];
                    }
                }

                if(sections == 1)
                {
                    if(tokens[0] != "members")  //make sure you haven't reached the next section
                    {
                            // add joint coordinates
                            std::vector<double> xSingleJoint;

                            xSingleJoint.push_back(stod(tokens[0]));
                            xSingleJoint.push_back(stod(tokens[1]));

                            myStructure.xstruct.push_back(xSingleJoint);

                            std::vector<std::string> oneLine;
                            oneLine.push_back("joint");
                            oneLine.push_back("1");
                            undoList.push_back(oneLine);
                            numTotalObj++;
                    }
                }
                else if(sections == 2)
                {
                    if(tokens[0] != "constraints")  //make sure you haven't reached the next section
                    {
                        // add members to thier joints
                        std::vector<int> xSingleMember;

                        xSingleMember.push_back(stoi(tokens[0]));
                        xSingleMember.push_back(stoi(tokens[1]));

                        myStructure.conn.push_back(xSingleMember);

                        std::vector<std::string> oneLine;
                        oneLine.push_back("member");
                        oneLine.push_back("1");
                        undoList.push_back(oneLine);
                        numTotalObj++;
                    }
                }
                else if(sections == 3)
                {
                    if(tokens[0] != "loads")  //make sure you haven't reached the next section
                    {
                        std::vector<int> constSingleDOF;

                        constSingleDOF.push_back(stoi(tokens[0]));
                        constSingleDOF.push_back(stoi(tokens[1]));

                        myStructure.constMat.push_back(constSingleDOF);

                        std::vector<std::string> oneLine;
                        oneLine.push_back("constraint");
                        oneLine.push_back("1");
                        undoList.push_back(oneLine);
                        numTotalObj++;
                    }
                }
                else if(sections == 4)
                {
                    if(tokens[0] != "properties")  //make sure you haven't reached the next section
                    {
                        std::vector<double> singleForce;

                        singleForce.push_back(stod(tokens[0]));
                        singleForce.push_back(stod(tokens[1]));
                        singleForce.push_back(stod(tokens[2]));

                        myStructure.loadMat.push_back(singleForce);

                        std::vector<std::string> oneLine;
                        oneLine.push_back("force");
                        oneLine.push_back("1");
                        undoList.push_back(oneLine);
                        numTotalObj++;
                    }
                }
                else if(sections == 5)
                {
                    myStructure.properties.push_back(stod(tokens[0]));
                }

                // check what section you are in

                if(tokens[0]=="joints")
                {
                    sections++;
                }
                else if(tokens[0]=="members") // make sure the next section is members
                {
                    sections++;
                }
                else if(tokens[0]=="constraints") // make sure the next section is constraints
                {
                    sections++;
                }
                else if(tokens[0]=="loads") // make sure the next section is loads
                {
                    sections++;
                }
                else if(tokens[0]=="properties") // make sure the next section is properties
                {
                    sections++;
                }
        }
    }

    std::string NumObj;
    NumObj = std::to_string(numTotalObj);
    std::vector<std::string> oneLine;
    oneLine.push_back("file");
    oneLine.push_back(NumObj);
    undoList.push_back(oneLine);

    ui->pushButton_solve->setEnabled(true);
}

void MainWindow::clearToolbars()
{
    // get rid of all tool bars
    if(jToolBarActive == true)
    {
        this->removeToolBar(&jointToolBar);
        jToolBarActive = false;
    }
    if(mToolBarActive == true)
    {
        this->removeToolBar(&memberToolBar);
        mToolBarActive = false;
    }
    if(cToolBarActive == true)
    {
        this->removeToolBar(&constraintToolBar);
        cToolBarActive = false;
    }
    if(fToolBarActive == true)
    {
        this->removeToolBar(&forceToolBar);
        fToolBarActive = false;
    }
    if(pToolBarActive == true)
    {
        this->removeToolBar(&propToolBar);
        pToolBarActive = false;
    }

}

void MainWindow::solveReady()
{
    if(myStructure.checkReady()==true)
        ui->pushButton_solve->setEnabled(true);
}

void MainWindow::on_actionUndo_triggered()
{
    if(!undoList.empty())
    {
        std::vector<std::string> lastAction;
        std::string stringNumberOfThings;
        std::string stringTheThing;

        lastAction = undoList.back();

        stringTheThing = lastAction[0];
        stringNumberOfThings = lastAction[1];

        int intNumberOfThings = stoi(stringNumberOfThings);

        if(stringTheThing=="file")
        {
            clearFile(intNumberOfThings);
        }
        else
        {
            for(int i = 0; i < intNumberOfThings; i++)
            {
                if(stringTheThing=="joint")
                {
                    clearJoint();
                }
                else if(stringTheThing=="member")
                {
                    clearMember();
                }
                else if(stringTheThing=="constraint")
                {
                    clearConstraint();
                }
                else if(stringTheThing=="force")
                {
                    clearForce();
                }
            }

            std::vector<std::string> row;
            row.push_back(stringTheThing);
            row.push_back(stringNumberOfThings);
            redoList.push_back(row);

            undoList.pop_back();
        }

        clearDraw();
    }
}

void MainWindow::clearJoint()
{
    undidXStruct.push_back(myStructure.xstruct.back());
    myStructure.xstruct.pop_back();
}

void MainWindow::clearMember()
{
    undidConn.push_back(myStructure.conn.back());
    myStructure.conn.pop_back();
}

void MainWindow::clearConstraint()
{
    undidConst.push_back(myStructure.constMat.back());
    myStructure.constMat.pop_back();
}

void MainWindow::clearForce()
{
    undidLoad.push_back(myStructure.loadMat.back());
    myStructure.loadMat.pop_back();
}

void MainWindow::clearFile(int num)
{
    undoList.pop_back();

    std::vector<std::string> lastAction;
    std::string stringTheThing;

    for(int i = 0; i < num; i++)
    {
        lastAction = undoList.back();
        stringTheThing = lastAction[0];

        if(stringTheThing=="joint")
            clearJoint();
        else if(stringTheThing=="member")
            clearMember();
        else if(stringTheThing=="constraint")
            clearConstraint();
        else if(stringTheThing=="force")
            clearForce();

        undoList.pop_back();
    }
}

void MainWindow::clearDraw()
{
    scene->clear();

    if(constraint == true)
    {
        drawConstraints();
    }

    drawMembers();
    drawJoint();

    if(force == true)
    {
        drawForces();
    }

    if(displace==true)
        drawDStructure();
}

void MainWindow::on_actionRedo_triggered()
{
    if(!redoList.empty())
    {
        std::vector<std::string> lastUndidAction;
        std::string stringNumberOfThings;
        std::string stringTheThing;

        lastUndidAction = redoList.back();

        stringTheThing = lastUndidAction[0];
        stringNumberOfThings = lastUndidAction[1];

        int intNumberOfThings = stoi(stringNumberOfThings);

        for(int i = 0; i < intNumberOfThings; i++)
        {
            if(stringTheThing=="joint")
            {
                redoJoint();
            }
            else if(stringTheThing=="member")
            {
                redoMember();
            }
            else if(stringTheThing=="constraint")
            {
                redoConstraint();
            }
            else if(stringTheThing=="force")
            {
                redoForce();
            }
        }

        std::vector<std::string> row;
        row.push_back(stringTheThing);
        row.push_back(stringNumberOfThings);
        undoList.push_back(row);

        redoList.pop_back();
    }

    clearDraw();
}

void MainWindow::redoJoint()
{
    myStructure.xstruct.push_back(undidXStruct.back());
    undidXStruct.pop_back();
}

void MainWindow::redoMember()
{
    myStructure.conn.push_back(undidConn.back());
    undidConn.pop_back();
}

void MainWindow::redoConstraint()
{
    myStructure.constMat.push_back(undidConst.back());
    undidConst.pop_back();
}

void MainWindow::redoForce()
{
    myStructure.loadMat.push_back(undidLoad.back());
    undidLoad.pop_back();
}
