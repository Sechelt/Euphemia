#ifndef AWDATATABLE_H
#define AWDATATABLE_H

//
#include "AWObject.h"

/*!
 * \brief A table for viewing/editing AWObjects.
 *  
 * This is a good base class for implementing a widget which displays a table of                                
 * child objects (those which inherit stringInherits).                                                           
 *                                                                                                               
 * The child objects must be derived from AWObject. The User can edit the table rows                            
 * directly or, more typically, invoke a properties dialog for the child object                                  
 * displayed on a row.                                                                                           
 *                                                                                                               
 * Set bUses = true to work from the listUses objects instead of the regular child objects list.                 
 *                                                                                                               
 * This widget contains a toolbar which, by default, has buttons for;                                            
 * = new row           - creates a object and appends to child list and to QTable                                
 * = edit row          - invokes the child objects properties dialog                                             
 * = delete row        - delete the object from the child list and from the QTable                               
 * = move row up       - move the object up in the child list (relative to other stringInherits) and in QTable   
 * = move row down     - move the object down in the child list (relative to other stringInherits) and in QTable 
 *                                                                                                               
 * The columns are created in the constructor and the rows are Set/Get using doRowGet() and doRowSet().          
 *  
 * A widget can be used to view/edit a cell by creating a delegate and assigning it as follows; 
 *  
 *      ptable->setItemDelegate( new MyDelegate );
 *  
 * Start by looking at QStyledItemDelegate::createEditor(). See StarDelegate in Qt docs for details. 
 *  
 * \note The 1st column (column 0) is reserved for holding a pointer to the object represented on the given row.                                                                                                    
 *  
 * \author pharvey (12/11/19)
 */
class AWDataTable : public QMainWindow
{
    Q_OBJECT
public:
    // match on inherit
    AWDataTable( AWObject *pmodelParent, const QString &stringInherits, QWidget *pwidgetParent, bool bUses = false );
    // match on class
    AWDataTable( AWObject *pmodelParent, QWidget *pwidgetParent, const QString &stringClass, bool bUses = false );
    virtual ~AWDataTable();

    virtual void setNew( bool b = true );
    virtual void setEdit( bool b = true );
    virtual void setDelete( bool b = true );
    virtual void setUp( bool b = true );
    virtual void setDown( bool b = true );

    virtual void doStabalize();

public slots:
    virtual void slotNew();
    virtual void slotEdit();
    virtual void slotDelete();
    virtual void slotUp();
    virtual void slotDown();
    virtual void slotValueChanged( int nRow, int nCol );
    virtual void slotApply() {;}

protected:
    QToolBar *      ptoolbar;
    QAction *       pActionNew;
    QAction *       pActionEdit;
    QAction *       pActionDelete;
    QAction *       pActionUp;
    QAction *       pActionDown;
    QTableWidget *  ptable;
    AWObject *      pmodelParent;
    QString         stringClass;
    bool            bClassInherits;
    bool            bUses;

    // SETTERS
    
    // GETTERS
    virtual AWObject *getObject( int nRow );
    virtual int getRowNumStrict();
    virtual int getRowNumNotStrict();
    
    // DO'RS
    virtual void doInit();
    virtual void doTableSet();
    virtual void doRowAppend( AWObject *pmodel );
    virtual void doRowGet( int nRow, AWObject *pmodel );
    virtual void doRowSet( int nRow, AWObject *pmodel );

    virtual void doSaveState();
    virtual void doLoadState();
};

#endif 


