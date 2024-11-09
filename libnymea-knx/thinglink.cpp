#include "thinglink.h"

void ThingLink::connected() const {
    emit connectedEvent();
}

void ThingLink::disconnected() const {
    emit disconnectedEvent();
}

void ThingLink::frameReceived(const QKnxLinkLayerFrame &frame) const {
    emit frameReceivedEvent(frame);
}
