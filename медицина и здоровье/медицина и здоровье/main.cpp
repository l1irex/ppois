#include <iostream>
#include <string>
#include <vector>
#include "Windows.h"

#include "HealthExceptions.h"
#include "PatientCore.h"
#include "MedCardHistory.h"
#include "StaffDoctors.h"
#include "OtherStaff.h"
#include "VisitsDiagnosis.h"
#include "TreatmentPlan.h"
#include "LabDiagnostics.h"
#include "PharmacyStock.h"
#include "BillingPay.h"
#include "ClinicWard.h"
#include "ClinicSystem.h"

namespace {

    void printMenu() {
        std::cout << "\n=== Меню медицинского центра ===\n"
            << " 1. Добавить отделение\n"
            << " 2. Зарегистрировать пациента\n"
            << " 3. Добавить аллергию пациенту\n"
            << " 4. Записать пациента к врачу\n"
            << " 5. Выписать рецепт и проверить аллергию\n"
            << " 6. Выдать лекарство со склада\n"
            << " 7. Занять койку в палате\n"
            << " 8. Принять оплату в кассу\n"
            << " 9. Отчёт по клинике\n"
            << " 0. Выход\n"
            << "Выбор: ";
    }

} // namespace

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    MedicalCenter center;
    center.centerName = "City Medical Center";

    Department defaultDept;
    defaultDept.name = "Therapy";
    defaultDept.roomsCount = 8;
    center.addDepartment(defaultDept);

    Doctor doctor;
    doctor.id = "D-01";
    doctor.name = "Dr. Smirnov";
    doctor.specialty = "Therapist";
    doctor.maxPatients = 10;

    DrugStock stock;
    stock.medicine.code = "MED-1";
    stock.medicine.title = "Paracetamol";
    stock.medicine.price = 12.0;
    stock.amount = 30;

    WardRoom ward;
    ward.roomCode = "W-101";
    ward.totalBeds = 4;
    ward.freeBedsCount = 4;

    std::cout << "Клиника \"" << center.centerName << "\" открыта.\n";

    bool running = true;
    while (running) {
        printMenu();
        int choice = 0;
        if (!(std::cin >> choice)) {
            break;
        }

        try {
            switch (choice) {
            case 1: {
                Department dept;
                std::cout << "Название отделения: ";
                std::cin.ignore();
                std::getline(std::cin, dept.name);
                std::cout << "Количество кабинетов: ";
                std::cin >> dept.roomsCount;
                center.addDepartment(dept);
                std::cout << "Отделение добавлено.\n";
                break;
            }
            case 2: {
                Patient p;
                std::cout << "ID пациента: ";
                std::cin >> p.id;
                std::cout << "ФИО пациента: ";
                std::cin.ignore();
                std::getline(std::cin, p.name);
                std::cout << "Возраст: ";
                std::cin >> p.age;
                std::cout << "Телефон: ";
                std::cin >> p.contact.phone;
                center.addPatient(p);
                std::cout << "Пациент зарегистрирован: [" << p.id << "] " << p.name << '\n';
                break;
            }
            case 3: {
                std::string id;
                std::cout << "ID пациента: ";
                std::cin >> id;
                Patient& p = center.findPatientById(id);

                Allergy a;
                std::cout << "Аллерген (препарат): ";
                std::cin >> a.name;
                std::cout << "Реакция: ";
                std::cin >> a.reaction;
                std::cout << "Уровень опасности (1-5): ";
                std::cin >> a.level;
                p.addAllergy(a);
                std::cout << "Аллергия добавлена в карту пациента.\n";
                break;
            }
            case 4: {
                std::string id;
                std::cout << "ID пациента: ";
                std::cin >> id;
                Patient& p = center.findPatientById(id);
                doctor.takePatient();
                std::cout << "Пациент " << p.name << " записан к " << doctor.name
                    << ". Осталось мест: " << doctor.freeSlots() << '\n';
                break;
            }
            case 5: {
                std::string id;
                std::cout << "ID пациента: ";
                std::cin >> id;
                Patient& p = center.findPatientById(id);

                Prescription rx;
                rx.id = "RX-100";
                rx.patient = p;
                std::cout << "Название препарата: ";
                std::cin >> rx.medicine.title;
                std::cout << "Количество упаковок: ";
                std::cin >> rx.packs;
                rx.verifyForPatient();
                std::cout << "Рецепт одобрен. Противопоказаний нет.\n";
                break;
            }
            case 6: {
                int qty = 0;
                std::cout << "На складе (" << stock.medicine.title << "): " << stock.amount << " уп.\n";
                std::cout << "Сколько выдать: ";
                std::cin >> qty;
                stock.takeDrug(qty);
                std::cout << "Выдано. Остаток на складе: " << stock.amount << " уп.\n";
                break;
            }
            case 7: {
                ward.admit();
                std::cout << "Пациент размещён в палате " << ward.roomCode
                    << ". Свободных коек: " << ward.freeBedsCount << '\n';
                break;
            }
            case 8: {
                double sum = 0.0;
                std::cout << "Сумма оплаты: ";
                std::cin >> sum;
                center.cashDesk.acceptCash(sum);
                std::cout << "Оплата принята. Баланс кассы: " << center.cashDesk.balance << '\n';
                break;
            }
            case 9: {
                std::cout << "\n--- Отчёт по клинике ---\n"
                    << "Название: " << center.centerName << '\n'
                    << "Отделений: " << center.departments.size() << '\n'
                    << "Пациентов в базе: " << center.totalPatients() << '\n'
                    << "Записей к врачу сегодня: " << doctor.todayPatients << '\n'
                    << "Свободных коек в " << ward.roomCode << ": " << ward.freeBedsCount << '\n'
                    << "Баланс кассы: " << center.cashDesk.balance << '\n';
                break;
            }
            case 0: {
                running = false;
                break;
            }
            default:
                std::cout << "Неизвестная команда.\n";
            }
        }
        catch (const MedicalException& ex) {
            std::cerr << "Ошибка: " << ex.what() << '\n';
        }
    }

    std::cout << "Работа завершена.\n";
    return 0;
}