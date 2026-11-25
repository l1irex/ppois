#pragma once
#include <string>
#include "PatientCore.h"

class LabSample {
public:
    std::string sampleId;
    std::string type;
    bool valid = true;

    void spoil() { valid = false; }
    void check() const {
        if (!valid || sampleId.empty()) throw BadLabSampleException();
    }
};

class BloodTest {
public:
    double hemoglobin = 140.0;
    double leukocytes = 6.0;
    double sugar = 5.0;

    bool isNormal() const {
        return hemoglobin >= 115.0 && sugar >= 3.3 && sugar <= 6.0;
    }
};

class UrineTest {
public:
    double density = 1.018;
    bool proteinFound = false;

    bool isClean() const { return !proteinFound && density >= 1.005 && density <= 1.030; }
};

class XRayScan {
public:
    std::string area;
    bool hasFracture = false;
    std::string comment;

    std::string summary() const {
        return area + (hasFracture ? ": fracture" : ": normal");
    }
};

class LabOrder {
public:
    std::string orderId;
    Patient patient;
    LabSample sample;
    bool ready = false;

    void completeOrder() {
        sample.check();
        ready = true;
    }
};