#ifndef H_RouteSource
#define H_RouteSource

#include <RSource.h>

class RouteScene;

/*!
 * \brief RSource for use with RouteScene 
 *  
 * In this case we do not maintain a cache as there are no serious ineffiencies 
 * with calling RouteScene all the time. This makes us super simple. 
 * 
 * \author pharvey (3/4/20)
 */
class RouteSource : public RSource
{
    Q_OBJECT
public:
    RouteSource( RouteScene *pScene, QObject *pObjectParent );

    virtual QPointF getBegin();
    virtual QPointF getEnd();

    // map/scene width/height in cells  
    virtual QSize getGridSize();
    // cell width/height in pixels
    virtual QSize getCellSize();
    // cell contents
    virtual CellContentTypes getCellContent( const QPoint &pointCell );
    //
    virtual QWidget *getControls( QWidget *pParent );
	// used to indicate that there is no information in the source
	virtual bool isEmpty() { return !pScene; }

protected:
    RouteScene *pScene;
};

#endif

