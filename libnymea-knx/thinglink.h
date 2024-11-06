#ifndef THINGLINK_H
#define THINGLINK_H

#include "QKnxLinkLayerFrame"

class ThingLink: public QObject {
    Q_OBJECT
public:
    virtual ~ThingLink();
public slots:
    void connected();
    void disconnected();
    void frameReceived(const QKnxLinkLayerFrame &frame);
signals:
    void frameReceivedEvent(const QKnxLinkLayerFrame &frame);
    void connectedEvent();
    void disconnectedEvent();
};

#endif //THINGLINK_H
