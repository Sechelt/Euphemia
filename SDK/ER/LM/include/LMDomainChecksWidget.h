#ifndef H_LMDomainChecksWidget
#define H_LMDomainChecksWidget

#include "LMDomain.h"

class LMDomainChecksWidget : public QWidget
{
    Q_OBJECT
public:
    LMDomainChecksWidget( LMDomain *pDomain, QWidget *pwidgetParent );
    virtual ~LMDomainChecksWidget();

protected slots:
    virtual void slotMinimum( const QString &s )                  { pDomain->setMinimum( s );   }
    virtual void slotMaximum( const QString &s )                  { pDomain->setMaximum( s );   }
    virtual void slotDefault( const QString &s )                  { pDomain->setDefault( s );   }
    virtual void slotFormat( const QString &s )                   { pDomain->setFormat( s );     }  
    virtual void slotUnit( const QString &s )                     { pDomain->setUnit( s );         }
    virtual void slotForceUpper( Qt::CheckState n )        { pDomain->setForceUpperCase( n );   }
    virtual void slotForceLower( Qt::CheckState n )        { pDomain->setForceLowerCase( n );   }
    virtual void slotRestrictModify( Qt::CheckState n )    { pDomain->setRestrictModify( n );   }

protected:
    LMDomain *      pDomain;                
    QTableWidget *  pTableWidget;
    
    QLineEdit *     plineeditMinimum;       
    QLineEdit *     plineeditMaximum;       
    QLineEdit *     plineeditDefault;       
    QLineEdit *     plineeditFormat;        
    QLineEdit *     plineeditUnit;          
    QCheckBox *     pcheckboxForceUpperCase;
    QCheckBox *     pcheckboxForceLowerCase;
    QCheckBox *     pcheckboxRestrictModify;
};

#endif 


