/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_EFile
#define H_EFile

#include <LFile.h>
#include <LLayer.h>

/*!
 * \brief A file which contains layers and may be saved/loaded.
 * 
 * LLayers supported; background (transparency), bitmap, and vector.
 * 
 * File formats supported; bitmap (png, jpg, xpm, etc), vector (svg), and XML.
 * 
 * \note Layers will be flattened/reduced to 1 if saved as a bitmap.
 * \note Layers will be flattened/reduced to 1 if saved as a svg.
 * \note XML supports saving/loading multiple layers and additional info for any objects which have additional info.
 * 
 * \author pharvey (4/13/23)
 */
class EFile : public LFile
{
    Q_OBJECT
public:
    EFile( ADObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~EFile();

    virtual QMenu *         getContextMenu( QWidget * ) override;
    LLayer::LayerTypes      getLayerType() override      { return LLayer::LayerTypeRaster;   }
    LLayer::TransportTypes  getTransportType() override  { return LLayer::TransportTypeImage;}

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );
    bool doImport();
    bool doExport();
    bool doPrint();

public slots:
    virtual bool slotOpenEditor() override;
    virtual void slotNewBitmap();
    virtual void slotNewVector();
    virtual AWObject::SaveResults slotSaveAs( bool bRequest = true ) override;

protected:
    virtual bool doRead( const QString &stringFileName ) override;
    virtual bool doWrite( const QString &stringFileName ) override;

    virtual bool doReadXML( const QString &stringFileName );
    virtual bool doReadImage( const QString &stringFileName );
    virtual bool doWriteXML( const QString &stringFileName );
    virtual bool doWriteImage( const QString &stringFileName );

    virtual QString getFileSpec( const QString &stringName = QString() ) override;
    virtual QString getFileSpecImage();
    virtual QString getFileSpecXML();
    virtual QString getFileExtension();

    // ADObject: Make protected...
    ADObject *getObject( const QString &stringClass, ADObject *pObjectParent = nullptr ) override; // can create - transparent, paint and grid models
};

#endif



