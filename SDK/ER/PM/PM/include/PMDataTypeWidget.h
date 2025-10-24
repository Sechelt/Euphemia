#ifndef H_PMDataTypeWidget
#define H_PMDataTypeWidget

#include <AWObject.h>

class PMModel;
class PMDomain;
class PMColumn;
class PMDataTypeArgsWidget;

/*!
 * \class PMDataTypeWidget 
 * \brief An editor for specifying the data type. 
 *  
 * This can work with, or without, a domain selection. Just use the appropriate constructor. 
 *  
 * This is used by; \sa PMDomain, \sa PMDataItem, and \sa PMAttribute. 
 *  
 * This is suported by; \sa PMDataTypeArgsWidget. 
 *  
 * \author pharvey (12/29/19)
 */
class PMDataTypeWidget : public QWidget
{
    Q_OBJECT
public:
    PMDataTypeWidget( PMDomain *pDomain, QWidget *pParent );
    PMDataTypeWidget( PMColumn *pColumn, QWidget *pParent );
    virtual ~PMDataTypeWidget();

protected slots:
    virtual void slotDomainChanged( const QString &stringDomain );
    virtual void slotDataTypeChanged( const QString &stringDataType );

protected:
    // we are either working with a domain or a column (the column may also be using a domain)
    PMColumn *  pColumn;  // if set - we are working with this column
    PMDomain *  pDomain;  // if set - we are working with this domain

    QComboBox *             pComboBoxDomain;    
    QComboBox *             pComboBoxDataType;  
    QTextEdit *             pTextEditHelp;      
    PMDataTypeArgsWidget *  pDataTypeArgs;
    QList<ADObject*>        listDomains;          
};

#endif 


