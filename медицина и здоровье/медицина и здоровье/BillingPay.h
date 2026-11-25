#pragma once
#include <string>
#include "PatientCore.h"

class Bill {
public:
    std::string billId;
    Patient patient;
    double amount = 0.0;
    bool paid = false;

    void setAmount(double sum) {
        if (sum < 0.0) throw NoMoneyException();
        amount = sum;
    }
    void markPaid() { paid = true; }
};

class CashDesk {
public:
    double balance = 1000.0;

    void acceptCash(double sum) {
        if (sum <= 0.0) throw NoMoneyException();
        balance += sum;
    }
    void refundCash(double sum) {
        if (sum <= 0.0 || balance < sum) throw NoMoneyException();
        balance -= sum;
    }
};

class InsuranceClaim {
public:
    std::string claimId;
    Patient patient;
    double sum = 0.0;
    bool approved = false;

    void processClaim() {
        patient.insurance.spend(sum);
        approved = true;
    }
};

class DiscountCard {
public:
    std::string cardCode;
    int percent = 5;

    double applyDiscount(double price) const {
        if (price < 0.0) throw NoMoneyException();
        return price * (1.0 - percent / 100.0);
    }
};