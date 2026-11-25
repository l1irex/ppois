#pragma once
#include "CoreGeo.h"
#include "Exceptions.h"
#include "ProjectsSchedule.h"
#include <cmath>
#include <string>
#include <vector>

class CostItem {
public:
    std::string code;
    std::string name;
    double count = 0.0;
    double price = 0.0;

    double total() const {
        return count * price;
    }

    void multiplyCount(double k) {
        if (k <= 0.0) {
            throw InvalidProjectDataException("Коэффициент должен быть больше 0");
        }
        count *= k;
    }

    bool isOk() const {
        return !code.empty() && count > 0.0 && price >= 0.0;
    }
};

class Estimate {
public:
    std::string id;
    std::string title;
    double markup = 15.0;
    double reserve = 5.0;
    bool approved = false;
    std::vector<CostItem> items;

    void addItem(const CostItem& item) {
        if (!item.isOk()) {
            throw InvalidProjectDataException("Неверная позиция сметы");
        }
        items.push_back(item);
    }

    double baseSum() const {
        double sum = 0.0;
        for (int i = 0; i < (int)items.size(); i++) {
            sum += items[i].total();
        }
        return sum;
    }

    double totalSum() const {
        double b = baseSum();
        return b + b * markup / 100.0 + b * reserve / 100.0;
    }

    void approve() {
        if (items.empty()) {
            throw InvalidProjectDataException("Смета пустая");
        }
        approved = true;
    }

    double extraSum() const {
        return totalSum() - baseSum();
    }
};

class Invoice {
public:
    std::string number;
    double sum = 0.0;
    double paidSum = 0.0;
    bool paid = false;
    Client client;

    void payPart(double money) {
        if (money <= 0.0) {
            throw InvalidPaymentException("Сумма оплаты должна быть больше 0");
        }
        if (paidSum + money > sum) {
            throw InvalidPaymentException("Переплата по счёту " + number);
        }
        paidSum += money;
        if (std::abs(sum - paidSum) < 0.01) {
            paid = true;
        }
    }

    double leftToPay() const {
        double diff = sum - paidSum;
        return diff > 0.0 ? diff : 0.0;
    }

    void cancel() {
        if (paidSum > 0.0) {
            throw InvalidPaymentException("Счёт уже частично оплачен");
        }
        sum = 0.0;
        paid = false;
    }

    std::string getState() const {
        if (paid) return "Paid";
        if (paidSum > 0.0) return "PartPaid";
        return "Unpaid";
    }
};

class PaymentSchedule {
public:
    std::string code;
    int parts = 1;
    double penaltyPercent = 0.1;
    std::vector<Invoice> invoices;

    void addInvoice(const Invoice& inv) {
        invoices.push_back(inv);
    }

    double totalInvoicesSum() const {
        double s = 0.0;
        for (int i = 0; i < (int)invoices.size(); i++) {
            s += invoices[i].sum;
        }
        return s;
    }

    double calcPenalty(double debt, int days) const {
        if (debt < 0.0 || days < 0) {
            throw InvalidPaymentException("Неверные данные для расчёта пени");
        }
        return debt * penaltyPercent / 100.0 * days;
    }
};

class Contract {
public:
    std::string number;
    double amount = 0.0;
    int months = 12;
    bool signedContract = false;
    Client client;
    PaymentSchedule schedule;

    void sign() {
        client.checkData();
        if (amount <= 0.0 || months <= 0) {
            throw InvalidContractException("Неверная сумма или срок договора");
        }
        signedContract = true;
    }

    void changeAmount(double diff) {
        if (amount + diff <= 0.0) {
            throw InvalidContractException("Сумма договора должна остаться положительной");
        }
        amount += diff;
    }

    double monthSum() const {
        return amount / months;
    }

    void cancelContract() {
        signedContract = false;
    }
};

class BankAccount {
public:
    std::string accountNumber;
    std::string bankName;
    double balance = 0.0;
    bool blocked = false;

    void putMoney(double money) {
        if (blocked) {
            throw InvalidPaymentException("Счёт заблокирован");
        }
        if (money <= 0.0) {
            throw InvalidPaymentException("Сумма должна быть больше 0");
        }
        balance += money;
    }

    void takeMoney(double money) {
        if (blocked) {
            throw InvalidPaymentException("Счёт заблокирован");
        }
        if (money <= 0.0) {
            throw InvalidPaymentException("Сумма должна быть больше 0");
        }
        if (balance < money) {
            throw InsufficientBudgetException("Не хватает денег на счёте");
        }
        balance -= money;
    }

    void sendTo(BankAccount& other, double money) {
        takeMoney(money);
        other.putMoney(money);
    }

    void block() {
        blocked = true;
    }

    void unblock() {
        blocked = false;
    }
};

class Payment {
public:
    std::string id;
    double sum = 0.0;
    std::string payType;
    bool done = false;
    Client client;

    void makePayment(Invoice& inv, BankAccount& acc) {
        if (done) {
            throw InvalidPaymentException("Платёж уже проведён");
        }
        if (sum <= 0.0) {
            throw InvalidPaymentException("Неверная сумма платежа");
        }
        acc.putMoney(sum);
        inv.payPart(sum);
        done = true;
    }

    void rollbackPayment(BankAccount& acc, Invoice& inv) {
        if (!done) {
            throw InvalidPaymentException("Платёж ещё не проведён");
        }
        acc.takeMoney(sum);
        inv.paidSum -= sum;
        if (inv.paidSum < 0.0) inv.paidSum = 0.0;
        inv.paid = false;
        done = false;
    }

    bool isBankTransfer() const {
        return payType == "Bank" || payType == "Transfer";
    }

    std::string info() const {
        return id + " (" + payType + ")";
    }
};

class ExpenseRecord {
public:
    std::string id;
    double sum = 0.0;
    std::string type;
    bool refundable = false;

    void check() const {
        if (sum <= 0.0 || type.empty()) {
            throw InvalidPaymentException("Неверная запись расхода");
        }
    }

    double realCost() const {
        return refundable ? 0.0 : sum;
    }

    void setRefundable(bool flag) {
        refundable = flag;
    }
};

class TaxCalculator {
public:
    double ndsPercent = 20.0;
    double profitTaxPercent = 20.0;
    double salaryTaxPercent = 34.0;

    double calcNds(double sum) const {
        if (sum < 0.0) {
            throw InvalidPaymentException("Сумма не может быть отрицательной");
        }
        return sum * ndsPercent / 100.0;
    }

    double calcProfitTax(double profit) const {
        if (profit <= 0.0) return 0.0;
        return profit * profitTaxPercent / 100.0;
    }

    double calcSalaryTax(double fot) const {
        if (fot < 0.0) {
            throw InvalidPaymentException("ФОТ не может быть отрицательным");
        }
        return fot * salaryTaxPercent / 100.0;
    }
};

class ProfitAnalyzer {
public:
    double income = 0.0;
    double expenses = 0.0;
    double targetPercent = 15.0;

    double profit() const {
        return income - expenses;
    }

    double realPercent() const {
        if (income <= 0.0) return 0.0;
        return profit() * 100.0 / income;
    }

    bool isProfitOk() const {
        return realPercent() >= targetPercent;
    }

    double needIncome(double wantPercent) const {
        if (wantPercent >= 100.0) {
            throw InvalidPaymentException("Процент должен быть меньше 100");
        }
        return expenses / (1.0 - wantPercent / 100.0);
    }
};

class Budget {
public:
    std::string id;
    double totalMoney = 0.0;
    double spentMoney = 0.0;
    std::vector<ExpenseRecord> records;

    void addRecord(const ExpenseRecord& r) {
        r.check();
        if (spentMoney + r.sum > totalMoney) {
            throw InsufficientBudgetException("Не хватает средств в бюджете " + id);
        }
        records.push_back(r);
        spentMoney += r.sum;
    }

    double freeMoney() const {
        return totalMoney - spentMoney;
    }

    void addMoney(double extra) {
        if (extra <= 0.0) {
            throw InvalidPaymentException("Добавляемая сумма должна быть больше 0");
        }
        totalMoney += extra;
    }

    bool almostEmpty() const {
        if (totalMoney <= 0.0) return true;
        return (spentMoney * 100.0 / totalMoney) >= 90.0;
    }

    double nonRefundableSum() const {
        double s = 0.0;
        for (int i = 0; i < (int)records.size(); i++) {
            s += records[i].realCost();
        }
        return s;
    }

    void clearRecords() {
        spentMoney = 0.0;
        records.clear();
    }
};