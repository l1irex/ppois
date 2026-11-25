#pragma once

#include "Exceptions.h"
#include "ProjectsSchedule.h"
#include "Staff.h"
#include <string>
#include <vector>

class Operator : public Employee {
public:
    std::string permitCode;
    int operatingHours = 0;
    bool permitActive = true;
    Qualification qualification;

    void checkPermit(int minLevel) const {
        if (!permitActive || permitCode.empty()) {
            throw InvalidQualificationException("У оператора " + fullName + " неактивен допуск на технику");
        }
        if (!qualification.checkMinLevel(minLevel)) {
            throw InvalidQualificationException("Недостаточный разряд оператора " + fullName);
        }
    }

    void addOperatingHours(int hours) {
        if (hours <= 0) {
            throw InvalidProjectDataException("Моточасы должны быть положительными");
        }
        operatingHours += hours;
    }

    void suspendPermit() {
        permitActive = false;
    }

    bool isReadyForShift() const {
        return active && permitActive && !qualification.isExpired;
    }
};

class Mechanic : public Employee {
public:
    std::string workshop;
    int repairsCount = 0;
    bool certified = true;

    void performRepair(int hoursSpent) {
        if (!active || !certified) {
            throw InvalidQualificationException("Механик " + fullName + " не допущен к ремонту");
        }
        if (hoursSpent <= 0) {
            throw InvalidProjectDataException("Время ремонта должно быть больше нуля");
        }
        repairsCount++;
    }

    bool canInspectHeavyMachinery() const {
        return active && certified;
    }

    void revokeCertification() {
        certified = false;
    }

    std::string mechanicStatus() const {
        return fullName + " (ремонтов: " + std::to_string(repairsCount) + ")";
    }
};

class MaintenanceRecord {
public:
    std::string recordId;
    std::string description;
    double cost = 0.0;
    int motorHoursAtService = 0;
    bool completed = false;
    Mechanic mechanic;

    void completeService(double repairCost) {
        if (repairCost < 0.0) {
            throw InvalidPaymentException("Стоимость ТО не может быть отрицательной");
        }
        cost = repairCost;
        completed = true;
        mechanic.performRepair(2);
    }

    bool isWarrantyEligible() const {
        return motorHoursAtService < 500;
    }

    std::string shortInfo() const {
        return recordId + ": " + description;
    }
};

class Equipment {
public:
    std::string code;
    std::string model;
    double maxLoadTons = 0.0;
    int motorHours = 0;
    bool available = true;
    bool inRepair = false;
    Site currentSite;
    Operator assignedOperator;
    std::vector<MaintenanceRecord> history;

    void checkAvailability() const {
        if (!available || inRepair) {
            throw EquipmentUnavailableException("Техника " + model + " недоступна или на ремонте");
        }
    }

    void checkLoad(double loadTons) const {
        if (loadTons > maxLoadTons) {
            throw EquipmentOverloadException("Превышена грузоподъёмность техники " + model);
        }
    }

    void assignToSite(const Site& site, const Operator& op) {
        checkAvailability();
        op.checkPermit(2);
        currentSite = site;
        assignedOperator = op;
        available = false;
    }

    void releaseFromWork(int workedHours) {
        if (workedHours < 0) {
            throw InvalidProjectDataException("Отработанные часы не могут быть отрицательными");
        }
        motorHours += workedHours;
        available = true;
    }

    void sendToRepair(const MaintenanceRecord& rec) {
        inRepair = true;
        available = false;
        history.push_back(rec);
    }
};

class Excavator : public Equipment {
public:
    double bucketVolumeCubicM = 1.2;
    double maxDigDepthM = 5.5;
    bool hasHydraulicHammer = false;

    double calcDiggingCapacity(int cyclesPerHour) const {
        if (cyclesPerHour <= 0) {
            return 0.0;
        }
        return bucketVolumeCubicM * cyclesPerHour;
    }

    void digTrench(double depthM, double soilWeightTons) {
        checkAvailability();
        checkLoad(soilWeightTons);
        if (depthM > maxDigDepthM) {
            throw EquipmentOverloadException("Глубина траншеи превышает возможности экскаватора");
        }
        motorHours += 1;
    }

    void attachHammer() {
        hasHydraulicHammer = true;
    }
};

class Crane : public Equipment {
public:
    double boomLengthM = 25.0;
    double maxHeightM = 30.0;
    bool windLockEngaged = false;

    void liftCargo(double weightTons, double heightM) {
        checkAvailability();
        checkLoad(weightTons);
        if (windLockEngaged) {
            throw EquipmentUnavailableException("У крана включён ветровой стопор");
        }
        if (heightM > maxHeightM) {
            throw EquipmentOverloadException("Высота подъёма превышает максимум крана");
        }
        motorHours += 1;
    }

    void toggleWindLock(bool state) {
        windLockEngaged = state;
    }

    double maxMomentTonMeters() const {
        return maxLoadTons * boomLengthM;
    }

    bool canReach(double targetHeight) const {
        return targetHeight <= maxHeightM && !windLockEngaged;
    }
};

class ConcreteMixer : public Equipment {
public:
    double drumVolumeCubicM = 8.0;
    double currentConcreteVolume = 0.0;
    bool drumRotating = false;

    void loadConcrete(double volumeCubicM) {
        checkAvailability();
        if (volumeCubicM <= 0.0) {
            throw InvalidProjectDataException("Объём бетона должен быть больше нуля");
        }
        if (currentConcreteVolume + volumeCubicM > drumVolumeCubicM) {
            throw EquipmentOverloadException("Перегруз барабана автобетоносмесителя");
        }
        currentConcreteVolume += volumeCubicM;
        drumRotating = true;
    }

    void pourConcrete(double volumeCubicM) {
        if (volumeCubicM <= 0.0 || volumeCubicM > currentConcreteVolume) {
            throw InsufficientMaterialException("Недостаточно бетона в барабане для выгрузки");
        }
        currentConcreteVolume -= volumeCubicM;
        if (currentConcreteVolume == 0.0) {
            drumRotating = false;
        }
    }

    double freeDrumCapacity() const {
        return drumVolumeCubicM - currentConcreteVolume;
    }
};