#ifndef H_LMDataTypeWidget
#define H_LMDataTypeWidget

#include <AWObject.h>

class LMModel;
class LMDomain;
class LMDataTypeArgsWidget;

/*!
 * \class LMDataTypeWidget 
 * \brief An editor for specifying the data type. 
 *  
 * This can work with, or without, a domain selection. Just use the appropriate constructor. 
 *  
 * This is used by; \sa LMDomain, \sa LMDataItem, and \sa LMAttribute. 
 *  
 * This is suported by; \sa LMDataTypeArgsWidget. 
 *  
 * \author pharvey (12/29/19)
 */
class LMDataTypeWidget : public QWidget
{
    Q_OBJECT
public:
    LMDataTypeWidget( QWidget *pParent, LMModel *pModel, LMDomain *pDomain, const QString &stringDataType, const QString &stringLength, const QString &stringPrecision1, const QString &stringPrecision2, const QString &stringScale );
    LMDataTypeWidget( QWidget *pParent, LMModel *pModel, const QString &stringDataType, const QString &stringLength, const QString &stringPrecision1, const QString &stringPrecision2, const QString &stringScale );
    virtual ~LMDataTypeWidget();

signals:
    void signalDomainChanged( LMDomain *pDomain );
    void signalDataTypeChanged( const QString &s );
    void signalLengthChanged( const QString &s );
    void signalPrecision1Changed( const QString &s );
    void signalPrecision2Changed( const QString &s );
    void signalScaleChanged( const QString &s );

protected slots:
    virtual void slotDomainChanged( const QString &stringDomain );
    virtual void slotDataTypeChanged( const QString &stringDataType );

protected:
    LMModel *               pModel;
    QComboBox *             pComboBoxDomain;    
    QComboBox *             pComboBoxDataType;  
    QTextEdit *             pTextEditHelp;      
    LMDataTypeArgsWidget *  pDataTypeArgs;
    QList<ADObject*>        listDomains;          
};

#endif 


