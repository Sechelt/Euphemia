#include "LibInfo.h"
#include "ADValidation.h"
    
/*!
 * \brief Strictness strings 
 *  
 * This *MUST* be kept in sync with \sa ADValidation::Strictness 
 * as they are used to index into this array to get the corresponding 
 * string. 
 * 
 * \author pharvey (10/14/18)
 */
static int const g_nValidationStrictnessCount = 3; // 1-n
static const char * const g_apszValidationStrictness[] =
{
    "Low",
    "Medium",
    "high"
};
    
/*!
 * \brief Severity strings 
 *  
 * This *MUST* be kept in sync with \sa ADValidation::Severity
 * as they are used to index into this array to get the corresponding 
 * string. 
 * 
 * \author pharvey (10/14/18)
 */
static int const g_nValidationSeverityCount = 4; // 1-n
static const char * const g_apszValidationSeverity[] =
{
    "",
    "Information",
    "Warning",
    "Error"
};
    
/*!
 * \brief Pixmaps corresponding to Severity.
 * 
 * \author pharvey (11/28/19)
 */
static int const g_nValidationSeverityPixmapsCount = 4;
static const char * const g_apszValidationSeverityPixmaps[] =
{
    ":AW/Blank",
    ":AW/Information",
    ":AW/Warning",
    ":AW/Error"
};
    
    
QString ADValidation::getStrictness( ADValidation::Strictness nStrictness )
{
    Q_ASSERT( nStrictness < g_nValidationStrictnessCount );
    return g_apszValidationStrictness[nStrictness];
}

ADValidation::Strictness ADValidation::getStrictness( const QString stringStrictness )
{
    int n;
    for ( n = 0; n < g_nValidationStrictnessCount; n++ )
    {
        if ( g_apszValidationStrictness[n] == stringStrictness )
            return (ADValidation::Strictness)n;
    }

    return ADValidation::Low;
}

QComboBox * ADValidation::getEditorStrictness( QWidget *pwidgetParent, ADValidation::Strictness nStrictness )
{
    if ( !pwidgetParent ) return nullptr;

    QComboBox *pComboBox =  new QComboBox( pwidgetParent );

    for ( int n = 0; n < g_nValidationStrictnessCount; n++ )
    {
        pComboBox->addItem( g_apszValidationStrictness[n] );
    }

    pComboBox->setCurrentIndex( nStrictness );

    // caller should connect...
    // connect( pComboBox, SIGNAL(currentTextChanged(const QString&)), SLOT(setValue(const QString&)) );

    return pComboBox;
}

QString ADValidation::getSeverity( ADValidation::Severity nSeverity )
{
    Q_ASSERT( nSeverity < g_nValidationSeverityCount );
    return g_apszValidationSeverity[nSeverity];
}

QStringList ADValidation::getStrictnessStrings()
{
    QStringList stringlist;

    for ( int n = 0; n < g_nValidationStrictnessCount; n++ )
    {
        stringlist += g_apszValidationStrictness[n];
    }

    return stringlist;
}

ADValidation::Severity ADValidation::getSeverity( const QString stringSeverity )
{
    int n;
    for ( n = 0; n < g_nValidationSeverityCount; n++ )
    {
        if ( g_apszValidationSeverity[n] == stringSeverity )
            return (ADValidation::Severity)n;
    }

    return ADValidation::Ignore;
}

QPixmap ADValidation::getPixmapSeverity( ADValidation::Severity nSeverity )
{
    Q_ASSERT( nSeverity < g_nValidationSeverityPixmapsCount );
    return QPixmap( g_apszValidationSeverityPixmaps[nSeverity] );
}
    
QWidget * ADValidation::getEditorSeverity( QWidget *pwidgetParent, ADValidation::Severity nSeverity )
{
    if ( !pwidgetParent )
        return 0;

    QComboBox *pComboBox =  new QComboBox( pwidgetParent );

    for ( int n = 0; n < g_nValidationSeverityCount; n++ )
    {
        pComboBox->addItem( QPixmap( g_apszValidationSeverityPixmaps[n] ), g_apszValidationSeverity[n] );
    }

    pComboBox->setCurrentIndex( nSeverity );

    // caller should connect...
    // connect( pComboBox, SIGNAL(currentTextChanged(const QString&)), SLOT(setValue(const QString&)) );

    return pComboBox;
}


