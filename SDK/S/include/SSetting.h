/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
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
    /*
     * Derived classes can add Q_PROPERTY macros here allowing access to properties with just a SSetting*.
     */
public:
    SSetting( QObject *pParent = nullptr );                                         /*!< SSettings will take ownership in SSettings::doInsert()                     */

    /*
     * Derived classes can add setters for their properties here.
     * These are called by 'controls' to request a value change.
     * They should do nothing (short circuit) if value is same.
     * They emit signalChanged after value changed. This should be connected a slot in any interested objects - including 'controls' in thier ie slotRefresh().
     * They should NEVER emit signalSync.
     *
     * void setxxx()            
     */

    /*
     * Derived classes can add setters for their properties here.
     * These set (not request) properties but only notify 'controls' - useful when an object has been 'selected' ie shape in a drawing
     * They should do nothing (short circuit) if value is same.
     * They emit signalSync after value changed. This should be connected to a slot in any interested 'controls' ie slotRefresh()
     * They should NEVER emit signalChanged.
     *
     * void setSelectedxxx( ... );
     */

    /* 
     * Derived classes can add getters.
     * These are called by any connected objects/controls in thier ie slotRefresh()
     * These simply return a property - either directly or indirectly via Q_PROPERTY.
     *
     * t getxxx()
     */

    /*!
     * \brief Get a button that can be used, for example, to invoke a dialog from a tool bar.
     * 
     * \author pharvey (2026-11-27)
     * 
     * \param pParent 
     * 
     * \return QWidget* will be a nullptr if not supported
     */
    virtual QWidget *   getButton( QWidget *pParent );
    /*!
     * \brief Get a widget that can be used in a tool bar.
     * 
     * This widget should contain 1-3 most needed properties and a button to invoke a dialog as may be needed.
     * The properties are usually just from this setting class.
     * This should be suitable for a tool bar; tool bar height and horizontal layout.
     * 
     * \author pharvey (2026-11-27)
     * 
     * \param pParent 
     * 
     * \return QWidget* will be a nullptr if not supported
     */
    virtual QWidget *   getToolBar( QWidget *pParent );
    /*!
     * \brief Get a widget that can be used in, for example, a popup.
     * 
     * This typically contains the tool bar properties and more.
     * The properties are usually just from this setting class.
     * This should use a vertical layout - often a form.
     * 
     * \author pharvey (2026-11-27)
     * 
     * \param pParent 
     * 
     * \return QWidget* will be a nullptr if not supported
     */
    virtual QWidget *   getPanel( QWidget *pParent );
    /*!
     * \brief Get a widget that shows an example.
     * 
     * This is may be used as a compliment to the panel.
     * The best example of this is example text when selecting a; font, font colors, etc.
     * 
     * \author pharvey (2026-11-27)
     * 
     * \param pParent 
     * 
     * \return QWidget* will be a nullptr if not supported
     */
    virtual QWidget *   getSwatch( QWidget *pParent );
    /*!
     * \brief Get a widget which shows the panel from this class but also panels from related setting objects.
     * 
     * This can be, but does not have to be, a QToolBox with panel widgets.
     * 
     * \author pharvey (2026-11-27)
     * 
     * \param pParent 
     * 
     * \return QWidget* will be a nullptr if not supported
     */
    virtual QWidget *   getPanelMax( QWidget *pParent );
    /*!
     * \brief Invoke a dialog.
     * 
     * The dialog will contain widget from getPanelMax or getPanel+getSwatch or specified panel.
     * The panel widget will come from this (or derived) class or be provided.
     * 
     * \sa SDialog
     * 
     * \author pharvey (2026-11-27)
     * 
     * \param pParent 
     * \param bThis   
     */
    virtual void doDialog( QWidget *pParent, bool bThis = false );
    virtual void doDialog( QWidget *pParent, const QString &stringKey, const QString &stringTitle, QWidget *pPanel );
    /*!
     * \brief These are used to save/load the properties for this class.
     * 
     * The properties can be persisted to save current context (ie when app exits) and/or in a preferences editor. 
     * 
     * \author pharvey (2026-11-27)
     * 
     * \param pdomDoc  
     * \param pdomElem 
     */
    virtual void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) = 0;                                                                                        
    virtual void doLoad( QDomElement *pdomElem ) = 0;                                                                                                               

signals:                                                                                                                                                            
    void signalChanged();                                                           /*!< notify controls and any other connected objects                            */
    void signalSync();                                                              /*!< notify controls only                                                       */
};

/*!
 * \brief Dialog for editing most SSetting objects..
 *  
 * SDialog uses tool box.
 * LToolsWidget uses panels.
 * 
 * Use case;
 * 
 * 1. LToolsWidget and panel combined with tool box dock
 * 2. tool bar and more button invoking these dialogs
 *  
 *     SDialog dialog( pParent, "SBrush", "Brush" );
 *     dialog.exec();
 *  
 * \param pParent - parent widget
 * \param stringKey - key used for QSSettings::getSetting() and saving dialog geometry.
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
    SDialog( QWidget *pParent, const QString &stringKey, const QString &stringTitle, QWidget *pPanel );
    ~SDialog();

protected:
    QString stringKey;          /*!< usually setting class name - also used when saving/loading window geometry */

    void doLoadState();
    void doSaveState();
};

#endif

