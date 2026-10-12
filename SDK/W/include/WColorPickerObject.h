#ifndef H_WColorPickerObject
#define H_WColorPickerObject

#include "W.h"

#include <QtDBus/QtDBus>

class WColorPickerObject : public QObject 
{
    Q_OBJECT
public:
    explicit WColorPickerObject(QObject *parent = nullptr);
    void startPicking();

signals:
    void signalColorPicked( const QColor & );

private slots:
    void slotPortalResponse( uint responseCode, const QVariantMap &results );

private:
    QString m_handleToken;
};

#endif

