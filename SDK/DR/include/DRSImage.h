/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_DRSImage
#define H_DRSImage

#include <SSettings.h>

class DRSImage : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ getValue WRITE setValue NOTIFY signalChanged)
public:
    DRSImage();

    void setValue( const QImage & );
    void setSelected( const QImage & );

    QImage      getValue() { return image; }
    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QImage image;
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (9/29/25)
 */
class DRSImageButton : public QToolButton
{
    Q_OBJECT
public:
    DRSImageButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    DRSImage *pSetting;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit DRSImage using a widget which can be used in a tool bar. 
 *  
 * \author pharvey (9/29/25)
 */
class DRSImageToolBar : public QWidget
{
    Q_OBJECT
public:
    DRSImageToolBar( QWidget *pParent, bool bCompress = false );
};

/*!
 * \brief Panel for editing a DRSImage.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (9/29/25)
 */
class DRSImagePanel : public QWidget
{
    Q_OBJECT
public:
    DRSImagePanel( QWidget *pParent = nullptr );
};

/*!
 * \brief Swatch (example) for a DRSImage.
 *  
 * \author pharvey (9/29/25)
 */
class DRSImageSwatch : public QWidget
{
    Q_OBJECT
public:
    DRSImageSwatch( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    DRSImage *  pSetting;
    QImage      image;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

#endif

