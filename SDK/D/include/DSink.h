#ifndef H_DSink
#define H_DSink

#include "DHandle.h"

class DSource;
class DRectangle;

/*!
 * \class DSink 
 * \brief Rectangles have these to allow a source to be glued to it.
 *  
 * A DRectangle can have 0-n of these. 
 * A DSource can have 0-n \sa DSource's glued to it.
 *  
 * Sinks can be created as static. They existy with, or without, a source. 
 * Sinks can be created dynamically, to allow a source to be glued anywhere on a shape. 
 * Dynamic sinks are usually deleted when no source is glued to it. 
 *  
 * Call to DRectangle to glue/unglue sources.    
 *  
 * \author pharvey (10/5/19)
 */
class DSink : public QObject
{
    Q_OBJECT
    friend class DRectangle;    // shape glues sink/source
    friend class DSource;       // for destructor to cleanup
public:
    enum SinkTypes
    {
        SinkTypeStatic,     // pos is a specific point in the rectangle such as; top, left, center, etc
        SinkTypeDynamic     // pos is based upon a percent from center x/y
    };

    explicit DSink( DRectangle *p, const QString &stringName = "" );
    explicit DSink( DRectangle *p, const QPointF &pointPercent, const QString &stringName = "" );
    virtual ~DSink();

    void setPos( const QPointF &point );        // when SinkTypeStatic (but also called by setPos()
    void setPos();                              // when SinkTypeDynamic
    void setPosRaw( const QPointF &point );     // setPos without trying bring connect sources along
    void setPosRaw();                           // setPos without trying bring connect sources along
    void setVisible( bool b = true ); 

    DRectangle *    getRectangle() { return pRectangle; }
    QPointF         getPos() { return pointPos; }
    QPointF         getPercent() { return pointPercent; }
    DHandle *       getHandle() { return pHandle; }
    QList<DSource*> getSources() { return listSources; }
    SinkTypes       getType() { return nType; }
    int             getIndex(); // index into vectorSinks

    bool contains( const QPointF &pointScene );
    bool isVisible() { return pHandle != nullptr; }
    bool isGlued() { return listSources.count() > 0; }

protected:
    SinkTypes         nType;                
    DRectangle *      pRectangle;     // we exist to support DRectangle (but its not our parent)
    QPointF           pointPos;       // scene
    DHandle *         pHandle;        // our handle (presentation layer incl. coords)
    QList<DSource*>   listSources;    // we can be connected to 0-n sources
    QPointF           pointPercent;   // percent of width from center (-/+) - relevant only if we are SinkTypeDynamic
     
    void doGlue( DSource *p ) { listSources.append( p ); }
    void doUnglue( DSource *p ) { listSources.removeAll( p ); }
};

#endif


