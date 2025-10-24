#ifndef H_LMIdentifierOptionsWidget
#define H_LMIdentifierOptionsWidget

#include "LMIdentifier.h"

/*!
 * \class LMIdentifierOptionsWidget 
 * \brief For setting options. 
 *  
 * The identifier can be set as primary (or not). 
 * 
 * \author pharvey (12/29/19)
 */
class LMIdentifierOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    LMIdentifierOptionsWidget( LMIdentifier *pIdentifier, QWidget *pParent );
    virtual ~LMIdentifierOptionsWidget();

public slots:
    virtual void slotPrimary( int n ) { pIdentifier->setPrimary( n ); }

protected:
    LMIdentifier *pIdentifier;
    QTableWidget *pTableWidget;                 

    QCheckBox *pCheckBoxPrimary;
};

#endif 


