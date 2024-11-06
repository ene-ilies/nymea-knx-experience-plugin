#include "thinglink.h"

void ThingLink::connected() {
    emit connectedEvent();
}

void ThingLink::disconnected() {
    emit disconnectedEvent();
}

void ThingLink::frameReceived(const QKnxLinkLayerFrame &frame) {
    emit frameReceivedEvent(frame);
}
