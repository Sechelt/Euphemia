#ifndef H_LMEntityOptionsWidget
#define H_LMEntityOptionsWidget

#include "LMEntityBase.h"

class LMEntityOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    LMEntityOptionsWidget( LMEntityBase *pEntity, QWidget *pwidgetParent );
    virtual ~LMEntityOptionsWidget();

public slots:
    virtual void slotGenerate( int n )              { pEntity->setGenerate( n );  }
    virtual void slotRecords( const QString &s )    { pEntity->setRecords( s );   }

protected:
    LMEntityBase *  pEntity;
    QTableWidget *  pTableWidget;
    QCheckBox *     pGenerate;
    QLineEdit *     pRecords;
};

#endif 


