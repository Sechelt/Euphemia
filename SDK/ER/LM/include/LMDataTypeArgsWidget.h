#ifndef H_LMDataTypeArgsWidget
#define H_LMDataTypeArgsWidget

#include "LMDataItem.h"

/*!
 * \class LMDataTypeArgsWidget 
 * \brief An editor for specifying data type args. 
 *  
 * This will parse the given syntax and provide the appropriate widgets to get args. 
 *  
 * The Length etc that are provided are used to set default values. 
 *  
 * Created to support; \sa LMDataTypeWidget 
 * 
 * \author pharvey (12/29/19)
 */
class LMDataTypeArgsWidget : public QWidget
{
    Q_OBJECT
public:
    LMDataTypeArgsWidget( QWidget *pParent, const QString &stringLength, const QString &stringPrecision1, const QString &stringPrecision2, const QString &stringScale );
    virtual ~LMDataTypeArgsWidget();

signals:
    void signalLengthChanged( const QString &s );
    void signalPrecision1Changed( const QString &s );
    void signalPrecision2Changed( const QString &s );
    void signalScaleChanged( const QString &s );

public slots:
    virtual void slotDataTypeSyntax( const QString &stringDataTypeSyntax );

protected:                          
    QVBoxLayout *   pLayout;        
    QGroupBox *     pEditor;  
                    
    QString stringLength;
    QString stringPrecision1;
    QString stringPrecision2;
    QString stringScale;
                    
    QLineEdit * pLength;    
    QLineEdit * pPrecision1;
    QLineEdit * pPrecision2;
    QLineEdit * pScale;     
};

#endif 


