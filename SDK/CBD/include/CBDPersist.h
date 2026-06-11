/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_CBDPersist
#define H_CBDPersist

#include "CBD.h"

/*!
 * \brief Methods to persist standard Qt objects in xml.
 * 
 * \author pharvey (12/11/22)
 */
class CBDPersistNative
{
public:
    static QDomElement doSaveText( const CBD::CBDText &t, QDomDocument *pdomDoc, const QString &stringName = QString() );
    static QDomElement doSaveFont( const QFont &font, QDomDocument *pdomDoc, const QString &stringName = QString() );
    static QDomElement doSavePen( const QPen &pen, QDomDocument *pdomDoc, const QString &stringName = QString() );
    static QDomElement doSaveBrush( const QBrush &brush, QDomDocument *pdomDoc, const QString &stringName = QString() );
    static QDomElement doSaveColor( const QColor &color, QDomDocument *pdomDoc, const QString &stringName = QString() );
    static QDomElement doSaveImage( const QImage &image, QDomDocument *pdomDoc, const QString &stringName = QString() );

    static CBD::CBDText doLoadText( QDomElement *pdomElem );
    static QFont       doLoadFont( QDomElement *pdomElem );
    static QPen        doLoadPen( QDomElement *pdomElem );
    static QBrush      doLoadBrush( QDomElement *pdomElem );
    static QColor      doLoadColor( QDomElement *pdomElem );
    static QImage      doLoadImage( QDomElement *pdomElem );
};

/*!
 * \brief Methods to support persisting binary format in xml.
 * 
 * \author pharvey (12/11/22)
 */
class CBDPersistUtility
{
public:
    static  QString     getEncodedImage( const QImage &image, ulong *pnBytes );
    static  QImage      getDecodedImage( const QString &stringData, ulong nBytes );
    static  QString     getEncodedBinary( const QByteArray &ba, ulong *pnBytes );
    static  QByteArray  getDecodedBinary( const QString &stringData, ulong nBytes );
    static  QString     getEncodedText( const QString &stringIn, ulong *pnLen );
    static  QString     getDecodedText( const QString &stringIn, ulong nLen );

    static  QString     getEncodedBase64Text( const QString &stringIn );
    static  QString     getDecodedBase64Text( const QString &stringIn );

    static  QString     getEncodedUUText( const QString &stringIn, ulong *pnBytes );
    static  QString     getDecodedUUText( const QString &stringIn, ulong nBytes );
    static  QByteArray  getCompressedZLib( const QByteArray &bytesIn, ulong *pnBytesOut );
    static  QByteArray  getUncompressedZLib( const QByteArray &bytesIn, ulong nBytes );
    static  QString     getEncodedUU( const QByteArray &bytesIn );
    static  QByteArray  getDecodedUU( const QString &stringIn );

};

#endif

