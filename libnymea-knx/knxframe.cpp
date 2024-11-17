#include <knxframe.h>
#include <QKnxLinkLayerFrameBuilder>

QKnxLinkLayerFrame KnxFrame::createSwitchRequest(const QKnxAddress &groupAddress, QKnxSwitch::State state) {
    qCDebug(dcKNXLib()) << "Creating switch request for group: " << groupAddress.toString() << " with value: " << state;
    return KnxFrame::createWriteGroupFrame(groupAddress, QKnxSwitch(state).bytes());
}

QKnxLinkLayerFrame KnxFrame::createPercentageRequest(const QKnxAddress &groupAddress, quint8 value) {
    qCDebug(dcKNXLib()) << "Creating percentage request for group: " << groupAddress.toString() << " with value: " << value;
    return KnxFrame::createWriteGroupFrame(groupAddress, QKnxPercentU8((double)value).bytes());
}

QKnxLinkLayerFrame KnxFrame::createDimmingControlRequest(const QKnxAddress &groupAddress, QKnxControlDimming::Control control) {
    qCDebug(dcKNXLib()) << "Creating dimming control request for group: " << groupAddress.toString() << " with value: " << control;
    return KnxFrame::createWriteGroupFrame(groupAddress, QKnxControlDimming(control, QKnxControlDimming::NumberOfIntervals::Break).bytes());
}

QKnxLinkLayerFrame KnxFrame::createReadRequest(const QKnxAddress &groupAddress) {
    qCDebug(dcKNXLib()) << "Creating read group frame for group:" << groupAddress.toString();

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueRead);

    return QKnxLinkLayerFrame::builder()
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(groupAddress)
            .setTpdu(tpdu)
            .createFrame();
}

QKnxLinkLayerFrame KnxFrame::createWriteGroupFrame(const QKnxAddress &groupAddress, const QKnxByteArray &bytes) {
    qCDebug(dcKNXLib()) << "Creating write group frame for group:" << groupAddress.toString();

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueWrite);
    tpdu.setData(bytes);

    return QKnxLinkLayerFrame::builder()
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(groupAddress)
            .setTpdu(tpdu)
            .createFrame();
}
