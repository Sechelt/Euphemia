#ifndef H_PMColumnChecksWidget
#define H_PMColumnChecksWidget

#include "PMColumn.h"

class PMColumnChecksWidget : public QWidget
{
    Q_OBJECT
public:
    PMColumnChecksWidget( PMColumn *pColumn, QWidget *pwidgetParent );
    virtual ~PMColumnChecksWidget();

protected slots:
    virtual void slotMandatory( Qt::CheckState n )             { pColumn->setMandatory( n );   }
    virtual void slotHidden( Qt::CheckState n )                { pColumn->setHidden( n );      }
    virtual void slotMinimum( const QString &s )    { pColumn->setMinimum( s );   }
    virtual void slotMaximum( const QString &s )    { pColumn->setMaximum( s );   }
    virtual void slotDefault( const QString &s )    { pColumn->setDefault( s );   }
    virtual void slotFormat( const QString &s )     { pColumn->setFormat( s );    }  
    virtual void slotUnit( const QString &s )       { pColumn->setUnit( s );      }
    virtual void slotForceUpper( Qt::CheckState n )            { pColumn->setForceUpperCase( n );   }
    virtual void slotForceLower( Qt::CheckState n )            { pColumn->setForceLowerCase( n );   }
    virtual void slotRestrictModify( Qt::CheckState n )        { pColumn->setRestrictModify( n );   }

protected:
    PMColumn *          pColumn;             
    QTableWidget *      pTableWidget;                 

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


