#pragma once
#include <string>
#include <vector>
#include "PatientCore.h"

class ChronicDisease {
public:
    std::string code;
    std::string title;
    int startYear = 2020;
    bool active = true;

    int yearsSick(int nowYear) const { return nowYear >= startYear ? nowYear - startYear : 0; }
    void setRemission() { active = false; }
    void relapse() { active = true; }
};

class Vaccination {
public:
    std::string vaccineName;
    int yearDone = 2025;
    int validYears = 5;

    bool isValid(int nowYear) const { return nowYear <= yearDone + validYears; }
    void renew(int nowYear) { yearDone = nowYear; }
    int yearsLeft(int nowYear) const {
        int last = yearDone + validYears;
        return nowYear <= last ? last - nowYear : 0;
    }
};

class BodyMetrics {
public:
    double heightCm = 175.0;
    double weightKg = 70.0;
    std::string bloodGroup = "O+";

    double bmi() const {
        double h = heightCm / 100.0;
        return h > 0 ? weightKg / (h * h) : 0.0;
    }
    bool isOverweight() const { return bmi() >= 25.0; }
    double idealWeight() const {
        double h = heightCm / 100.0;
        return h > 0 ? 22.0 * h * h : 0.0;
    }
};

class MedCard {
public:
    std::string cardId;
    Patient patient;
    BodyMetrics metrics;
    std::vector<ChronicDisease> diseases;
    std::vector<Vaccination> vaccines;

    void addDisease(const ChronicDisease& d) { diseases.push_back(d); }
    void addVaccine(const Vaccination& v) { vaccines.push_back(v); }
    int activeDiseasesCount() const {
        int c = 0;
        for (const auto& d : diseases) if (d.active) c++;
        return c;
    }
    int validVaccinesCount(int nowYear) const {
        int c = 0;
        for (const auto& v : vaccines) if (v.isValid(nowYear)) c++;
        return c;
    }
};