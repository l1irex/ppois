#pragma once
#include <string>
#include <vector>
#include "VisitsDiagnosis.h"

class Procedure {
public:
    std::string title;
    int count = 1;
    int doneCount = 0;

    void markOneDone() {
        if (doneCount < count) doneCount++;
    }
    bool isFinished() const { return doneCount >= count; }
    int leftCount() const { return count > doneCount ? count - doneCount : 0; }
};

class TherapyCourse {
public:
    std::string name;
    int daysTotal = 7;
    int daysPassed = 0;

    void passDay() {
        if (daysPassed < daysTotal) daysPassed++;
    }
    int leftDays() const { return daysTotal - daysPassed; }
    int progressPercent() const {
        if (daysTotal <= 0) return 0;
        int p = daysPassed * 100 / daysTotal;
        return p > 100 ? 100 : p;
    }
};

class TreatmentPlan {
public:
    std::string planId;
    Diagnosis diagnosis;
    std::vector<Procedure> procedures;
    std::vector<TherapyCourse> courses;

    void addProcedure(const Procedure& p) { procedures.push_back(p); }
    void addCourse(const TherapyCourse& c) { courses.push_back(c); }
    int proceduresCount() const { return static_cast<int>(procedures.size()); }
    int coursesCount() const { return static_cast<int>(courses.size()); }
};

class RecoveryNote {
public:
    std::string date;
    std::string note;
    int score = 5; // 1..10

    bool isImproving() const { return score >= 7; }
    bool isCritical() const { return score <= 3; }
};