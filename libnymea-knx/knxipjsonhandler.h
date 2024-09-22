#ifndef KNXIPJSONHANDLER_H
#define KNXIPJSONHANDLER_H

#include <QObject>
#include "jsonrpc/jsonhandler.h"

class KNXIPJsonHandler : public JsonHandler
{
    Q_OBJECT
public:
    explicit KNXIPJsonHandler(QObject *parent = nullptr);
    QString name() const override;
};

#endif // KNXIPJSONHANDLER_H