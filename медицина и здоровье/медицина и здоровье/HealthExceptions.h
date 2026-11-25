#pragma once
#include <exception>
#include <string>

class MedicalException : public std::exception {
protected:
    std::string msg;
public:
    MedicalException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class PatientNotFoundException : public MedicalException {
public:
    PatientNotFoundException(const std::string& m = "Patient not found") : MedicalException(m) {}
};

class InvalidPatientDataException : public MedicalException {
public:
    InvalidPatientDataException(const std::string& m = "Bad patient data") : MedicalException(m) {}
};

class DoctorBusyException : public MedicalException {
public:
    DoctorBusyException(const std::string& m = "Doctor is busy") : MedicalException(m) {}
};

class TimeConflictException : public MedicalException {
public:
    TimeConflictException(const std::string& m = "Time slot conflict") : MedicalException(m) {}
};

class AllergyRiskException : public MedicalException {
public:
    AllergyRiskException(const std::string& m = "Allergy risk") : MedicalException(m) {}
};

class BadPrescriptionException : public MedicalException {
public:
    BadPrescriptionException(const std::string& m = "Invalid prescription") : MedicalException(m) {}
};

class NoMedicineStockException : public MedicalException {
public:
    NoMedicineStockException(const std::string& m = "Not enough medicine") : MedicalException(m) {}
};

class DrugConflictException : public MedicalException {
public:
    DrugConflictException(const std::string& m = "Drug interaction conflict") : MedicalException(m) {}
};

class BadLabSampleException : public MedicalException {
public:
    BadLabSampleException(const std::string& m = "Bad lab sample") : MedicalException(m) {}
};

class InsuranceDeniedException : public MedicalException {
public:
    InsuranceDeniedException(const std::string& m = "Insurance denied") : MedicalException(m) {}
};

class NoMoneyException : public MedicalException {
public:
    NoMoneyException(const std::string& m = "Not enough money") : MedicalException(m) {}
};

class DeviceErrorException : public MedicalException {
public:
    DeviceErrorException(const std::string& m = "Device error") : MedicalException(m) {}
};

class HygieneErrorException : public MedicalException {
public:
    HygieneErrorException(const std::string& m = "Hygiene violation") : MedicalException(m) {}
};