#include "HexUtil.h"

#include <QChar>
#include <QDateTime>
#include <QStringList>

namespace hmi {

QString toHexString(const QByteArray& data, bool withSpace) {
    // Qt 自带 toHex()，第二个参数是分隔符（QChar）
    if (data.isEmpty()) {
        return {};
    }
    const QByteArray hex = withSpace ? data.toHex(' ') : data.toHex();
    return QString::fromLatin1(hex).toUpper();
}

QByteArray fromHexString(const QString& text, bool* ok) {
    if (ok != nullptr) {
        *ok = true;
    }

    // 1) 先剔掉所有分隔符，只留下十六进制字符
    QString cleaned;
    cleaned.reserve(text.size());
    for (const QChar ch : text) {
        if (ch.isSpace() || ch == ',' || ch == '\\' || ch == 'x' || ch == 'X') {
            continue;
        }
        if (!ch.isDigit() && (ch.toUpper() < 'A' || ch.toUpper() > 'F')) {
            if (ok != nullptr) {
                *ok = false;
            }
            return {};
        }
        cleaned.append(ch);
    }

    // 2) 奇数个字符无法组成字节
    if (cleaned.size() % 2 != 0) {
        if (ok != nullptr) {
            *ok = false;
        }
        return {};
    }

    // 3) 两两转成字节
    QByteArray out;
    out.reserve(cleaned.size() / 2);
    for (int i = 0; i < cleaned.size(); i += 2) {
        const int high = QString(cleaned.at(i)).toInt(nullptr, 16);
        const int low  = QString(cleaned.at(i + 1)).toInt(nullptr, 16);
        out.append(static_cast<char>((high << 4) | low));
    }
    return out;
}

QString toPrintableText(const QByteArray& data) {
    QString text;
    text.reserve(data.size());
    for (const char c : data) {
        const unsigned char b = static_cast<unsigned char>(c);
        if (b == '\n') {
            text.append(QLatin1Char('\n'));
        } else if (b == '\r') {
            // 忽略 CR，避免 Windows 下出现空行
            continue;
        } else if (b >= 0x20 && b < 0x7F) {
            text.append(QLatin1Char(static_cast<char>(b)));
        } else {
            text.append(QLatin1Char('.'));
        }
    }
    return text;
}

QString timestampNow() {
    return QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss.zzz"));
}

}  // namespace hmi
