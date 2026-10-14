#include "MainWindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSerialPort>
#include <QSpinBox>
#include <QStatusBar>
#include <QTextDocument>
#include <QVBoxLayout>

#include "HexUtil.h"
#include "LoopbackTransport.h"
#include "SerialTransport.h"

namespace hmi {

namespace {
constexpr int kBaudRates[] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
constexpr const char* kLoopbackLabel = "回环（无硬件自测）";
}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("设备调试助手 v0.1  ——  project2-devicehmi 阶段 1"));

    auto* central = new QWidget(this);
    auto* layout  = new QVBoxLayout(central);
    layout->addWidget(buildConfigGroup());
    layout->addWidget(buildReceiveGroup(), 3);
    layout->addWidget(buildSendGroup(), 1);
    setCentralWidget(central);

    m_statsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_statsLabel);
    updateStats();

    connect(&m_loopTimer, &QTimer::timeout, this, &MainWindow::sendNow);

    refreshPorts();
    resize(1000, 720);
}

MainWindow::~MainWindow() = default;

QWidget* MainWindow::buildConfigGroup() {
    auto* box    = new QGroupBox(QStringLiteral("串口配置"), this);
    auto* layout = new QHBoxLayout(box);

    layout->addWidget(new QLabel(QStringLiteral("端口："), box));
    m_portCombo = new QComboBox(box);
    m_portCombo->setMinimumWidth(260);
    layout->addWidget(m_portCombo);

    m_refreshBtn = new QPushButton(QStringLiteral("刷新"), box);
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    layout->addWidget(m_refreshBtn);

    layout->addWidget(new QLabel(QStringLiteral("波特率："), box));
    m_baudCombo = new QComboBox(box);
    for (const int baud : kBaudRates) {
        m_baudCombo->addItem(QString::number(baud), baud);
    }
    m_baudCombo->setCurrentText(QStringLiteral("115200"));
    layout->addWidget(m_baudCombo);

    layout->addWidget(new QLabel(QStringLiteral("数据位："), box));
    m_dataCombo = new QComboBox(box);
    m_dataCombo->addItem(QStringLiteral("5"), static_cast<int>(QSerialPort::Data5));
    m_dataCombo->addItem(QStringLiteral("6"), static_cast<int>(QSerialPort::Data6));
    m_dataCombo->addItem(QStringLiteral("7"), static_cast<int>(QSerialPort::Data7));
    m_dataCombo->addItem(QStringLiteral("8"), static_cast<int>(QSerialPort::Data8));
    m_dataCombo->setCurrentIndex(3);
    layout->addWidget(m_dataCombo);

    layout->addWidget(new QLabel(QStringLiteral("校验："), box));
    m_parityCombo = new QComboBox(box);
    m_parityCombo->addItem(QStringLiteral("无 N"), static_cast<int>(QSerialPort::NoParity));
    m_parityCombo->addItem(QStringLiteral("偶 E"), static_cast<int>(QSerialPort::EvenParity));
    m_parityCombo->addItem(QStringLiteral("奇 O"), static_cast<int>(QSerialPort::OddParity));
    layout->addWidget(m_parityCombo);

    layout->addWidget(new QLabel(QStringLiteral("停止位："), box));
    m_stopCombo = new QComboBox(box);
    m_stopCombo->addItem(QStringLiteral("1"), static_cast<int>(QSerialPort::OneStop));
    m_stopCombo->addItem(QStringLiteral("1.5"), static_cast<int>(QSerialPort::OneAndHalfStop));
    m_stopCombo->addItem(QStringLiteral("2"), static_cast<int>(QSerialPort::TwoStop));
    layout->addWidget(m_stopCombo);

    layout->addStretch(1);
    m_openBtn = new QPushButton(QStringLiteral("打开"), box);
    m_openBtn->setMinimumWidth(90);
    connect(m_openBtn, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    layout->addWidget(m_openBtn);

    return box;
}

QWidget* MainWindow::buildReceiveGroup() {
    auto* box    = new QGroupBox(QStringLiteral("接收区"), this);
    auto* layout = new QVBoxLayout(box);

    auto* bar = new QHBoxLayout();
    m_timeChk = new QCheckBox(QStringLiteral("显示时间戳"), box);
    m_timeChk->setChecked(true);
    bar->addWidget(m_timeChk);

    m_hexViewChk = new QCheckBox(QStringLiteral("HEX 显示"), box);
    bar->addWidget(m_hexViewChk);

    m_autoScrollChk = new QCheckBox(QStringLiteral("自动滚动"), box);
    m_autoScrollChk->setChecked(true);
    bar->addWidget(m_autoScrollChk);

    bar->addStretch(1);

    m_clearBtn = new QPushButton(QStringLiteral("清空"), box);
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::clearReceiveView);
    bar->addWidget(m_clearBtn);

    m_saveBtn = new QPushButton(QStringLiteral("保存到文件"), box);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::saveReceivedToFile);
    bar->addWidget(m_saveBtn);

    layout->addLayout(bar);

    m_recvView = new QPlainTextEdit(box);
    m_recvView->setReadOnly(true);
    m_recvView->setLineWrapMode(QPlainTextEdit::NoWrap);
    // 关键：限制最大行数，否则长时间采集会把内存吃光（面试可以聊这个）
    m_recvView->document()->setMaximumBlockCount(5000);
    QFont mono(QStringLiteral("Consolas"));
    mono.setStyleHint(QFont::Monospace);
    m_recvView->setFont(mono);
    layout->addWidget(m_recvView);

    return box;
}

QWidget* MainWindow::buildSendGroup() {
    auto* box    = new QGroupBox(QStringLiteral("发送区"), this);
    auto* layout = new QVBoxLayout(box);

    auto* bar = new QHBoxLayout();
    m_hexSendChk = new QCheckBox(QStringLiteral("HEX 发送"), box);
    bar->addWidget(m_hexSendChk);

    m_loopChk = new QCheckBox(QStringLiteral("定时发送"), box);
    bar->addWidget(m_loopChk);

    m_loopInterval = new QSpinBox(box);
    m_loopInterval->setRange(10, 60000);
    m_loopInterval->setValue(1000);
    m_loopInterval->setSuffix(QStringLiteral(" ms"));
    bar->addWidget(m_loopInterval);

    bar->addStretch(1);

    m_sendBtn = new QPushButton(QStringLiteral("发送"), box);
    m_sendBtn->setMinimumWidth(90);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::sendNow);
    bar->addWidget(m_sendBtn);

    layout->addLayout(bar);

    m_sendView = new QPlainTextEdit(box);
    m_sendView->setPlaceholderText(
        QStringLiteral("文本模式直接输入内容；勾选 HEX 发送后输入 01 03 00 00 00 02"));
    m_sendView->setPlainText(QStringLiteral("hello device"));
    QFont mono(QStringLiteral("Consolas"));
    mono.setStyleHint(QFont::Monospace);
    m_sendView->setFont(mono);
    layout->addWidget(m_sendView);

    // 定时发送开关
    connect(m_loopChk, &QCheckBox::toggled, this, [this](bool on) {
        if (on) {
            m_loopTimer.start(m_loopInterval->value());
        } else {
            m_loopTimer.stop();
        }
    });
    connect(m_loopInterval, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int ms) {
        if (m_loopTimer.isActive()) {
            m_loopTimer.setInterval(ms);
        }
    });

    return box;
}

void MainWindow::refreshPorts() {
    const QString previous = m_portCombo->currentData().toString();
    m_portCombo->clear();
    m_portCombo->addItem(QString::fromUtf8(kLoopbackLabel), QString());

    const QStringList ports = SerialTransport::availablePortNames();
    for (const QString& label : ports) {
        // 下拉框显示 "COM3  (USB-SERIAL CH340)"，但 data 只存 "COM3"
        const QString portName = label.section(QLatin1Char(' '), 0, 0).trimmed();
        m_portCombo->addItem(label, portName);
    }

    if (!previous.isEmpty()) {
        const int index = m_portCombo->findData(previous);
        if (index >= 0) {
            m_portCombo->setCurrentIndex(index);
        }
    }

    statusBar()->showMessage(
        QStringLiteral("发现 %1 个串口（无硬件时请选择「%2」）")
            .arg(ports.size())
            .arg(QString::fromUtf8(kLoopbackLabel)),
        4000);
}

void MainWindow::connectTransport(ITransport* transport) {
    connect(transport, &ITransport::bytesReceived, this, &MainWindow::onBytesReceived);
    connect(transport, &ITransport::errorOccurred, this, &MainWindow::onTransportError);
    connect(transport, &ITransport::stateChanged, this, &MainWindow::onStateChanged);
}

void MainWindow::toggleConnection() {
    if (m_active != nullptr && m_active->isOpen()) {
        m_active->close();
        return;
    }

    if (m_active != nullptr) {
        m_active->deleteLater();
        m_active = nullptr;
    }

    const QString portName = m_portCombo->currentData().toString();
    if (portName.isEmpty()) {
        auto* loopback = new LoopbackTransport(this);
        connectTransport(loopback);
        m_active = loopback;
    } else {
        auto* serial = new SerialTransport(this);
        SerialTransport::Config config;
        config.portName = portName;
        config.baudRate = m_baudCombo->currentData().toInt();
        config.dataBits = static_cast<QSerialPort::DataBits>(m_dataCombo->currentData().toInt());
        config.parity   = static_cast<QSerialPort::Parity>(m_parityCombo->currentData().toInt());
        config.stopBits = static_cast<QSerialPort::StopBits>(m_stopCombo->currentData().toInt());
        serial->setConfig(config);

        connectTransport(serial);
        m_active = serial;
    }

    if (!m_active->open()) {
        m_active->deleteLater();
        m_active = nullptr;
        onStateChanged(false);
    }
}

void MainWindow::onStateChanged(bool open) {
    m_openBtn->setText(open ? QStringLiteral("关闭") : QStringLiteral("打开"));
    m_openBtn->setStyleSheet(open ? QStringLiteral("color:#0a0;font-weight:bold;") : QString());

    m_portCombo->setEnabled(!open);
    m_baudCombo->setEnabled(!open);
    m_dataCombo->setEnabled(!open);
    m_parityCombo->setEnabled(!open);
    m_stopCombo->setEnabled(!open);

    if (open && m_active != nullptr) {
        appendLine(QStringLiteral("** "), QByteArray());
        statusBar()->showMessage(QStringLiteral("已打开：") + m_active->describe(), 3000);
    } else {
        statusBar()->showMessage(QStringLiteral("已关闭"), 3000);
        if (m_loopChk->isChecked()) {
            m_loopChk->setChecked(false);
        }
    }
}

void MainWindow::sendNow() {
    if (m_active == nullptr || !m_active->isOpen()) {
        QMessageBox::warning(this, QStringLiteral("未连接"),
                             QStringLiteral("请先打开串口，或无硬件时选择「回环（无硬件自测）」。"));
        if (m_loopChk->isChecked()) {
            m_loopChk->setChecked(false);
        }
        return;
    }

    const QString text = m_sendView->toPlainText();
    QByteArray    payload;

    if (m_hexSendChk->isChecked()) {
        bool ok = false;
        payload = fromHexString(text, &ok);
        if (!ok || payload.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("HEX 格式错误"),
                                 QStringLiteral("请输入偶数个十六进制字符，例如：01 03 00 00 00 02"));
            if (m_loopChk->isChecked()) {
                m_loopChk->setChecked(false);
            }
            return;
        }
    } else {
        payload = text.toUtf8();
    }

    if (payload.isEmpty()) {
        return;
    }

    const qint64 written = m_active->write(payload);
    if (written > 0) {
        m_txBytes += written;
        appendLine(QStringLiteral(">> "), payload);
        updateStats();
    }
}

void MainWindow::onBytesReceived(const QByteArray& data) {
    m_rxBytes += data.size();
    appendLine(QStringLiteral("<< "), data);
    updateStats();
}

void MainWindow::onTransportError(const QString& message) {
    appendLine(QStringLiteral("!! "), message.toUtf8());
    statusBar()->showMessage(message, 6000);
}

void MainWindow::appendLine(const QString& prefix, const QByteArray& data) {
    QString line;
    if (m_timeChk->isChecked()) {
        line += QStringLiteral("[") + timestampNow() + QStringLiteral("] ");
    }
    line += prefix;

    if (data.isEmpty()) {
        // 纯状态行
        m_recvView->appendPlainText(line.trimmed().isEmpty() ? QString() : line);
        return;
    }

    line += m_hexViewChk->isChecked()
                ? toHexString(data)
                : toPrintableText(data);

    m_recvView->appendPlainText(line);

    if (m_autoScrollChk->isChecked()) {
        m_recvView->verticalScrollBar()->setValue(m_recvView->verticalScrollBar()->maximum());
    }
}

void MainWindow::clearReceiveView() {
    m_recvView->clear();
    m_rxBytes = 0;
    m_txBytes = 0;
    updateStats();
}

void MainWindow::saveReceivedToFile() {
    const QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("保存接收数据"), QStringLiteral("capture.txt"),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));
    if (path.isEmpty()) {
        return;
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), file.errorString());
        return;
    }
    file.write(m_recvView->toPlainText().toUtf8());
    statusBar()->showMessage(QStringLiteral("已保存到 ") + path, 5000);
}

void MainWindow::updateStats() {
    m_statsLabel->setText(QStringLiteral("  发送 %1 B  /  接收 %2 B  ")
                              .arg(m_txBytes)
                              .arg(m_rxBytes));
}

}  // namespace hmi
