/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_WFindTextWidget
#define H_WFindTextWidget

#include "W.h"

class WFindTextWidget : public QWidget
{
    Q_OBJECT
public:
    WFindTextWidget( QWidget *pParent );

    QString getFindText()       { return pFindText->text();     }
    QString getReplaceText()    { return pReplaceText->text();  }

    void doClear();

    bool isRegularExpression()  { return pExp->isChecked(); }
    bool isCaseSensitive()      { return pCaseSensitive->isChecked(); }
    bool isWholeWord()          { return pWholeWord->isChecked(); }

signals:
    void signalFindTextChanged( const QString & );
    void signalNext();
    void signalPrevious();
    void signalReplace();
    void signalReplaceAll();

protected:
    QLineEdit *     pFindText;
    QLineEdit *     pReplaceText;
    QToolButton *   pNext;
    QToolButton *   pPrevious;
    QToolButton *   pReplace;
    QToolButton *   pReplaceAll;

    QToolButton *   pExp;
    QToolButton *   pCaseSensitive;
    QToolButton *   pWholeWord;
};

#endif 

