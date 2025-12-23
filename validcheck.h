#pragma once
#include <QString>
namespace validCheck {
    bool isValidLuhn(const QString &number, qsizetype length);
    bool isValidID(const QString& id);
}
