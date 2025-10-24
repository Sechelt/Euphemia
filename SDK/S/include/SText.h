/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef H_SText
#define H_SText

#include <CBD.h>
#include <SSettings.h>

#include <WTextHAlignComboBox.h>
#include <WTextVAlignComboBox.h>
#include <WColorButton.h>
#include <WBrush.h>

/*!
 * \brief Font combined with foreground color, background brush, etc 
 * 
 * \author pharvey (12/27/24)
 */
class SText : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(CBD::CBDText text READ getValue WRITE setValue NOTIFY signalChanged)
public:
    SText();

    void setValue( const CBD::CBDText & );
    void setSelected( const CBD::CBDText & );

    CBD::CBDText    getValue() { return text; }
    QWidget *       getButton( QWidget *pParent ) override;
    QWidget *       getToolBar( QWidget *pParent ) override;
    QWidget *       getPanel( QWidget *pParent ) override;
    QWidget *       getSwatch( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    CBD::CBDText text;
};

/*!
 * \brief Swatch (example) for a SText.
 *  
 * \author pharvey (12/27/24)
 */
class STextSwatch : public QWidget
{
    Q_OBJECT
public:
    STextSwatch( QWidget *pParent, const QString &stringText = QString( tr("AaBbYyZz") ) );

    void setText( const QString & );

public slots:
    void slotRefresh();

protected:
    SText *         pSetting;
    CBD::CBDText    text;
    QString         stringText;

    void paintEvent( QPaintEvent *pEvent ) override;
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (12/27/24)
 */
class STextButton : public QToolButton
{
    Q_OBJECT
public:
    STextButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SText *pText;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit SText using a widget which can be used in a tool bar. 
 *  
 * Horizontal layout with subset of options. 
 * Includes 'More' button to invoke dialog to edit more details. 
 * 
 * \author pharvey (12/27/24)
 */
class STextToolBar : public QWidget
{
    Q_OBJECT
public:
    STextToolBar( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SText *         pSetting;
    CBD::CBDText    text;

    QFontComboBox * pFontComboBox;
    QSpinBox *      pSize;
    QToolButton *   pMore;

protected slots:
    void slotFont( const QFont & );
    void slotSize( int );
    void slotMore();
};

/*!
 * \brief Panel for editing a SText.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (12/27/24)
 */
class STextPanel : public QWidget
{
    Q_OBJECT
public:
    STextPanel( QWidget *pParent = nullptr );

public slots:
    void slotRefresh();

protected:
    SText *         pSetting;
    CBD::CBDText    text;

    QFontDatabase           fontDatabase;

    // font
    QLineEdit *             pLineEditFont;
    QListWidget *           pListWidgetFont;

    // font style
    QLineEdit *             pLineEditFontStyle;
    QListWidget *           pListWidgetFontStyle;

    // size
    QLineEdit *             pLineEditSize;
    QListWidget *           pListWidgetSize;

    // effects - other effects are part of style
    QCheckBox *             pCheckBoxStrikeOut;
    QCheckBox *             pCheckBoxUnderline;
    WTextHAlignComboBox *   pAlignHTextWidget;
    WTextVAlignComboBox *   pAlignVTextWidget;
    WColorButton *          pColorButton;
    WBrushButton *          pBrushButton;
    STextSwatch *           pSwatch;

    // writing system
    QComboBox *             pComboBoxWritingSystem;

    int getSelectedWritingSystem();
    QString getSelectedFont();
    QString getSelectedFontStyle();
    QString getSelectedSize(); 

    void doInitWritingSystems();
    void doInitFonts();
    void doInitFontStyles();
    void doInitSizes();
    void doInitEffects();

protected slots:
    void slotSize( const QString & );

    void slotWritingSystems( int );
    void slotFonts( QListWidgetItem * );
    void slotFontStyles( QListWidgetItem * );
    void slotSizes( QListWidgetItem * );
    void slotStrikeout( Qt::CheckState );
    void slotUnderline( Qt::CheckState );
    void slotAlignH( Qt::AlignmentFlag ); 
    void slotAlignV( Qt::AlignmentFlag ); 
    void slotColor( QColor ); 
    void slotBrush( QBrush ); 
};

#endif

