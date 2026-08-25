// Copyright (c) 2025 Kirill Gavrilov

#include "OcctQtImage.h"

//! Convert QImage format to OCCT enumeration.
static Image_Format formatFromQt(QImage::Format theFormat)
{
  switch (theFormat)
  {
    case QImage::Format_RGB32:      return Image_Format_BGR32;
    case QImage::Format_ARGB32:     return Image_Format_BGRA;
    case QImage::Format_Alpha8:     return Image_Format_Alpha;
    case QImage::Format_Grayscale8: return Image_Format_Gray;
    default:                        return Image_Format_UNKNOWN;
  }
}

// ================================================================
// Function : OcctQtImage
// ================================================================
OcctQtImage::OcctQtImage()
{
  //
}

// ================================================================
// Function : ~OcctQtImage
// ================================================================
OcctQtImage::~OcctQtImage()
{
  //
}

// ================================================================
// Function : Clear
// ================================================================
void OcctQtImage::Clear()
{
  Image_PixMap::Clear();
  myQImage = QImage();
}

// ================================================================
// Function : InitTrash
// ================================================================
bool OcctQtImage::InitTrash(Image_Format        thePixelFormat,
                            const Standard_Size theSizeX,
                            const Standard_Size theSizeY,
                            const Standard_Size theSizeRowBytes)
{
  (void)theSizeRowBytes;
  Clear();

  if (thePixelFormat != Image_Format_RGBA  && thePixelFormat != Image_Format_BGRA
   && thePixelFormat != Image_Format_RGB32 && thePixelFormat != Image_Format_BGR32
   && thePixelFormat != Image_Format_RGB   && thePixelFormat != Image_Format_BGR)
    return false;

  QImage aQImage(QSize(int(theSizeX), int(theSizeY)), QImage::Format_RGB32);
  if (aQImage.isNull())
    return false;

  SetTopDown(true);
  Image_PixMap::InitWrapper(Image_Format_BGRA, aQImage.bits(), theSizeX, theSizeY, aQImage.bytesPerLine());
  myQImage = aQImage;
  return true;
}

// ================================================================
// Function : Load
// ================================================================
bool OcctQtImage::Load(const Standard_Byte* theData, size_t theLength, const TCollection_AsciiString& theFileName)
{
  Clear();

  QImage aQImage;
  if (theData != nullptr)
  {
    if (theLength <= 0 || theLength > (size_t )std::numeric_limits<int>::max()
     || !aQImage.loadFromData((const uchar* )theData, (int )theLength))
      return false;
  }
  else if (!aQImage.load(QString::fromUtf8(theFileName.ToCString())))
  {
    return false;
  }

  const Image_Format aFormat = formatFromQt(aQImage.format());
  if (aFormat == Image_Format_UNKNOWN)
    return false;

  SetTopDown(true);
  Image_PixMap::InitWrapper(aFormat, aQImage.bits(), aQImage.width(), aQImage.height(), aQImage.bytesPerLine());
  myQImage = aQImage;
  return true;
}

// ================================================================
// Function : Save
// ================================================================
bool OcctQtImage::Save(const TCollection_AsciiString& theFileName)
{
  return myQImage.save(QString::fromUtf8(theFileName.ToCString()));
}

// ================================================================
// Function : LoadQtResource
// ================================================================
bool OcctQtImage::LoadQtResource(const QString& theResource)
{
  Clear();

  // QML path to resource path
  QString anImgPath = theResource;
  anImgPath.replace("qrc:///", ":/");

  QImage aQImage(anImgPath);
  if (aQImage.isNull())
    return false;

  const Image_Format aFormat = formatFromQt(aQImage.format());
  if (aFormat == Image_Format_UNKNOWN)
    return false;

  SetTopDown(true);
  Image_PixMap::InitWrapper(aFormat, aQImage.bits(), aQImage.width(), aQImage.height(), aQImage.bytesPerLine());
  myQImage = aQImage;
  return true;
}
