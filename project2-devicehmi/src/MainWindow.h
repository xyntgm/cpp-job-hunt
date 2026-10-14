#pragma once

#include <QMainWindow>
#include <QTimer>

class QCheckBox;
class QComboBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;

namespace hmi {

class ITransport;

/// 阶段 1：串口调试助手。
///
/// 界面分三块（和真实的工业调试工具一样）：
///   上：串口配置（端口/波特率/数据位/校验/停止位 + 打开关闭）
///   中：接收区（时间戳、HEX/文本切换、清空、保存）
///   下：发送区（HEX/文本切换、定时发送、发送）
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void refreshPorts();
    void toggleConnection();
    void sendNow();
    void clearReceiveView();
    void saveReceivedToFile();
    void onBytesReceived(const QByteArray& data);
    void onTransportError(const QString& message);
    void onStateChanged(bool open);

private:
    QWidget* buildConfigGroup();
    QWidget* buildReceiveGroup();
    QWidget* buildSendGroup();
    void     connectTransport(ITransport* transport);
    void     updateStats();
    void     appendLine(const QString& prefix, const QByteArray& data);

    ITransport* m_active = nullptr;

    // 串口配置
    QComboBox*   m_portCombo    = nullptr;
    QPushButton* m_refreshBtn   = nullptr;
    QComboBox*   m_baudCombo    = nullptr;
    QComboBox*   m_dataCombo    = nullptr;
    QComboBox*   m_parityCombo  = nullptr;
    QComboBox*   m_stopCombo    = nullptr;
    QPushButton* m_openBtn      = nullptr;

    // 接收
    QPlainTextEdit* m_recvView      = nullptr;
    QCheckBox*      m_hexViewChk    = nullptr;
    QCheckBox*      m_timeChk       = nullptr;
    QCheckBox*      m_autoScrollChk = nullptr;
    QPushButton*    m_clearBtn      = nullptr;
    QPushButton*    m_saveBtn       = nullptr;

    // 发送
    QPlainTextEdit* m_sendView     = nullptr;
    QCheckBox*      m_hexSendChk   = nullptr;
    QCheckBox*      m_loopChk      = nullptr;
    QSpinBox*       m_loopInterval = nullptr;
    QPushButton*    m_sendBtn      = nullptr;

    QLabel* m_statsLabel = nullptr;
    QTimer  m_loopTimer;

    qint64 m_rxBytes = 0;
    qint64 m_txBytes = 0;
};

}  // namespace hmi
