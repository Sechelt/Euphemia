/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "STextContent.h"

/****************************************************************
 * STextContent
 ****************************************************************/
STextContent::STextContent()
{
}

void STextContent::setCode( const QString &t )
{
    if ( t == stringCode ) return;
    stringCode = t;
    emit signalChanged();
}

void STextContent::setName( const QString &t )
{
    if ( t == stringName ) return;
    stringName = t;
    emit signalChanged();
}

void STextContent::setComment( const QString &t )
{
    if ( t == stringComment ) return;
    stringComment = t;
    emit signalChanged();
}

void STextContent::setDisplayCode( bool b )
{
    if ( b == bDisplayCode ) return;
    bDisplayCode = b;
    emit signalChanged();
}

void STextContent::setDisplayName( bool b )
{
    if ( b == bDisplayName ) return;
    bDisplayName = b;
    emit signalChanged();
}

void STextContent::setDisplayComment( bool b )
{
    if ( b == bDisplayComment ) return;
    bDisplayComment = b;
    emit signalChanged();
}

void STextContent::setSelectedCode( const QString &t )
{
    if ( t == stringCode ) return;
    stringCode = t;
    emit signalSync();
}

void STextContent::setSelectedName( const QString &t )
{
    if ( t == stringName ) return;
    stringName = t;
    emit signalSync();
}

void STextContent::setSelectedComment( const QString &t )
{
    if ( t == stringComment ) return;
    stringComment = t;
    emit signalSync();
}

void STextContent::setSelectedDisplayCode( bool b )
{
    if ( b == bDisplayCode ) return;
    bDisplayCode = b;
    emit signalSync();
}

void STextContent::setSelectedDisplayName( bool b )
{
    if ( b == bDisplayName ) return;
    bDisplayName = b;
    emit signalSync();
}

void STextContent::setSelectedDisplayComment( bool b )
{
    if ( b == bDisplayComment ) return;
    bDisplayComment = b;
    emit signalSync();
}

QWidget *STextContent::getButton( QWidget *pParent )
{
    Q_UNUSED( pParent );
    return nullptr;
}

QWidget *STextContent::getToolBar( QWidget *pParent )
{
    return new STextContentToolBar( pParent );
}

QWidget *STextContent::getPanel( QWidget *pParent )
{
    return new STextContentPanel( pParent );
}

void STextContent::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElem = pdomDoc->createElement( "STextContent" );

    // may not want to save/load these?
    if ( !stringCode.isEmpty() )
    {
        unsigned long nLen = 0;
        domElem.setAttribute( "Code", CBDPersistUtility::getEncodedText( stringCode, &nLen ) ); 
        domElem.setAttribute( "CodeSize", QString::number( nLen ) );
    }
    if ( !stringName.isEmpty() )
    {
        unsigned long nLen = 0;
        domElem.setAttribute( "Name", CBDPersistUtility::getEncodedText( stringName, &nLen ) ); 
        domElem.setAttribute( "NameSize", QString::number( nLen ) );
    }
    if ( !stringComment.isEmpty() )
    {
        unsigned long nLen = 0;
        domElem.setAttribute( "Comment", CBDPersistUtility::getEncodedText( stringComment, &nLen ) ); 
        domElem.setAttribute( "CommentSize", QString::number( nLen ) );
    }

    if ( bDisplayCode ) domElem.setAttribute( "DisplayCode", QString::number( 1 ) );
    if ( bDisplayName ) domElem.setAttribute( "DisplayName", QString::number( 1 ) );
    if ( bDisplayComment ) domElem.setAttribute( "DisplayComment", QString::number( 1 ) );

    pdomElemParent->appendChild( domElem );
}

void STextContent::doLoad( QDomElement *pdomElem )
{
    QString stringAttribute;

    stringAttribute = pdomElem->attribute( "CodeSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();
        stringAttribute = pdomElem->attribute( "Code" );
        if ( !stringAttribute.isNull() )
            stringCode = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }
    stringAttribute = pdomElem->attribute( "NameSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();
        stringAttribute = pdomElem->attribute( "Name" );
        if ( !stringAttribute.isNull() )
            stringName = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }
    stringAttribute = pdomElem->attribute( "CommentSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();
        stringAttribute = pdomElem->attribute( "Comment" );
        if ( !stringAttribute.isNull() )
            stringComment = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    bDisplayCode = pdomElem->attribute( "DisplayCode", "0" ).toInt();
    bDisplayName = pdomElem->attribute( "DisplayCode", "0" ).toInt();
    bDisplayComment = pdomElem->attribute( "DisplayCode", "0" ).toInt();
}

/****************************************************************
 * STextContentToolBar
 ****************************************************************/
STextContentToolBar::STextContentToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "STextContentToolBar" );

    pSetting = (STextContent*)g_SSettings->getSetting( "STextContent" ); Q_ASSERT( pSetting );

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    // 
    pLabel = new QLabel( this );
    pLayout->addWidget( pLabel );

    //
    pLineEdit = new QLineEdit( this );
    pLineEdit->setToolTip( tr("text") );
    connect( pLineEdit, SIGNAL(textChanged(const QString &)), SLOT(slotLineEdit(const QString &)) );
    pLayout->addWidget( pLineEdit );

    {
        // button to invoke dialog + panelmax
        pMore = new QToolButton( this );
        pMore->setText( ":" );
        pMore->setToolTip( tr("more") );
        connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );
        // add popup arrow to invoke panel as a popup window
        QMenu *pMenu = new QMenu( pMore );
        QWidgetAction *pWidgetAction = new QWidgetAction( pMenu );
        pWidgetAction->setDefaultWidget( pSetting->getPanel( pMenu ) );
        pMenu->addAction( pWidgetAction );
        pMore->setMenu( pMenu );
        pMore->setPopupMode( QToolButton::MenuButtonPopup );
        pLayout->addWidget( pMore );
    }

    pLayout->addStretch( 10 );

    slotRefresh();

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void STextContentToolBar::slotRefresh()
{
    // do these in order...
    // needs to be same pri as slotLineEdit
    if ( pSetting->getDisplayName() )
    {
        pLabel->setText( tr("Name:") );
        pLineEdit->setText( pSetting->getName() );
        return;
    }

    if ( pSetting->getDisplayCode() )
    {
        pLabel->setText( tr("Code:") );
        pLineEdit->setText( pSetting->getCode() );
        return;
    }

    if ( pSetting->getDisplayComment() ) 
    {
        pLabel->setText( tr("Comment:") );
        pLineEdit->setText( pSetting->getComment() );
        return;
    }

    // then default to this...
    pLabel->setText( tr("Name:") );
    pLineEdit->setText( pSetting->getName() );
}

void STextContentToolBar::slotLineEdit( const QString &t )
{
    // needs to be same pri as slotRefresh
    if ( pSetting->getDisplayName() )
    {
        pSetting->setName( t );
        return;
    }

    if ( pSetting->getDisplayCode() )
    {
        pSetting->setCode( t );
        return;
    }

    if ( pSetting->getDisplayComment() ) 
    {
        pSetting->setComment( t );
        return;
    }

    // then default to this...
    pSetting->setName( t );
}

void STextContentToolBar::slotMore()
{
    pSetting->doDialog( this );
}

STextContentPanel::STextContentPanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (STextContent*)g_SSettings->getSetting( "STextContent" ); Q_ASSERT( pSetting );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QGridLayout *pLayout = new QGridLayout();

    // cols 1 and 2
    //
    int nRow = 1;
    pCode = new QLineEdit( pSetting->getCode(), this );
    connect( pCode, SIGNAL(textChanged(const QString &)), SLOT(slotCode(const QString &)) );
    pLayout->addWidget( new QLabel( tr("Code"), this ), nRow, 0 );
    pLayout->addWidget( pCode, nRow, 1 );
    //
    nRow++;
    pName = new QLineEdit( pSetting->getName(), this );
    connect( pName, SIGNAL(textChanged(const QString &)), SLOT(slotName(const QString &)) );
    pLayout->addWidget( new QLabel( tr("Name"), this ), nRow, 0 );
    pLayout->addWidget( pName, nRow, 1 );
    //
    nRow++;
    pComment = new QLineEdit( pSetting->getComment(), this );
    connect( pComment, SIGNAL(textChanged(const QString &)), SLOT(slotComment(const QString &)) );
    pLayout->addWidget( new QLabel( tr("Comment"), this ), nRow, 0 );
    pLayout->addWidget( pComment, nRow, 1 );

    // col 3
    nRow = 0;
    pLayout->addWidget( new QLabel( tr("Display"), this ), nRow, 2 );
    //
    nRow++;
    pDisplayCode = new QCheckBox( this );
    pDisplayCode->setChecked( pSetting->getDisplayCode() );
    connect( pDisplayCode, SIGNAL(toggled(bool)), SLOT(slotDisplayCode(bool)) );
    pLayout->addWidget( pDisplayCode, nRow, 2 );

    //
    nRow++;
    pDisplayName = new QCheckBox( this );
    pDisplayName->setChecked( pSetting->getDisplayName() );
    connect( pDisplayName, SIGNAL(toggled(bool)), SLOT(slotDisplayName(bool)) );
    pLayout->addWidget( pDisplayName, nRow, 2 );

    //
    nRow++;
    pDisplayComment = new QCheckBox( this );
    pDisplayComment->setChecked( pSetting->getDisplayComment() );
    connect( pDisplayComment, SIGNAL(toggled(bool)), SLOT(slotDisplayComment(bool)) );
    pLayout->addWidget( pDisplayComment, nRow, 2 );

    //
    pLayoutTop->addLayout( pLayout );
    pLayoutTop->addStretch( 10 );

    //
    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void STextContentPanel::slotRefresh()
{
    pCode->setText( pSetting->getCode() );
    pName->setText( pSetting->getName() );
    pComment->setText( pSetting->getComment() );
    pDisplayCode->setChecked( pSetting->getDisplayCode() );
    pDisplayName->setChecked( pSetting->getDisplayName() );
    pDisplayComment->setChecked( pSetting->getDisplayComment() );
}

void STextContentPanel::slotCode( const QString &t )
{
    pSetting->setCode( t );
}

void STextContentPanel::slotName( const QString &t )
{
    pSetting->setName( t );
}

void STextContentPanel::slotComment( const QString &t )
{
    pSetting->setComment( t );
}

void STextContentPanel::slotDisplayCode( bool b )
{
    pSetting->setDisplayCode( b );
}

void STextContentPanel::slotDisplayName( bool b )
{
    pSetting->setDisplayName( b );
}

void STextContentPanel::slotDisplayComment( bool b )
{
    pSetting->setDisplayComment( b );
}

