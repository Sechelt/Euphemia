#ifndef H_LMAttributeChecksWidget
#define H_LMAttributeChecksWidget

#include "LMAttribute.h"

class LMAttributeChecksWidget : public QWidget
{
    Q_OBJECT
public:
    LMAttributeChecksWidget( LMAttribute *pAttribute, QWidget *pwidgetParent );
    virtual ~LMAttributeChecksWidget();

protected slots:
    virtual void slotIdentifier( Qt::CheckState n )            { pAttribute->setIdentifier( n );  }
    virtual void slotMandatory( Qt::CheckState n )             { pAttribute->setMandatory( n );   }
    virtual void slotHidden( Qt::CheckState n )                { pAttribute->setHidden( n );      }

    virtual void slotMinimum( const QString &s )    { pAttribute->setMinimum( s );   }
    virtual void slotMaximum( const QString &s )    { pAttribute->setMaximum( s );   }
    virtual void slotDefault( const QString &s )    { pAttribute->setDefault( s );   }
    virtual void slotFormat( const QString &s )     { pAttribute->setFormat( s );    }  
    virtual void slotUnit( const QString &s )       { pAttribute->setUnit( s );      }
    virtual void slotForceUpper( int n )            { pAttribute->setForceUpperCase( n );   }
    virtual void slotForceLower( int n )            { pAttribute->setForceLowerCase( n );   }
    virtual void slotRestrictModify( int n )        { pAttribute->setRestrictModify( n );   }

protected:
    LMAttribute *       pAttribute;             
    QTableWidget *      pTableWidget;                 

    QCheckBox *         pcheckboxIdentifier;    
    QCheckBox *         pcheckboxMandatory;     
    QCheckBox *         pcheckboxHidden;
            
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


