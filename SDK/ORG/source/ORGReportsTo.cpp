#include "LibInfo.h"
#include "ORGReportsTo.h"

#include <DGraphicsProxyItem.h>
#include <DRectangle.h>
#include <DSource.h>

#include "ORGDiagram.h"
#include "ORGPerson.h"

ORGReportsTo::ORGReportsTo( ORGDiagram *pDiagram, const QString &stringName )
    : DLine( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p"0 __FILE__, __LINE__, metaObject()->className(), this );
#endif

    nStyleEnd       = W::WLineEndArrow; 
    bTruncateBegin  = true;
    bTruncateEnd    = true;

    // we always have 4 points - even when we are a straight line
    // so add 2 more points
    polygon.append( polygon.at(1) );
    polygon.append( polygon.at(1) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ORGReportsTo::~ORGReportsTo()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QPixmap ORGReportsTo::getIcon()
{
    return QPixmap( ":ORG/ReportsTo" );
}

ORGPerson *ORGReportsTo::getSuperior()
{
    return (ORGPerson*)getSource( CBD::EOLBegin )->getRectangle();
}

ORGPerson *ORGReportsTo::getSubordinate()
{
    return (ORGPerson*)getSource( CBD::EOLEnd )->getRectangle();
}

/*!
 * \brief Set route between Begin and End. 
 *  
 * Assumes a top-down hierarchial layout has been applied. 
 * Assumes Begin and End points have been set. 
 *  
 * \author pharvey (4/10/20)
 */
void ORGReportsTo::doRoute()
{
    // assume rectangles are in correct location
    ORGPerson *pSuperior = getSuperior();
    if ( !pSuperior ) return;
    ORGPerson *pSubordinate = getSubordinate();
    if ( !pSubordinate ) return;

    if ( pSuperior->getPos() == pSubordinate->getPos() ) return;

    qreal nY = polygon[3].y() - (polygon[3].y() - polygon[0].y()) / 2;

    polygon[1] = QPointF( polygon[0].x(), nY ); 
    polygon[2] = QPointF( polygon[3].x(), nY ); 
}

void ORGReportsTo::mousePressHandle( DHandle *pHandle, const QPointF &pointScene )  
{                                                                                    
    Q_UNUSED(pHandle);                                                               
    Q_UNUSED(pointScene);                                                                
}                                                                                    
                                                                                     
void ORGReportsTo::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )   
{                                                                                    
    Q_UNUSED(pHandle);                                                               
    Q_UNUSED(pointScene);                                                                
}                                                                                    
                                                                                     
void ORGReportsTo::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{                                                                                    
    Q_UNUSED(pHandle);                                                               
    Q_UNUSED(pointScene);                                                                
}                                                                                    

void ORGReportsTo::slotDelete()
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
void ORGReportsTo::doRoute( DSource *pSource, const QPointF &pointScene )                                                          
{
    Q_UNUSED(pSource);
    Q_UNUSED(pointScene);
    
    doMoveSource( pSource, pointScene );                                                                                                
    doRoute();    

return;


/*                                                                                                                                
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
                                                                                                                                                
    ORGRectangle *pRectangleBegin = (ORGRectangle*)getRectangle( CBD::EOLBegin );                                                               
    ORGRectangle *pRectangleEnd   = (ORGRectangle*)getRectangle( CBD::EOLEnd );                                                                 
                                                                                                                                                
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
*/
                                                                                                                                                
    // straight line                                                                                                                            
    if ( polygon.count() != 2 )                                                                                                                 
    {                                                                                                                                           
        QList<QPointF> listPoints;                                                                                                              
        doMergePoints( listPoints );                                                                                                            
    }                                                                                                                                           
    DLine::doRoute( pSource, pointScene );                                                                                                      
}                                                                                                                            


