#include "validcheck.h"
namespace validCheck {
    /**
     * 计算数字是否符合luhn校验
     * @param number 数字字符串
     * @param length 数字长度
     * @return 符合返回true，否则返回false
     */
    bool isValidLuhn(const QString &number, const qsizetype length) {
        // 检查是否为数字
        for(QChar c : number) {
            if (!c.isDigit()) {
                return false; // 非数字字符
            }
        }
        qsizetype sum = 0;
        for (qsizetype i = 0; i < length; i++) {
            //如果是偶数位，x2
            if ((length-i) % 2 == 0) {
                const int digit = number.at(i).digitValue() * 2;
                //如果乘2后是2位数字，取各位数字和
                if (digit > 9) {
                    sum += digit % 10 + 1;
                } else sum += digit;
            } else {
                sum += number.at(i).digitValue();
            }
        }
        return sum % 10 == 0;
    }
    /**
     * 验证身份证号码合法性
     * @param id 身份证号码
     * @return 验证通过返回true，否则false
     */
    bool isValidID(const QString& id) {
        //长度检验
        if (id.length() != 18) return false;
        // 权重因子
        // 校验码映射表
        constexpr char checkCodes[11] = {'1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'};
        // 检查前17位是否为数字
        for (int i = 0; i < 17; ++i) {
            if (!id.at(i).isDigit()) {
                return false;
            }
        }
        // 计算加权和
        int sum = 0;
        for (int i = 0; i < 17; ++i) {
            const int weights[17] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
            sum += id.at(i).digitValue() * weights[i];
        }
        // 计算余数并获取正确校验码
        const char correctCheckCode = checkCodes[sum % 11];
        // 比较校验码（注意X的大小写处理）
        return (correctCheckCode == id[17]) ||
               (correctCheckCode == 'X' && id[17] == 'x');
    }
}
