#pragma once

#include <QSerialPort>

#include "ITransport.h"

namespace hmi {

/// 串口实现。
///
/// TODO-学（标记-回填法）：下面这几个点卡住了就记进缺口清单，别停下来啃书
///   - 为什么 QSerialPort 要作为成员变量而不是 new 出来的裸指针？（RAII / 父子对象）
///   - readyRead 是"来一个字节就触发"还是"来一批才触发"？（都不是，取决于 OS 缓冲）
///   - 串口读到的数据为什么可能"粘包"？（字节流没有消息边界）
class SerialTransport : public ITransport {
    Q_OBJECT

public:
    explicit SerialTransport(QObject* parent = nullptr);
    ~SerialTransport() override;

    struct Config {
        QString              portName;
        qint32               baudRate = 115200;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::Parity   parity   = QSerialPort::NoParity;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    };

    void setConfig(const Config& config) { m_config = config; }
    const Config& config() const { return m_config; }

    bool    open() override;
    void    close() override;
    bool    isOpen() const override;
    qint64  write(const QByteArray& data) override;
    QString describe() const override;

    /// 供 UI 下拉框使用：当前系统上可用的串口名
    static QStringList availablePortNames();

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort m_port;      // 值成员：随 SerialTransport 一起析构，不用手动 delete
    Config      m_config;
    bool        m_reportedError = false;
};

}  // namespace hmi
