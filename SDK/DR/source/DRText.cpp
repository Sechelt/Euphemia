/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRText.h"

#include <LGraphicsObject.h>
#include <LGraphicsScene.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRText::DRText( DRDraw *pCanvas, const QPointF &pointPos )
    : DRRectangleBase( pCanvas, pointPos )
{
    // NOTE: This adds to DObject ie CBD::CBDText etc
    pSettingText = (DRSText*)g_SSettings->getSetting( "DRSText" ); Q_ASSERT(pSettingText);
    slotRefreshText();
}

QPixmap DRText::getIcon()
{
    return QPixmap( ":DR/Text" );
}

QDomElement DRText::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRRectangleBase::doSave( pdomDoc, pdomElemParent );

//    domElemThis.setAttribute( "Text", stringText );

    return domElemThis;
}

bool DRText::doLoad( QDomElement *pdomElemThis )
{
    DRRectangleBase::doLoad( pdomElemThis );

//    stringText  = pdomElemThis->attribute( "Text" ); 

    return true;
}

void DRText::doDraw( QPainter *pPainter )
{
    // draw rect for manipulation
    if ( nSelectionState == SelectionStateStandard )
    {
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawRect( getRect() );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawRect( getRect() );                         
    }

    // apply context
    pPainter->setPen( font.color );
    pPainter->setBrush( font.brush );
    pPainter->setFont( font.font );

    // fill background
//    pPainter->setPen( Qt::NoPen );
    QRectF rectBackground = pPainter->boundingRect( r, font.nHAlign | font.nVAlign, getComment() );
    pPainter->fillRect( rectBackground, font.brush );

    // draw shape
    pPainter->drawText( r, font.nHAlign | font.nVAlign, getComment() );

    // draw outline as selected
    if ( nSelectionState == SelectionStateMinimal )
    {
        pPainter->setBrush( Qt::NoBrush );
        pPainter->setPen( QPen( Qt::white ) );
        pPainter->drawRect( getRect() );                         

        pPainter->setPen( QPen( Qt::DashLine ) );
        pPainter->drawRect( getRect() );                         
    }
}

void DRText::doConnectSettings()
{
    // skip DRRectangleBase
    DRObject::doConnectSettings();
    pSettingText->setSelected( getComment() );
    connect( pSettingText, SIGNAL(signalChanged()), this, SLOT(slotRefreshText()) );
}

void DRText::doDisconnectSettings()
{
    // skip DRRectangleBase
    DRObject::doDisconnectSettings();
    disconnect( pSettingText, SIGNAL(signalChanged()), this, SLOT(slotRefreshText()) );
}

void DRText::slotRefreshText()
{
    setComment( pSettingText->getText() );
    if ( getGraphics() ) getGraphics()->update();
}

DRSText::DRSText()
{
}

void DRSText::setText( const QString &t )
{
    if ( t == stringText ) return;
    stringText = t;
    emit signalChanged();
}

void DRSText::setSelected( const QString &stringText )
{
    this->stringText    = stringText;
    emit signalSync();
}

QWidget *DRSText::getButton( QWidget *pParent )
{
    Q_UNUSED( pParent );
    return nullptr;
}

QWidget *DRSText::getToolBar( QWidget *pParent )
{
    return new DRSTextToolBar( pParent );
}

QWidget *DRSText::getPanel( QWidget *pParent )
{
    return new DRSTextPanel( pParent );
}

QWidget *DRSText::getSwatch( QWidget *pParent )
{
    Q_UNUSED( pParent );
    return nullptr;
}

QToolBox *DRSText::getToolBox( QWidget *pParent )
{
    QToolBox *p = SSetting::getToolBox( pParent );
    p->addItem( getPanel( p ), QIcon(), "Text" );
    p->addItem( pFont->getPanel( p ), QIcon(), "Font" );
    return p;
}

void DRSText::doDialog( QWidget *pParent, bool bThis )
{
    Q_UNUSED( bThis );
    DRSTextDialog dialog( pParent );
    dialog.exec();
}

void DRSText::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "DRSText" );

    // default text - if any
    if ( !stringText.isEmpty() )
    {
        unsigned long nLen = 0;
        domElem.setAttribute( "Text", CBDPersistUtility::getEncodedText( stringText, &nLen ) ); 
        domElem.setAttribute( "TextSize", QString::number( nLen ) );
    }

    pdomElem->appendChild( domElem );
}

void DRSText::doLoad( QDomElement *pdomElem )
{
    QString stringAttribute = pdomElem->attribute( "TextSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElem->attribute( "Text" );
        if ( !stringAttribute.isNull() )
            stringText = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }
}

DRSTextToolBar::DRSTextToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (DRSText*)g_SSettings->getSetting( "DRSText" ); Q_ASSERT( pSetting );

    //
    pLineEdit = new QLineEdit( pSetting->getText(), this );
    connect( pLineEdit, SIGNAL(textChanged(const QString &)), SLOT(slotText(const QString &)) );
    pMore = new QToolButton( this );
    pMore->setText( ":" );
    connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );
    QHBoxLayout *pLayout = new QHBoxLayout( this );
    pLayout->addWidget( pLineEdit );
    pLayout->addWidget( pMore );
    pLayout->addStretch( 10 );
    
    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void DRSTextToolBar::slotRefresh()
{
    pLineEdit->setText( pSetting->getText() );
}

void DRSTextToolBar::slotText( const QString &t )
{
    pSetting->setText( t );
}

void DRSTextToolBar::slotMore()
{
    pSetting->doDialog( this );
    return;

    // Popup
    auto *popup = new QWidget( pMore, Qt::Popup );
    popup->setAttribute(Qt::WA_DeleteOnClose);

    QToolBox *pToolBox = pSetting->getToolBox( popup );

    auto *layout = new QVBoxLayout(popup);
    layout->addWidget( pToolBox );

    QPoint belowPos = pMore->mapToGlobal(QPoint(0, pMore->height()));
    popup->move(belowPos);

    int nWidth = pToolBox->sizeHint().width();
    int nHeight = pToolBox->sizeHint().height();
    for ( int n = 0; n < pToolBox->count(); n++ )
    {
        nWidth = qMax( nWidth, pToolBox->widget( n )->sizeHint().width() );
        nHeight = qMax( nHeight, pToolBox->widget( n )->sizeHint().height() );
    }
    popup->resize( nWidth, nHeight );
    popup->show();
}

DRSTextPanel::DRSTextPanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (DRSText*)g_SSettings->getSetting( "DRSText" ); Q_ASSERT( pSetting );

    //
    pLineEdit = new QLineEdit( pSetting->getText(), this );
    connect( pLineEdit, SIGNAL(textChanged(const QString &)), SLOT(slotText(const QString &)) );
    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Text"), pLineEdit );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void DRSTextPanel::slotRefresh()
{
    pLineEdit->setText( pSetting->getText() );
}

void DRSTextPanel::slotText( const QString &t )
{
    pSetting->setText( t );
}

DRSTextDialog::DRSTextDialog( QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Text") );

    DRSText *   pSettingText = (DRSText*)g_SSettings->getSetting( "DRSText" ); Q_ASSERT( pSettingText );
    SText*      pSettingFont = (SText*)g_SSettings->getSetting( "SText" ); Q_ASSERT( pSettingFont );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    pLayoutTop->addWidget( pSettingText->getPanel( this ) );
    pLayoutTop->addWidget( pSettingFont->getPanel( this ), 10 );
//    pLayoutTop->addStretch( 10 );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayoutTop->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );

    doLoadState();
}

DRSTextDialog::~DRSTextDialog()
{
    doSaveState();
}

void DRSTextDialog::doSaveState()
{
    QSettings settings;
    settings.setValue( metaObject()->className() + QString("/width"), width() );
    settings.setValue( metaObject()->className() + QString("/height"), height() );
}

void DRSTextDialog::doLoadState()
{
    QSettings settings;
    int nWidth = settings.value( metaObject()->className() + QString("/width"), width() ).toInt();
    int nHeight= settings.value( metaObject()->className() + QString("/height"), height() ).toInt();
    resize( nWidth, nHeight );
}

