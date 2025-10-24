#ifndef H_LMDataItemChecksWidget
#define H_LMDataItemChecksWidget

#include "LMDataItem.h"

class LMDataItemChecksWidget : public QWidget
{
    Q_OBJECT
public:
    LMDataItemChecksWidget( LMDataItem *pDataItem, QWidget *pwidgetParent );
    virtual ~LMDataItemChecksWidget();

protected slots:
    virtual void slotMinimum( const QString &s )                  { pDataItem->setMinimum( s );   }
    virtual void slotMaximum( const QString &s )                  { pDataItem->setMaximum( s );   }
    virtual void slotDefault( const QString &s )                  { pDataItem->setDefault( s );   }
    virtual void slotFormat( const QString &s )                   { pDataItem->setFormat( s );     }  
    virtual void slotUnit( const QString &s )                     { pDataItem->setUnit( s );         }
    virtual void slotForceUpper( Qt::CheckState n )        { pDataItem->setForceUpperCase( n );   }
    virtual void slotForceLower( Qt::CheckState n )        { pDataItem->setForceLowerCase( n );   }
    virtual void slotRestrictModify( Qt::CheckState n )    { pDataItem->setRestrictModify( n );   }

protected:
    LMDataItem *        pDataItem;              
    QTableWidget *      pTableWidget;                 
         
    QLineEdit *         plineeditMinimum;       
    QLineEdit *         plineeditMaximum;       
    QLineEdit *         plineeditDefault;       
    QLineEdit *         plineeditFormat;        
    QLineEdit *         plineeditUnit;          
    QCheckBox *         pcheckboxForceUpperCase;
    QCheckBox *         pcheckboxForceLowerCase;
    QCheckBox *         pcheckboxRestrictModify;
};

#endif 


