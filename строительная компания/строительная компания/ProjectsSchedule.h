#pragma once
#include "CoreGeo.h"
#include "Exceptions.h"
#include <string>
#include <vector>

class Apartment {
public:
    std::string number;
    int floor = 1;
    double area = 0.0;
    bool sold = false;

    double calcPrice(double pricePerMeter) const {
        if (area <= 0.0 || pricePerMeter <= 0.0) {
            throw InvalidProjectDataException("Неверная площадь или цена метра");
        }
        return area * pricePerMeter;
    }

    void sell() {
        if (sold) {
            throw InvalidProjectDataException("Квартира уже продана");
        }
        sold = true;
    }
};

class LeaseUnit {
public:
    std::string code;
    double area = 0.0;
    double rentPerMeter = 0.0;
    bool rented = false;

    double calcMonthRent() const {
        return area * rentPerMeter;
    }

    void rentOut() {
        if (rented) {
            throw InvalidProjectDataException("Помещение уже сдано");
        }
        rented = true;
    }
};

class RoadSection {
public:
    std::string code;
    double lengthKm = 0.0;
    int lanes = 2;
    double donePercent = 0.0;

    void addProgress(double percent) {
        if (percent < 0.0) {
            throw InvalidScheduleException("Прогресс не может быть отрицательным");
        }
        donePercent += percent;
        if (donePercent > 100.0) donePercent = 100.0;
    }

    double calcArea() const {
        return lengthKm * 1000.0 * lanes * 3.5;
    }
};

class WorkStage {
public:
    std::string id;
    std::string name;
    double weight = 0.0;
    bool accepted = false;

    void acceptStage() {
        accepted = true;
    }

    void rejectStage() {
        accepted = false;
    }

    double calcContribution(double totalProgress) const {
        return totalProgress * weight / 100.0;
    }
};

class Task {
public:
    std::string id;
    std::string title;
    int days = 1;
    double progress = 0.0;
    bool done = false;
    std::vector<WorkStage> stages;

    void addProgress(double delta) {
        if (delta < 0.0) {
            throw InvalidScheduleException("Неверный прирост прогресса");
        }
        progress += delta;
        if (progress >= 100.0) {
            progress = 100.0;
            done = true;
        }
    }

    void addStage(const WorkStage& s) {
        stages.push_back(s);
    }

    bool allStagesAccepted() const {
        for (int i = 0; i < (int)stages.size(); i++) {
            if (!stages[i].accepted) return false;
        }
        return true;
    }

    int leftDays() const {
        double rem = days * (1.0 - progress / 100.0);
        return (int)rem;
    }

    void finishTask() {
        if (!allStagesAccepted()) {
            throw QualityDefectException("Не все этапы приняты по задаче " + title);
        }
        progress = 100.0;
        done = true;
    }
};

class WorkSchedule {
public:
    std::string id;
    int planDays = 0;
    int passedDays = 0;
    bool late = false;
    std::vector<Task> tasks;

    void addTask(const Task& t) {
        tasks.push_back(t);
    }

    void passDays(int d) {
        if (d <= 0) {
            throw InvalidScheduleException("Число дней должно быть больше 0");
        }
        passedDays += d;
        if (passedDays > planDays && getProgress() < 100.0) {
            late = true;
        }
    }

    double getProgress() const {
        if (tasks.empty()) return 0.0;
        double sum = 0.0;
        for (int i = 0; i < (int)tasks.size(); i++) {
            sum += tasks[i].progress;
        }
        return sum / tasks.size();
    }

    int countDoneTasks() const {
        int c = 0;
        for (int i = 0; i < (int)tasks.size(); i++) {
            if (tasks[i].done) c++;
        }
        return c;
    }

    void addPlanDays(int extra) {
        if (extra <= 0) {
            throw InvalidScheduleException("Добавляемые дни должны быть больше 0");
        }
        planDays += extra;
        if (passedDays <= planDays) {
            late = false;
        }
    }
};

class Site {
public:
    std::string id;
    std::string name;
    Address address;
    double areaGa = 0.0;
    bool permit = false;
    bool open = false;

    void givePermit() {
        if (!address.isFilled() || areaGa <= 0.0) {
            throw InvalidProjectDataException("Не хватает данных для разрешения на стройку");
        }
        permit = true;
    }

    void openSite() {
        if (!permit) {
            throw InvalidProjectDataException("Нет разрешения на открытие площадки");
        }
        open = true;
    }

    void closeSite() {
        open = false;
    }

    double distToSite(const Site& other) const {
        return address.distanceTo(other.address);
    }

    std::string getSummary() const {
        return name + ": " + address.fullAddress();
    }
};

class Project {
public:
    std::string id;
    std::string name;
    std::string status = "New";
    double budgetPlan = 0.0;
    double budgetSpent = 0.0;
    Client client;
    Site site;
    WorkSchedule schedule;

    void checkProject() const {
        if (name.empty() || budgetPlan <= 0.0) {
            throw InvalidProjectDataException("Неверные данные проекта");
        }
        client.checkData();
    }

    void spendMoney(double sum) {
        if (sum <= 0.0) {
            throw InvalidProjectDataException("Сумма расхода должна быть больше 0");
        }
        if (budgetSpent + sum > budgetPlan) {
            throw InsufficientBudgetException("Превышен бюджет проекта " + name);
        }
        budgetSpent += sum;
    }

    double leftBudget() const {
        return budgetPlan - budgetSpent;
    }

    double spentPercent() const {
        if (budgetPlan <= 0.0) return 0.0;
        return budgetSpent * 100.0 / budgetPlan;
    }

    void startProject() {
        checkProject();
        site.openSite();
        status = "Active";
    }

    void finishProject() {
        if (schedule.getProgress() < 100.0) {
            throw InvalidScheduleException("График ещё не выполнен на 100%");
        }
        status = "Finished";
    }
};

class ResidentialProject : public Project {
public:
    int floors = 1;
    int parkingCount = 0;
    bool hasLift = true;
    std::vector<Apartment> apartments;

    void addApartment(const Apartment& a) {
        apartments.push_back(a);
    }

    double totalArea() const {
        double sum = 0.0;
        for (int i = 0; i < (int)apartments.size(); i++) {
            sum += apartments[i].area;
        }
        return sum;
    }

    int countUnsold() const {
        int c = 0;
        for (int i = 0; i < (int)apartments.size(); i++) {
            if (!apartments[i].sold) c++;
        }
        return c;
    }
};

class CommercialProject : public Project {
public:
    std::string levelClass;
    int parkingCount = 0;
    bool hasGate = false;
    std::vector<LeaseUnit> units;

    void addUnit(const LeaseUnit& u) {
        units.push_back(u);
    }

    double totalMonthRent() const {
        double sum = 0.0;
        for (int i = 0; i < (int)units.size(); i++) {
            sum += units[i].calcMonthRent();
        }
        return sum;
    }

    double rentedPercent() const {
        if (units.empty()) return 0.0;
        int r = 0;
        for (int i = 0; i < (int)units.size(); i++) {
            if (units[i].rented) r++;
        }
        return r * 100.0 / units.size();
    }
};

class InfrastructureProject : public Project {
public:
    std::string typeName;
    double speedLimit = 60.0;
    bool ecoCheck = true;
    std::vector<RoadSection> sections;

    void addSection(const RoadSection& s) {
        sections.push_back(s);
    }

    double totalLength() const {
        double sum = 0.0;
        for (int i = 0; i < (int)sections.size(); i++) {
            sum += sections[i].lengthKm;
        }
        return sum;
    }

    double avgProgress() const {
        if (sections.empty()) return 0.0;
        double sum = 0.0;
        for (int i = 0; i < (int)sections.size(); i++) {
            sum += sections[i].donePercent;
        }
        return sum / sections.size();
    }
};