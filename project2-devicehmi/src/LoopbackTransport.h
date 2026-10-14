#pragma once

#include <QTimer>

#include "ITransport.h"

namespace hmi {

/// 回环传输：把发出去的数据原样"回"给你。
///
/// 存在意义：**在没有真实串口硬件的情况下，也能把整条链路和界面跑通**。
/// 面试时这句话值钱 ——「我没有硬件，所以写了一个回环实现来做全链路自测」。
class LoopbackTransport : public ITransport {
    Q_OBJECT

public:
    explicit LoopbackTransport(QObject* parent = nullptr);
    ~LoopbackTransport() override = default;

    bool    open() override;
    void    close() override;
    bool    isOpen() const override { return m_open; }
    qint64  write(const QByteArray& data) override;
    QString describe() const override { return QStringLiteral("回环（无硬件自测）"); }

private:
    bool   m_open = false;
    QTimer m_delayTimer;   // 模拟设备响应延迟，让界面上的时间戳有意义
    QByteArray m_pending;
};

}  // namespace hmi
