/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */
#include "LibInfo.h"
#include "CBDPersist.h"

#ifdef Q_OS_WIN
    #include <QtZlib/zlib.h>
//    #include <QtZlib>
#else
    #include <zlib.h>
#endif

//
// CBDPersistNative
//

QDomElement CBDPersistNative::doSaveText( const CBD::CBDText &font, QDomDocument *pdomDoc, const QString &stringName )
{
    QDomElement domElem = pdomDoc->createElement( stringName.isEmpty() ? "Font" : stringName );
    domElem.appendChild( doSaveFont( font.font, pdomDoc ) );
    domElem.appendChild( doSaveColor( font.color, pdomDoc ) );
    domElem.appendChild( doSaveBrush( font.brush, pdomDoc ) );

    return domElem;
}

QDomElement CBDPersistNative::doSaveFont( const QFont &font, QDomDocument *pdomDoc, const QString &stringName )
{
    QDomElement domElem = pdomDoc->createElement( stringName.isEmpty() ? "Font" : stringName );
    domElem.setAttribute( "String", font.toString() );
    return domElem;
}

/*
QDomElement CBDPersistNative::doSaveFont( const QFont &font, QDomDocument *pdomDoc, const QString &stringName )
{
    // an alternative would be to use QFont::toString
    QDomElement domElem = pdomDoc->createElement( stringName.isEmpty() ? "Font" : stringName );
    domElem.setAttribute( "Bold", font.bold() );
    domElem.setAttribute( "Caps", font.capitalization() );
    domElem.setAttribute( "Family", font.family() );
    domElem.setAttribute( "FixedPitch", font.fixedPitch() );
    domElem.setAttribute( "HintingPreference", font.hintingPreference() );
    domElem.setAttribute( "Italic", font.italic() );
    domElem.setAttribute( "Kerning", font.kerning() );
    // domElem.setAttribute( "letterSpacing", font.letterSpacing() );
    domElem.setAttribute( "Overline", font.overline() );
    if ( font.pixelSize() >= 0 ) domElem.setAttribute( "PixelSize", font.pixelSize() );
    domElem.setAttribute( "PointSize", font.pointSize() );
    domElem.setAttribute( "Stretch", font.stretch() );
    domElem.setAttribute( "StrikeOut", font.strikeOut() );
    domElem.setAttribute( "Style", font.style() );
    // domElem.setAttribute( "styleHint", font.styleHint() );
    domElem.setAttribute( "StyleStrategy", font.styleStrategy() );
    domElem.setAttribute( "Underline", font.underline() );
    domElem.setAttribute( "Weight", font.weight() );
    domElem.setAttribute( "WordSpacing", font.wordSpacing() );

    return domElem;
}
*/

QDomElement CBDPersistNative::doSavePen( const QPen &pen, QDomDocument *pdomDoc, const QString &stringName )
{
    QDomElement domElem = pdomDoc->createElement(  stringName.isEmpty() ? "Pen" : stringName );
    domElem.setAttribute( "CapStyle", pen.capStyle() );
    domElem.setAttribute( "Cosmetic", pen.isCosmetic() );
    domElem.setAttribute( "DashOffset", pen.dashOffset() );
    // domElem.setAttribute( "dashPattern", pen.dashPattern() );
    domElem.setAttribute( "JoinStyle", pen.joinStyle() );
    domElem.setAttribute( "MiterLimit", pen.miterLimit() );
    domElem.setAttribute( "Style", pen.style() );
    domElem.setAttribute( "Width", pen.width() );

    domElem.appendChild( doSaveBrush( pen.brush(), pdomDoc ) );
    if ( pen.color().isValid() )
        domElem.appendChild( doSaveColor( pen.color(), pdomDoc ) );

    return domElem;
}

QDomElement CBDPersistNative::doSaveBrush( const QBrush &brush, QDomDocument *pdomDoc, const QString &stringName )
{
    QDomElement domElem = pdomDoc->createElement( stringName.isEmpty() ? "Brush" : stringName );
    // domElem.setAttribute( "matrix", brush.matrix() );
    domElem.setAttribute( "Style", brush.style() );
    // domElem.setAttribute( "transform", brush.transform() );

    if ( brush.color().isValid() )
        domElem.appendChild( doSaveColor( brush.color(), pdomDoc ) );
    if ( !brush.textureImage().isNull() )
        domElem.appendChild( doSaveImage( brush.textureImage(), pdomDoc, "Texture" ) );

    return domElem;
}

QDomElement CBDPersistNative::doSaveColor( const QColor &color, QDomDocument *pdomDoc, const QString &stringName )
{
    QColor c = color.convertTo( QColor::Rgb );

    QDomElement domElem = pdomDoc->createElement( stringName.isEmpty() ? "Color" : stringName );
    domElem.setAttribute( "Alpha", c.alpha() );
    domElem.setAttribute( "Red", c.red() );
    domElem.setAttribute( "Green", c.green() );
    domElem.setAttribute( "Blue", c.blue() );

    return domElem;
}

/*!
 * \brief Saves a QImage into XML. 
 *  
 * \note Avoid saving QPixmap because they need a qApp and, in most cases, a destructor will 
 *       not have a qApp.... the app will segfault in such a case.
 * 
 * \author pharvey (12/10/22)
 * 
 * \param image 
 * \param pdomDoc 
 * \param stringName 
 * 
 * \return QDomElement 
 */
QDomElement CBDPersistNative::doSaveImage( const QImage &image, QDomDocument *pdomDoc, const QString &stringName )
{
    Q_UNUSED(image);

    QDomElement domElem = pdomDoc->createElement( stringName.isEmpty() ? "Image" : stringName );

    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( image, &nBytesCompressed );
    domElem.setAttribute( "Size", QString::number( nBytesCompressed ) );
    domElem.setAttribute( "Data", stringEncodedImage );

    return domElem;
}

CBD::CBDText CBDPersistNative::doLoadText( QDomElement *pdomElem )
{
    CBD::CBDText font;

    // load additional nodes...
    QDomNode domNode = pdomElem->firstChild();
    while( !domNode.isNull() ) 
    {
        QDomElement domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Font" )
            font.font = doLoadFont( &domElem );
        else if ( domElem.tagName() == "Color" )
            font.color = doLoadColor( &domElem );
        else if ( domElem.tagName() == "Brush" )
            font.brush = doLoadBrush( &domElem );

        domNode = domNode.nextSibling();
    }

    return font;
}

QFont CBDPersistNative::doLoadFont( QDomElement *pdomElem )
{
    QFont font;
    font.fromString( pdomElem->attribute( "String", font.toString() ) );
    return font;
}

/*
QFont CBDPersistNative::doLoadFont( QDomElement *pdomElem )
{
    // an alternative to this would be to use QFont::fromString
    QFont font;

    font.setBold( pdomElem->attribute( "Bold" ).toInt() );
    font.setCapitalization( (QFont::Capitalization)pdomElem->attribute( "Caps" ).toInt() );
    font.setFamily( pdomElem->attribute( "Family" ) );
    font.setFixedPitch( pdomElem->attribute( "FixedPitch" ).toInt() );
    font.setHintingPreference( (QFont::HintingPreference)pdomElem->attribute( "HintingPreference" ).toInt() );
    font.setItalic( pdomElem->attribute( "Italic" ).toInt() );
    font.setKerning( pdomElem->attribute( "Kerning" ).toInt() );
    // font.setLetterSpacing( (QFont::SpacingType)pdomElem->attribute( "letterSpacing" ).toInt() );
    font.setOverline( pdomElem->attribute( "Overline" ).toInt() );
    if ( !pdomElem->attribute( "PixelSize" ).isEmpty() ) font.setPixelSize( pdomElem->attribute( "PixelSize" ).toInt() );
    if ( !pdomElem->attribute( "PointSize" ).isEmpty() ) font.setPointSize( pdomElem->attribute( "PointSize" ).toInt() );
    font.setStretch( pdomElem->attribute( "Stretch" ).toInt() );
    font.setStrikeOut( pdomElem->attribute( "StrikeOut" ).toInt() );
    font.setStyle( (QFont::Style)pdomElem->attribute( "Style" ).toInt() );
    // pdomElem->attribute( "styleHint" );
    font.setStyleStrategy( (QFont::StyleStrategy)pdomElem->attribute( "StyleStrategy" ).toInt() );
    font.setUnderline( pdomElem->attribute( "Underline" ).toInt() );
    if ( !pdomElem->attribute( "Weight" ).isEmpty() ) font.setWeight( (QFont::Weight)pdomElem->attribute( "Weight" ).toInt() );
    font.setWordSpacing( pdomElem->attribute( "WordSpacing" ).toLong() );

    return font;
}
*/

QPen CBDPersistNative::doLoadPen( QDomElement *pdomElem )
{
    QPen pen;

    pen.setCapStyle( (Qt::PenCapStyle)pdomElem->attribute( "CapStyle" ).toInt() );
    pen.setCosmetic( pdomElem->attribute( "Cosmetic" ).toInt() );
    pen.setDashOffset( pdomElem->attribute( "DashOffset" ).toLong() );
    // pdomElem->attribute( "dashPattern", pen.dashPattern() );
    pen.setJoinStyle( (Qt::PenJoinStyle)pdomElem->attribute( "JoinStyle" ).toInt() );
    pen.setMiterLimit( pdomElem->attribute( "MiterLimit" ).toLong() );
    pen.setStyle( (Qt::PenStyle)pdomElem->attribute( "Style" ).toInt() );
    pen.setWidth( pdomElem->attribute( "Width" ).toInt() );

    // load child nodes (of interest)
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElem->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Brush" )
            pen.setBrush( doLoadBrush( &domElem ) );
        else if ( domElem.tagName() == "Color" )
            pen.setColor( doLoadColor( &domElem ) );

        domNode = domNode.nextSibling();
    }

    return pen;
}

QBrush CBDPersistNative::doLoadBrush( QDomElement *pdomElem )
{
    QBrush brush;

    Qt::BrushStyle nStyle = (Qt::BrushStyle)pdomElem->attribute( "Style" ).toInt();
    QImage imageTexture;

    // domElem.setAttribute( "matrix", brush.matrix() );
    // domElem.setAttribute( "transform", brush.transform() );

    // load child nodes (of interest)
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElem->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Color" )
            brush.setColor( doLoadColor( &domElem ) );
        else if ( domElem.tagName() == "Texture" )
           imageTexture = doLoadImage( &domElem );

        domNode = domNode.nextSibling();
    }

    brush.setTextureImage( imageTexture );
    brush.setStyle( nStyle );

    return brush;
}

QColor CBDPersistNative::doLoadColor( QDomElement *pdomElem )
{
    int nRed    = pdomElem->attribute( "Red" ).toInt();
    int nGreen  = pdomElem->attribute( "Green" ).toInt();
    int nBlue   = pdomElem->attribute( "Blue" ).toInt();
    int nAlpha  = pdomElem->attribute( "Alpha" ).toInt();

    return QColor( nRed, nGreen, nBlue, nAlpha );
}

QImage CBDPersistNative::doLoadImage( QDomElement *pdomElem )
{
    QImage image;

    ulong nBytesCompressed = pdomElem->attribute( "Size", "0" ).toULong();
    if ( nBytesCompressed )
    { 
        QString stringAttribute = pdomElem->attribute( "Data" );
        if ( !stringAttribute.isEmpty() )
            image = CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed );
    }

    return image;
}

//
// CBDPersistUtility
//

/*!
 * \brief Encode a QImage so it can be safely saved in a text file (ie XML, JSON,...).
 *  
 * This is done by; 
 *  
 * 1 - compressing the image data using zlib
 * 2 - uuencoding the compressed data 
 *  
 * The resulting string can be saved in a text file. The number of bytes will need 
 * to be saved with it in order to call \sa getDecodedImage() later. 
 *  
 * \note Saved in PNG format.
 *  
 * \note The number of bytes processed can not exceed capacity of a 'ulong'. 
 *  
 * \author pharvey (11/24/19)
 * 
 * \param image 
 * \param pnBytes The number of bytes required by getDecodeImage().
 * 
 * \return QString 
 */
QString CBDPersistUtility::getEncodedImage( const QImage &image, ulong *pnBytes )
{
    QByteArray bytes;
    {
        QBuffer buffer( &bytes );
        buffer.open( QIODevice::WriteOnly );
        image.save( &buffer, "PNG" );
    }

    return getEncodedBinary( bytes, pnBytes );
}

/*!
 * \brief Decode a QImage. 
 *  
 * See \sa getEncodedImage() for details. 
 * 
 * \author pharvey (11/24/19)
 * 
 * \param stringEncoded The encoded string as provided by getEncodedImage(). 
 * \param nBytes The number of bytes needed for the image. This was provided by getEncodedImage(). 
 * 
 * \return QImage 
 */
QImage CBDPersistUtility::getDecodedImage( const QString &stringEncoded, ulong nBytes )
{
    QImage image;
    image.loadFromData( getDecodedBinary( stringEncoded, nBytes ) );
    return image;
}

QString CBDPersistUtility::getEncodedBinary( const QByteArray &bytes, ulong *pnBytes )
{
    Q_ASSERT( pnBytes );

    *pnBytes = bytes.size(); 

    // compress bytearray...
    ulong nBufferSize = bytes.size() * 2;  // larger than we need but safer?   
    QByteArray bytesCompressed( nBufferSize, '\0' );
    ::compress( (uchar*)bytesCompressed.data(), &nBufferSize, (uchar*)bytes.data(), bytes.size() );

    // double duty for nBufferSize: buffer size IN and bytes compressed OUT
    int nBytesCompressed = int(nBufferSize); 

    // encode bytearray 
    // - make all chars safe for a string
    // - result in a string that can be saved to text (ie XML) file
    QString stringEncodedImage;
    static const char szHexChars[] = "0123456789abcdef";
    for ( int nIndex = 0; nIndex < nBytesCompressed; ++nIndex )
    {
        uchar s = (uchar)bytesCompressed[nIndex];
        stringEncodedImage += szHexChars[s >> 4];
        stringEncodedImage += szHexChars[s & 0x0f];
    }

    return stringEncodedImage;
}

QByteArray CBDPersistUtility::getDecodedBinary( const QString &stringEncoded, ulong nBytes )
{
    // uudecode (string to bytearray)
    // - stringEncoded will have 2 chars for each original byte so the
    //   bytesCompressed will be half the len
    char *bytesCompressed = new char[ stringEncoded.length() / 2 ];
    for ( int nIndex = 0; nIndex < stringEncoded.length() / 2; ++nIndex )
    {
        char cHi = stringEncoded[ 2 * nIndex ].toLatin1();
        char cLo = stringEncoded[ 2 * nIndex  + 1 ].toLatin1();

        uchar cDecoded = 0;
        if ( cHi <= '9' ) cDecoded += cHi - '0';
        else cDecoded += cHi - 'a' + 10;
        cDecoded = cDecoded << 4;

        if ( cLo <= '9' ) cDecoded += cLo - '0';
        else cDecoded += cLo - 'a' + 10;

        bytesCompressed[ nIndex ] = cDecoded;
    }

    // uncompress
    // - we need to be told how big the data was hence nBytes
    QByteArray bytesDecoded( nBytes, '\0' );
    ::uncompress( (uchar*) bytesDecoded.data(), &nBytes, (uchar*) bytesCompressed, stringEncoded.length()/2 );

    delete [] bytesCompressed;

    return bytesDecoded;
}

QString CBDPersistUtility::getEncodedText( const QString &stringIn, ulong *pnBytes )
{
    return getEncodedBinary( stringIn.toUtf8(), pnBytes );

//    return getEncodedUUText( stringIn, pnBytes );

    *pnBytes = 0;
    QByteArray bytesCompressed = qCompress( stringIn.toUtf8() );
    return getEncodedBase64Text( QString( bytesCompressed ) );
}

QString CBDPersistUtility::getDecodedText( const QString &stringIn, ulong nBytes )
{
    return getDecodedBinary( stringIn, nBytes );

//    return getDecodedUUText( stringIn, nBytes );

    Q_UNUSED(nBytes);
    QString stringDecoded = getDecodedBase64Text( stringIn );
    return QString( qUncompress( stringDecoded.toUtf8() ) );
}

QString CBDPersistUtility::getEncodedBase64Text( const QString &stringIn )
{
    QByteArray bytesIn( stringIn.toUtf8() );

    // compress
    QByteArray bytesCompressed = qCompress( bytesIn );

    // encode
    QByteArray bytesBase64 = bytesCompressed.toBase64();

    return QString( bytesBase64 );
}

QString CBDPersistUtility::getDecodedBase64Text( const QString &stringIn )
{
    QByteArray bytesIn( stringIn.toUtf8() );

    // decode
    QByteArray bytesCompressed = QByteArray::fromBase64( bytesIn );

    // uncompress
    QByteArray bytesOut = qUncompress( bytesCompressed );

    return QString( bytesOut );
}

QString CBDPersistUtility::getEncodedUUText( const QString &stringIn, ulong *pnBytes )
{
    // test
    QString s( "Hello world!" );
printf( "[PAH][%s][%s][%d] [%s]\n", __FILE__, __FUNCTION__, __LINE__, getDecodedUU( getEncodedUU( s.toUtf8() ) ).constData() );

printf( "[PAH][%s][%s][%d] [%s] %lld %ld\n", __FILE__, __FUNCTION__, __LINE__,  getUncompressedZLib( getCompressedZLib( s.toUtf8(), pnBytes ), *pnBytes ).constData(), s.toUtf8().size(), *pnBytes );

    return getEncodedUU( getCompressedZLib( stringIn.toUtf8(), pnBytes ) );
}

QString CBDPersistUtility::getDecodedUUText( const QString &stringIn, ulong nBytes )
{
    return QString( getUncompressedZLib( getDecodedUU( stringIn ), nBytes ) );
}

/*!
 * \brief Compress data using zlib. \sa getUncompressedZLib
 * 
 * \author pharvey (9/19/25)
 * 
 * \param bytesIn       Data to compress.
 * \param pnBytesOut    Upon entry, pnBytesOut is the total size of the destination buffer, which must be at least the size of bytesIn.
 *                      Upon exit, pnBytesOut is the actual size of the compressed data.
 *                      This can be used for nBytes when calling getUncompressedZLib.
 * 
 * \return QByteArray 
 */
QByteArray CBDPersistUtility::getCompressedZLib( const QByteArray &bytesIn, ulong *pnBytesOut )
{
// printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    ulong       nBytesIn = bytesIn.size() + 2;
    QByteArray  bytesOut( nBytesIn, '\0' ); // assume result will be same or smaller 

    *pnBytesOut = nBytesIn;                 // actual number of bytes
    int nError = ::compress( (uchar*)bytesOut.data(), pnBytesOut, (uchar*)bytesIn.data(), nBytesIn );
    if ( nError != Z_OK ) printf( "[PAH][%s][%s][%d] %d\n", __FILE__, __FUNCTION__, __LINE__, nError );

printf( "[PAH][%s][%s][%d] %ld\n", __FILE__, __FUNCTION__, __LINE__, *pnBytesOut );
    return bytesOut;
}

/*!
 * \brief Uncompress data using zlib. \sa getCompressedZLib 
 * 
 * \author pharvey (9/19/25)
 * 
 * \param bytesIn   Data to uncompress.
 * \param nBytes    Number of bytes that will be produced.
 *                  This is the value returned in pnBytesOut when calling getCompressedZLib.
 * 
 * \return QByteArray 
 */
QByteArray CBDPersistUtility::getUncompressedZLib( const QByteArray &bytesIn, ulong nBytes )
{
printf( "[PAH][%s][%s][%d] %ld\n", __FILE__, __FUNCTION__, __LINE__, nBytes );
    QByteArray bytesOut( nBytes, '\0' );
    int nError = ::uncompress( (uchar*)bytesOut.data(), &nBytes, (uchar*)bytesIn.data(), bytesIn.size() );
    if ( nError != Z_OK ) printf( "[PAH][%s][%s][%d] %d\n", __FILE__, __FUNCTION__, __LINE__, nError );

printf( "[PAH][%s][%s][%d] %ld\n", __FILE__, __FUNCTION__, __LINE__, nBytes );
    return bytesOut;
}

QString CBDPersistUtility::getEncodedUU( const QByteArray &bytesIn )
{
// printf( "[PAH][%s][%s][%d] %lld\n", __FILE__, __FUNCTION__, __LINE__, bytesIn.size() );
    ulong   nBytes = bytesIn.size(); 
    QString stringOut;
    static const char szHexChars[] = "0123456789abcdef";
    for ( ulong nIndex = 0; nIndex < nBytes; ++nIndex )
    {
        uchar c = (uchar)bytesIn[nIndex];
        stringOut += szHexChars[c >> 4];
        stringOut += szHexChars[c & 0x0f];
    }

// printf( "[PAH][%s][%s][%d] %lld\n", __FILE__, __FUNCTION__, __LINE__, stringOut.length() );
    return stringOut;
}

QByteArray CBDPersistUtility::getDecodedUU( const QString &stringIn )
{
// printf( "[PAH][%s][%s][%d] %lld\n", __FILE__, __FUNCTION__, __LINE__, stringIn.length() );
    ulong nChars = stringIn.length() / 2; // div by 2 because we process 2 chars each loop
    QByteArray bytesOut( nChars, '\0' );

    for ( ulong nIndex = 0; nIndex < nChars; ++nIndex )
    {
        char cHi = stringIn[ 2 * nIndex ].toLatin1();
        char cLo = stringIn[ 2 * nIndex  + 1 ].toLatin1();

        uchar cDecoded = 0;
        if ( cHi <= '9' ) cDecoded += cHi - '0';
        else cDecoded += cHi - 'a' + 10;
        cDecoded = cDecoded << 4;

        if ( cLo <= '9' ) cDecoded += cLo - '0';
        else cDecoded += cLo - 'a' + 10;

        bytesOut[ nIndex ] = cDecoded;
    }

// printf( "[PAH][%s][%s][%d] %lld\n", __FILE__, __FUNCTION__, __LINE__, bytesOut.size() );
    return bytesOut;
}


