/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_PASFreeHand
#define H_PASFreeHand

#include <WImageButton.h>

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for drawing 'free hand'.
 * 
 * \author pharvey (3/15/23)
 */
class PASFreeHand : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(PASFreeHand::Shapes nShape READ getShape WRITE setShape NOTIFY signalChanged)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY signalChanged)
    Q_PROPERTY(QImage image READ getImage WRITE setImage NOTIFY signalChanged)
    Q_PROPERTY(bool bLineTo READ getLineTo WRITE setLineTo NOTIFY signalChanged)
public:
    enum Shapes
    {
        ShapePen,           /*!< uses current pen to draw a line from last point to current point - easily most efficient   */
        ShapeEllipse,       /*!< ellipse filled with current pen color and of specified size                                */
        ShapeRectangle,     /*!< rectangle filled with current pen color and of specified size                              */
        ShapeCross,         /*!< a horizontal and vertical line with current pen color and of specified size                */
        ShapeImageScaled,   /*!< specified image scaled to size                                                             */
        ShapeImage          /*!< specified image                                                                            */
    };                                                                                                                      
    Q_ENUM(Shapes)

    PASFreeHand();

    void setShape( Shapes );
    void setSize( const QSize & );
    void setImage( const QImage & );
    void setLineTo( bool );
    void setSelected( Shapes, const QSize &, const QImage &, bool );

    Shapes getShape()  { return nShape;    }
    QSize  getSize()   { return size;      }
    QImage getImage()  { return image;     }
    bool   getLineTo() { return bLineTo;   }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    Shapes  nShape  = ShapePen;                                                                                            
    QSize   size    = QSize( 5, 5 );                                                                                        
    QImage  image;          /*!< this will be 'stamped' and/or 'dragged' - so not like using QBrush::texture                */
    bool    bLineTo = false;
};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits PASFreeHand.
 *  
 * Horizontal layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASFreeHandToolBar : public QWidget
{
    Q_OBJECT
public:
    PASFreeHandToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotShape( int );
    void slotWidth( int );
    void slotHeight( int );
    void slotImage();
    void slotLineTo( Qt::CheckState );

protected:
    PASFreeHand *     pSetting;

    QComboBox *     pShape;
    QSpinBox *      pWidth;
    QSpinBox *      pHeight;
    WImageButton *  pImage;
    QCheckBox *     pLineTo;
};

/*!
 * \brief   Edit widget for use in a dialog or dock window. 
 *          Edits PASFreeHand.
 *  
 * Vertical layout using form layout with labels. 
 *  
 * \author pharvey (3/14/23)
 */
class PASFreeHandPanel : public QWidget
{
    Q_OBJECT
public:
    PASFreeHandPanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotShape( int );
    void slotWidth( int );
    void slotHeight( int );
    void slotImage();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotLineTo( Qt::CheckState n );
#else
    void slotLineTo( int n );
#endif

protected:
    PASFreeHand *        pSetting;

    QComboBox *     pShape;
    QSpinBox *      pWidth;
    QSpinBox *      pHeight;
    WImageButton *  pImage;
    QCheckBox *     pLineTo;
};

#endif

