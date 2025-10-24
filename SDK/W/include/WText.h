/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef H_WText
#define H_WText

#include <CBD.h>

#include "WTextHAlignComboBox.h"
#include "WTextVAlignComboBox.h"
#include "WColorButton.h"
#include "WBrush.h"

/*!
 * \brief Shows an example of the given CBD:CBDText.
 * 
 * \author pharvey (12/31/24)
 */
class WTextSwatch : public QWidget
{
    Q_OBJECT
public:
    WTextSwatch( const CBD::CBDText &t, QWidget *pParent, const QString &stringText = QString( tr("AaBbYyZz") ) );

    void setText( const QString & );

public slots:
    void slotRefresh( const CBD::CBDText &t );

protected:
    CBD::CBDText    t;
    QString         stringText;

    void paintEvent( QPaintEvent *pEvent ) override;
};

/*!
 * \brief A tool button for invoking an editor for CBD::CBDText.
 * 
 * The button will give some indication of what the CBD::CBDText looks like.
 * Use this in a tool bar when space is limited.
 * 
 * \author pharvey (12/31/24)
 */
class WTextButton : public QToolButton
{
    Q_OBJECT
public:
    WTextButton( const CBD::CBDText &t, QWidget *pParent );

    CBD::CBDText getText() { return t; }

signals:
    void signalChanged( const CBD::CBDText &t );

public slots:
    void slotRefresh( const CBD::CBDText &t );

protected:
    CBD::CBDText t;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief A widget which can be put into a tool bar. Includes a more button for invoking an editor.
 * 
 * Use this in a tool bar when you have the space.
 * 
 * \author pharvey (12/31/24)
 */
class WTextToolBar : public QWidget
{
    Q_OBJECT
public:
    WTextToolBar( const CBD::CBDText &t, QWidget *pParent );

    CBD::CBDText getText() { return t; }

signals:
    void signalChanged( const CBD::CBDText &t );

public slots:
    void slotRefresh( const CBD::CBDText &t );

protected:
    CBD::CBDText    t;

    QFontComboBox * pFontComboBox;
    QSpinBox *      pSize;
    QToolButton *   pMore;

protected slots:
    void slotFont( const QFont & );
    void slotSize( int );
    void slotMore();
};

/*!
 * \brief A widget which allows all CBD::CBDText options to be edited.
 * 
 * This is primarily used in a dialog.
 * 
 * \author pharvey (12/31/24)
 */
class WTextPanel : public QWidget
{
    Q_OBJECT
public:
    WTextPanel( const CBD::CBDText &t, QWidget *pParent = nullptr );

    CBD::CBDText getText() { return t; }

signals:
    void signalChanged( const CBD::CBDText &t );

public slots:
    void slotRefresh( const CBD::CBDText &t );

protected:
    CBD::CBDText t;

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
    WTextSwatch *           pSwatch;

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

/*!
 * \brief A dialog for editing the CBD::CBDText. 
 * 
 * \author pharvey (12/31/24)
 */
class WTextDialog : public QDialog
{
    Q_OBJECT
public:
    WTextDialog( const CBD::CBDText &t, QWidget *pParent = nullptr );

    CBD::CBDText getText() { return pPanel->getText(); }
    static CBD::CBDText doEdit( const CBD::CBDText &t, QWidget *pParent );

protected:
    WTextPanel *pPanel;
};

#endif

