/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SSettings.h"

SSettings::SSettings( Mode n )
{
    nMode = n;
/*

    // QStandardPaths::ConfigLocation returns ~/AppData/Local on WIN
    // dir does not exist on first run so ensure it exists
#ifdef Q_OS_WIN
    {
        QDir dir;
        QString stringDir = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
printf( "%s\n", stringDir.toUtf8().constData() );
        stringDir += "/AppData";
        if ( !dir.exists( stringDir ) ) dir.mkdir( stringDir );
        stringDir += "/Local";
        if ( !dir.exists( stringDir ) ) dir.mkdir( stringDir );
    }
#endif
    
*/
    QString stringDir = QStandardPaths::writableLocation( QStandardPaths::ConfigLocation );
    stringDir += ("/" LIB_NAME);
    QDir dir( stringDir );
    if ( !dir.exists() ) 
    {
        if ( !dir.mkpath( stringDir ) ) printf( "WARNING: Failed to create %s", stringDir.toUtf8().constData() );
    }

    stringFileName += stringDir + ("/" SSETTINGS_DOC_CLASS ".xml");
}

SSetting *SSettings::getSetting( const QString &stringKey )
{
    if ( !mapSettings.contains( stringKey ) )
    {
        qInfo() << "CAUTION: [" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [" << stringKey << "].";
        return nullptr;
    }

    return mapSettings.value( stringKey );
}

void SSettings::doInsert( const QString stringKey, SSetting *p )
{
    Q_ASSERT( !stringKey.isEmpty() );
    Q_ASSERT( p );

    if ( mapSettings.contains( stringKey ) )
    {
        QMessageBox::warning( qApp->activeWindow(), LIB_NAME, "Setting exists [" + stringKey + "]."  );
        return;
    }

    p->setParent( this );

    mapSettings.insert( stringKey, p );
}

void SSettings::doRemove( const QString stringKey )
{
    SSetting *p = mapSettings.take( stringKey );
    if ( p )
    {
        QMessageBox::warning( qApp->activeWindow(), LIB_NAME, "Setting does not exists [" + stringKey + "]."  );
        return;
    }

    delete p;
}

void SSettings::doSave()
{
    // create
    QFile file( stringFileName );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        QMessageBox::information( qApp->activeWindow(), tr("Save Default Context"), tr("Could not open ") + file.fileName() );
        return;
    }

    // create an XML document with class name and document version...
    QDomDocument domDoc( SSETTINGS_DOC_CLASS );
    QDomElement domElementRoot = domDoc.createElement( SSETTINGS_DOC_CLASS );
    domElementRoot.setAttribute( "DocVer", SDK_DOC_VER );

    domDoc.appendChild( domElementRoot );

    // add settings...
    QMapIterator<QString,SSetting*> i( mapSettings );
    while ( i.hasNext() ) 
    {
        i.next();
        ((SSetting*)i.value())->doSave( &domDoc, &domElementRoot );
    }

    // write
    QString stringXML = domDoc.toString();
    file.write( stringXML.toLatin1(), stringXML.length() );

    // close
    file.close();
}

void SSettings::doLoad()
{
    // open a file...
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        // assume we could not load because its the first time running and config file not yet saved
        doSave();
        // QMessageBox::information( qApp->activeWindow(), tr("Open Default Context"), tr("Could not open file.\n\n") + stringFileName, QMessageBox::Ok );
        return;
    }

    // read file into XML document...
    QDomDocument domDoc( SSETTINGS_DOC_CLASS );
    if ( !domDoc.setContent( &file ) )
    {
        QMessageBox::information( qApp->activeWindow(), tr("Open Default Context"), tr("Invalid file format.\nCould not create an XML document from file.\n\n") + file.fileName() );
        file.close();
        return;
    }
    file.close();

    // validate XML...
    // Root element is only used to check that class in XML matches this class.
    QDomElement domElemRoot = domDoc.documentElement();
    if ( domElemRoot.isNull() )
    {
        QMessageBox::information( qApp->activeWindow(), tr("Open Default Context"), tr("Invalid file format.\nCould not find a document element.\n\n") + stringFileName );
        return;
    }
    if ( domElemRoot.tagName() != SSETTINGS_DOC_CLASS )
    {
        QMessageBox::information( qApp->activeWindow(), tr("Open Default Context"), tr("Invalid file format.\nFile does not appear to be a ") + SSETTINGS_DOC_CLASS + "\n\n" + stringFileName );
        return;
    }
    if ( domElemRoot.attribute( "DocVer" ) != SDK_DOC_VER ) 
    {
        QMessageBox::information( qApp->activeWindow(), tr("Open Default Context"), tr("Invalid file format.\nFile is not Version ") + SDK_DOC_VER );
        return;
    }

    // Its our data so lets start processing starting at first child...
    QDomNode domNode = domElemRoot.firstChild();

    while ( !domNode.isNull() )
    {
        if ( domNode.nodeType() != QDomNode::ElementNode )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        QDomElement domElem = domNode.toElement();
        if ( domElem.isNull() )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        SSetting *p = mapSettings.value( domElem.tagName() );
        if ( p ) p->doLoad( &domElem );

        domNode = domNode.nextSibling();
    }
    return;
}



/*!
 * \brief Single global instance for context.
 *  
 * We avoid auto initialization so we can control exactly when this gets 
 * created, initialized, finalized, and deleted. 
 *  
 * \note Other instances of SSettings can be created, as needed, for preferences. 
 *  
 * \sa main() 
 * 
 * \author pharvey (12/23/22)
 */
SSettings *g_SSettings;

