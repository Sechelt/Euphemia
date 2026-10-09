#include "WColorPickerObject.h"

#include <QColor>

WColorPickerObject::WColorPickerObject(QObject *parent) : QObject(parent) 
{
//    qDBusRegisterMetaType<QColor>();
    // Generate a unique token identifier for this request handler
    m_handleToken = QString( "qt_picker_%1" ).arg( QRandomGenerator::global()->bounded( 100000 ) );
}

void WColorPickerObject::startPicking() 
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if ( !bus.isConnected() ) 
    {
        qCritical() << "Cannot connect to D-Bus session bus.";
        return;
    }

    // Step 1: Formulate the expected request object path where the portal will broadcast its reply signal
    QString senderName = bus.baseService().replace( ".", "_" ).replace( ":", "" );
    QString requestPath = QString( "/org/freedesktop/portal/desktop/request/%1/%2" ).arg( senderName, m_handleToken );

    // Dynamic signal mapping: Listen for the Response signal at our generated path
    bus.connect(
        "org.freedesktop.portal.Desktop",
        requestPath,
        "org.freedesktop.portal.Request",
        "Response",
        this,
        SLOT(slotPortalResponse(uint, QVariantMap))
    );

    // Step 2: Establish an interface to the cross-desktop Screenshot Portal
    QDBusInterface portalInterface(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Screenshot",
        bus
    );

    // Pack the options dictionary along with our custom handle token string
    QVariantMap options;
    options.insert("handle_token", m_handleToken);

    // Step 3: Trigger the PickColor request asynchronously
    // Method signature: PickColor(QString parent_window, QVariantMap options) -> Returns ObjectPath
    QDBusReply<QDBusObjectPath> reply = portalInterface.call("PickColor", "", options);

    if ( !reply.isValid() )
    {
        qCritical() << "D-Bus portal call failed:" << reply.error().message();
    } 
}

void WColorPickerObject::slotPortalResponse( uint responseCode, const QVariantMap &results ) 
{
    // responseCode values: 0 = Success, 1 = Cancelled by user, 2 = Dismissed by OS
    if (responseCode != 0) 
    {
        qWarning() << "Color picking was cancelled or dismissed by the user.";
        emit signalColor( QColor() );
        return;
    }

    // Extract the sRGB color structure from the portal's payload dictionary
    if ( !results.contains( "color" ) ) 
    {
        qWarning() << "Results returned [" << results << "] and did not contain 'color'";
        emit signalColor( QColor() );
        return;
    }

    QVariant colorVar = results.value( QStringLiteral( "color" ) );

    // Standard (ddd)
    if ( colorVar.userType() == qMetaTypeId<QDBusArgument>() ) 
    {
        const QDBusArgument arg = colorVar.value<QDBusArgument>();
        double r, g, b;

        arg.beginStructure();
        arg >> r >> g >> b;
        arg.endStructure();

        emit signalColor( QColor::fromRgbF( r, g, b ) );
        return;
    }
    // Legacy Packed u (32-bit uint (0x00RRGGBB))
    else if ( colorVar.canConvert<uint>() ) 
    {
        uint packedColor = colorVar.toUInt();
        int r = (packedColor >> 16) & 0xFF;
        int g = (packedColor >> 8) & 0xFF;
        int b = packedColor & 0xFF;
        
        emit signalColor( QColor::fromRgbF( r, g, b ) );
        return;
    }


    qWarning() << "Failed to convert value to RGB values.";
    emit signalColor( QColor() );
}

