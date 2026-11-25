#include "pch.h"
#include "CppUnitTest.h"

#include <cmath>
#include <string>

#include "CoreGeo.h"
#include "Equipment.h"
#include "Exceptions.h"
#include "FinanceContracts.h"
#include "Materials.h"
#include "ProjectsSchedule.h"
#include "QualitySafetyCompany.h"
#include "Staff.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ConstructionCppUnitTests
{
    TEST_CLASS(CoreGeoTests)
    {
    public:
        TEST_METHOD(GeoPoint_Distance_And_Validation)
        {
            GeoPoint p1;
            p1.x = 0.0;
            p1.y = 0.0;
            p1.height = 100.0;

            GeoPoint p2;
            p2.x = 3.0;
            p2.y = 4.0;
            p2.height = 115.0;

            Assert::IsTrue(p1.isValid());
            Assert::AreEqual(5.0, p1.distanceTo(p2), 0.001);
            Assert::AreEqual(15.0, p1.heightDiff(p2), 0.001);

            GeoPoint badPoint;
            badPoint.x = 5000.0;
            Assert::IsFalse(badPoint.isValid());
        }

        TEST_METHOD(Address_Client_Vendor_Checks)
        {
            Address addr;
            Assert::IsFalse(addr.isFilled());
            addr.country = "Belarus";
            addr.city = "Minsk";
            addr.street = "Stroiteley, 10";
            addr.zipCode = "220000";
            Assert::IsTrue(addr.isFilled());
            Assert::IsFalse(addr.fullAddress().empty());

            Client client;
            Assert::ExpectException<InvalidContractException>([&]() {
                client.checkData();
            });

            client.id = "CL-1";
            client.name = "Gorizont";
            client.inn = "190111222";
            client.creditLimit = 10000.0;
            client.checkData();
            Assert::IsTrue(client.canPay(5000.0));
            Assert::IsFalse(client.canPay(15000.0));

            client.addCreditLimit(10000.0);
            Assert::IsTrue(client.canPay(15000.0));

            client.disableClient();
            Assert::IsFalse(client.canPay(100.0));

            Vendor vendor;
            vendor.id = "V-1";
            vendor.name = "BetonStroy";
            vendor.workType = "Concrete";
            Assert::IsTrue(vendor.isGoodVendor());
            vendor.changeRating(-4.0);
            Assert::AreEqual(1.0, vendor.rating, 0.001);
            Assert::IsFalse(vendor.isGoodVendor());
        }
    };

    TEST_CLASS(ProjectsScheduleTests)
    {
    public:
        TEST_METHOD(Objects_Calculations_And_Exceptions)
        {
            Apartment apt;
            apt.number = "12";
            apt.area = 50.0;
            Assert::AreEqual(60000.0, apt.calcPrice(1200.0), 0.001);
            apt.sell();
            Assert::IsTrue(apt.sold);
            Assert::ExpectException<InvalidProjectDataException>([&]() {
                apt.sell();
            });

            LeaseUnit unit;
            unit.code = "OF-1";
            unit.area = 80.0;
            unit.rentPerMeter = 25.0;
            Assert::AreEqual(2000.0, unit.calcMonthRent(), 0.001);
            unit.rentOut();
            Assert::IsTrue(unit.rented);

            RoadSection road;
            road.lengthKm = 2.0;
            road.lanes = 4;
            Assert::AreEqual(28000.0, road.calcArea(), 0.001);
            road.addProgress(60.0);
            road.addProgress(60.0);
            Assert::AreEqual(100.0, road.donePercent, 0.001);
        }

        TEST_METHOD(Task_Schedule_And_Project_Lifecycle)
        {
            WorkStage stage;
            stage.id = "ST-1";
            stage.name = "Foundation";
            stage.weight = 100.0;

            Task task;
            task.id = "T-1";
            task.title = "Pit";
            task.days = 10;
            task.addStage(stage);

            Assert::ExpectException<QualityDefectException>([&]() {
                task.finishTask();
            });

            task.stages[0].acceptStage();
            Assert::IsTrue(task.allStagesAccepted());
            task.addProgress(40.0);
            Assert::AreEqual(6, task.leftDays());
            task.finishTask();
            Assert::IsTrue(task.done);

            WorkSchedule schedule;
            schedule.planDays = 10;
            schedule.addTask(task);
            Assert::AreEqual(100.0, schedule.getProgress(), 0.001);
            Assert::AreEqual(1, schedule.countDoneTasks());

            WorkSchedule lateSchedule;
            lateSchedule.planDays = 10;
            Task unfinishedTask;
            unfinishedTask.days = 10;
            unfinishedTask.addProgress(40.0);
            lateSchedule.addTask(unfinishedTask);
            lateSchedule.passDays(12);
            Assert::IsTrue(lateSchedule.late);
            lateSchedule.addPlanDays(5);
            Assert::IsFalse(lateSchedule.late);

            Project proj;
            proj.id = "PRJ-1";
            proj.name = "House 1";
            proj.budgetPlan = 100000.0;
            proj.client.name = "Client";
            proj.client.inn = "123456";
            proj.site.address.city = "Minsk";
            proj.site.address.street = "Lenina, 5";
            proj.site.areaGa = 2.0;
            proj.site.givePermit();
            proj.site.openSite();
            proj.schedule = schedule;

            proj.startProject();
            Assert::AreEqual(std::string("Active"), proj.status);
            proj.spendMoney(30000.0);
            Assert::AreEqual(70000.0, proj.leftBudget(), 0.001);
            Assert::AreEqual(30.0, proj.spentPercent(), 0.001);

            Assert::ExpectException<InsufficientBudgetException>([&]() {
                proj.spendMoney(90000.0);
            });

            proj.finishProject();
            Assert::AreEqual(std::string("Finished"), proj.status);
        }
    };

    TEST_CLASS(FinanceContractsTests)
    {
    public:
        TEST_METHOD(Estimate_Contract_Payment_And_Budget)
        {
            Estimate est;
            CostItem item;
            item.code = "CI-1";
            item.name = "Rebar";
            item.count = 10.0;
            item.price = 100.0;
            est.addItem(item);
            est.markup = 10.0;
            est.reserve = 5.0;
            Assert::AreEqual(1000.0, est.baseSum(), 0.001);
            Assert::AreEqual(1150.0, est.totalSum(), 0.001);
            est.approve();
            Assert::IsTrue(est.approved);

            Contract contract;
            contract.number = "CTR-1";
            contract.amount = 60000.0;
            contract.months = 6;
            contract.client.name = "Client";
            contract.client.inn = "123456";
            contract.sign();
            Assert::IsTrue(contract.signedContract);
            Assert::AreEqual(10000.0, contract.monthSum(), 0.001);

            BankAccount acc1;
            acc1.accountNumber = "ACC-1";
            acc1.putMoney(2000.0);

            BankAccount acc2;
            acc2.accountNumber = "ACC-2";
            acc1.sendTo(acc2, 500.0);
            Assert::AreEqual(1500.0, acc1.balance, 0.001);
            Assert::AreEqual(500.0, acc2.balance, 0.001);

            Invoice inv;
            inv.number = "INV-1";
            inv.sum = 500.0;

            Payment pay;
            pay.id = "PAY-1";
            pay.sum = 500.0;
            pay.payType = "Transfer";
            pay.makePayment(inv, acc2);
            Assert::IsTrue(pay.done);
            Assert::IsTrue(inv.paid);

            Assert::ExpectException<InvalidPaymentException>([&]() {
                pay.makePayment(inv, acc2);
            });

            TaxCalculator taxCalc;
            Assert::AreEqual(200.0, taxCalc.calcNds(1000.0), 0.001);
            Assert::AreEqual(100.0, taxCalc.calcProfitTax(500.0), 0.001);
            Assert::AreEqual(340.0, taxCalc.calcSalaryTax(1000.0), 0.001);

            ProfitAnalyzer analyzer;
            analyzer.income = 1000.0;
            analyzer.expenses = 750.0;
            analyzer.targetPercent = 20.0;
            Assert::AreEqual(250.0, analyzer.profit(), 0.001);
            Assert::IsTrue(analyzer.isProfitOk());
        }
    };

    TEST_CLASS(StaffAndEquipmentTests)
    {
    public:
        TEST_METHOD(Staff_Safety_And_Qualification)
        {
            Qualification qual;
            qual.title = "Electrician";
            qual.level = 4;

            Worker worker;
            worker.id = "W-1";
            worker.fullName = "Ivanov Ivan";
            worker.hourlyRate = 20.0;
            worker.qualification = qual;
            worker.logHours(16);
            Assert::AreEqual(2, worker.workedDays);
            Assert::AreEqual(320.0, worker.calcPieceworkPay(), 0.001);
            Assert::IsTrue(worker.canDoTask(4));
            Assert::IsFalse(worker.canDoTask(5));

            worker.hasHelmetAndVest = false;
            Assert::ExpectException<SafetyViolationException>([&]() {
                worker.checkSafetyGear();
            });

            Engineer eng;
            eng.fullName = "Petrov Petr";
            Assert::ExpectException<InvalidQualificationException>([&]() {
                eng.verifyLicense();
            });
            eng.licenseNumber = "LIC-101";
            eng.verifyLicense();

            Employee emp;
            emp.id = "EMP-1";
            emp.fullName = "Sidorov Sid";
            emp.baseSalary = 2100.0;
            emp.addWorkedDays(21);
            Assert::AreEqual(2100.0, emp.calcMonthlyPay(21), 0.001);

            HRManager hr;
            hr.hireEmployee(emp);
            Assert::AreEqual(1, hr.countActiveStaff());
            hr.dismissEmployee("EMP-1");
            Assert::AreEqual(0, hr.countActiveStaff());
        }

        TEST_METHOD(Equipment_Operations_And_Exceptions)
        {
            Operator op;
            op.fullName = "Operator 1";
            op.permitCode = "PRM-1";
            op.qualification.level = 3;

            Site site;
            site.name = "Site 1";

            Excavator exc;
            exc.model = "CAT-320";
            exc.maxLoadTons = 5.0;
            exc.bucketVolumeCubicM = 1.5;
            exc.maxDigDepthM = 6.0;

            exc.assignToSite(site, op);
            Assert::IsFalse(exc.available);
            Assert::ExpectException<EquipmentUnavailableException>([&]() {
                exc.checkAvailability();
            });

            exc.releaseFromWork(5);
            Assert::IsTrue(exc.available);
            Assert::AreEqual(5, exc.motorHours);

            Assert::ExpectException<EquipmentOverloadException>([&]() {
                exc.digTrench(10.0, 3.0);
            });

            Crane crane;
            crane.maxLoadTons = 10.0;
            crane.boomLengthM = 20.0;
            Assert::AreEqual(200.0, crane.maxMomentTonMeters(), 0.001);
            crane.liftCargo(5.0, 10.0);

            ConcreteMixer mixer;
            mixer.drumVolumeCubicM = 8.0;
            mixer.loadConcrete(5.0);
            Assert::IsTrue(mixer.drumRotating);
            Assert::AreEqual(3.0, mixer.freeDrumCapacity(), 0.001);
            mixer.pourConcrete(5.0);
            Assert::IsFalse(mixer.drumRotating);
        }
    };

    TEST_CLASS(MaterialsQualityCompanyTests)
    {
    public:
        TEST_METHOD(Materials_Stock_And_Orders)
        {
            Material cement;
            cement.id = "MAT-1";
            cement.name = "Cement M500";
            cement.unit = "bag";
            cement.price = 15.0;
            cement.weightPerUnit = 50.0;

            Assert::AreEqual(150.0, cement.calcCost(10.0), 0.001);
            Assert::AreEqual(500.0, cement.calcWeight(10.0), 0.001);

            MaterialStock stock;
            stock.stockId = "STK-1";
            stock.material = cement;
            stock.totalQty = 100.0;
            stock.minQty = 20.0;

            stock.reserve(30.0);
            Assert::AreEqual(70.0, stock.freeQty(), 0.001);
            stock.writeOff(40.0);
            Assert::AreEqual(60.0, stock.totalQty, 0.001);

            Assert::ExpectException<InsufficientMaterialException>([&]() {
                stock.writeOff(500.0);
            });
        }

        TEST_METHOD(Safety_Quality_And_Company)
        {
            SafetyInspection insp;
            insp.id = "INSP-1";
            insp.site.name = "Site 1";

            SafetyViolation critical;
            critical.id = "VIO-2";
            critical.description = "No safety harness";
            critical.severity = 5;

            Assert::ExpectException<SafetyViolationException>([&]() {
                insp.addViolation(critical);
            });

            QualityControl qc;
            qc.controlId = "QC-1";
            DefectReport defect;
            defect.id = "DEF-1";
            defect.defectText = "Uneven wall";
            defect.severity = 2;
            qc.addDefect(defect);

            Assert::ExpectException<QualityDefectException>([&]() {
                qc.approveStageIfClean();
            });

            qc.defects[0].fixDefect();
            qc.approveStageIfClean();
            Assert::IsTrue(qc.approved);

            ConstructionCompany company;
            company.name = "StroyMaster";
            company.inn = "190123456";

            Project proj;
            proj.id = "PRJ-100";
            proj.name = "Residential Block";
            proj.budgetPlan = 200000.0;
            proj.client.name = "Client";
            proj.client.inn = "123456";

            company.addProject(proj);
            Assert::AreEqual(200000.0, company.totalPlannedBudget(), 0.001);

            Assert::ExpectException<ProjectNotFoundException>([&]() {
                company.findProjectById("UNKNOWN");
            });
        }
    };
}