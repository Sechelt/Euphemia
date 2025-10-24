#ifndef H_AWEditorWidget
#define H_AWEditorWidget

#include <CBD.h>

class AWObject;
class AWPalWidget;
class AWPanWidget;
/*
class AWEditorControls
{
public:
    QVector<QAction*>   vectorActions;
    QVector<QMenu*>     vectorMenus;
    QVector<QToolBar*>  vectorToolBars;
    QVector<QWidget*>   vectorStatusBarWidgets;

    void doDeleteAll()
    {
        qDeleteAll(vectorStatusBarWidgets); vectorStatusBarWidgets.resize(0);
        qDeleteAll(vectorToolBars); vectorToolBars.resize(0);
        qDeleteAll(vectorMenus); vectorMenus.resize(0);
        qDeleteAll(vectorActions); vectorActions.resize(0); // must be last
    }
};
*/

/*!
 * \class AWEditorWidget 
 * \brief Common interface for editors (diagrams, text editors etc). 
 *  
 * This is a base class for editor widgets. These widgets are;
 * - a container for the actual editor 
 * - an interface between the application and the actual editor 
 *  
 * The actual editor depends upon the data. It can be some form of a 
 * QGraphicsView or QTextEdit for example. The application does not 
 * need to know. 
 *  
 * \note In the future; an object factory (plugin) will provide these. So 
 * keep the interface generic. 
 *  
 * \sa AWPalWidget 
 * \sa AWPanWidget 
 *  
 * \author pharvey (10/26/19)
 */
class AWEditorWidget: public QMainWindow
{
    Q_OBJECT
public:
    AWEditorWidget( AWObject *pObject, QWidget *pParent = nullptr );
    virtual ~AWEditorWidget();

    // SETTERS
    virtual void doGoTo();

    // GETTERS
    virtual AWObject *          getObject() { return pObject; }
    virtual AWPalWidget *       getPalWidget() { return pPalWidget; }  
    virtual AWPanWidget *       getPanWidget() { return pPanWidget; }  

    // as per QScintilla - noops by default
    virtual bool doFindFirst( const QString &expr, bool re, bool cs, bool wo, bool wrap, bool forward = true, int line = -1, int index = -1, bool show = true, bool	posix = false, bool cxx11 = false );
    virtual bool doFindFirstInSelection( const QString &s, bool re, bool cs, bool wo, bool forward = true, bool show = true, bool posix = false, bool cxx11 = false );
    virtual void doFindNext();
    virtual void doReplace( const QString &stringNew );
    virtual void doReplaceAll( const QString &stringNew );
    virtual void doCancelFind();

    virtual bool isFind()       { return false; }
    virtual bool canFind()      { return false; }
    virtual bool canReplace()   { return false; }

signals:
    void signalModified( AWEditorWidget * );
    void signalGoTo( AWEditorWidget * );

protected:
    AWObject *          pObject;
    AWPalWidget *       pPalWidget;     // editor will have 0-1 of these
    AWPanWidget *       pPanWidget;     // editor will have 0-1 of these

protected slots:
    void slotModified();
};

#endif


