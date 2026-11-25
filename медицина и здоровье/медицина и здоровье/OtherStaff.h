#pragma once
#include <string>
#include "StaffDoctors.h"

class Dentist : public Doctor {
public:
    int teethTreated = 0;
    bool hasXRay = true;

    void treatTooth(int toothNum) {
        if (toothNum < 1 || toothNum > 32) throw InvalidPatientDataException();
        teethTreated++;
    }
};

class Pharmacist : public Staff {
public:
    int issuedDrugs = 0;
    bool canGiveStrong = false;

    void issueDrug(const std::string& drug, bool strongDrug) {
        if (drug.empty()) throw BadPrescriptionException();
        if (strongDrug && !canGiveStrong) throw BadPrescriptionException();
        issuedDrugs++;
    }
};

class Receptionist : public Staff {
public:
    int bookedCount = 0;

    void bookVisit(Doctor& d) {
        d.takePatient();
        bookedCount++;
    }
};

class LabWorker : public Staff {
public:
    int testsDone = 0;

    void checkSample(bool ok) {
        if (!ok) throw BadLabSampleException();
        testsDone++;
    }
};