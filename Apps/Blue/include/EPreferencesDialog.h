/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_EPreferencesDialog
#define H_EPreferencesDialog

#include <CBD.h>

class EPreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    EPreferencesDialog( QWidget *pParent = nullptr );

    static void doPreferences( QWidget *pParent );

protected:
    QSplitter *     pSplitter; 
    QTreeWidget *   pTree;
    QStackedWidget *pPanels;

    QMap<QTreeWidgetItem*,int> mapItemPanel;

    QTreeWidgetItem *doAddPanel( const QString &stringText, QWidget *pPanel );
    QTreeWidgetItem *doAddPanel( QTreeWidgetItem *pItem, const QString &stringText, QWidget *pPanel );
    void doLoadState();
    void doSaveState();

protected slots:
    void slotItemActivated( QTreeWidgetItem *, int );
    void slotOk();
    void slotRejected();
};

/*!
 * \brief View/edit startup options. 
 *  
 * These are stored using QSettings (not SSettings). 
 * 
 * \author pharvey (3/21/23)
 */
class EPreferencesStartUpPanel : public QWidget
{
    Q_OBJECT
public:
    EPreferencesStartUpPanel( QWidget *pParent );

protected:
    QCheckBox *pRestoreState;      
    QCheckBox *pSplash;      

protected slots:
    void slotRestoreState( Qt::CheckState n );
    void slotSplash( Qt::CheckState n );
};

#endif
