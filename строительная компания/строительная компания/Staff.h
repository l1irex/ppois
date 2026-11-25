#pragma once

#include "Exceptions.h"
#include "ProjectsSchedule.h"
#include <string>
#include <vector>

class Qualification {
public:
    std::string title;
    int level = 1; // разряд или уровень допуска
    int validYears = 3;
    bool isExpired = false;

    bool checkMinLevel(int requiredLevel) const {
        if (isExpired) {
            return false;
        }
        return level >= requiredLevel;
    }

    void renewQualification(int extraYears) {
        if (extraYears <= 0) {
            throw InvalidQualificationException("Срок продления допуска должен быть больше 0");
        }
        validYears += extraYears;
        isExpired = false;
    }

    void expire() {
        isExpired = true;
    }
};

class Employee {
public:
    std::string id;
    std::string fullName;
    std::string position;
    double baseSalary = 0.0;
    int workedDays = 0;
    bool active = true;

    void addWorkedDays(int days) {
        if (days <= 0) {
            throw InvalidProjectDataException("Нельзя добавить 0 или меньше рабочих дней");
        }
        workedDays += days;
    }

    double calcMonthlyPay(int normDays = 21) const {
        if (normDays <= 0) {
            return 0.0;
        }
        return baseSalary * (static_cast<double>(workedDays) / normDays);
    }

    void fireEmployee() {
        active = false;
    }

    std::string info() const {
        return fullName + " (" + position + ")";
    }
};

class Salary {
public:
    std::string period;
    double grossAmount = 0.0;
    double bonus = 0.0;
    double taxPercent = 13.0;
    bool paid = false;
    Employee employee;

    void addBonus(double amount) {
        if (amount < 0.0) {
            throw InvalidPaymentException("Премия не может быть отрицательной");
        }
        bonus += amount;
    }

    double calcNetSalary() const {
        double total = grossAmount + bonus;
        return total - (total * taxPercent / 100.0);
    }

    void payOut() {
        if (paid) {
            throw InvalidPaymentException("Зарплата за период " + period + " уже выплачена");
        }
        if (!employee.active) {
            throw InvalidPaymentException("Сотрудник уволен, выплата невозможна");
        }
        paid = true;
    }

    double totalWithBonus() const {
        return grossAmount + bonus;
    }
};

class Worker : public Employee {
public:
    std::string specialty;
    int hoursWorked = 0;
    double hourlyRate = 0.0;
    bool hasHelmetAndVest = true;
    Qualification qualification;

    void logHours(int hours) {
        if (hours <= 0) {
            throw InvalidProjectDataException("Часы работы должны быть положительными");
        }
        hoursWorked += hours;
        workedDays = hoursWorked / 8;
    }

    double calcPieceworkPay() const {
        return hoursWorked * hourlyRate;
    }

    void checkSafetyGear() const {
        if (!hasHelmetAndVest) {
            throw SafetyViolationException("Рабочий " + fullName + " вышел без каски или жилета");
        }
    }

    bool canDoTask(int reqLevel) const {
        return active && qualification.checkMinLevel(reqLevel);
    }
};

class Foreman : public Employee {
public:
    std::string brigadeName;
    int brigadeSize = 0;
    bool onSite = true;
    std::vector<Worker> workers;

    void addWorker(const Worker& w) {
        workers.push_back(w);
        brigadeSize = static_cast<int>(workers.size());
    }

    void checkAllBrigadeGear() const {
        for (const auto& w : workers) {
            w.checkSafetyGear();
        }
    }

    int countQualifiedWorkers(int minLevel) const {
        int cnt = 0;
        for (const auto& w : workers) {
            if (w.canDoTask(minLevel)) {
                cnt++;
            }
        }
        return cnt;
    }

    void sendToSite() {
        onSite = true;
    }
};

class Engineer : public Employee {
public:
    std::string specialization;
    std::string licenseNumber;
    int inspectedObjects = 0;
    bool licenseValid = true;
    Site assignedSite;

    void verifyLicense() const {
        if (!licenseValid || licenseNumber.empty()) {
            throw InvalidQualificationException("У инженера " + fullName + " нет действующего допуска");
        }
    }

    void assignSite(const Site& site) {
        verifyLicense();
        assignedSite = site;
        inspectedObjects++;
    }

    bool canApproveDrawings() const {
        return active && licenseValid && !licenseNumber.empty();
    }

    void revokeLicense() {
        licenseValid = false;
    }

    std::string getSiteSummary() const {
        return assignedSite.getSummary();
    }
};

class Accountant : public Employee {
public:
    std::string certNumber;
    int processedDocs = 0;
    bool canApprovePayments = true;

    void registerDoc() {
        if (!active) {
            throw InvalidPaymentException("Бухгалтер не активен");
        }
        processedDocs++;
    }

    double calcTaxForSum(double sum, double rate) const {
        if (sum < 0.0 || rate < 0.0) {
            throw InvalidPaymentException("Неверные значения для расчёта налога");
        }
        return sum * rate / 100.0;
    }

    bool checkPaymentLimit(double amount, double limit) const {
        return canApprovePayments && amount <= limit;
    }

    void resetDocCounter() {
        processedDocs = 0;
    }

    void blockPayments() {
        canApprovePayments = false;
    }
};

class HRManager : public Employee {
public:
    int hiresCount = 0;
    int dismissalsCount = 0;
    std::vector<Employee> staffList;

    void hireEmployee(const Employee& emp) {
        staffList.push_back(emp);
        hiresCount++;
    }

    void dismissEmployee(const std::string& empId) {
        for (auto& emp : staffList) {
            if (emp.id == empId && emp.active) {
                emp.fireEmployee();
                dismissalsCount++;
                return;
            }
        }
        throw InvalidProjectDataException("Сотрудник с ID " + empId + " не найден");
    }

    int countActiveStaff() const {
        int cnt = 0;
        for (const auto& emp : staffList) {
            if (emp.active) {
                cnt++;
            }
        }
        return cnt;
    }

    void checkWorkerQualification(const Worker& w, int minLevel) const {
        if (!w.canDoTask(minLevel)) {
            throw InvalidQualificationException("Недостаточный уровень квалификации у " + w.fullName);
        }
    }

    double averageSalary() const {
        if (staffList.empty()) {
            return 0.0;
        }
        double sum = 0.0;
        for (const auto& emp : staffList) {
            sum += emp.baseSalary;
        }
        return sum / staffList.size();
    }
};