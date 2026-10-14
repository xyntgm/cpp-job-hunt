#pragma once

#include <QByteArray>
#include <QString>

namespace hmi {

/// 字节数组 → HEX 字符串，例如 {0x01,0xA0} → "01 A0"
QString toHexString(const QByteArray& data, bool withSpace = true);

/// HEX 字符串 → 字节数组（忽略空格/换行/逗号）。
/// ok 为 nullptr 时不报错；否则写入是否解析成功。
QByteArray fromHexString(const QString& text, bool* ok = nullptr);

/// 字节数组 → 可读文本：可打印 ASCII 原样显示，其余显示为 '.'
QString toPrintableText(const QByteArray& data);

/// 当前时间戳，格式 hh:mm:ss.zzz
QString timestampNow();

}  // namespace hmi
