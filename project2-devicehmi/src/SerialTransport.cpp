#include "SerialTransport.h"

#include <QSerialPortInfo>
#include <QStringList>

namespace hmi {

SerialTransport::SerialTransport(QObject* parent) : ITransport(parent) {
    connect(&m_port, &QSerialPort::readyRead, this, &SerialTransport::onReadyRead);
    connect(&m_port, &QSerialPort::errorOccurred, this, &SerialTransport::onErrorOccurred);
}

SerialTransport::~SerialTransport() {
    if (m_port.isOpen()) {
        m_port.close();
    }
}

QStringList SerialTransport::availablePortNames() {
    QStringList names;
    const auto  ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& info : ports) {
        // 存进下拉框的显示文本里带上描述，方便识别 USB 转串口芯片
        QString label = info.portName();
        if (!info.description().isEmpty()) {
            label += QStringLiteral("  (") + info.description() + QStringLiteral(")");
        }
        names.append(label);
    }
    return names;
}

bool SerialTransport::open() {
    if (m_port.isOpen()) {
        return true;
    }
    if (m_config.portName.isEmpty()) {
        emit errorOccurred(QStringLiteral("没有选择串口"));
        return false;
    }

    m_reportedError = false;
    m_port.setPortName(m_config.portName);
    m_port.setBaudRate(m_config.baudRate);
    m_port.setDataBits(m_config.dataBits);
    m_port.setParity(m_config.parity);
    m_port.setStopBits(m_config.stopBits);
    m_port.setFlowControl(m_config.flowControl);

    if (!m_port.open(QIODevice::ReadWrite)) {
        emit errorOccurred(QStringLiteral("打开 %1 失败：%2")
                               .arg(m_config.portName, m_port.errorString()));
        return false;
    }

    m_port.clearError();
    emit stateChanged(true);
    return true;
}

void SerialTransport::close() {
    if (!m_port.isOpen()) {
        return;
    }
    m_port.close();
    emit stateChanged(false);
}

bool SerialTransport::isOpen() const {
    return m_port.isOpen();
}

qint64 SerialTransport::write(const QByteArray& data) {
    if (!m_port.isOpen()) {
        emit errorOccurred(QStringLiteral("串口未打开，无法发送"));
        return -1;
    }
    const qint64 written = m_port.write(data);
    m_port.flush();
    return written;
}

QString SerialTransport::describe() const {
    return QStringLiteral("串口 %1 @ %2").arg(m_config.portName).arg(m_config.baudRate);
}

void SerialTransport::onReadyRead() {
    const QByteArray chunk = m_port.readAll();
    if (!chunk.isEmpty()) {
        emit bytesReceived(chunk);
    }
}

void SerialTransport::onErrorOccurred(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) {
        return;
    }
    // 拔掉 USB 转串口时会连续触发多次同一个错误，去重避免刷屏
    if (m_reportedError) {
        return;
    }
    m_reportedError = true;
    emit errorOccurred(QStringLiteral("串口错误：%1").arg(m_port.errorString()));

    if (error == QSerialPort::ResourceError || error == QSerialPort::DeviceNotFoundError) {
        close();
    }
}

}  // namespace hmi
