/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LSImage
#define H_LSImage

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for PPaint.
 * 
 * \author pharvey (4/21/23)
 */
class LSImage : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(Qt::ImageConversionFlag nColorMode READ getImageColorMode WRITE setImageColorMode NOTIFY signalChanged)
    Q_PROPERTY(Qt::ImageConversionFlag nAlphaDither READ getImageAlphaDither WRITE setImageAlphaDither NOTIFY signalChanged)
    Q_PROPERTY(Qt::ImageConversionFlag nDither READ getImageDither WRITE setImageDither NOTIFY signalChanged)
    Q_PROPERTY(Qt::ImageConversionFlag nDitherMode READ getImageDitherMode WRITE setImageDitherMode NOTIFY signalChanged)
public:
    LSImage();

    void setImageColorMode( Qt::ImageConversionFlag );
    void setImageAlphaDither( Qt::ImageConversionFlag );
    void setImageDither( Qt::ImageConversionFlag );
    void setImageDitherMode( Qt::ImageConversionFlag );
    void setSelected( Qt::ImageConversionFlag, Qt::ImageConversionFlag, Qt::ImageConversionFlag, Qt::ImageConversionFlag );

    Qt::ImageConversionFlag getImageColorMode()     { return nColorMode;    }
    Qt::ImageConversionFlag getImageAlphaDither()   { return nAlphaDither;  }
    Qt::ImageConversionFlag getImageDither()        { return nDither;       }
    Qt::ImageConversionFlag getImageDitherMode()    { return nDitherMode;   }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    // ImageConversionFlags
    //      QPainter::drawImage()
    Qt::ImageConversionFlag nColorMode  = Qt::AutoColor;
    Qt::ImageConversionFlag nAlphaDither= Qt::ThresholdAlphaDither;
    Qt::ImageConversionFlag nDither     = Qt::DiffuseDither;
    Qt::ImageConversionFlag nDitherMode = Qt::AutoDither;
};

/*!
 * \brief Widget to edit LSImage.
 * 
 * \author pharvey (4/21/23)
 */
class LSImageWidget : public QWidget
{
    Q_OBJECT
public:
    LSImageWidget( QWidget *p, Qt::Orientation n );

public slots:
    void slotRefresh();

protected:
    LSImage *       pSetting;
    // nImageConversion
    QComboBox *     pColorMode;
    QComboBox *     pDither;
    QComboBox *     pAlphaDither;
    QComboBox *     pDitherMode;

protected slots:
    void slotImageColorMode( int );
    void slotImageAlphaDither( int );
    void slotImageDither( int );
    void slotImageDitherMode( int );
};

#endif


