/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_SAPreferencesDialog
#define H_SAPreferencesDialog

#include <CBD.h>

class SAPreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    SAPreferencesDialog( QWidget *pParent = nullptr );

    static void doPreferences( QWidget *pParent );

protected:
    QSplitter *     pSplitter; 
    QTreeWidget *   pTree;
    QStackedWidget *pPanel;

protected slots:
    void slotItemActivated( QTreeWidgetItem *, int );
};

/*!
 * \brief View/edit startup options. 
 *  
 * These are stored using QSettings (not SSettings). 
 * 
 * \author pharvey (3/21/23)
 */
class SAPreferencesStartUpPanel : public QWidget
{
    Q_OBJECT
public:
    SAPreferencesStartUpPanel( QWidget *pParent );

protected:
    QCheckBox *pRestoreState;      
    QCheckBox *pSplash;      

protected slots:
    void slotRestoreState( Qt::CheckState );
    void slotSplash( Qt::CheckState );
};

#endif
