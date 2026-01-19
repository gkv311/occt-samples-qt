// Copyright (c) 2025 Kirill Gavrilov

#include "OcctQMainWindowSample.h"

#include "../occt-qt-tools/OcctQtTools.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <Standard_WarningsRestore.hxx>

#include <Message.hxx>
#include <Standard_Version.hxx>

int main(int theNbArgs, char** theArgVec)
{
  // handle application options
  for (int anArgIter = 0; anArgIter < theNbArgs; ++anArgIter)
  {
    // enable verbose messages from OCCT algorithms
    if (std::strcmp(theArgVec[anArgIter], "-v") == 0
     || std::strcmp(theArgVec[anArgIter], "--verbose") == 0)
    {
      if (anArgIter + 1 < theNbArgs)
        theArgVec[anArgIter] = theArgVec[anArgIter + 1];

      --anArgIter;
      --theNbArgs;
      Message::DefaultMessenger()->Printers().First()->SetTraceLevel(Message_Trace);
    }
  }

  // before creating QApplication: define platform plugin to load (e.g. xcb on Linux)
  // and graphic driver (e.g. desktop OpenGL with desired profile/surface)
  OcctQtTools::qtGlPlatformSetup();
  QApplication aQApp(theNbArgs, theArgVec);

  QCoreApplication::setApplicationName("OCCT Qt/QWidget Viewer sample");
  QCoreApplication::setOrganizationName("OpenCASCADE");
  QCoreApplication::setApplicationVersion(OCC_VERSION_STRING_EXT);

  OcctQMainWindowSample aMainWindow;
  aMainWindow.resize(aMainWindow.sizeHint());
  aMainWindow.show();
  return aQApp.exec();
}
