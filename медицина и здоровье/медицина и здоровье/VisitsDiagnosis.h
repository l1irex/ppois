#pragma once
#include <string>
#include <vector>
#include "StaffDoctors.h"

class Symptom {
public:
    std::string name;
    int days = 1;
    int severity = 1; // 1..5

    bool isSevere() const { return severity >= 4; }
    void addDay() { days++; }
};

class Diagnosis {
public:
    std::string code;
    std::string title;
    bool mainDiag = true;

    std::string fullText() const { return code + ": " + title; }
};

class Appointment {
public:
    std::string id;
    Patient patient;
    Doctor doctor;
    std::string date;
    bool done = false;

    void complete() {
        patient.checkValid();
        done = true;
    }
};

class SickLeave {
public:
    std::string number;
    Patient patient;
    int days = 3;
    bool closed = false;

    void extend(int extra) {
        if (closed || extra <= 0) throw InvalidPatientDataException();
        days += extra;
    }
    void closeSheet() { closed = true; }
};

class Referral {
public:
    std::string id;
    Patient patient;
    std::string toSpecialty;
    bool used = false;

    void useReferral() {
        if (used) throw TimeConflictException();
        used = true;
    }
};