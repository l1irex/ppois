#pragma once
#include <string>
#include "PatientCore.h"

class Medicine {
public:
    std::string code;
    std::string title;
    double price = 10.0;
    bool prescriptionOnly = false;

    double costFor(int qty) const {
        if (qty <= 0) throw BadPrescriptionException();
        return price * qty;
    }
};

class DrugStock {
public:
    Medicine medicine;
    int amount = 50;
    int minAmount = 10;

    bool isLow() const { return amount <= minAmount; }
    void takeDrug(int qty) {
        if (qty <= 0 || amount < qty) throw NoMedicineStockException();
        amount -= qty;
    }
    void addDrug(int qty) {
        if (qty > 0) amount += qty;
    }
};

class Prescription {
public:
    std::string id;
    Patient patient;
    Medicine medicine;
    int packs = 1;

    void verifyForPatient() const {
        patient.checkValid();
        if (patient.hasAllergy(medicine.title)) throw AllergyRiskException();
        if (packs <= 0) throw BadPrescriptionException();
    }
};

class DrugSupplier {
public:
    std::string name;
    std::string phone;
    bool reliable = true;

    void supplyStock(DrugStock& stock, int qty) {
        if (!reliable) throw InsuranceDeniedException("Supplier unreliable");
        stock.addDrug(qty);
    }
};