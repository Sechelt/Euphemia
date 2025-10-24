#ifndef H_PMReferenceWidget
#define H_PMReferenceWidget

#include <AWDataTable.h>

#include "PMReference.h"

// class PMTableBase;
// class PMColumn;
// class PMKey;

class PMReferenceRulesWidget;
class PMReferenceColumnsWidget;
class PMReferenceColumnsDisplayWidget;
class PMReferenceColumnDisplay;

/*!
 * \brief Configure the join. 
 *  
 * 1. there should be, at least, one PK/AK in the parent table 
 * 2. the column(s) to be used as the FK have been created in the FK table 
 * 
 * \author pharvey (5/20/20)
 */
class PMReferenceWidget : public QWidget
{
    Q_OBJECT
public:
    PMReferenceWidget( PMReference *pReference, QWidget *pwidgetParent );

    virtual void setKey( PMKey *pKey );

protected:
    PMReference *                       pReference;
    QComboBox *                         pKeys;
    PMReferenceRulesWidget *            pRulesWidget;
    PMReferenceColumnsWidget *          pColumnsWidget;
    PMReferenceColumnsDisplayWidget *   pColumnsDisplayWidget;

protected slots:
    void slotKeyChanged( int nIndex );
};

/*!
 * \brief Allows rules to be configured.
 * 
 * \author pharv (3/11/2025)
 */
class PMReferenceRulesWidget : public QWidget
{
    Q_OBJECT
public:
    PMReferenceRulesWidget( PMReference *pReference, QWidget *pwidgetParent );

    void doReInit();

protected slots:
    virtual void slotMatch( const QString &s );
    virtual void slotOnDelete( const QString &s );
    virtual void slotOnUpdate( const QString &s );

protected:
    PMReference *pReference;                

    void doInit();
};

/*!
 * \brief Allows selection of parent table columns to be displayed in a lookup.
 * 
 * \author pharv (3/11/2025)
 */
class PMReferenceColumnsDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    PMReferenceColumnsDisplayWidget( PMReference *pReference, QWidget *pwidgetParent );

    void doReInit();

protected:
    PMReference *               pReference;
    PMReferenceColumnDisplay *  pColumnDisplay;
    QListWidget *               pColumnsUsed;           // from parent table
    QListWidget *               pColumnsAvailable;      // from parent table
    QToolButton *               pToUsed;
    QToolButton *               pToAvailable;

    virtual void doInit();

protected slots:
    void slotToUsed();
    void slotToAvailable();
};

#endif 


