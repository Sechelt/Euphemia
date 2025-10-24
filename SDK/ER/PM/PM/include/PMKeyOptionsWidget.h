#ifndef H_PMKeyOptionsWidget
#define H_PMKeyOptionsWidget

#include "PMKey.h"

/*!
 * \class PMKeyOptionsWidget 
 * \brief For setting options. 
 *  
 * The key can be set as primary (or not). 
 * 
 * \author pharvey (5/17/20)
 */
class PMKeyOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    PMKeyOptionsWidget( PMKey *pKey, QWidget *pParent );
    virtual ~PMKeyOptionsWidget();

public slots:
    virtual void slotPrimary( Qt::CheckState n ) { pKey->setPrimary( bool(n) ); }

protected:
    PMKey *pKey;
    QTableWidget *pTableWidget;                 

    QCheckBox *pCheckBoxPrimary;
};

#endif 


