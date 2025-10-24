#ifndef H_LMIdentifiersWidget
#define H_LMIdentifiersWidget

#include <AWDataTable.h>

#include "LMEntityNormal.h"

/*!
 * \class LMIdentifiersWidget
 * \brief Add/remove Identifiers.
 *  
 * An Identifier uses 0-n Attributes. 
 * A useful Identifier uses a least one. 
 *   
 * \author pharvey (12/29/19)
 */
class LMIdentifiersWidget : public AWDataTable
{
    Q_OBJECT
public:
    LMIdentifiersWidget( LMEntityNormal *pEntity, QWidget *pParent );
    virtual ~LMIdentifiersWidget();

public slots:
    virtual void slotNew();

protected:
    LMEntityNormal *pEntity;

    virtual void doRowGet( int nRow, AWObject *pObject );
    virtual void doRowSet( int nRow, AWObject *pObject );
};


#endif 


