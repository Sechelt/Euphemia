#include "LibInfo.h"
#include "LEditorWidget.h"

#include <SSettings.h>

#include "LGraphicsScene.h"
#include "LGraphicsView.h"
#include "LGraphicsLine.h"
#include "LFile.h"

LEditorWidget::LEditorWidget( LFile *pFile, QWidget *pParent )
    : AWEditorWidget( pFile, pParent )
{
    this->pFile  = pFile;
    pRulerCorner    = nullptr;
    pRulerH         = nullptr;
    pRulerV         = nullptr;
    pLayoutHTop     = nullptr;
    pLayoutRulerLeft= nullptr;
    pLayoutVTop     = nullptr;

    doInitCentralArea();

    pSettingGeneral = g_SSettings->getSetting( "LSGeneral" ); Q_ASSERT( pSettingGeneral );
    setRuler( pSettingGeneral->property( "bRuler" ).toBool() );
    connect( pSettingGeneral, SIGNAL(signalChanged()), SLOT(slotRefreshGeneral()) );
}

LEditorWidget::~LEditorWidget()
{
/*
    if ( pPalWidget )        
    {                        
        delete pPalWidget;   
        pPalWidget = nullptr;
    }                        
                             
    if ( pPanWidget )        
    {                        
        delete pPanWidget;   
        pPanWidget = nullptr;
    }                        
*/

}

void LEditorWidget::setRuler( bool b )
{
    // clear
    if ( pRulerCorner )
    {
        delete pRulerCorner;
        delete pRulerH;
        delete pRulerV;
        pRulerCorner    = nullptr;
        pRulerH         = nullptr;
        pRulerV         = nullptr;

        delete pLayoutRulerLeft;
        delete pLayoutVTop;
        delete pLayoutHTop;
        pLayoutHTop         = nullptr;
        pLayoutRulerLeft    = nullptr;
        pLayoutVTop         = nullptr;
    }

    if ( pLayoutVTop )
    { 
        delete pLayoutVTop;
        pLayoutVTop         = nullptr;
    }

    // add rulers?
    if ( b )
    {
        pLayoutHTop         = new QHBoxLayout();
        pLayoutRulerLeft    = new QVBoxLayout();
        pLayoutVTop         = new QVBoxLayout();

        pRulerCorner = new WRulerCornerWidget( this );
        pLayoutRulerLeft->addWidget( pRulerCorner );

        pRulerV = new WRulerWidget( this, Qt::Vertical, pFile->getCell().height() / 2 );
        pRulerV->slotUpdate( pView->getOffsetV(), pView->getScaleV(), pView->getLengthV() );
        connect( pView, SIGNAL(signalChangedV(qreal,qreal,qreal)), pRulerV, SLOT(slotUpdate(qreal,qreal,qreal)) );

        connect( pRulerV, SIGNAL(signalGuideNewRequest(Qt::Orientation,qreal)), pFile, SLOT(slotGuideNewRequest(Qt::Orientation,qreal)) );
        connect( pRulerV, SIGNAL(signalGuideDeleteRequest(Qt::Orientation,int)), pFile, SLOT(slotGuideDeleteRequest(Qt::Orientation,int)) );
        connect( pRulerV, SIGNAL(signalGuideMoveRequest(Qt::Orientation,int,qreal)), pFile, SLOT(slotGuideMoveRequest(Qt::Orientation,int,qreal)) );

        pLayoutRulerLeft->addWidget( pRulerV );
        pLayoutHTop->addLayout( pLayoutRulerLeft );

        pRulerH = new WRulerWidget( this, Qt::Horizontal, pFile->getCell().width() / 2 );
        pRulerH->slotUpdate( pView->getOffsetH(), pView->getScaleH(), pView->getLengthH() );
        connect( pView, SIGNAL(signalChangedH(qreal,qreal,qreal)), pRulerH, SLOT(slotUpdate(qreal,qreal,qreal)) );

        connect( pRulerH, SIGNAL(signalGuideNewRequest(Qt::Orientation,qreal)), pFile, SLOT(slotGuideNewRequest(Qt::Orientation,qreal)) );
        connect( pRulerH, SIGNAL(signalGuideDeleteRequest(Qt::Orientation,int)), pFile, SLOT(slotGuideDeleteRequest(Qt::Orientation,int)) );
        connect( pRulerH, SIGNAL(signalGuideMoveRequest(Qt::Orientation,int,qreal)), pFile, SLOT(slotGuideMoveRequest(Qt::Orientation,int,qreal)) );

        pLayoutVTop->addWidget( pRulerH );
        pLayoutVTop->addWidget( pView );
        pLayoutHTop->addLayout( pLayoutVTop );

        pLayoutCentral->addLayout( pLayoutHTop );

        // add any guides
        {
            LGraphicsLine *pItem;
            QVector<LGraphicsLine*> v = pFile->getGuidesX();
            foreach( pItem, v )
            {
                pRulerH->doGuideNew( pItem->getLine().x1() );
            }

            v = pFile->getGuidesY();
            foreach( pItem, v )
            {
                pRulerV->doGuideNew( pItem->getLine().y1() );
            }
        }

        return; 
    }

    // no rulers
    pLayoutVTop = new QVBoxLayout();
    pLayoutVTop->addWidget( pView );
    pLayoutCentral->addLayout( pLayoutVTop );
}

LFile *LEditorWidget::getFile() 
{ 
    return pFile; 
}

LGraphicsScene *LEditorWidget::getScene() 
{ 
    return pFile->getGraphics(); 
}

WRulerWidget *LEditorWidget::getRuler( Qt::Orientation n )
{
    if ( n == Qt::Horizontal ) return pRulerH;
    return pRulerV;
}

void LEditorWidget::doInitCentralArea()
{
    setCentralWidget( new QWidget( this ) );
    pLayoutCentral = new QVBoxLayout( centralWidget() );

    pView = new LGraphicsView( pFile, this );
    pView->setRenderHint( QPainter::Antialiasing, true );
    pView->setMouseTracking( true );
}

void LEditorWidget::slotRefreshGeneral()
{
    setRuler( pSettingGeneral->property( "bRuler" ).toBool() );
}


