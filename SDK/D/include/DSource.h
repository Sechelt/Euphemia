#ifndef H_DSource
#define H_DSource

#include "DLine.h"

class DSink;
class DRectangle;

/*!
 * \class DSource 
 * \brief Lines use these to glue themselves to a DRectangle.
 *  
 * A \sa DLine has one of these for the begin and end points. 
 * This can be connected to 0-1 DSink's.  
 *  
 * Call to DRectangle to glue/unglue sources. 
 *  
 * Sinks and Sources are 'glued' together. This is independent of the line 
 * 'connecting' to the rectangle - but is usually done in support of it. 
 *  
 * Sinks and Sources are all about managing the presentation. 
 *  
 * \author pharvey (10/5/19)
 */
class DSource : public QObject
{
    Q_OBJECT
    friend class DRectangle;   // shape glues sink/source
    friend class DSink;        // for destructor to cleanup
public:
    explicit DSource( DLine *p, CBD::EOLTypes nSourceType );
    virtual ~DSource();
    
    void setPos( const QPointF &point ) ;
    void setVisible( bool b = true );

    DLine *         getLine() { return pLine; }
    QPointF         getPos() { return pointPos; }
    DHandle *       getHandle() { return pHandle; }
    DRectangle *    getRectangle();
    DSink *         getSink() { return pSink; }
    CBD::EOLTypes   getType() { return nSourceType; }
    int             getIndex(); // index into vectorSources

    bool isVisible() { return pHandle != nullptr; }
    bool isGlued() { return pSink != nullptr; }
    bool isBegin() { return nSourceType == CBD::EOLBegin; }
    bool isEnd() { return nSourceType == CBD::EOLEnd; }

protected:
    CBD::EOLTypes nSourceType;
    DLine *         pLine;          // we exist to support this DLine (but its not our parent)
    QPointF         pointPos;       // scene
    DHandle *       pHandle;        // our handle (presentation layer incl. coords)
    DSink *         pSink;          // we can be connected to 0-1 sink

    void doGlue( DSink *p );
    void doUnglue();

};

#endif


