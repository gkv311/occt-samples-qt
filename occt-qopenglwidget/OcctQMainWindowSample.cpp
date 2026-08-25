// Copyright (c) 2025 Kirill Gavrilov

#include "OcctQMainWindowSample.h"

#include "OcctQOpenGLWidgetViewer.h"

#include "../occt-qt-tools/OcctQtImage.h"

#include <Standard_Version.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QDir>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <Standard_WarningsRestore.hxx>

// ================================================================
// Function : OcctQMainWindowSample
// ================================================================
OcctQMainWindowSample::OcctQMainWindowSample()
{
  // 3D Viewer widget as a central widget
  myViewer = new OcctQOpenGLWidgetViewer();
  setCentralWidget(myViewer);

  // menu bar
  createMenuBar();

  // some controls on top of 3D Viewer
  createLayoutOverViewer();
}

// ================================================================
// Function : createMenuBar
// ================================================================
void OcctQMainWindowSample::createMenuBar()
{
  QMenuBar* aMenuBar    = new QMenuBar();
  QMenu*    aMenuWindow = aMenuBar->addMenu("&File");
#if (OCC_VERSION_HEX >= 0x070700)
  {
    QAction* anActionSplit = new QAction(aMenuWindow);
    anActionSplit->setText("Split Views");
    aMenuWindow->addAction(anActionSplit);
    connect(anActionSplit, &QAction::triggered, [this]() { splitSubviews(); });
  }
#endif
  {
    QAction* anActionImage = new QAction(aMenuWindow);
    anActionImage->setText("Dump image");
    aMenuWindow->addAction(anActionImage);
    connect(anActionImage, &QAction::triggered, [this]() { screenshot(); });
  }
  {
    QAction* anActionQuit = new QAction(aMenuWindow);
    anActionQuit->setText("Quit");
    aMenuWindow->addAction(anActionQuit);
    connect(anActionQuit, &QAction::triggered, [this]() { close(); });
  }
  setMenuBar(aMenuBar);
}

// ================================================================
// Function : createLayoutOverViewer
// ================================================================
void OcctQMainWindowSample::createLayoutOverViewer()
{
  QVBoxLayout* aLayout = new QVBoxLayout(myViewer);
  aLayout->setDirection(QBoxLayout::BottomToTop);
  aLayout->setAlignment(Qt::AlignBottom);
  {
    // button displaying message window with OpenGL info
    QPushButton* aQuitBtn = new QPushButton("About");
    aLayout->addWidget(aQuitBtn);
    connect(aQuitBtn, &QPushButton::clicked, [this]() {
      QMessageBox::information(0,
                               "About Sample",
                               QString() + "OCCT 3D Viewer sample embedded into Qt Widgets.\n\n"
                                 + "Open CASCADE Technology v." OCC_VERSION_STRING_EXT "\n"
                                 + "Qt v." QT_VERSION_STR "\n\n" + "OpenGL info:\n" + myViewer->getGlInfo());
    });
  }
  {
    // slider changing viewer background color

    // the widgets on top of OCCT 3D Viewer (implemented as QOpenGLWidget) might have transparent background
    QWidget* aSliderBox = new QWidget();
    aSliderBox->setStyleSheet("QWidget { background-color: rgba(0, 0, 0, 0); }");

    QHBoxLayout* aSliderLayout = new QHBoxLayout(aSliderBox);
    {
      QLabel* aSliderLabel = new QLabel("Background");
      aSliderLabel->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 0); color: white; }");
      aSliderLabel->setGeometry(50, 50, 50, 50);
      aSliderLabel->adjustSize();
      aSliderLayout->addWidget(aSliderLabel);
    }
    {
      QSlider* aSlider = new QSlider(Qt::Horizontal);
      aSlider->setRange(0, 255);
      aSlider->setSingleStep(1);
      aSlider->setPageStep(15);
      aSlider->setTickInterval(15);
      aSlider->setTickPosition(QSlider::TicksRight);
      aSlider->setValue(0);
      aSliderLayout->addWidget(aSlider);
      connect(aSlider, &QSlider::valueChanged, [this](int theValue) {
        const float          aVal = theValue / 255.0f;
        const Quantity_Color aColor(aVal, aVal, aVal, Quantity_TOC_sRGB);
#if (OCC_VERSION_HEX >= 0x070700)
        for (const Handle(V3d_View)& aSubviewIter : myViewer->View()->Subviews())
        {
          aSubviewIter->SetBgGradientColors(aColor, Quantity_NOC_BLACK, Aspect_GradientFillMethod_Elliptical);
          aSubviewIter->Invalidate();
        }
#endif
        // myViewer->View()->SetBackgroundColor(aColor);
        myViewer->View()->SetBgGradientColors(aColor, Quantity_NOC_BLACK, Aspect_GradientFillMethod_Elliptical);
        myViewer->View()->Invalidate();
        myViewer->update();
      });
    }

    aLayout->addWidget(aSliderBox);
  }
}

// ================================================================
// Function : screenshot
// ================================================================
void OcctQMainWindowSample::screenshot()
{
  const QString aPath = QDir::currentPath() + "/screenshot.png";

  int anImgDims[2] = {};
  myViewer->View()->Window()->Size(anImgDims[0], anImgDims[1]);

  OcctQtImage aPixmap;
  if (!myViewer->View()->ToPixMap(aPixmap, anImgDims[0], anImgDims[1]))
    QMessageBox::critical(0, "Screenshot info", QString("Unable to dump 3d view %1x%2").arg(anImgDims[0]).arg(anImgDims[1]));
  else if (!aPixmap.Save(aPath.toUtf8().data()))
    QMessageBox::critical(0, "Screenshot info", QString("Unable to save screenshot to\n'%1'").arg(aPath));
  else
    QMessageBox::information(0, "Screenshot info", QString("Screenshot %1x%2 saved to\n'%3'").arg(anImgDims[0]).arg(anImgDims[1]).arg(aPath));
}

// ================================================================
// Function : splitSubviews
// ================================================================
void OcctQMainWindowSample::splitSubviews()
{
#if (OCC_VERSION_HEX >= 0x070700)
  if (!myViewer->View()->Subviews().IsEmpty())
  {
    // remove subviews
    myViewer->View()->View()->SetSubviewComposer(false);
    NCollection_Sequence<Handle(V3d_View)> aSubviews = myViewer->View()->Subviews();
    for (const Handle(V3d_View)& aSubviewIter : aSubviews)
      aSubviewIter->Remove();

    myViewer->OnSubviewChanged(myViewer->Context(), nullptr, myViewer->View());
  }
  else
  {
    // create two subviews splitting window horizontally
    myViewer->View()->View()->SetSubviewComposer(true);

    Handle(V3d_View) aSubView1 = new V3d_View(myViewer->Viewer());
    aSubView1->SetImmediateUpdate(false);
    aSubView1->SetWindow(myViewer->View(),
                         Graphic3d_Vec2d(0.5, 1.0),
                         Aspect_TOTP_LEFT_UPPER,
                         Graphic3d_Vec2d(0.0, 0.0));

    Handle(V3d_View) aSubView2 = new V3d_View(myViewer->Viewer());
    aSubView2->SetImmediateUpdate(false);
    aSubView2->SetWindow(myViewer->View(),
                         Graphic3d_Vec2d(0.5, 1.0),
                         Aspect_TOTP_LEFT_UPPER,
                         Graphic3d_Vec2d(0.5, 0.0));

    myViewer->OnSubviewChanged(myViewer->Context(), nullptr, aSubView1);
  }
  myViewer->View()->Invalidate();
  myViewer->update();
#endif
}
