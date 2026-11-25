#pragma once
#include "Exceptions.h"
#include <cmath>
#include <string>

class GeoPoint {
public:
    double x = 0.0;
    double y = 0.0;
    double height = 0.0;

    bool isValid() const {
        return x >= -1000.0 && x <= 1000.0 && y >= -1000.0 && y <= 1000.0;
    }

    double distanceTo(const GeoPoint& p) const {
        double dx = p.x - x;
        double dy = p.y - y;
        return std::sqrt(dx * dx + dy * dy);
    }

    double heightDiff(const GeoPoint& p) const {
        return p.height - height;
    }
};

class Address {
public:
    std::string country;
    std::string city;
    std::string street;
    std::string zipCode;
    GeoPoint point;

    bool isFilled() const {
        return !city.empty() && !street.empty();
    }

    std::string fullAddress() const {
        return city + ", " + street + " (" + zipCode + ")";
    }

    double distanceTo(const Address& other) const {
        return point.distanceTo(other.point);
    }
};

class Client {
public:
    std::string id;
    std::string name;
    std::string inn;
    double creditLimit = 0.0;
    bool active = true;

    void checkData() const {
        if (name.empty() || inn.empty()) {
            throw InvalidContractException("Не заполнены данные заказчика");
        }
    }

    bool canPay(double sum) const {
        return active && sum > 0.0 && sum <= creditLimit;
    }

    void addCreditLimit(double addSum) {
        if (addSum <= 0.0) {
            throw InvalidContractException("Сумма увеличения лимита должна быть больше 0");
        }
        creditLimit += addSum;
    }

    void disableClient() {
        active = false;
    }
};

class Vendor {
public:
    std::string id;
    std::string name;
    std::string workType;
    double rating = 5.0;

    void changeRating(double diff) {
        rating += diff;
        if (rating < 0.0) rating = 0.0;
        if (rating > 5.0) rating = 5.0;
    }

    bool isGoodVendor() const {
        return rating >= 3.5;
    }

    std::string getInfo() const {
        return name + " - " + workType;
    }
};