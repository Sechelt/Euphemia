#ifndef H_ADValidation
#define H_ADValidation

#include <CBD.h>

/*!
 * \class ADValidation 
 * \brief A namespace for validation with some static helper methods.
 *  
 * \author pharvey (10/14/18)
 */
class ADValidation
{
public:
    // keep in sync with g_apszValidationStrictness
    enum Strictness
    {
        Low = 0,
        Medium,
        High
    };

    // keep in sync with g_apszValidationSeverity
    enum Severity
    {
        Ignore = 0, 
        Information,
        Warning,
        Error
    };

    // These static methods perform lookups in static data in ADValidation.cpp.
    // Use these for convenience.
    static QString getStrictness( ADValidation::Strictness nStrictness );
    static ADValidation::Strictness getStrictness( const QString stringStrictness );
    static QStringList getStrictnessStrings();
    static QComboBox* getEditorStrictness( QWidget *pwidgetParent, ADValidation::Strictness nStrictness = ADValidation::High );

    static QString getSeverity( ADValidation::Severity nSeverity );
    static ADValidation::Severity getSeverity( const QString stringSeverity );
    static QPixmap getPixmapSeverity( ADValidation::Severity nSeverity );
    static QWidget* getEditorSeverity( QWidget *pwidgetParent, ADValidation::Severity nSeverity = ADValidation::Ignore );
};

/*!
 * \brief Use the validation Name to lookup the Severity. 
 *  
 * The Severity can be used during a validation to determine what message, if any, to emit 
 * when a validation failure is found.
 *  
 * This is loaded by calling static methods in all of the classes that will be involved 
 * in the validation request. 
 *  
 * A configuration can be applied to alter the default Severity for each. 
 *  
 * Example data; 
 *  
 * { "Missing code.", Warning   } 
 * { "Missing name.", Error     } 
 *  
 * \author pharvey (10/14/18)
 */
typedef QMap<QString,ADValidation::Severity> ADValidationList;

/*!
 * \brief Use the Strictness to lookup a list of validations.
 *  
 * The validation strictness can be; "Low", "Medium", or "High" and 
 * is used as the key. 
 *  
 * The validations are \sa ADValidationList  
 *  
 * Example data; 
 *  
 * { "Dline", { Low,   { "Missing name", Ignore    }, 
 *                     { "Missing code", Ignore    } }
 *            { Medium,{ "Missing name", Warning   },
 *                     { "Missing code", Warning   } }  
 *            { High,  { "Missing name", Error     },
 *                     { "Missing code", Warning   } }
 *  
 * Or to look at it another way; 
 *  
 * ClassName <<-- Strictness <<-- Validation <-- Severity
 *    0-n              3              0-n           1
 *  
 * \author pharvey (10/14/18)
 */
typedef QMap<ADValidation::Strictness,ADValidationList> ADValidationStrictness;

/*!
 * \brief Use a class name to lookup the validations data. 
 *  
 * This provides a 'namespace' for all of a class's validation configuration data. 
 *  
 * This is the entry point to all validation configuration data. 
 *  
 * The validation configuration gets loaded with the defaults via static methods in 
 * the ADObject based classes. This configuration can then be customized 
 * to meet User needs/preferences. 
 *  
 * Example data; 
 *  
 * { "ADObject", { Low,   { "Missing name", Ignore    } } }
 * { "ADObject", { Medium,{ "Missing name", Warning   } } } 
 * { "ADObject", { High,  { "Missing name", Error     } } }
 * 
 * \author pharvey (11/28/19)
 */
typedef QMap<QString,ADValidationStrictness> ADValidationDatabase;

#endif 


