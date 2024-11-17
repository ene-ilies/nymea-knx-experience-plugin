#ifndef KNXFRAME_H
#define KNXFRAME_H

#include <QObject>
#include <QKnxLinkLayerFrame>
#include <QKnxAddress>
#include <QKnx1Bit>
#include <QKnx8BitUnsignedValue>
#include <QKnx3BitControlled>
#include <QKnxByteArray>
#include <loggingcategories.h>

Q_DECLARE_LOGGING_CATEGORY(dcKNXLib)

class KnxFrame: public QObject  {
    Q_OBJECT
    public:
        static QKnxLinkLayerFrame createSwitchRequest(const QKnxAddress &groupAddress, QKnxSwitch::State state);
        static QKnxLinkLayerFrame createPercentageRequest(const QKnxAddress &groupAddress, quint8 value);
        static QKnxLinkLayerFrame createDimmingControlRequest(const QKnxAddress &groupAddress, QKnxControlDimming::Control control);
        static QKnxLinkLayerFrame createReadRequest(const QKnxAddress &groupAddress);

    private:
        static QKnxLinkLayerFrame createWriteGroupFrame(const QKnxAddress &groupdAddress, const QKnxByteArray &bytes);
};

#endif // KNXFRAME_H