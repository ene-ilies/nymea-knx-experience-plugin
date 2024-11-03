/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
*
* GNU Lesser General Public License Usage
* This project may be redistributed and/or modified under the
* terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; version 3. This project is distributed in the hope that
* it will be useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this project. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "knxtunnel.h"

#include <QDateTime>
#include <QNetworkInterface>

#include <QKnxTpdu>
#include <QKnx1Bit>
#include <QKnxDatapointType>
#include <QKnxLinkLayerFrame>
#include <QKnx8BitUnsignedValue>
#include <QKnxLinkLayerFrameBuilder>
#include <QKnxNetIpSecureConfiguration>

Q_LOGGING_CATEGORY(dcKnxTunnelTraffic, "KnxTunnelTraffic")

KnxTunnel::KnxTunnel(const QHostAddress &remoteAddress, QObject *parent) :
    QObject(parent),
    m_remoteAddress(remoteAddress)
{
    qCDebug(dcKnxTunnelTraffic()) << "New knx tunnel";
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);
    m_timer->setInterval(5000);
    connect(m_timer, &QTimer::timeout, this, &KnxTunnel::onTimeout);

    m_queueTimer = new QTimer(this);
    m_queueTimer->setSingleShot(false);
    m_queueTimer->setInterval(500);
    connect(m_queueTimer, &QTimer::timeout, this, &KnxTunnel::onQueueTimeout);    
    
    QKnxAddress interfaceAddress = { QKnxAddress::Type::Individual, "1.1.255" };
    QKnxNetIpSecureConfiguration secureConfiguration = QKnxNetIpSecureConfiguration::fromKeyring(QKnxNetIpSecureConfiguration::Type::Tunneling, interfaceAddress, QString("/etc/nymea/House.knxkeys"), QString("8d929dff-23b8-455d-840d-698b7e152e99").toUtf8(), true);
    m_tunnel = new QKnxNetIpTunnel(this);
    m_tunnel->setLocalPort(0);
    m_tunnel->setHeartbeatTimeout(1000);
    m_tunnel->setSecureConfiguration(secureConfiguration);

    connect(m_tunnel, &QKnxNetIpTunnel::frameReceived, this, &KnxTunnel::onTunnelFrameReceived);
    connect(m_tunnel, &QKnxNetIpTunnel::connected, this, &KnxTunnel::onTunnelConnected);
    connect(m_tunnel, &QKnxNetIpTunnel::disconnected, this, &KnxTunnel::onTunnelDisconnected);
    connect(m_tunnel, &QKnxNetIpTunnel::stateChanged, this, &KnxTunnel::onTunnelStateChanged);
}

QHostAddress KnxTunnel::remoteAddress() const
{
    return m_remoteAddress;
}

void KnxTunnel::setRemoteAddress(const QHostAddress &remoteAddress)
{
    m_remoteAddress = remoteAddress;
}

bool KnxTunnel::connected() const
{
    return m_tunnel->state() == QKnxNetIpTunnel::State::Connected;
}

bool KnxTunnel::connectTunnel()
{
    // Get the local address for this tunnel remote address
    QHostAddress localAddress = getLocalAddress(m_remoteAddress);
    if (localAddress.isNull()) {
        qCWarning(dcKnxTunnelTraffic()) << "Could connect to" << m_remoteAddress.toString() << ". There is no local interface for this server address. Make sure this device is connected to the correct network.";
        return false;
    }

    m_tunnel->setLocalAddress(localAddress);
    qCDebug(dcKnxTunnelTraffic()) << "Connecting tunnel to" << m_remoteAddress.toString() << "using" << m_tunnel->localAddress();
    m_tunnel->connectToHostEncrypted(m_remoteAddress, m_port);
    return true;
}

void KnxTunnel::disconnectTunnel()
{
    qCDebug(dcKnxTunnelTraffic()) << "Disonnecting tunnel from" << m_remoteAddress.toString();
    m_tunnel->disconnectFromHost();
}

void KnxTunnel::sendKnxDpdSwitchFrame(const QKnxAddress &knxAddress, bool power)
{
    qCDebug(dcKnxTunnelTraffic()) << "Send DpdSwitch" << knxAddress.toString() << (power ? "ON" : "OFF");

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueWrite);
    tpdu.setData(QKnxSwitch(power ? QKnxSwitch::State::On : QKnxSwitch::State::Off).bytes());

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::sendKnxDpdUpDownFrame(const QKnxAddress &knxAddress, bool status)
{
    qCDebug(dcKnxTunnelTraffic()) << "Send DpdUpDown" << knxAddress.toString() << (status ? "UP" : "DOWN");

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueWrite);
    tpdu.setData(QKnxUpDown(status ? QKnxUpDown::State::Up : QKnxUpDown::State::Down).bytes());

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::sendKnxDpdStepFrame(const QKnxAddress &knxAddress, bool status)
{
    qCDebug(dcKnxTunnelTraffic()) << "Send DpdStep" << knxAddress.toString() << (status ? "INCREASE" : "DECREASE");

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueWrite);
    tpdu.setData(QKnxStep(status ? QKnxStep::State::Increase : QKnxStep::State::Decrease).bytes());

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::sendKnxDpdScalingFrame(const QKnxAddress &knxAddress, int scale)
{
    qCDebug(dcKnxTunnelTraffic()) << "Send DpdScaling" << knxAddress.toString() << scale;

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueWrite);
    tpdu.setData(QKnxScaling(scale).bytes());

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::readKnxGroupValue(const QKnxAddress &knxAddress)
{
    qCDebug(dcKnxTunnelTraffic()) << "Read knx group value from group address" << knxAddress.toString();

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueRead);

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMessageCode(QKnxLinkLayerFrame::MessageCode::DataRequest)
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::readKnxDpdSwitchState(const QKnxAddress &knxAddress)
{
    qCDebug(dcKnxTunnelTraffic()) << "Read switch group value" << knxAddress.toString();

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueRead);

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMessageCode(QKnxLinkLayerFrame::MessageCode::DataRequest)
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::readKnxDpdScalingState(const QKnxAddress &knxAddress)
{
    qCDebug(dcKnxTunnelTraffic()) << "Read scaling group value" << knxAddress.toString();

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueRead);

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMessageCode(QKnxLinkLayerFrame::MessageCode::DataRequest)
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::readKnxDpdTemperatureSensor(const QKnxAddress &knxAddress)
{
    qCDebug(dcKnxTunnelTraffic()) << "Read temperature sensor group value" << knxAddress.toString();

    QKnxTpdu tpdu;
    tpdu.setTransportControlField(QKnxTpdu::TransportControlField::DataGroup);
    tpdu.setApplicationControlField(QKnxTpdu::ApplicationControlField::GroupValueRead);

    QKnxLinkLayerFrame frame = QKnxLinkLayerFrame::builder()
            .setMessageCode(QKnxLinkLayerFrame::MessageCode::DataRequest)
            .setMedium(QKnx::MediumType::NetIP)
            .setDestinationAddress(knxAddress)
            .setTpdu(tpdu)
            .createFrame();

    requestSendFrame(frame);
}

void KnxTunnel::printFrame(const QKnxLinkLayerFrame &frame)
{
    qCDebug(dcKnxTunnelTraffic()) << "Frame: (" << frame.sourceAddress().toString() << "-->" << frame.destinationAddress().toString() << ")" << frame;
    qCDebug(dcKnxTunnelTraffic()) << "    Message code:" << frame.messageCode();
    qCDebug(dcKnxTunnelTraffic()) << "    MediumType" << frame.mediumType();
    qCDebug(dcKnxTunnelTraffic()) << "    Control field:" << frame.controlField();
    qCDebug(dcKnxTunnelTraffic()) << "    Extended control field:" << frame.extendedControlField();
    qCDebug(dcKnxTunnelTraffic()) << "    Additional infos:" << frame.additionalInfos();
    qCDebug(dcKnxTunnelTraffic()) << "    Bytes:" << frame.bytes().toHex().toByteArray();
    qCDebug(dcKnxTunnelTraffic()) << "    TPDU:" << frame.tpdu() << "Size:" << frame.tpdu().size();
    qCDebug(dcKnxTunnelTraffic()) << "       " << frame.tpdu().transportControlField();
    qCDebug(dcKnxTunnelTraffic()) << "       " << frame.tpdu().applicationControlField();
    qCDebug(dcKnxTunnelTraffic()) << "       " << frame.tpdu().mediumType();
    qCDebug(dcKnxTunnelTraffic()) << "        Sequence number:" << frame.tpdu().sequenceNumber();
    qCDebug(dcKnxTunnelTraffic()) << "        Data:" << frame.tpdu().data().toHex().toByteArray();
}

void KnxTunnel::requestSendFrame(const QKnxLinkLayerFrame &frame)
{
    // If the timer is running, enqueue
    if (m_queueTimer->isActive()) {
        m_sendingQueue.enqueue(frame);
        return;
    }

    // No queue, timer not running, lets send the message directly
    sendFrame(frame);
    m_queueTimer->start();
}

void KnxTunnel::sendFrame(const QKnxLinkLayerFrame &frame)
{
    if (!connected()) {
        qCWarning(dcKnxTunnelTraffic()) << "Cannot send frame on tunnel" << m_remoteAddress.toString() << " because the tunnel is not connected.";
        return;
    }

    qCDebug(dcKnxTunnelTraffic()) << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "--> Sending frame" << frame << frame.destinationAddress().toString() << frame.tpdu().data().toHex().toByteArray();
    printFrame(frame);
    m_tunnel->sendFrame(frame);
}

QHostAddress KnxTunnel::getLocalAddress(const QHostAddress &remoteAddress)
{
    QHostAddress localAddress;
    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces()) {
        qCDebug(dcKnxTunnelTraffic()) << "Network interface" << interface.name() << interface.type();
        foreach (const QNetworkAddressEntry &addressEntry, interface.addressEntries()) {
            if (addressEntry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                qCDebug(dcKnxTunnelTraffic()) << "    - " << addressEntry.ip().toString() << addressEntry.netmask().toString();
                if (remoteAddress.isInSubnet(addressEntry.ip(), addressEntry.prefixLength())) {
                    qCDebug(dcKnxTunnelTraffic()) << "Found local interface address for" << remoteAddress.toString() << "-->" << addressEntry.ip().toString() << interface.name();
                    localAddress = addressEntry.ip();
                }
            }
        }
    }

    return localAddress;
}

void KnxTunnel::onTimeout()
{
    qCDebug(dcKnxTunnelTraffic()) << "Tunnel reconnection timeout.";
    connectTunnel();
}

void KnxTunnel::onQueueTimeout()
{
    // If queue is empty, we are don for now.
    if (m_sendingQueue.isEmpty()) {
        m_queueTimer->stop();
        return;
    }

    // Send next message
    QKnxLinkLayerFrame frame = m_sendingQueue.dequeue();
    sendFrame(frame);
    m_queueTimer->start();
}

void KnxTunnel::onTunnelConnected()
{
    qCDebug(dcKnxTunnelTraffic()) << "Tunnel connected.";
    emit connectedChanged();

    // Stop the reconnect timer
    m_timer->stop();
}

void KnxTunnel::onTunnelDisconnected()
{
    qCDebug(dcKnxTunnelTraffic()) << "Tunnel disconnected.";
    emit connectedChanged();

    // Start the reconnect timer
    m_timer->start();
}

void KnxTunnel::onTunnelStateChanged(QKnxNetIpEndpointConnection::State state)
{
    qCDebug(dcKnxTunnelTraffic()) << "Tunnel state changed" << state;
}

void KnxTunnel::onTunnelFrameReceived(const QKnxLinkLayerFrame &frame)
{
    qCDebug(dcKnxTunnelTraffic()) << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "<-- Tunnel frame received" << frame << frame.destinationAddress().toString() << frame.tpdu().data().toHex().toByteArray();
    printFrame(frame);
    emit frameReceived(frame);
}

