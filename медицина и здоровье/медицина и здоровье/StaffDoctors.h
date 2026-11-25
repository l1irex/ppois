#pragma once
#include <string>
#include "PatientCore.h"

class MedLicense {
public:
    std::string number;
    int expireYear = 2028;
    int category = 1;

    bool isValid(int year) const { return !number.empty() && year <= expireYear; }
    double bonusRate() const { return 1.0 + 0.1 * category; }
};

class Staff {
public:
    std::string id;
    std::string name;
    double salary = 1000.0;
    bool busy = false;
    MedLicense license;

    double totalSalary() const { return salary * license.bonusRate(); }
    void setBusy(bool state) { busy = state; }
};

class Doctor : public Staff {
public:
    std::string specialty;
    int maxPatients = 15;
    int todayPatients = 0;

    void takePatient() {
        if (busy || todayPatients >= maxPatients) throw DoctorBusyException();
        todayPatients++;
    }
    int freeSlots() const { return maxPatients - todayPatients; }
};

class Surgeon : public Doctor {
public:
    int operationsCount = 0;
    bool sterileReady = true;

    void doOperation(const Patient& p) {
        if (!sterileReady) throw HygieneErrorException();
        if (p.hasAllergy("Latex")) throw AllergyRiskException();
        operationsCount++;
    }
};

class Nurse : public Staff {
public:
    int proceduresDone = 0;
    bool canDoIV = true;

    void giveInjection(const Patient& p, const std::string& drug, bool iv) {
        if (p.hasAllergy(drug)) throw AllergyRiskException();
        if (iv && !canDoIV) throw BadPrescriptionException();
        proceduresDone++;
    }
};