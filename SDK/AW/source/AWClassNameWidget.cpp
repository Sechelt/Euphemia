#include "LibInfo.h"
#include "AWClassNameWidget.h"

AWClassNameWidget::AWClassNameWidget( QWidget *pwidgetParent )
    : QTextEdit( pwidgetParent )
{
    // here we put in some product information as default
    QString stringText = getProductInfo();
    stringText += "<BR><BR>";
    stringText += getCompanyInfo();
    setText( stringText );

    setReadOnly( true );
}

QString AWClassNameWidget::getCompanyInfo()
{
    QString stringText;
    stringText += "<TABLE>";
#if QT_VERSION < 0x060000
    stringText += "<TR><TD>" + QStringLiteral("<img src=':W/CodeByDesign64'>") + "</TD><TD>" + tr("COMPANY") + "</TD></TR>";
#else
    stringText += "<TR><TD>" + QString( "<img src=':W/CodeByDesign64'>" ) + "</TD><TD>" + tr("COMPANY") + "</TD></TR>";
#endif
    stringText += "</TABLE>";
    stringText += "<TABLE border=1>";
    stringText += "<TR><TD>" + tr("Name:") + "</TD><TD>" + CBD_COMPANY + "</TD></TR>";
    stringText += "<TR><TD>" + tr("Domain: ") + "</TD><TD>" + "<A HREF=http://www." + CBD_DOMAIN + "/>" + CBD_DOMAIN + "</TD></TR>";
    stringText += "</TABLE>";
    return stringText;
}

QString AWClassNameWidget::getProductInfo()
{
    QString stringText;
    stringText += "<TABLE>";
#if QT_VERSION < 0x060000
    stringText += "<TR><TD>" + QStringLiteral("<img src=':W/CodeByDesign64'>") + "</TD><TD>" + tr("PRODUCT") + "</TD></TR>";
#else
    stringText += "<TR><TD>" + QString("<img src=':W/CodeByDesign64'>") + "</TD><TD>" + tr("PRODUCT") + "</TD></TR>";
#endif
    stringText += "</TABLE>";
    stringText += "<TABLE border=1>";
    stringText += "<TR><TD>" + tr("Name:") + "</TD><TD>" + SDK_NAME + "</TD></TR>";
    stringText += "<TR><TD>" + tr("Version: ") + "</TD><TD>" + SDK_VER + "</TD></TR>";
    stringText += "<TR><TD>" + tr("Author: ") + "</TD><TD>" + "Peter Harvey" + "</TD></TR>";
    stringText += "<TR><TD>" + tr("License: ") + "</TD><TD>" + CBD_LICENSE + "</TD></TR>";
    stringText += "<TR><TD>" + tr("Copyright: ") + "</TD><TD>" + CBD_COPYRIGHT + "</TD></TR>";
    stringText += "</TABLE>";
    return stringText;
}



