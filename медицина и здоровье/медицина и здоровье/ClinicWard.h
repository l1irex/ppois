#pragma once
#include <string>
#include "PatientCore.h"

class HospitalBed {
public:
    int bedNumber = 1;
    bool occupied = false;

    void occupy() {
        if (occupied) throw TimeConflictException("Bed is already occupied");
        occupied = true;
    }
    void freeBed() { occupied = false; }
};

class WardRoom {
public:
    std::string roomCode;
    int totalBeds = 4;
    int freeBedsCount = 4;

    bool hasFreeBed() const { return freeBedsCount > 0; }
    void admit() {
        if (freeBedsCount <= 0) throw TimeConflictException("No free beds in ward");
        freeBedsCount--;
    }
    void release() {
        if (freeBedsCount < totalBeds) freeBedsCount++;
    }
    double occupancyRate() const {
        return totalBeds > 0 ? static_cast<double>(totalBeds - freeBedsCount) / totalBeds : 0.0;
    }
};

class MedDevice {
public:
    std::string serial;
    std::string name;
    bool working = true;
    int hoursUsed = 0;

    void useDevice(int h) {
        if (!working) throw DeviceErrorException();
        if (h > 0) hoursUsed += h;
    }
    void breakDevice() { working = false; }
    void repairDevice() { working = true; }
    bool needsService() const { return hoursUsed >= 100; }
};

class Sterilizer {
public:
    std::string deviceId;
    int tempC = 20;
    bool cycleDone = false;

    void runCycle(int targetTemp) {
        if (targetTemp < 120) throw HygieneErrorException("Sterilization temperature too low");
        tempC = targetTemp;
        cycleDone = true;
    }
};