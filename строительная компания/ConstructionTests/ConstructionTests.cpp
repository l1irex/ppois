#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "CoreGeo.h"
#include "Equipment.h"
#include "Exceptions.h"
#include "FinanceContracts.h"
#include "Materials.h"
#include "ProjectsSchedule.h"
#include "QualitySafetyCompany.h"
#include "Staff.h"

namespace {

    bool approxEqual(double a, double b, double eps = 1e-6) {
        return std::fabs(a - b) <= eps;
    }

    void testCoreGeo() {
        GeoPoint p1;
        p1.x = 0.0;
        p1.y = 0.0;
        p1.height = 100.0;

        GeoPoint p2;
        p2.x = 3.0;
        p2.y = 4.0;
        p2.height = 115.0;

        assert(p1.isValid());
        assert(approxEqual(p1.distanceTo(p2), 5.0));
        assert(approxEqual(p1.heightDiff(p2), 15.0));

        GeoPoint badPoint;
        badPoint.x = 5000.0;
        assert(!badPoint.isValid());

        Address addr;
        assert(!addr.isFilled());
        addr.country = "Belarus";
        addr.city = "Minsk";
        addr.street = "Stroiteley, 10";
        addr.zipCode = "220000";
        addr.point = p1;
        assert(addr.isFilled());
        assert(!addr.fullAddress().empty());

        Client client;
        bool threwClientData = false;
        try {
            client.checkData();
        }
        catch (const InvalidContractException&) {
            threwClientData = true;
        }
        assert(threwClientData);

        client.id = "CL-1";
        client.name = "Gorizont";
        client.inn = "190111222";
        client.creditLimit = 10000.0;
        client.checkData();
        assert(client.canPay(5000.0));
        assert(!client.canPay(15000.0));
        client.addCreditLimit(10000.0);
        assert(client.canPay(15000.0));
        client.disableClient();
        assert(!client.canPay(100.0));

        Vendor vendor;
        vendor.id = "V-1";
        vendor.name = "BetonStroy";
        vendor.workType = "Concrete";
        assert(vendor.isGoodVendor());
        vendor.changeRating(-4.0);
        assert(approxEqual(vendor.rating, 1.0));
        assert(!vendor.isGoodVendor());
        vendor.changeRating(10.0);
        assert(approxEqual(vendor.rating, 5.0));
    }

    void testProjectsSchedule() {
        Apartment apt;
        apt.number = "12";
        apt.area = 50.0;
        assert(approxEqual(apt.calcPrice(1200.0), 60000.0));
        apt.sell();
        assert(apt.sold);

        bool threwSoldTwice = false;
        try {
            apt.sell();
        }
        catch (const InvalidProjectDataException&) {
            threwSoldTwice = true;
        }
        assert(threwSoldTwice);

        LeaseUnit unit;
        unit.code = "OF-1";
        unit.area = 80.0;
        unit.rentPerMeter = 25.0;
        assert(approxEqual(unit.calcMonthRent(), 2000.0));
        unit.rentOut();
        assert(unit.rented);

        RoadSection road;
        road.lengthKm = 2.0;
        road.lanes = 4;
        assert(approxEqual(road.calcArea(), 28000.0));
        road.addProgress(40.0);
        road.addProgress(80.0);
        assert(approxEqual(road.donePercent, 100.0));

        WorkStage stage;
        stage.id = "ST-1";
        stage.name = "Markup";
        stage.weight = 100.0;

        Task task;
        task.id = "T-1";
        task.title = "Earthworks";
        task.days = 10;
        task.addStage(stage);

        bool threwUnaccepted = false;
        try {
            task.finishTask();
        }
        catch (const QualityDefectException&) {
            threwUnaccepted = true;
        }
        assert(threwUnaccepted);

        task.stages[0].acceptStage();
        assert(task.allStagesAccepted());
        task.addProgress(40.0);
        assert(task.leftDays() == 6);
        task.finishTask();
        assert(task.done);
        assert(approxEqual(task.progress, 100.0));

        WorkSchedule schedule;
        schedule.planDays = 10;
        schedule.addTask(task);
        assert(approxEqual(schedule.getProgress(), 100.0));
        assert(schedule.countDoneTasks() == 1);
        schedule.passDays(12);
        assert(schedule.late);
        schedule.addPlanDays(5);
        assert(!schedule.late);

        Site site;
        site.id = "S-1";
        site.name = "Severny";
        site.address.city = "Minsk";
        site.address.street = "Lenina, 5";
        site.areaGa = 2.0;
        site.givePermit();
        site.openSite();
        assert(site.open);
        assert(!site.getSummary().empty());

        Project proj;
        proj.id = "PRJ-1";
        proj.name = "House 1";
        proj.budgetPlan = 100000.0;
        proj.client.name = "Client";
        proj.client.inn = "123456";
        proj.site = site;
        proj.schedule = schedule;

        proj.startProject();
        assert(proj.status == "Active");
        proj.spendMoney(30000.0);
        assert(approxEqual(proj.leftBudget(), 70000.0));
        assert(approxEqual(proj.spentPercent(), 30.0));

        bool threwBudget = false;
        try {
            proj.spendMoney(90000.0);
        }
        catch (const InsufficientBudgetException&) {
            threwBudget = true;
        }
        assert(threwBudget);

        proj.finishProject();
        assert(proj.status == "Finished");
    }

    void testFinanceContracts() {
        Estimate est;
        CostItem item;
        item.code = "CI-1";
        item.name = "Rebar";
        item.count = 10.0;
        item.price = 100.0;
        est.addItem(item);
        est.markup = 10.0;
        est.reserve = 5.0;
        assert(approxEqual(est.baseSum(), 1000.0));
        assert(approxEqual(est.totalSum(), 1150.0));
        est.approve();
        assert(est.approved);

        Contract contract;
        contract.number = "CTR-1";
        contract.amount = 60000.0;
        contract.months = 6;
        contract.client.name = "Client";
        contract.client.inn = "123456";
        contract.sign();
        assert(contract.signedContract);
        assert(approxEqual(contract.monthSum(), 10000.0));

        PaymentSchedule schedule;
        schedule.penaltyPercent = 1.0;
        assert(approxEqual(schedule.calcPenalty(1000.0, 5), 50.0));

        BankAccount acc1;
        acc1.accountNumber = "ACC-1";
        acc1.putMoney(2000.0);

        BankAccount acc2;
        acc2.accountNumber = "ACC-2";
        acc1.sendTo(acc2, 500.0);
        assert(approxEqual(acc1.balance, 1500.0));
        assert(approxEqual(acc2.balance, 500.0));

        Invoice inv;
        inv.number = "INV-1";
        inv.sum = 500.0;

        Payment pay;
        pay.id = "PAY-1";
        pay.sum = 500.0;
        pay.payType = "Transfer";
        pay.makePayment(inv, acc2);
        assert(pay.done);
        assert(inv.paid);
        assert(inv.getState() == "Paid");

        bool threwDoublePay = false;
        try {
            pay.makePayment(inv, acc2);
        }
        catch (const InvalidPaymentException&) {
            threwDoublePay = true;
        }
        assert(threwDoublePay);

        pay.rollbackPayment(acc2, inv);
        assert(!pay.done);
        assert(!inv.paid);

        Budget budget;
        budget.id = "BUD-1";
        budget.totalMoney = 1000.0;
        ExpenseRecord exp;
        exp.id = "EXP-1";
        exp.type = "Materials";
        exp.sum = 950.0;
        budget.addRecord(exp);
        assert(budget.almostEmpty());
        assert(approxEqual(budget.freeMoney(), 50.0));

        TaxCalculator taxCalc;
        assert(approxEqual(taxCalc.calcNds(1000.0), 200.0));
        assert(approxEqual(taxCalc.calcProfitTax(500.0), 100.0));
        assert(approxEqual(taxCalc.calcSalaryTax(1000.0), 340.0));

        ProfitAnalyzer analyzer;
        analyzer.income = 1000.0;
        analyzer.expenses = 750.0;
        analyzer.targetPercent = 20.0;
        assert(approxEqual(analyzer.profit(), 250.0));
        assert(approxEqual(analyzer.realPercent(), 25.0));
        assert(analyzer.isProfitOk());
    }

    void testStaff() {
        Qualification qual;
        qual.title = "Electrician";
        qual.level = 4;

        Worker worker;
        worker.id = "W-1";
        worker.fullName = "Ivanov Ivan";
        worker.hourlyRate = 20.0;
        worker.qualification = qual;
        worker.logHours(16);
        assert(worker.workedDays == 2);
        assert(approxEqual(worker.calcPieceworkPay(), 320.0));
        assert(worker.canDoTask(4));
        assert(!worker.canDoTask(5));

        worker.hasHelmetAndVest = false;
        bool threwSafety = false;
        try {
            worker.checkSafetyGear();
        }
        catch (const SafetyViolationException&) {
            threwSafety = true;
        }
        assert(threwSafety);

        Foreman foreman;
        foreman.brigadeName = "Brigade A";
        foreman.addWorker(worker);
        assert(foreman.countQualifiedWorkers(4) == 1);

        Engineer eng;
        eng.fullName = "Petrov Petr";
        bool threwNoLicense = false;
        try {
            eng.verifyLicense();
        }
        catch (const InvalidQualificationException&) {
            threwNoLicense = true;
        }
        assert(threwNoLicense);

        eng.licenseNumber = "LIC-101";
        eng.verifyLicense();

        Employee emp;
        emp.id = "EMP-1";
        emp.fullName = "Sidorov Sid";
        emp.baseSalary = 2100.0;
        emp.addWorkedDays(21);
        assert(approxEqual(emp.calcMonthlyPay(21), 2100.0));

        Salary sal;
        sal.period = "2026-11";
        sal.grossAmount = 1000.0;
        sal.taxPercent = 13.0;
        sal.employee = emp;
        sal.addBonus(200.0);
        assert(approxEqual(sal.totalWithBonus(), 1200.0));
        assert(approxEqual(sal.calcNetSalary(), 1044.0));
        sal.payOut();
        assert(sal.paid);

        HRManager hr;
        hr.hireEmployee(emp);
        assert(hr.countActiveStaff() == 1);
        assert(approxEqual(hr.averageSalary(), 2100.0));
        hr.dismissEmployee("EMP-1");
        assert(hr.countActiveStaff() == 0);

        Accountant acc;
        acc.registerDoc();
        assert(acc.processedDocs == 1);
        assert(approxEqual(acc.calcTaxForSum(500.0, 20.0), 100.0));
    }

    void testEquipment() {
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
        assert(!exc.available);

        bool threwUnavail = false;
        try {
            exc.checkAvailability();
        }
        catch (const EquipmentUnavailableException&) {
            threwUnavail = true;
        }
        assert(threwUnavail);

        exc.releaseFromWork(5);
        assert(exc.available);
        assert(exc.motorHours == 5);
        exc.digTrench(4.0, 3.0);

        bool threwOverload = false;
        try {
            exc.digTrench(10.0, 3.0);
        }
        catch (const EquipmentOverloadException&) {
            threwOverload = true;
        }
        assert(threwOverload);

        Crane crane;
        crane.maxLoadTons = 10.0;
        crane.boomLengthM = 20.0;
        assert(approxEqual(crane.maxMomentTonMeters(), 200.0));
        crane.liftCargo(5.0, 10.0);
        crane.toggleWindLock(true);

        bool threwCraneLock = false;
        try {
            crane.liftCargo(2.0, 5.0);
        }
        catch (const EquipmentUnavailableException&) {
            threwCraneLock = true;
        }
        assert(threwCraneLock);

        ConcreteMixer mixer;
        mixer.drumVolumeCubicM = 8.0;
        mixer.loadConcrete(5.0);
        assert(mixer.drumRotating);
        assert(approxEqual(mixer.freeDrumCapacity(), 3.0));
        mixer.pourConcrete(5.0);
        assert(!mixer.drumRotating);

        Mechanic mech;
        mech.fullName = "Mechanic 1";
        MaintenanceRecord rec;
        rec.recordId = "MR-1";
        rec.description = "Scheduled check";
        rec.motorHoursAtService = 200;
        rec.mechanic = mech;
        assert(rec.isWarrantyEligible());
        rec.completeService(150.0);
        assert(rec.completed);
        assert(rec.mechanic.repairsCount == 1);
    }

    void testMaterials() {
        Material cement;
        cement.id = "MAT-1";
        cement.name = "Cement M500";
        cement.unit = "bag";
        cement.price = 15.0;
        cement.weightPerUnit = 50.0;

        assert(approxEqual(cement.calcCost(10.0), 150.0));
        assert(approxEqual(cement.calcWeight(10.0), 500.0));

        MaterialStock stock;
        stock.stockId = "STK-1";
        stock.material = cement;
        stock.totalQty = 100.0;
        stock.minQty = 20.0;

        stock.reserve(30.0);
        assert(approxEqual(stock.freeQty(), 70.0));
        stock.writeOff(40.0);
        assert(approxEqual(stock.totalQty, 60.0));

        bool threwMat = false;
        try {
            stock.writeOff(500.0);
        }
        catch (const InsufficientMaterialException&) {
            threwMat = true;
        }
        assert(threwMat);

        MaterialRequirement req;
        req.material = cement;
        req.requiredQty = 50.0;
        assert(approxEqual(req.missingQty(stock), 0.0));
        req.fulfillFromStock(stock);
        assert(req.fulfilled);
        assert(stock.needOrder());

        Rack rack;
        rack.code = "RCK-1";
        rack.maxWeightKg = 1000.0;
        rack.putWeight(400.0);
        assert(approxEqual(rack.freeWeight(), 600.0));

        WarehouseZone zone;
        zone.name = "Main Zone";
        zone.addRack(rack);
        assert(zone.canStoreWeight(500.0));

        DeliveryOrder order;
        order.orderId = "ORD-1";
        order.quantity = 10.0;
        order.deliveryCost = 100.0;
        order.material = cement;
        order.supplier.name = "Supplier 1";
        assert(approxEqual(order.fullOrderPrice(), 250.0));
        order.confirmDelivery();
        assert(order.delivered);
    }

    void testQualitySafetyCompany() {
        SafetyInspection insp;
        insp.id = "INSP-1";
        insp.site.name = "Site 1";

        SafetyViolation minor;
        minor.id = "VIO-1";
        minor.description = "Missing sign";
        minor.severity = 2;
        insp.addViolation(minor);
        assert(insp.passed);
        assert(insp.openViolationsCount() == 1);

        SafetyViolation critical;
        critical.id = "VIO-2";
        critical.description = "No safety harness";
        critical.severity = 5;

        bool threwCritical = false;
        try {
            insp.addViolation(critical);
        }
        catch (const SafetyViolationException&) {
            threwCritical = true;
        }
        assert(threwCritical);

        insp.fixAllViolations();
        assert(insp.openViolationsCount() == 0);

        QualityControl qc;
        qc.controlId = "QC-1";
        DefectReport defect;
        defect.id = "DEF-1";
        defect.defectText = "Uneven wall";
        defect.severity = 2;
        qc.addDefect(defect);

        bool threwDefect = false;
        try {
            qc.approveStageIfClean();
        }
        catch (const QualityDefectException&) {
            threwDefect = true;
        }
        assert(threwDefect);

        qc.defects[0].fixDefect();
        qc.approveStageIfClean();
        assert(qc.approved);
        assert(qc.stage.accepted);

        ConstructionCompany company;
        company.name = "StroyMaster";
        company.inn = "190123456";
        company.mainBudget.id = "BUD-MAIN";
        company.mainBudget.totalMoney = 500000.0;

        Project proj;
        proj.id = "PRJ-100";
        proj.name = "Residential Block";
        proj.budgetPlan = 200000.0;
        proj.client.name = "Client";
        proj.client.inn = "123456";

        company.addProject(proj);
        assert(approxEqual(company.totalPlannedBudget(), 200000.0));

        Project& found = company.findProjectById("PRJ-100");
        assert(found.name == "Residential Block");

        bool threwNotFound = false;
        try {
            company.findProjectById("UNKNOWN");
        }
        catch (const ProjectNotFoundException&) {
            threwNotFound = true;
        }
        assert(threwNotFound);
    }

} // namespace

int main() {
    testCoreGeo();
    testProjectsSchedule();
    testFinanceContracts();
    testStaff();
    testEquipment();
    testMaterials();
    testQualitySafetyCompany();

    std::cout << "All unit tests passed successfully!\n";
    return 0;
}