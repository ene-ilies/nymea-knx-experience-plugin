#ifndef THINGLINK_H
#define THINGLINK_H

#include "QKnxLinkLayerFrame"

class ThingLink: public QObject {
    Q_OBJECT
public slots:
    void connected() const;
    void disconnected() const;
    void frameReceived(const QKnxLinkLayerFrame &frame) const;
signals:
    void frameReceivedEvent(const QKnxLinkLayerFrame &frame) const;
    void connectedEvent() const;
    void disconnectedEvent() const;
};

#endif //THINGLINK_H
