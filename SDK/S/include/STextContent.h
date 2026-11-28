/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_STextContent
#define H_STextContent

#include <CBD.h>
#include <ADObject.h>
#include <SSettings.h>

/*!
 * \brief Settings for ADObject - OID (read-only), Code, Name, and Comment.
 * 
 * \author pharvey (2026-11-26)
 */
class STextContent : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QString stringCode READ getCode WRITE setCode NOTIFY signalChanged)
    Q_PROPERTY(QString stringName READ getName WRITE setName NOTIFY signalChanged)
    Q_PROPERTY(QString stringComment READ getComment WRITE setComment NOTIFY signalChanged)
public:
    STextContent();

    void setCode( const QString & );
    void setName( const QString & );
    void setComment( const QString & );
    void setDisplayCode( bool );
    void setDisplayName( bool );
    void setDisplayComment( bool );

    void setSelectedCode( const QString & );
    void setSelectedName( const QString & );
    void setSelectedComment( const QString & );
    void setSelectedDisplayCode( bool );
    void setSelectedDisplayName( bool );
    void setSelectedDisplayComment( bool );


    QString getCode()       { return stringCode; }
    QString getName()       { return stringName; }
    QString getComment()    { return stringComment; }
    bool    getDisplayCode()    { return bDisplayCode; }
    bool    getDisplayName()    { return bDisplayName; }
    bool    getDisplayComment() { return bDisplayComment; }

    QWidget *       getButton( QWidget *pParent ) override;
    QWidget *       getToolBar( QWidget *pParent ) override;
    QWidget *       getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QString stringCode;
    QString stringName;
    QString stringComment;

    bool bDisplayCode = false;
    bool bDisplayName = false;
    bool bDisplayComment = false;
};

class STextContentToolBar : public QWidget
{
    Q_OBJECT
public:
    STextContentToolBar( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    STextContent *  pSetting;
    QLabel *        pLabel;
    QLineEdit *     pLineEdit;
    QToolButton *   pMore;

protected slots:
    virtual void slotLineEdit( const QString & );
    virtual void slotMore();
};

class STextContentPanel : public QWidget
{
    Q_OBJECT
public:
    STextContentPanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotName( const QString & );
    void slotCode( const QString & );
    void slotComment( const QString & );
    void slotDisplayName( bool );
    void slotDisplayCode( bool );
    void slotDisplayComment( bool );

protected:
    STextContent *  pSetting;
    QLineEdit *     pCode = nullptr;
    QLineEdit *     pName = nullptr;
    QLineEdit *     pComment = nullptr;
    QCheckBox *     pDisplayCode = nullptr;
    QCheckBox *     pDisplayName = nullptr;
    QCheckBox *     pDisplayComment = nullptr;
};

#endif

