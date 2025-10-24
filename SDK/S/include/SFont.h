/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef H_SFont
#define H_SFont

#include <CBD.h>
#include <SSettings.h>

#include "SPen.h"
#include "SBrush.h"

/*!
 * \brief Setting for a SFont.
 * 
 * \author pharvey (3/14/23)
 */
class SFont : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QFont font READ getValue WRITE setValue NOTIFY signalChanged)
public:
    SFont();

    void setValue( const QFont & );
    void setSelected( const QFont & ); 

    QFont       getValue() { return font; }
    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QFont font;               // current 
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (12/27/24)
 */
class SFontButton : public QToolButton
{
    Q_OBJECT
public:
    SFontButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SFont *pFont;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit SFont using a widget which can be used in a tool bar. 
 *  
 * Horizontal layout with subset of options. 
 * Includes 'More' button to invoke dialog to edit more details. 
 * 
 * \author pharvey (3/14/23)
 */
class SFontToolBar : public QWidget
{
    Q_OBJECT
public:
    SFontToolBar( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SFont *    pSetting;
    QFont       font;

    QFontComboBox * pFontComboBox;
    QSpinBox *      pSize;
    QToolButton *   pMore;

protected slots:
    void slotFont( const QFont & );
    void slotSize( int );
    void slotMore();
};

/*!
 * \brief Panel for editing a SFont.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (3/14/23)
 */
class SFontPanel : public QWidget
{
    Q_OBJECT
public:
    SFontPanel( QWidget *pParent = nullptr );

public slots:
    void slotRefresh();

protected:
    SFont *    pSetting;
    QFont       font;

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
};

/*!
 * \brief Swatch (example) for a SFont.
 *  
 * \author pharvey (3/14/23)
 */
class SFontSwatch : public QWidget
{
    Q_OBJECT
public:
    SFontSwatch( QWidget *pParent, const QString &stringText = QString( tr("AaBbYyZz") ) );

public slots:
    void slotRefresh();
    void slotRefreshPen();
    void slotRefreshBrush();

protected:
    SFont *    pSetting;
    SPen *     pSettingPen;        /*!< foreground     */
    SBrush *   pSettingBrush;      /*!< background     */
    QFont       font;
    QPen        pen;
    QBrush      brush;
    QString     stringText;

    void paintEvent( QPaintEvent *pEvent ) override;
};

#endif

