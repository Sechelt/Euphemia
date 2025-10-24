#ifndef H_PMDataTypeArgsWidget
#define H_PMDataTypeArgsWidget

#include "PMDomain.h"
#include "PMColumn.h"

/*!
 * \class PMDataTypeArgsWidget 
 * \brief An editor for specifying data type args. 
 *  
 * This will parse the given syntax and provide the appropriate widgets to get args. 
 *  
 * The Length etc that are provided are used to set default values. 
 *  
 * Created to support; \sa PMDataTypeWidget 
 * 
 * \author pharvey (12/29/19)
 */
class PMDataTypeArgsWidget : public QWidget
{
    Q_OBJECT
public:
    PMDataTypeArgsWidget( PMDomain *pDomain, QWidget *pParent );
    PMDataTypeArgsWidget( PMColumn *pColumn, QWidget *pParent );
    virtual ~PMDataTypeArgsWidget();

    virtual void setDataTypeSyntax( const QVector<QString> &vectorSyntax );

protected slots:
    virtual void slotLengthChanged( const QString &s );
    virtual void slotPrecision1Changed( const QString &s );
    virtual void slotPrecision2Changed( const QString &s );
    virtual void slotScaleChanged( const QString &s );

protected:           
    // we are either working with a domain or a column (the column may also be using a domain)
    PMColumn *pColumn;  // if set - we are working with this column
    PMDomain *pDomain;  // if set - we are working with this domain
                   
    QVBoxLayout *   pLayout;        
    QGroupBox *     pEditor;  
                    
    QLineEdit * pLength;    
    QLineEdit * pPrecision1;
    QLineEdit * pPrecision2;
    QLineEdit * pScale;     
};

#endif 


