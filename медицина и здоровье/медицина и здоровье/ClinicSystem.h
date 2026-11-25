#pragma once
#include <string>
#include <vector>
#include "BillingPay.h"
#include "PatientCore.h"

class SanitaryCheck {
public:
    std::string checkId;
    int score = 100;
    bool passed = true;

    void failCheck(int penalty) {
        score -= penalty;
        if (score < 60) passed = false;
    }
};

class QualityReport {
public:
    std::string reportId;
    int complaints = 0;
    int resolved = 0;

    void addComplaint() { complaints++; }
    void resolveOne() {
        if (complaints > resolved) resolved++;
    }
    int openComplaints() const { return complaints - resolved; }
};

class Department {
public:
    std::string name;
    int roomsCount = 5;
    bool open = true;

    void closeDept() { open = false; }
    void openDept() { open = true; }
};

class MedicalCenter {
public:
    std::string centerName;
    std::vector<Patient> patients;
    std::vector<Department> departments;
    CashDesk cashDesk;

    void addPatient(const Patient& p) {
        p.checkValid();
        patients.push_back(p);
    }
    Patient& findPatientById(const std::string& id) {
        for (auto& p : patients) {
            if (p.id == id) return p;
        }
        throw PatientNotFoundException();
    }
    void addDepartment(const Department& d) {
        departments.push_back(d);
    }
    int totalPatients() const {
        return static_cast<int>(patients.size());
    }
};