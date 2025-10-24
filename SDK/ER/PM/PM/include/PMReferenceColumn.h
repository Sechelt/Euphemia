#ifndef H_PMReferenceColumn
#define H_PMReferenceColumn

#include <AWObject.h>

class PMReference;
class PMColumn;

/*!
 * \brief A parent key column and a child key column that come together to join two tables. 
 *  
 * A \sa PMReference will create one of these for each column in the selected key. It will 
 * create a ref from us to a \sa PMKey column. It does not create a ref to the \sa PMColumn 
 * in the child table. This is done via \sa PMReferenceWidget. 
 *  
 * Implementation 
 *  
 * This is implemented by creating the following refs; 
 *  
 * 1. ChildColumn -> ReferenceColumn 
 * 2. ReferenceColumn -> ParentColumn 
 *  
 * With the following implications; 
 *  
 * A. delete ReferenceColumn and both refs go away 
 * B. delete ParentColumn and its ref goes away but this remains
 * C. delete ChildColumn and its ref goes away but this remains
 *  
 * \note    We could get rid of this and make refs directly between the columns - but this is problematic. 
 *          We need this, intermediate, object to differentiate between references that connect the same tables. 
 *          An example of this is;
 *  
 *         +----------+                             +----------+
 *         | Employees| <--------- CreatedBy ------ | Tasks    |
 *         +----------+ <--------- AssignedTo ----- +----------+
 *  
 *          Even still - we need to differentiate between ParentColumn ref and ChildColumn ref. It makes sense
 *          for ReferenceColumn to "Use" each but to differentiate ReferenceColumn "Uses" ParentColumn and is
 *          "UsedBy" ChildColumn.
 *  
 * \author pharvey (5/20/20)
 */
class PMReferenceColumn : public AWObject
{
    Q_OBJECT
public:
    PMReferenceColumn( PMReference *pParent );
    virtual ~PMReferenceColumn();

    virtual void setName( const QString & ) override {} // name is calculated
    virtual void setParentColumn( PMColumn * );
    virtual void setChildColumn( PMColumn * );

    virtual QPixmap     getIcon() override;
    virtual QString     getName() override;
    virtual QString     getComment() override;
    virtual PMColumn *  getParentColumn();
    virtual PMColumn *  getChildColumn();
};

/*!
 * \brief Holds connections to 0-n columns in the parent table.
 * 
 * These columns are display columns. ie used in a pick-list then resolved to the key value.
 * 
 * \author pharv (3/10/2025)
 */
class PMReferenceColumnDisplay : public AWObject
{
    Q_OBJECT
public:
    PMReferenceColumnDisplay( PMReference *pParent );
    virtual ~PMReferenceColumnDisplay();

    virtual void setName( const QString & ) override {} // name is fixed

    virtual QPixmap             getIcon() override;
    virtual QString             getName() override  { return tr("Display"); }
    virtual QVector<PMColumn *> getColumns();

    virtual void doConnectColumn( PMColumn * );
    virtual void doDisconnectColumn( PMColumn * );
};

#endif 


