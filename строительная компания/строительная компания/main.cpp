#include <iostream>
#include <string>
#include "Windows.h"

#include "Exceptions.h"
#include "CoreGeo.h"
#include "ProjectsSchedule.h"
#include "FinanceContracts.h"
#include "Staff.h"
#include "Equipment.h"
#include "Materials.h"
#include "QualitySafetyCompany.h"

namespace {

    void printMenu() {
        std::cout << "\n=== Меню строительной компании ===\n"
            << " 1. Добавить строительный проект\n"
            << " 2. Запустить проект в работу\n"
            << " 3. Освоить бюджет по проекту\n"
            << " 4. Нанять сотрудника в штат\n"
            << " 5. Списать стройматериалы со склада\n"
            << " 6. Выделить экскаватор на площадку\n"
            << " 7. Провести проверку техники безопасности\n"
            << " 8. Отчёт по компании\n"
            << " 0. Выход\n"
            << "Выбор: ";
    }

} // namespace

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    ConstructionCompany company;
    company.name = "StroyMaster";
    company.inn = "190123456";

    HRManager hr;

    MaterialStock cementStock;
    cementStock.stockId = "STK-1";
    cementStock.material.id = "MAT-1";
    cementStock.material.name = "Cement M500";
    cementStock.material.unit = "bag";
    cementStock.material.price = 15.0;
    cementStock.totalQty = 200.0;
    cementStock.minQty = 20.0;

    Excavator excavator;
    excavator.model = "CAT-320";
    excavator.maxLoadTons = 5.0;
    excavator.bucketVolumeCubicM = 1.5;
    excavator.maxDigDepthM = 6.0;

    std::cout << "Строительная компания \"" << company.name << "\" готова к работе.\n";

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
                Project proj;
                std::cout << "ID проекта: ";
                std::cin >> proj.id;
                std::cout << "Название объекта: ";
                std::cin.ignore();
                std::getline(std::cin, proj.name);
                std::cout << "Плановый бюджет: ";
                std::cin >> proj.budgetPlan;

                proj.client.name = "City Developer";
                proj.client.inn = "190999888";
                proj.site.name = "Site-" + proj.id;
                proj.site.address.city = "Minsk";
                proj.site.address.street = "Nezavisimosti, 10";
                proj.site.areaGa = 1.5;
                proj.site.givePermit();
                proj.site.openSite();

                company.addProject(proj);
                std::cout << "Проект [" << proj.id << "] добавлен.\n";
                break;
            }
            case 2: {
                std::string id;
                std::cout << "ID проекта: ";
                std::cin >> id;
                Project& proj = company.findProjectById(id);
                proj.startProject();
                std::cout << "Проект \"" << proj.name << "\" переведён в статус: " << proj.status << '\n';
                break;
            }
            case 3: {
                std::string id;
                double sum = 0.0;
                std::cout << "ID проекта: ";
                std::cin >> id;
                Project& proj = company.findProjectById(id);
                std::cout << "Сумма расходов: ";
                std::cin >> sum;
                proj.spendMoney(sum);
                std::cout << "Списано. Остаток бюджета: " << proj.leftBudget() << '\n';
                break;
            }
            case 4: {
                Employee emp;
                std::cout << "ID сотрудника: ";
                std::cin >> emp.id;
                std::cout << "ФИО: ";
                std::cin.ignore();
                std::getline(std::cin, emp.fullName);
                std::cout << "Базовый оклад: ";
                std::cin >> emp.baseSalary;
                hr.hireEmployee(emp);
                std::cout << "Сотрудник принят. Активных сотрудников: " << hr.countActiveStaff() << '\n';
                break;
            }
            case 5: {
                double qty = 0.0;
                std::cout << "На складе (" << cementStock.material.name << "): "
                    << cementStock.freeQty() << " " << cementStock.material.unit << '\n';
                std::cout << "Сколько списать: ";
                std::cin >> qty;
                cementStock.writeOff(qty);
                std::cout << "Материалы списаны. Остаток: " << cementStock.totalQty << '\n';
                break;
            }
            case 6: {
                Site site;
                site.name = "Main Construction Site";
                Operator op;
                op.fullName = "Ivanov A.A.";
                op.permitCode = "OP-77";
                op.qualification.level = 3;

                excavator.assignToSite(site, op);
                std::cout << "Экскаватор " << excavator.model << " назначен на площадку.\n";
                break;
            }
            case 7: {
                SafetyInspection insp;
                insp.id = "INSP-1";
                insp.site.name = "Main Construction Site";

                SafetyViolation vio;
                vio.id = "VIO-1";
                std::cout << "Описание нарушения: ";
                std::cin.ignore();
                std::getline(std::cin, vio.description);
                std::cout << "Критичность (1-5): ";
                std::cin >> vio.severity;

                insp.addViolation(vio);
                std::cout << "Нарушение зафиксировано (некритичное).\n";
                break;
            }
            case 8: {
                std::cout << "\n--- Отчёт по строительной компании ---\n"
                    << "Компания: " << company.name << " (УНП " << company.inn << ")\n"
                    << "Проектов в работе: " << company.projects.size() << '\n'
                    << "Общий плановый бюджет: " << company.totalPlannedBudget() << '\n'
                    << "Сотрудников в штате: " << hr.countActiveStaff() << '\n'
                    << "Остаток цемента на складе: " << cementStock.totalQty << '\n';
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
        catch (const std::exception& ex) {
            std::cerr << "Ошибка: " << ex.what() << '\n';
        }
    }

    std::cout << "Работа завершена.\n";
    return 0;
}