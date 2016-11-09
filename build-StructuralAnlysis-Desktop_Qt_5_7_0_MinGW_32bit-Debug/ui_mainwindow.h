/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave_Results;
    QAction *actionClear;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QSplitter *splitter;
    QPushButton *pushButton_ZoomIn;
    QPushButton *pushButton_ZOut;
    QSpacerItem *verticalSpacer_3;
    QPushButton *pushButton_solve;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer;
    QLCDNumber *lcdNumber;
    QLabel *label;
    QSlider *horizontalSlider_scaleDisp;
    QGraphicsView *graphicsView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 300);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave_Results = new QAction(MainWindow);
        actionSave_Results->setObjectName(QStringLiteral("actionSave_Results"));
        actionClear = new QAction(MainWindow);
        actionClear->setObjectName(QStringLiteral("actionClear"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        pushButton_ZoomIn = new QPushButton(splitter);
        pushButton_ZoomIn->setObjectName(QStringLiteral("pushButton_ZoomIn"));
        splitter->addWidget(pushButton_ZoomIn);
        pushButton_ZOut = new QPushButton(splitter);
        pushButton_ZOut->setObjectName(QStringLiteral("pushButton_ZOut"));
        splitter->addWidget(pushButton_ZOut);

        gridLayout->addWidget(splitter, 0, 1, 1, 2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 1, 2, 1, 1);

        pushButton_solve = new QPushButton(centralWidget);
        pushButton_solve->setObjectName(QStringLiteral("pushButton_solve"));

        gridLayout->addWidget(pushButton_solve, 4, 1, 1, 2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 2, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 54, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 2, 1, 1);

        lcdNumber = new QLCDNumber(centralWidget);
        lcdNumber->setObjectName(QStringLiteral("lcdNumber"));
        lcdNumber->setFrameShadow(QFrame::Plain);
        lcdNumber->setDigitCount(2);

        gridLayout->addWidget(lcdNumber, 5, 2, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 5, 1, 1, 1);

        horizontalSlider_scaleDisp = new QSlider(centralWidget);
        horizontalSlider_scaleDisp->setObjectName(QStringLiteral("horizontalSlider_scaleDisp"));
        horizontalSlider_scaleDisp->setSliderPosition(0);
        horizontalSlider_scaleDisp->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider_scaleDisp, 5, 0, 1, 1);

        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));

        gridLayout->addWidget(graphicsView, 0, 0, 5, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave_Results);
        menuFile->addAction(actionClear);

        retranslateUi(MainWindow);
        QObject::connect(horizontalSlider_scaleDisp, SIGNAL(sliderMoved(int)), lcdNumber, SLOT(display(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0));
        actionSave_Results->setText(QApplication::translate("MainWindow", "Save Results", 0));
        actionClear->setText(QApplication::translate("MainWindow", "Clear", 0));
        pushButton_ZoomIn->setText(QApplication::translate("MainWindow", "Zoom In", 0));
        pushButton_ZOut->setText(QApplication::translate("MainWindow", "Zoom Out", 0));
        pushButton_solve->setText(QApplication::translate("MainWindow", "Solve", 0));
        label->setText(QApplication::translate("MainWindow", "Scale Factor:", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
