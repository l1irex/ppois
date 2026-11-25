#pragma once
#include <stdexcept>
#include <string>

// базовый класс исключений для стройки
class ConstructionException : public std::runtime_error {
public:
    ConstructionException(const std::string& msg) : std::runtime_error(msg) {}
};

class InsufficientBudgetException : public ConstructionException {
public:
    InsufficientBudgetException(const std::string& msg) : ConstructionException(msg) {}
};

class InvalidProjectDataException : public ConstructionException {
public:
    InvalidProjectDataException(const std::string& msg) : ConstructionException(msg) {}
};

class ProjectNotFoundException : public ConstructionException {
public:
    ProjectNotFoundException(const std::string& msg) : ConstructionException(msg) {}
};

class InvalidContractException : public ConstructionException {
public:
    InvalidContractException(const std::string& msg) : ConstructionException(msg) {}
};

class InsufficientMaterialException : public ConstructionException {
public:
    InsufficientMaterialException(const std::string& msg) : ConstructionException(msg) {}
};

class EquipmentUnavailableException : public ConstructionException {
public:
    EquipmentUnavailableException(const std::string& msg) : ConstructionException(msg) {}
};

class EquipmentOverloadException : public ConstructionException {
public:
    EquipmentOverloadException(const std::string& msg) : ConstructionException(msg) {}
};

class InvalidQualificationException : public ConstructionException {
public:
    InvalidQualificationException(const std::string& msg) : ConstructionException(msg) {}
};

class SafetyViolationException : public ConstructionException {
public:
    SafetyViolationException(const std::string& msg) : ConstructionException(msg) {}
};

class QualityDefectException : public ConstructionException {
public:
    QualityDefectException(const std::string& msg) : ConstructionException(msg) {}
};

class InvalidScheduleException : public ConstructionException {
public:
    InvalidScheduleException(const std::string& msg) : ConstructionException(msg) {}
};

class InvalidPaymentException : public ConstructionException {
public:
    InvalidPaymentException(const std::string& msg) : ConstructionException(msg) {}
};