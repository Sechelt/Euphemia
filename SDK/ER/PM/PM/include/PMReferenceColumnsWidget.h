#ifndef H_PMReferenceColumnsWidget
#define H_PMReferenceColumnsWidget

#include "PMReference.h"
#include "PMKey.h"

/*!
 * \brief 
 * 
 * \author pharvey (5/21/20)
 */
class PMReferenceColumnWidget : public QComboBox
{
    Q_OBJECT
public:
    PMReferenceColumnWidget( PMReferenceColumn *pReferenceColumn, PMTableBase *pChildTable, QWidget *pwidgetParent );

protected slots:
    virtual void slotChanged( int );

protected:
    PMReferenceColumn *pReferenceColumn;
};

/*!
 * \brief
 * 
 * \author pharvey (5/21/20)
 */
class PMReferenceColumnsWidget : public QWidget
{
    Q_OBJECT
public:
    PMReferenceColumnsWidget( PMReference *pReference, QWidget *pwidgetParent );
    virtual ~PMReferenceColumnsWidget();

    virtual void doReInit();

protected:
    PMReference *   pReference;
    QTableWidget *  pTableWidget;

    virtual void    setKey( PMKey *pKey );

    virtual PMKey * getKey() { return pReference->getKey(); }

    virtual void doInit();
    virtual void doLoad();

    void doSaveState();
    void doLoadState();
};

#endif 


