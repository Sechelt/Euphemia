/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_PASErase
#define H_PASErase

#include <WImageButton.h>

#include "SSetting.h"

/*!
 * \brief Setting for a PASErase.
 * 
 * \author pharvey (3/14/23)
 */
class PASErase : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(PASErase::Shapes nShape READ getShape WRITE setShape NOTIFY signalChanged)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY signalChanged)
    Q_PROPERTY(QImage image READ getImage WRITE setImage NOTIFY signalChanged)
public:
    // NOTE: Very similar to FreeHand but Erase will not use pen/color as composition will be set to erase background with any color.
    //       Convenient to have seperate settings to save/restore default to.
    enum Shapes
    {
        ShapePen,                       /*!< uses current pen for width, shape is a circle              */
        ShapeEllipse,                   /*!< ellipse of specified size                                  */
        ShapeRectangle,                 /*!< rectangle of specified size                                */
        ShapeCross,                     /*!< a horizontal and vertical line of specified size           */
        ShapeImageScaled,               /*!< specified image scaled to size                             */
        ShapeImage                      /*!< specified image                                            */
    };
    Q_ENUM(Shapes)

    PASErase();

    void setShape( Shapes );
    void setSize( const QSize & );
    void setImage( const QImage & );
    void setSelected( Shapes, const QSize &, const QImage & );

    Shapes      getShape() const  { return nShape;    }
    QSize       getSize() const   { return size;      }
    QImage      getImage() const  { return image;     }

    QWidget *   getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *   getToolBar( QWidget *pParent );
    QWidget *   getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    Shapes  nShape  = ShapeEllipse;
    QSize   size    = QSize( 5, 5 );
    QImage  image;                      /*!< this will be 'stamped' and/or 'dragged'                    */

};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits PASErase.
 *  
 * Horizontal layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASEraseToolBar : public QWidget
{
    Q_OBJECT
public:
    PASEraseToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotShape( int );
    void slotWidth( int );
    void slotHeight( int );
    void slotImage();

protected:
    PASErase *        pSetting;

    QComboBox *     pShape;
    QSpinBox *      pWidth;
    QSpinBox *      pHeight;
    WImageButton *  pImage;
};

/*!
 * \brief   Edit widget for use in a dialog or dock window. 
 *          Edits PASErase.
 *  
 * Vertical layout using form layout with labels. 
 *  
 * \author pharvey (3/14/23)
 */
class PASErasePanel : public QWidget
{
    Q_OBJECT
public:
    PASErasePanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotShape( int );
    void slotWidth( int );
    void slotHeight( int );
    void slotImage();

protected:
    PASErase *        pSetting;

    QComboBox *     pShape;
    QSpinBox *      pWidth;
    QSpinBox *      pHeight;
    WImageButton *  pImage;
};

#endif

