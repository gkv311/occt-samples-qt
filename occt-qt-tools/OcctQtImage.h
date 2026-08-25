// Copyright (c) 2025 Kirill Gavrilov

#ifndef _OcctQtImage_HeaderFile
#define _OcctQtImage_HeaderFile

#include <Image_PixMap.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QImage>
#include <Standard_WarningsRestore.hxx>

//! Auxiliary wrapper over QImage to use as alternative to Image_AlienPixMap.
class OcctQtImage : public Image_PixMap
{
public:

  //! Empty constructor.
  OcctQtImage();

  //! Destructor
  virtual ~OcctQtImage();

  //! Return QImage instance.
  QImage& GetQImage() { return myQImage; }

public:

  //! Load image from file.
  //! @sa Image_AlienPixMap::Load()
  bool Load(const TCollection_AsciiString& theFileName) { return Load(nullptr, 0, theFileName); }

  //! Read image data from memory buffer
  //! @sa Image_AlienPixMap::Load()
  bool Load(const Standard_Byte* theData, size_t theLength, const TCollection_AsciiString& theFileName);

  //! Write image data to file using file extension to determine compression format.
  //! @sa Image_AlienPixMap::Save()
  bool Save(const TCollection_AsciiString& theFileName);

  //! Load image from Qt resource.
  bool LoadQtResource(const QString& theResource);

protected: //! @name override low-level interface of Image_PixMap to wrap QImage

  //! Clear the image.
  virtual void Clear() override;

  //! Initialize image plane with required dimensions.
  virtual bool InitTrash(Image_Format        thePixelFormat,
                         const Standard_Size theSizeX,
                         const Standard_Size theSizeY,
                         const Standard_Size theSizeRowBytes = 0) override;

private:

  QImage myQImage;

};

#endif // _OcctQtImage_HeaderFile
