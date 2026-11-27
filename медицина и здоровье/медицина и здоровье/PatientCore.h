#pragma once
#include <string>
#include <vector>
#include "HealthExceptions.h"

class Contact {
public:
    std::string phone;
    std::string email;
    std::string city;

    bool hasPhone() const { return phone.size() >= 7; }
    bool hasEmail() const { return email.size() >= 5 && email.find('@') != std::string::npos; }
    std::string info() const { return city + ", " + phone; }
};

class Allergy {
public:
    std::string name;
    std::string reaction;
    int level = 1; // 1..5

    bool isDangerous() const { return level >= 4; }
    void raiseLevel() {
        if (level < 5) level++;
    }
    void lowerLevel() {
        if (level > 1) level--;
    }
};

class Vitals {
public:
    double temp = 36.6;
    int pulse = 75;
    int sysPressure = 120;
    int diaPressure = 80;

    bool hasFever() const { return temp >= 37.5; }
    bool highPressure() const { return sysPressure >= 140 || diaPressure >= 90; }
    bool isBad() const { return temp >= 39.0 || pulse > 120 || sysPressure >= 170; }
    bool isStable() const { return !hasFever() && !highPressure() && !isBad(); }
};

class Insurance {
public:
    std::string number;
    std::string company;
    double limit = 0.0;
    bool active = true;

    bool canCover(double sum) const { return active && limit >= sum; }
    void spend(double sum) {
        if (!canCover(sum)) throw InsuranceDeniedException();
        limit -= sum;
    }
    void topUp(double sum) {
        if (!active || sum <= 0.0) throw InsuranceDeniedException();
        limit += sum;
    }
};

class Patient {
public:
    std::string id;
    std::string name;
    int age = 20;
    Contact contact;
    Vitals vitals;
    Insurance insurance;
    std::vector<Allergy> allergies;

    void checkValid() const {
        if (id.empty() || name.empty() || age < 0) throw InvalidPatientDataException();
    }
    void addAllergy(const Allergy& a) { allergies.push_back(a); }
    bool hasAllergy(const std::string& drug) const {
        for (const auto& a : allergies) {
            if (a.name == drug) return true;
        }
        return false;
    }
    int allergyLevel(const std::string& drug) const {
        for (const auto& a : allergies) {
            if (a.name == drug) return a.level;
        }
        return 0;
    }
    bool needsUrgentHelp() const { return vitals.isBad(); }
};