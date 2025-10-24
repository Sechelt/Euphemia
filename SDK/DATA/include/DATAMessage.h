#pragma once

#include "DATA.h"

/*! 
 * \brief   An ODBC message.
 * 
 *          An ODBC message is created by the class library and is provided to the application via
 *          the \sa eventMessage() method. This class compliments \sa DATADiagnostic (and the eventDiagnostic()
 *          method) by providing additional diagnostic information. For example; when a request fails due to 
 *          an invalid handle - in such a case no diagnostic information can be found using DATADiagnostic. 
 *
 * \sa      DATAEnvironment
 *          DATAConnection
 *          DATAStatement
 */
class DATAMessage
{
public:
    enum Types 
    { 
        Success = 0,
        Info,       /*!< the application may ignore this message                    */
        Warning,    /*!< the application may want to pay attention to this message  */ 
        Error       /*!< the application should pay attention to this message       */ 
    };

    DATAMessage( Types nType, const QString &stringRequest = QString(), const QString &stringText = QString(), SQLINTEGER nNumeric = 0 );
    DATAMessage();

    Types       getType();
    QString     getRequest();
    QString     getText();
    SQLINTEGER  getNumeric();
    QString     getTypeText();

    bool        isNull() { return bNull; }

protected:
    bool        bNull;
    Types       nType;          /*!< message type \sa Types                                */
    QString     stringRequest;  /*!< usually function ie SQLAllocHandle                    */
    QString     stringText;     /*!< message text                                          */
    SQLINTEGER  nNumeric;       /*!< numeric value dependent upon message                  */
};


