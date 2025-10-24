#include "LibInfo.h"
#include "FLFlow.h"

#include <DGraphicsProxyItem.h>
#include <DRectangle.h>
#include <DSource.h>

#include "FLDiagram.h"
#include "FLRectangle.h"
#include "FLConditionOpen.h"
#include "FLConditionClose.h"

FLFlow::FLFlow( FLDiagram *pDiagram, const QString &stringName )
    : DLine( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p"0 __FILE__, __LINE__, metaObject()->className(), this );
#endif

    nStyleEnd       = W::WLineEndArrow; 
    bTruncateBegin  = true;
    bTruncateEnd    = true;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

FLFlow::~FLFlow()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QPixmap FLFlow::getIcon()
{
    return QPixmap( ":FL/Flow" );
}

void FLFlow::doRoute()
{
    DLine::doRoute();
}

void FLFlow::mousePressHandle( DHandle *pHandle, const QPointF &pointScene )  
{                                                                                    
    Q_UNUSED(pHandle);                                                               
    Q_UNUSED(pointScene);                                                                
}                                                                                    
                                                                                     
void FLFlow::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )   
{                                                                                    
    Q_UNUSED(pHandle);                                                               
    Q_UNUSED(pointScene);                                                                
}                                                                                    
                                                                                     
void FLFlow::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{                                                                                    
    Q_UNUSED(pHandle);                                                               
    Q_UNUSED(pointScene);                                                                
}                                                                                    

void FLFlow::slotDelete()
{
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
}

/*!
 * \brief Route line. 
 *  
 * Basically we just want the default straight line - except when dealing with FLConditionOpen/FLConditionClose. 
 * We create 'elbows' for a condition. 
 *  
 * \author pharvey (3/24/20)
 * 
 * \param pSource The source to be moved.
 * \param pointScene The desired destination for the source.
 */
void FLFlow::doRoute( DSource *pSource, const QPointF &pointScene )                                                          
{                                                                                                                            
    QPointF pointBegin;
    QPointF pointEnd;

    // pSource may be Begin or End so...
    if ( pSource->isBegin() ) 
    { 
        pointBegin  = pointScene;
        pointEnd    = pSource->getLine()->getSource( CBD::EOLEnd )->getPos();
    }
    else
    { 
        pointBegin  = pSource->getLine()->getSource( CBD::EOLBegin )->getPos();
        pointEnd    = pointScene;
    }

    FLRectangle *pRectangleBegin = (FLRectangle*)getRectangle( CBD::EOLBegin );
    FLRectangle *pRectangleEnd   = (FLRectangle*)getRectangle( CBD::EOLEnd );

    // condition: with no content on this flow?
    if ( pRectangleBegin->inherits( "FLConditionOpen" ) && pRectangleEnd->inherits( "FLConditionClose" ) )
    {
        FLConditionOpen *   pOpen       = (FLConditionOpen*)pRectangleBegin;
        CBD::DirectionTypes nDir        = ( getSource( CBD::EOLBegin )->getSink()->objectName() == "Left" ? CBD::DirectionW : CBD::DirectionE );
        qreal               nStretch    = pOpen->getStretch( nDir );

         // has stretch been requested - explicitly?
        if ( nStretch != 0 )
        { 
            // update/create a double elbow and apply stretch
            
            // updating instead of merging because its faster
            if ( polygon.count() == 4 )
            {
                // points are kept in item coordinates so map from scene
                polygon[1] = getProxy()->mapFromScene( QPointF( pointBegin.x() + nStretch, pointBegin.y() ) );        
                polygon[2] = getProxy()->mapFromScene( QPointF( pointEnd.x() + nStretch, pointEnd.y() ) );        
            }
            else
            {
                // points are kept in item coordinates but doMergePoints will map from scene for us
                QPointF pointElbow1( pointBegin.x() + nStretch, pointBegin.y() );
                QPointF pointElbow2( pointEnd.x() + nStretch, pointEnd.y() );
                QList<QPointF> listPoints;
                listPoints.append( pointElbow1 );
                listPoints.append( pointElbow2 );
                doMergePoints( listPoints );  // points in scene coords
            }
            doMoveSource( pSource, pointScene );
            return;
        }
    }
    // condition: open?
    else if ( pRectangleBegin->inherits( "FLConditionOpen" ) )                                                       
    {
        // do we need an elbow?
        if ( pointBegin.x() != pointEnd.x() ) 
        {
            if ( polygon.count() == 3 )
                // update elbow
                polygon[1] = getProxy()->mapFromScene( QPointF( pointEnd.x(), pointBegin.y() ) );        
            else                                                                                                                 
            { 
                // create elbow                                                                                                                   
                QList<QPointF> listPoints;                                                                                       
                listPoints.append( QPointF( pointEnd.x(), pointBegin.y() ) );
                doMergePoints( listPoints );  // points in scene coords                                                                                     
            }                                                                                                                    
            doMoveSource( pSource, pointScene );
            return;
        }
    }
    // condition: close?
    else if ( pRectangleEnd->inherits( "FLConditionClose" ) )                                                  
    {                                                                                                                        
        // do we need an elbow?
        if ( pointBegin.x() != pointEnd.x() ) 
        {
            if ( polygon.count() == 3 )                                                                                          
                polygon[1] = getProxy()->mapFromScene( QPointF( pointBegin.x(), pointEnd.y() ) );        
            else                                                                                                                 
            {                                                                                                                    
                QList<QPointF> listPoints;                                                                                       
                listPoints.append( QPointF( pointBegin.x(), pointEnd.y() ) );
                doMergePoints( listPoints ); // points in scene coords                                                                                     
            }
            doMoveSource( pSource, pointScene );
            return;
        }
    }

    // straight line                                                                                                                        
    if ( polygon.count() != 2 )                                                                                          
    {                                                                                                                    
        QList<QPointF> listPoints;                                                                                       
        doMergePoints( listPoints );                                                                                     
    }                                                                                                                    
    DLine::doRoute( pSource, pointScene );
}                                                                                                                            


