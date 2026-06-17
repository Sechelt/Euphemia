/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LSRender.h"

#include "SSettings.h"

/****************************************************************
 * LSRender
 ****************************************************************/
LSRender::LSRender()
{
}

void LSRender::setRenderAntialiasing( int t )
{
    if ( t == nAntialiasing ) return;
    nAntialiasing = t;
    emit signalChanged();
}

void LSRender::setRenderTextAntialiasing( int t )
{
    if ( t == nTextAntialiasing ) return;
    nTextAntialiasing = t;
    emit signalChanged();
}

void LSRender::setRenderSmoothPixmapTransform( int t )
{
    if ( t == nSmoothPixmapTransform ) return;
    nSmoothPixmapTransform = t;
    emit signalChanged();
}

void LSRender::setRenderLosslessImageRendering( int t )
{
    if ( t == nLosslessImageRendering ) return;
    nLosslessImageRendering = t;
    emit signalChanged();
}

void LSRender::setSelected( int nAntialiasing, int nTextAntialiasing, int nSmoothPixmapTransform, int nLosslessImageRendering )
{
    this->nAntialiasing             = nAntialiasing;
    this->nTextAntialiasing         = nTextAntialiasing;
    this->nSmoothPixmapTransform    = nSmoothPixmapTransform;
    this->nLosslessImageRendering   = nLosslessImageRendering;
    emit signalSync();
}

QWidget *LSRender::getToolBar( QWidget *pParent )
{
    return new LSRenderWidget( pParent, Qt::Horizontal );
}

QWidget *LSRender::getPanel( QWidget *pParent )
{
    return new LSRenderWidget( pParent, Qt::Vertical );
}

void LSRender::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSRender" );

    domElem.setAttribute( "Antialiasing", nAntialiasing );
    domElem.setAttribute( "TextAntialiasing", nTextAntialiasing );
    domElem.setAttribute( "SmoothPixmapTransform", nSmoothPixmapTransform );
    domElem.setAttribute( "LosslessImageRendering", nLosslessImageRendering );

    pdomElem->appendChild( domElem );
}

void LSRender::doLoad( QDomElement *pdomElem )
{
    nAntialiasing           = pdomElem->attribute( "Antialiasing", QString::number( nAntialiasing ) ).toInt();
    nTextAntialiasing       = pdomElem->attribute( "TextAntialiasing", QString::number( nTextAntialiasing ) ).toInt();
    nSmoothPixmapTransform  = pdomElem->attribute( "SmoothPixmapTransform", QString::number( nSmoothPixmapTransform ) ).toInt();
    nLosslessImageRendering = pdomElem->attribute( "LosslessImageRendering", QString::number( nLosslessImageRendering ) ).toInt();
}

/****************************************************************
 * LSRenderWidget
 ****************************************************************/
LSRenderWidget::LSRenderWidget( QWidget *p, Qt::Orientation n )
    : QWidget( p )
{
    pSetting = (LSRender*)g_SSettings->getSetting( "LSRender" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [LSRender].";
        return;
    }

    // nRenderHint
    pAntialiasing = new QCheckBox( this );
    pAntialiasing->setTristate( false );
    pAntialiasing->setToolTip( tr("Indicates that the engine should antialias edges of primitives if possible.") );
    pAntialiasing->setChecked( pSetting->getRenderAntialiasing() );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pAntialiasing, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRenderAntialiasing(Qt::CheckState)) );
#else
    connect( pAntialiasing, SIGNAL(stateChanged(int)), SLOT(slotRenderAntialiasing(int)) );
#endif


    pTextAntialiasing = new QCheckBox( this );
    pTextAntialiasing->setTristate( false );
    pTextAntialiasing->setToolTip( tr("Indicates that the engine should antialias text if possible.") );
    pTextAntialiasing->setChecked( pSetting->getRenderTextAntialiasing() );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pTextAntialiasing, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRenderTextAntialiasing(Qt::CheckState)) );
#else
    connect( pTextAntialiasing, SIGNAL(stateChanged(int)), SLOT(slotRenderTextAntialiasing(int)) );
#endif

    pSmoothPixmapTransform = new QCheckBox( this );
    pSmoothPixmapTransform->setTristate( false );
    pSmoothPixmapTransform->setToolTip( tr("Indicates that the engine should use a smooth pixmap\ntransformation algorithm (such as bilinear) rather than nearest neighbor.") );
    pSmoothPixmapTransform->setChecked( pSetting->getRenderSmoothPixmapTransform() );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pSmoothPixmapTransform, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRenderSmoothPixmapTransform(Qt::CheckState)) );
#else
    connect( pSmoothPixmapTransform, SIGNAL(stateChanged(int)), SLOT(slotRenderSmoothPixmapTransform(int)) );
#endif

    pLosslessImageRendering = new QCheckBox( this );
    pLosslessImageRendering->setTristate( false );
    pLosslessImageRendering->setToolTip( tr("Encode images using a lossless compression algorithm instead of lossy JPEG compression.") );
    pLosslessImageRendering->setChecked( pSetting->getRenderLosslessImageRendering() );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pLosslessImageRendering, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRenderLosslessImageRendering(Qt::CheckState)) );
#else
    connect( pLosslessImageRendering, SIGNAL(stateChanged(int)), SLOT(slotRenderLosslessImageRendering(int)) );
#endif

    if ( n == Qt::Horizontal )
    {
        pAntialiasing->setText( tr("Antialiasing") );
        pTextAntialiasing->setText( tr("TextAntialiasing") );
        pSmoothPixmapTransform->setText( tr("SmoothPixmapTransform") );
        pLosslessImageRendering->setText( tr("LosslessImageRendering") );

        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pAntialiasing );
        pLayout->addWidget( pTextAntialiasing );
        pLayout->addWidget( pSmoothPixmapTransform );
        pLayout->addWidget( pLosslessImageRendering );
        pLayout->addStretch( 10 );
    }
    else
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        {
            QGroupBox *pGroupBox = new QGroupBox( tr("Render Hint") );
            QFormLayout *pForm = new QFormLayout();
            pGroupBox->setLayout( pForm );
            pForm->addRow( tr("Antialiasing"), pAntialiasing );
            pForm->addRow( tr("TextAntialiasing"), pTextAntialiasing );
            pForm->addRow( tr("SmoothPixmapTransform"), pSmoothPixmapTransform );
            pForm->addRow( tr("LosslessImageRendering"), pLosslessImageRendering );
            pLayout->addWidget( pGroupBox );
        }
        pLayout->addStretch( 10 );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSRenderWidget::slotRefresh()
{
    pAntialiasing->setChecked( pSetting->getRenderAntialiasing() );
    pTextAntialiasing->setChecked( pSetting->getRenderTextAntialiasing() );
    pSmoothPixmapTransform->setChecked( pSetting->getRenderSmoothPixmapTransform() );
    pLosslessImageRendering->setChecked( pSetting->getRenderLosslessImageRendering() );
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
void LSRenderWidget::slotRenderAntialiasing( Qt::CheckState t )
#else
void LSRenderWidget::slotRenderAntialiasing( int t )
#endif
{
    pSetting->setRenderAntialiasing( t );
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
void LSRenderWidget::slotRenderTextAntialiasing( Qt::CheckState t )
#else
void LSRenderWidget::slotRenderTextAntialiasing( int t )
#endif
{
    pSetting->setRenderTextAntialiasing( t );
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
void LSRenderWidget::slotRenderSmoothPixmapTransform( Qt::CheckState t )
#else
void LSRenderWidget::slotRenderSmoothPixmapTransform( int t )
#endif
{
    pSetting->setRenderSmoothPixmapTransform( t );
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
void LSRenderWidget::slotRenderLosslessImageRendering( Qt::CheckState t )
#else
void LSRenderWidget::slotRenderLosslessImageRendering( int t )
#endif
{
    pSetting->setRenderLosslessImageRendering( t );
}


