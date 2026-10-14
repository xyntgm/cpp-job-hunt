#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>

namespace hmi {

/// 传输层抽象接口。
///
/// 这是整个项目的关键设计：UI 只认 ITransport，不认串口或 TCP。
/// 于是「换一种通信方式」不需要动 UI 一行代码 —— 面试问到"你的架构怎么分层"，
/// 答案就是这个接口。（可对照 [[04-三个项目规格书]] 里的策略模式）
class ITransport : public QObject {
    Q_OBJECT

public:
    explicit ITransport(QObject* parent = nullptr) : QObject(parent) {}
    ~ITransport() override = default;

    virtual bool    open()                   = 0;
    virtual void    close()                  = 0;
    virtual bool    isOpen() const           = 0;
    virtual qint64  write(const QByteArray& data) = 0;
    virtual QString describe() const         = 0;

signals:
    /// 收到数据（可能是一次 readyRead，也可能是一个完整帧）
    void bytesReceived(const QByteArray& data);

    /// 出错信息（已转成人类可读的字符串）
    void errorOccurred(const QString& message);

    /// 打开状态变化
    void stateChanged(bool open);
};

}  // namespace hmi
