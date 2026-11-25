#pragma once
#include "CoreGeo.h"
#include "Equipment.h"
#include "Exceptions.h"
#include "FinanceContracts.h"
#include "Materials.h"
#include "ProjectsSchedule.h"
#include "Staff.h"
#include <string>
#include <vector>

class SafetyViolation {
public:
    std::string id;
    std::string description;
    int severity = 1; // 1..5
    bool fixed = false;
    Worker worker;

    void checkCritical() const {
        if (severity >= 4 && !fixed) {
            throw SafetyViolationException("Критическое нарушение ТБ: " + description);
        }
    }

    void markFixed() {
        fixed = true;
    }

    std::string info() const {
        return "[" + std::to_string(severity) + "] " + description;
    }
};

class SafetyInspection {
public:
    std::string id;
    std::string date;
    bool passed = true;
    Site site;
    Engineer inspector;
    std::vector<SafetyViolation> violations;

    void addViolation(const SafetyViolation& v) {
        violations.push_back(v);
        if (v.severity >= 3) {
            passed = false;
        }
        v.checkCritical();
    }

    int openViolationsCount() const {
        int c = 0;
        for (int i = 0; i < (int)violations.size(); i++) {
            if (!violations[i].fixed) c++;
        }
        return c;
    }

    void fixAllViolations() {
        for (int i = 0; i < (int)violations.size(); i++) {
            violations[i].markFixed();
        }
        passed = true;
    }

    std::string summary() const {
        return "Проверка " + id + " на " + site.name;
    }
};

class DefectReport {
public:
    std::string id;
    std::string defectText;
    int severity = 1;
    bool fixed = false;
    WorkStage stage;
    Task task;

    void checkDefect() const {
        if (severity >= 4 && !fixed) {
            throw QualityDefectException("Критический дефект: " + defectText);
        }
    }

    void fixDefect() {
        fixed = true;
    }

    std::string shortDesc() const {
        return id + ": " + defectText;
    }
};

class QualityControl {
public:
    std::string controlId;
    int checkedCount = 0;
    bool approved = true;
    WorkStage stage;
    std::vector<DefectReport> defects;

    void addDefect(const DefectReport& d) {
        defects.push_back(d);
        checkedCount++;
        if (d.severity >= 3) {
            approved = false;
        }
        d.checkDefect();
    }

    int unfixedCount() const {
        int c = 0;
        for (int i = 0; i < (int)defects.size(); i++) {
            if (!defects[i].fixed) c++;
        }
        return c;
    }

    void approveStageIfClean() {
        if (unfixedCount() > 0) {
            throw QualityDefectException("Нельзя принять этап: есть неустранённые дефекты");
        }
        stage.acceptStage();
        approved = true;
    }

    double defectRate() const {
        if (checkedCount <= 0) return 0.0;
        return (double)defects.size() * 100.0 / checkedCount;
    }
};

class ConstructionCompany {
public:
    std::string name;
    std::string inn;
    Budget mainBudget;
    std::vector<Client> clients;
    std::vector<Project> projects;
    std::vector<Site> sites;
    std::vector<Employee> employees;

    void addClient(const Client& c) {
        c.checkData();
        clients.push_back(c);
    }

    void addProject(const Project& p) {
        p.checkProject();
        projects.push_back(p);
    }

    Project& findProjectById(const std::string& projectId) {
        for (int i = 0; i < (int)projects.size(); i++) {
            if (projects[i].id == projectId) {
                return projects[i];
            }
        }
        throw ProjectNotFoundException("Проект с ID " + projectId + " не найден");
    }

    void addSite(const Site& s) {
        sites.push_back(s);
    }

    void hireEmployee(const Employee& e) {
        employees.push_back(e);
    }

    double totalPlannedBudget() const {
        double sum = 0.0;
        for (int i = 0; i < (int)projects.size(); i++) {
            sum += projects[i].budgetPlan;
        }
        return sum;
    }
};