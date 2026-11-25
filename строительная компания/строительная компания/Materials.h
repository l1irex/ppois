#pragma once
#include "CoreGeo.h"
#include "Exceptions.h"
#include "ProjectsSchedule.h"
#include <string>
#include <vector>

class Material {
public:
    std::string id;
    std::string name;
    std::string unit;
    double price = 0.0;
    double weightPerUnit = 0.0;

    double calcCost(double qty) const {
        if (qty <= 0.0) {
            throw InsufficientMaterialException("Количество материала должно быть больше 0");
        }
        return qty * price;
    }

    double calcWeight(double qty) const {
        if (qty <= 0.0) {
            throw InsufficientMaterialException("Количество материала должно быть больше 0");
        }
        return qty * weightPerUnit;
    }

    bool isValid() const {
        return !name.empty() && !unit.empty() && price >= 0.0;
    }

    std::string shortName() const {
        return name + " (" + unit + ")";
    }
};

class Supplier {
public:
    std::string id;
    std::string name;
    std::string phone;
    double rating = 5.0;
    bool active = true;

    void checkActive() const {
        if (!active) {
            throw InvalidContractException("Поставщик " + name + " неактивен");
        }
    }

    void changeRating(double delta) {
        rating += delta;
        if (rating < 0.0) rating = 0.0;
        if (rating > 5.0) rating = 5.0;
    }

    bool isReliable() const {
        return active && rating >= 3.5;
    }

    void blockSupplier() {
        active = false;
    }
};

class DeliveryOrder {
public:
    std::string orderId;
    double quantity = 0.0;
    double deliveryCost = 0.0;
    bool delivered = false;
    Supplier supplier;
    Material material;
    Site targetSite;

    void confirmDelivery() {
        if (delivered) {
            throw InvalidProjectDataException("Поставка " + orderId + " уже принята");
        }
        if (quantity <= 0.0) {
            throw InsufficientMaterialException("Некорректный объём поставки");
        }
        supplier.checkActive();
        delivered = true;
    }

    double fullOrderPrice() const {
        return material.calcCost(quantity) + deliveryCost;
    }

    double totalCargoWeight() const {
        return material.calcWeight(quantity);
    }

    void cancelOrder() {
        if (delivered) {
            throw InvalidProjectDataException("Нельзя отменить уже доставленный заказ");
        }
        quantity = 0.0;
    }
};

class Rack {
public:
    std::string code;
    double maxWeightKg = 1000.0;
    double currentWeightKg = 0.0;

    void putWeight(double kg) {
        if (kg <= 0.0) {
            throw InvalidProjectDataException("Вес должен быть больше 0");
        }
        if (currentWeightKg + kg > maxWeightKg) {
            throw EquipmentOverloadException("Перегруз стеллажа " + code);
        }
        currentWeightKg += kg;
    }

    void takeWeight(double kg) {
        if (kg <= 0.0 || kg > currentWeightKg) {
            throw InvalidProjectDataException("Некорректный вес снятия со стеллажа");
        }
        currentWeightKg -= kg;
    }

    double freeWeight() const {
        return maxWeightKg - currentWeightKg;
    }
};

class WarehouseZone {
public:
    std::string name;
    int shelvesCount = 0;
    bool covered = true;
    std::vector<Rack> racks;

    void addRack(const Rack& r) {
        racks.push_back(r);
        shelvesCount = (int)racks.size();
    }

    double totalFreeCapacity() const {
        double sum = 0.0;
        for (int i = 0; i < (int)racks.size(); i++) {
            sum += racks[i].freeWeight();
        }
        return sum;
    }

    bool canStoreWeight(double kg) const {
        return totalFreeCapacity() >= kg;
    }
};

class MaterialStock {
public:
    std::string stockId;
    double totalQty = 0.0;
    double reservedQty = 0.0;
    double minQty = 0.0;
    Material material;
    WarehouseZone zone;

    double freeQty() const {
        return totalQty - reservedQty;
    }

    void reserve(double qty) {
        if (qty <= 0.0 || qty > freeQty()) {
            throw InsufficientMaterialException("Не хватает материала " + material.name + " для брони");
        }
        reservedQty += qty;
    }

    void writeOff(double qty) {
        if (qty <= 0.0 || qty > totalQty) {
            throw InsufficientMaterialException("Не хватает материала " + material.name + " для списания");
        }
        totalQty -= qty;
        if (reservedQty >= qty) {
            reservedQty -= qty;
        }
    }

    void addStock(double qty) {
        if (qty <= 0.0) {
            throw InsufficientMaterialException("Добавляемое количество должно быть больше 0");
        }
        totalQty += qty;
    }

    bool needOrder() const {
        return freeQty() <= minQty;
    }
};

class MaterialRequirement {
public:
    std::string reqId;
    double requiredQty = 0.0;
    bool fulfilled = false;
    Material material;
    Task task;

    double calcNeedCost() const {
        return material.calcCost(requiredQty);
    }

    void fulfillFromStock(MaterialStock& stock) {
        if (fulfilled) return;
        stock.writeOff(requiredQty);
        fulfilled = true;
    }

    double missingQty(const MaterialStock& stock) const {
        double diff = requiredQty - stock.freeQty();
        return diff > 0.0 ? diff : 0.0;
    }
};