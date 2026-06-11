/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LSText.h"

#include <SFont.h>

/****************************************************************
 * LSText
 ****************************************************************/
LSText::LSText()
{
}

void LSText::setText( const QString & t )
{
    if ( t == stringText ) return;
    stringText = t;
    emit signalChanged();
}

void LSText::setHAlign( Qt::AlignmentFlag t )
{
    if ( t == nHAlign ) return;
    nHAlign = t;
    emit signalChanged();
}

void LSText::setVAlign( Qt::AlignmentFlag t )
{
    if ( t == nVAlign ) return;
    nVAlign = t;
    emit signalChanged();
}

void LSText::setSelected( const QString &stringText, Qt::AlignmentFlag nHAlign, Qt::AlignmentFlag nVAlign )
{
    this->stringText    = stringText;
    this->nHAlign       = nHAlign;
    this->nVAlign       = nVAlign;
    emit signalSync();
}

QWidget *LSText::getToolBar( QWidget *pParent )
{
    return new LSTextPanel( pParent, Qt::Horizontal );
}

QWidget *LSText::getPanel( QWidget *pParent )
{
    return new LSTextPanel( pParent, Qt::Vertical );
}

void LSText::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSText" );

    // default text - if any
    if ( !stringText.isEmpty() )
    {
        unsigned long nLen = 0;
        domElem.setAttribute( "Text", CBDPersistUtility::getEncodedText( stringText, &nLen ) ); 
        domElem.setAttribute( "TextSize", QString::number( nLen ) );
    }

    domElem.setAttribute( "HAlign", nHAlign );
    domElem.setAttribute( "VAlign", nVAlign );

    pdomElem->appendChild( domElem );
}

void LSText::doLoad( QDomElement *pdomElem )
{
    QString stringAttribute = pdomElem->attribute( "TextSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElem->attribute( "Text" );
        if ( !stringAttribute.isNull() )
            stringText = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    nHAlign = Qt::AlignmentFlag(pdomElem->attribute( "HAlign", QString::number( int(nHAlign) ) ).toInt());
    nVAlign = Qt::AlignmentFlag(pdomElem->attribute( "VAlign", QString::number( int(nVAlign) ) ).toInt());
}

/****************************************************************
 * LSTextPanel
 ****************************************************************/
LSTextPanel::LSTextPanel( QWidget *p, Qt::Orientation nOrientation )
    : QWidget( p )
{
    pSetting = (LSText*)g_SSettings->getSetting( "LSText" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [LSText].";
        return;
    }

    // create widgets
    SFontToolBar *  pFontToolBar    = nullptr;
    SFontPanel *    pFontPanel      = nullptr;
    //
    pLineEdit = new QLineEdit( pSetting->getText(), this );
    connect( pLineEdit, SIGNAL(textChanged(const QString &)), SLOT(slotText(const QString &)) );
    //
    pHAlign = new WTextHAlignComboBox( pSetting->getHAlign(), this );
    connect( pHAlign, SIGNAL(signalChanged(Qt::AlignmentFlag)), SLOT(slotHAlign(Qt::AlignmentFlag)) );
    //
    pVAlign = new WTextVAlignComboBox( pSetting->getVAlign(), this );
    connect( pVAlign, SIGNAL(signalChanged(Qt::AlignmentFlag)), SLOT(slotVAlign(Qt::AlignmentFlag)) );
    //
    if ( nOrientation == Qt::Vertical )
    {
        pFontPanel = new SFontPanel( this );
    }
    else
    {
        pFontToolBar = new SFontToolBar( this );
        pMore = new QToolButton( this );
        pMore->setText( ":" );
        connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );
    }

    // layout
    if ( nOrientation == Qt::Vertical )
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Font"), pFontPanel );
        pLayout->addRow( tr("Text"), pLineEdit );
        pLayout->addRow( tr("H-Align"), pHAlign );
        pLayout->addRow( tr("V-Align"), pVAlign );
    }
    else
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pFontToolBar );
        pLayout->addWidget( pLineEdit );
        pLayout->addWidget( pHAlign );
        pLayout->addWidget( pVAlign );
        pLayout->addWidget( pMore );
        pLayout->addStretch( 10 );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSTextPanel::slotRefresh()
{
    pLineEdit->setText( pSetting->getText() );
    pHAlign->setValue( pSetting->getHAlign() );
    pVAlign->setValue( pSetting->getVAlign() );
}

void LSTextPanel::slotText( const QString &t )
{
    pSetting->setText( t );
}

void LSTextPanel::slotHAlign( Qt::AlignmentFlag n )
{
    pSetting->setHAlign( n );
}

void LSTextPanel::slotVAlign( Qt::AlignmentFlag n )
{
    pSetting->setVAlign( n );
}

void LSTextPanel::slotMore()
{
    pSetting->doDialog( this );
}


