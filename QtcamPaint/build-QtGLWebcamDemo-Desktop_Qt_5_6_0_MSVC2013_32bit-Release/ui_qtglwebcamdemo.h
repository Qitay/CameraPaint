/********************************************************************************
** Form generated from reading UI file 'qtglwebcamdemo.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTGLWEBCAMDEMO_H
#define UI_QTGLWEBCAMDEMO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "cqtopencvviewergl.h"

QT_BEGIN_NAMESPACE

class Ui_QtGLWebcamDemo
{
public:
    QAction *actionStart;
    QAction *actionVertical_Flip;
    QAction *action_Horizontal_Mirror;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    CQtOpenCVViewerGl *openCVviewer;
    QMenuBar *menuBar;
    QMenu *menuStart;
    QMenu *menuOptions;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtGLWebcamDemo)
    {
        if (QtGLWebcamDemo->objectName().isEmpty())
            QtGLWebcamDemo->setObjectName(QStringLiteral("QtGLWebcamDemo"));
        QtGLWebcamDemo->resize(995, 541);
        actionStart = new QAction(QtGLWebcamDemo);
        actionStart->setObjectName(QStringLiteral("actionStart"));
        actionVertical_Flip = new QAction(QtGLWebcamDemo);
        actionVertical_Flip->setObjectName(QStringLiteral("actionVertical_Flip"));
        actionVertical_Flip->setCheckable(true);
        action_Horizontal_Mirror = new QAction(QtGLWebcamDemo);
        action_Horizontal_Mirror->setObjectName(QStringLiteral("action_Horizontal_Mirror"));
        action_Horizontal_Mirror->setCheckable(true);
        centralWidget = new QWidget(QtGLWebcamDemo);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        openCVviewer = new CQtOpenCVViewerGl(centralWidget);
        openCVviewer->setObjectName(QStringLiteral("openCVviewer"));

        gridLayout->addWidget(openCVviewer, 0, 0, 1, 1);

        QtGLWebcamDemo->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtGLWebcamDemo);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 995, 21));
        menuStart = new QMenu(menuBar);
        menuStart->setObjectName(QStringLiteral("menuStart"));
        menuOptions = new QMenu(menuBar);
        menuOptions->setObjectName(QStringLiteral("menuOptions"));
        QtGLWebcamDemo->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtGLWebcamDemo);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtGLWebcamDemo->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtGLWebcamDemo);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtGLWebcamDemo->setStatusBar(statusBar);

        menuBar->addAction(menuStart->menuAction());
        menuBar->addAction(menuOptions->menuAction());
        menuStart->addAction(actionStart);
        menuOptions->addAction(actionVertical_Flip);
        menuOptions->addAction(action_Horizontal_Mirror);

        retranslateUi(QtGLWebcamDemo);

        QMetaObject::connectSlotsByName(QtGLWebcamDemo);
    } // setupUi

    void retranslateUi(QMainWindow *QtGLWebcamDemo)
    {
        QtGLWebcamDemo->setWindowTitle(QApplication::translate("QtGLWebcamDemo", "QtGLWebcamDemo", 0));
        actionStart->setText(QApplication::translate("QtGLWebcamDemo", "Start", 0));
        actionVertical_Flip->setText(QApplication::translate("QtGLWebcamDemo", "&Vertical Mirror", 0));
        action_Horizontal_Mirror->setText(QApplication::translate("QtGLWebcamDemo", "&Horizontal Mirror", 0));
        menuStart->setTitle(QApplication::translate("QtGLWebcamDemo", "Camera", 0));
        menuOptions->setTitle(QApplication::translate("QtGLWebcamDemo", "Options", 0));
    } // retranslateUi

};

namespace Ui {
    class QtGLWebcamDemo: public Ui_QtGLWebcamDemo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTGLWEBCAMDEMO_H
