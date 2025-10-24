/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef H_LSText
#define H_LSText

#include <WColorButton.h>
#include <WTextHAlignComboBox.h>
#include <WTextVAlignComboBox.h>

#include "SSetting.h"

/*!
 * \brief 
 * 
 * \author pharvey (3/15/23)
 */
class LSText : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QString stringText READ getText WRITE setText NOTIFY signalChanged)
    Q_PROPERTY(Qt::AlignmentFlag nHAlign READ getHAlign WRITE setHAlign NOTIFY signalChanged)
    Q_PROPERTY(Qt::AlignmentFlag nVAlign READ getVAlign WRITE setVAlign NOTIFY signalChanged)
public:
    LSText();

    void setText( const QString & );
    void setHAlign( Qt::AlignmentFlag );
    void setVAlign( Qt::AlignmentFlag );
    void setSelected( const QString &, Qt::AlignmentFlag, Qt::AlignmentFlag );

    QString             getText()       { return stringText;    }
    Qt::AlignmentFlag   getHAlign()     { return nHAlign;       }
    Qt::AlignmentFlag   getVAlign()     { return nVAlign;       }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent ) override;
    QWidget *getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QString stringText;
    Qt::AlignmentFlag nHAlign = Qt::AlignHCenter;
    Qt::AlignmentFlag nVAlign = Qt::AlignVCenter;
};

/*!
 * \brief   This does double-duty. 
 *          Use as a tool bar widget - set orientation to horizontal.
 *          Use as a panel widget - set orientation to vertical
 * 
 * \author pharvey (3/15/23)
 */
class LSTextPanel : public QWidget
{
    Q_OBJECT
public:
    LSTextPanel( QWidget *p, Qt::Orientation nOrientation = Qt::Vertical );

public slots:
    void slotRefresh();

protected slots:
    void slotText( const QString & );
    void slotHAlign( Qt::AlignmentFlag n );
    void slotVAlign( Qt::AlignmentFlag n );
    void slotMore();

protected:
    LSText *                 pSetting;

    QLineEdit *             pLineEdit = nullptr;
    WTextHAlignComboBox *   pHAlign;
    WTextVAlignComboBox *   pVAlign;
    QToolButton *           pMore;
};

#endif


