#ifndef H_DStencilItem
#define H_DStencilItem

#include <QSvgRenderer>

#include "AWObject.h"

class DStencilItem : public AWObject
{
    Q_OBJECT
public:
    DStencilItem( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~DStencilItem();

    virtual void            setIcon( const QPixmap &pixmap );

    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Stencil Item" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QMenu *         getContextMenu( QWidget * );
    virtual bool            isNull() = 0;

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

public slots:
    virtual void slotIcon( const QPixmap &pixmap );

protected:
    QPixmap pixmapIcon;
};

/*!
 * \brief Stencil item based upon an Svg.
 * 
 * \author pharvey (8/8/21)
 */
class DStencilItemSvg : public DStencilItem
{
    Q_OBJECT
public:
    DStencilItemSvg( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~DStencilItemSvg();

    virtual void setVector( const QByteArray &ba );

    virtual QString         getFriendlyClassName() { return tr( "Svg" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QByteArray      getVector() { return ByteArray; }
    virtual bool            isNull();

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

public slots:
    virtual void slotVector( const QByteArray &ba );

protected:
    QByteArray ByteArray;

    virtual void doUpdateIcon();
};

/*!
 * \brief Stencil item based upon a bitmap.
 * 
 * \author pharvey (1/18/22)
 */
class DStencilItemBitmap : public DStencilItem
{
    Q_OBJECT
public:
    DStencilItemBitmap( AWObject *pParent = nullptr, const QString &stringName = QString() );
    virtual ~DStencilItemBitmap();

    virtual void setPixmap( const QPixmap & );

    virtual QString         getFriendlyClassName() { return tr( "Bitmap" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QPixmap         getPixmap() { return pixmap; }
    virtual bool            isNull();

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

public slots:
    virtual void slotPixmap( const QPixmap & );

protected:
    QPixmap pixmap;

    virtual void doUpdateIcon();
};

#endif


