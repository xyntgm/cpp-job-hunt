#include "LoopbackTransport.h"

namespace hmi {

LoopbackTransport::LoopbackTransport(QObject* parent) : ITransport(parent) {
    m_delayTimer.setSingleShot(true);
    connect(&m_delayTimer, &QTimer::timeout, this, [this] {
        if (!m_pending.isEmpty()) {
            const QByteArray data = m_pending;
            m_pending.clear();
            emit bytesReceived(data);
        }
    });
}

bool LoopbackTransport::open() {
    m_open = true;
    emit stateChanged(true);
    return true;
}

void LoopbackTransport::close() {
    m_delayTimer.stop();
    m_pending.clear();
    m_open = false;
    emit stateChanged(false);
}

qint64 LoopbackTransport::write(const QByteArray& data) {
    if (!m_open) {
        emit errorOccurred(QStringLiteral("回环通道未打开"));
        return -1;
    }
    // 模拟"发出 50ms 后收到回应"
    m_pending.append(data);
    m_delayTimer.start(50);
    return data.size();
}

}  // namespace hmi
