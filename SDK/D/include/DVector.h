#ifndef H_DVector
#define H_DVector

#include <QSvgRenderer>

#include "DRectangle.h"

/*!
 * \brief A shape based upon a vector (SVG) image. 
 *  
 * The image is a ByteArray. The ByteArray is the entire svg or svgz file that 
 * is imported via QFile::readAll(). It is set via slotVector() and from that point 
 * it is saved/loaded as part of our shape (no ties to the original file). 
 *  
 * The ByteArray is rendered using QSvgRenderer. This provides far superior scaling 
 * over a bitmap. 
 *  
 * The image is always scaled to fit the underlying rectangle. 
 *  
 * \sa DStencil 
 * \sa DStencilItem 
 *  
 * \author pharvey (5/3/20)
 */
class DVector : public DRectangle
{
    Q_OBJECT
public:
    explicit DVector( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DVector();
 
    // SETTERS

    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Vector" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QByteArray      getVector() { return ByteArray; }

    // DO'RS
    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

public slots:
    virtual void slotVector( const QByteArray &ba );

protected:
    QSvgRenderer *  pRenderer;
    QByteArray      ByteArray;
};
 
#endif


