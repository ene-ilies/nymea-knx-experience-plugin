#ifndef THINGLINK_H
#define THINGLINK_H

#include <QKnxLinkLayerFrame>

class ThingLink {
    Q_OBJECT

public slots:
    void frameReceivedHandler(const QKnxLinkLayerFrame &frame);
signals:
    void frameReceived(const QKnxLinkLayerFrame &frame);
};

#endif //THINGLINK_H