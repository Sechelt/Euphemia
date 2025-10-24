#include "LibInfo.h"
#include "DATAMessage.h"

/*! 
 * \brief   Constructs an DATAMessage.
 *
 *          This copies and stores the given message information.
 *  
 * \param   nType           Input. One of DATAMessage::MessageTypes.
 * \param   stringRequest   Input. Usually function ie "SQLAllocHandle".
 * \param   stringText      Input. Message text. Can be NULL - pref. not.
 * \param   nNumeric        Input. Numeric value dependent upon message.
 *
 */
DATAMessage::DATAMessage( Types nType, const QString &stringRequest, const QString &stringText, SQLINTEGER nNumeric )
{
    bNull                   = false;
    this->nType             = nType;
    this->stringRequest     = stringRequest;
    this->stringText        = stringText;
    this->nNumeric          = nNumeric;
}

// this is needed so we can return a null DATAMessage from getInstallerError
DATAMessage::DATAMessage()
{
    bNull                   = true;
}

/*! 
 * \brief   Message type.
 * 
 *          This can used as the message severity if you want.
 * 
 * \return  Types
 *
 * \sa      getTypeText
 */
DATAMessage::Types DATAMessage::getType() 
{ 
    return nType; 
}

/*! 
 * \brief   Message type text.
 * 
 *          This can be used to format a nicer message.
 * 
 * \return  QString
 *
 * \sa      getType
 */
QString DATAMessage::getTypeText()
{
    switch( nType )
    {
        case Success:
            return "SUCCESS";
        case Info:
            return "INFO";
        case Warning:
            return "WARNING";
        case Error:
            return "ERROR";
    }

    return "UNKNOWN";
}

/*! 
 * \brief   Message request.
 * 
 *          The request which generated this message. This is usually a function or method name.
 * 
 * \return  QString
 */
QString DATAMessage::getRequest() 
{ 
    return stringRequest; 
}

/*! 
 * \brief   Message text.
 * 
 *          The message text. This can usually be presented, as is, but
 *          one may want to enhance it using other bits of info from 
 *          DATAMessage.
 * 
 * \return  QString
 */
QString DATAMessage::getText() 
{ 
    return stringText; 
}

/*! 
 * \brief   Numeric information.
 * 
 *          The value of this is dependent upon the message context.
 * 
 * \return  SQLINTEGER
 * \retval  -1          Usually (but not always) means null.
 */
SQLINTEGER DATAMessage::getNumeric() 
{
    return nNumeric; 
}


