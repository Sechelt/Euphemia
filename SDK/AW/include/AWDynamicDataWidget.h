#ifndef H_AWDynamicDataWidget
#define H_AWDynamicDataWidget

#include "AWObject.h"

/*!
 * \brief A table widget for; adding, editing and deleting dynamic data. 
 *  
 * Dynamic data uses/is QObject properties.  
 * 
 * \author pharvey (4/18/20)
 */
class AWDynamicDataWidget : public QMainWindow
{
    Q_OBJECT
public:
    AWDynamicDataWidget( AWObject *pObject, QWidget *pwidgetParent );
    virtual ~AWDynamicDataWidget();

public slots:
    virtual void slotNew();
    virtual void slotDelete();
    virtual void slotValueChanged( int nRow, int nCol );

protected:
    QToolBar *      ptoolbar;
    QAction *       pActionNew;
    QAction *       pActionDelete;
    QTableWidget *  ptable;
    AWObject *      pObject;

    // we cache the name/values here (in the same order as in the table) 
    // because we need to know old name/values when edited (until changes validated as ok)
    QVector<QString>    vectorNames;
    QVector<QString>    vectorValues;

    // SETTERS
    
    // GETTERS
    
    // DO'RS
    virtual void doInit();
    virtual void doLoad();
};

#endif 


