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

#ifndef KNXTUNNEL_H
#define KNXTUNNEL_H

#include <QTimer>
#include <QObject>

#include <QQueue>
#include <QHostAddress>
#include <QKnxNetIpTunnel>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dcKnxTunnelTraffic)

class KnxTunnel : public QObject
{
    Q_OBJECT
public:
    explicit KnxTunnel(const QHostAddress &remoteAddress, QObject *parent = nullptr);

    QHostAddress remoteAddress() const;
    void setRemoteAddress(const QHostAddress &remoteAddress);

    bool connected() const;

    bool connectTunnel();
    void disconnectTunnel();

    void sendKnxDpdSwitchFrame(const QKnxAddress &knxAddress, bool power);
    void sendKnxDpdUpDownFrame(const QKnxAddress &knxAddress, bool status);
    void sendKnxDpdStepFrame(const QKnxAddress &knxAddress, bool status);
    void sendKnxDpdScalingFrame(const QKnxAddress &knxAddress, int scale);

    void readKnxGroupValue(const QKnxAddress &knxAddress);

    void readKnxDpdSwitchState(const QKnxAddress &knxAddress);
    void readKnxDpdScalingState(const QKnxAddress &knxAddress);
    void readKnxDpdTemperatureSensor(const QKnxAddress &knxAddress);

    void requestSendFrame(const QKnxLinkLayerFrame &frame);
private:
    QHostAddress m_localAddress;
    QHostAddress m_remoteAddress;
    quint16 m_port = 3671;

    QTimer *m_timer = nullptr;
    QTimer *m_queueTimer = nullptr;
    QKnxNetIpTunnel *m_tunnel = nullptr;

    QQueue<QKnxLinkLayerFrame> m_sendingQueue;

    // Helper
    static void printFrame(const QKnxLinkLayerFrame &frame);
    void sendFrame(const QKnxLinkLayerFrame &frame);
    QHostAddress getLocalAddress(const QHostAddress &remoteAddress);

signals:
    void connectedChanged();
    void frameReceived(const QKnxLinkLayerFrame &frame);

private slots:
    void onTimeout();
    void onQueueTimeout();

    void onTunnelConnected();
    void onTunnelDisconnected();
    void onTunnelStateChanged(QKnxNetIpEndpointConnection::State state);
    void onTunnelFrameReceived(const QKnxLinkLayerFrame &frame);
};

#endif // KNXTUNNEL_H
