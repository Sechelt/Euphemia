/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_SSetting
#define H_SSetting

#include <CBDPersist.h>

/*!
 * \brief Base class for classes which provide; read/write, save/load, and signalChanged() for related group of settings such as a pen in a draw app.
 * 
 * 0. Default/Preference
 * 
 *      Default values are hard coded values (typically Qt default) until saved (\sa SSettings::doSave) as Preference after which the preferences are the default.
 *      Preference values are persisted and are silently reloaded at app start to create the starting Context.
 * 
 * 1. Context
 * 
 *      The Preferred values but they can be changed to create a different, temporary, value.
 * 
 * 2. Create New
 * 
 *      New objects are created using the Context.
 *      The Context can be changed (and all connected objects will get notified).
 *      The Context value will remain even after the object is unselected.
 * 
 * 3. Manipulate Existing
 * 
 *      The Context is changed to the values found in the object being manipulated - without notifying any connected objects.
 *      This is done when the object selected - the object will call SSetting::setSelected( ... ).
 *      The Context can be changed (and all connected objects will get notified).
 *      The Context value will remain even after the object is unselected.
 * 
 * \author pharvey (3/13/23)
 */
class SSetting : public QObject 
{
    Q_OBJECT
public:
    SSetting( QObject *pParent = nullptr );                                         /*!< SSettings will take ownership in SSettings::doInsert()                     */

    /* void setSelected( ... ); */                                                  /*!< set values, emits signalSync but NOT signalChanged, see derived classes    */

    virtual QWidget *getButton( QWidget *pParent ) = 0;                             /*!< button to invoke dialog/panel for edit or nullptr                          */
    virtual QWidget *getToolBar( QWidget *pParent ) = 0;                            /*!< edit widget suitable for a tool bar or nullptr                             */
    virtual QWidget *getPanel( QWidget *pParent ) = 0;                              /*!< edit widget suitable for a dialog/dock or a nullptr                        */
    virtual QWidget *getSwatch( QWidget *pParent );                                 /*!< used to show example - not always needed - default is nullptr              */

    virtual void doDialog( QWidget *pParent, bool bThis = false );                  /*!< edit dialog - default uses panel - may return nullptr                      */
    virtual void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) = 0;                                                                                        
    virtual void doLoad( QDomElement *pdomElem ) = 0;                                                                                                               

signals:                                                                                                                                                            
    void signalChanged();                                                           /*!< notify controls and any other connected objects                            */
    void signalSync();                                                              /*!< notify controls only                                                       */
};

/*!
 * \brief Dialog for editing most SSetting objects..
 *  
 * Combines the panel and the swatch. Only really useful if 
 * SSetting::getPanel() returns a viable widget but it will 
 * try to use SSettings::getToolBar() as an alternative. 
 *  
 *     SDialog dialog( pParent, "SBrush", "Brush" );
 *     dialog.exec();
 *  
 * \param pParent - parent widget
 * \param stringKey - key used for QSSettings::getSetting()
 * \param stringTitle - window title
 *  
 * \author pharvey (3/14/23)
 */
class SDialog : public QDialog
{
    Q_OBJECT
public:
    SDialog( QWidget *pParent, const QString &stringKey, const QString &stringTitle = QString() );
    SDialog( QWidget *pParent, SSetting *pSetting, const QString &stringTitle = QString() );
    ~SDialog();

protected:
    QString stringKey;
    QWidget *pPanel = nullptr;

    void doLoadState();
    void doSaveState();
};

#endif

