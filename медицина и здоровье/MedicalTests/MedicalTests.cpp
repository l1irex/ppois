#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "BillingPay.h"
#include "ClinicSystem.h"
#include "ClinicWard.h"
#include "HealthExceptions.h"
#include "LabDiagnostics.h"
#include "MedCardHistory.h"
#include "OtherStaff.h"
#include "PatientCore.h"
#include "PharmacyStock.h"
#include "StaffDoctors.h"
#include "TreatmentPlan.h"
#include "VisitsDiagnosis.h"

namespace {

    bool approxEqual(double a, double b, double eps = 1e-6) {
        return std::fabs(a - b) <= eps;
    }

    template <typename Exception, typename Func>
    void expectThrow(Func func) {
        bool threw = false;
        try {
            func();
        }
        catch (const Exception&) {
            threw = true;
        }
        catch (...) {
            assert(false && "unexpected exception type");
        }
        assert(threw);
    }

    Patient validPatient(const std::string& id = "P-1") {
        Patient p;
        p.id = id;
        p.name = "Ivanov Ivan";
        p.age = 35;
        p.contact.phone = "1234567";
        p.contact.city = "Minsk";
        p.insurance.limit = 1000.0;
        return p;
    }

    void testPatientCore() {
        Contact contact;
        contact.phone = "12345";
        assert(!contact.hasPhone());
        contact.phone = "1234567";
        contact.city = "Minsk";
        assert(contact.hasPhone());
        assert(contact.info() == "Minsk, 1234567");

        Allergy allergy;
        allergy.name = "Latex";
        allergy.level = 3;
        assert(!allergy.isDangerous());
        allergy.raiseLevel();
        assert(allergy.isDangerous());
        allergy.raiseLevel();
        allergy.raiseLevel();
        assert(allergy.level == 5);

        Vitals vitals;
        assert(!vitals.hasFever());
        vitals.temp = 37.5;
        assert(vitals.hasFever());
        vitals.diaPressure = 90;
        assert(vitals.highPressure());
        vitals.temp = 39.0;
        assert(vitals.isBad());

        Insurance insurance;
        insurance.number = "POL-1";
        insurance.limit = 500.0;
        assert(insurance.canCover(300.0));
        assert(!insurance.canCover(600.0));
        insurance.spend(300.0);
        assert(approxEqual(insurance.limit, 200.0));
        expectThrow<InsuranceDeniedException>([&]() { insurance.spend(500.0); });

        Patient patient;
        expectThrow<InvalidPatientDataException>([&]() { patient.checkValid(); });
        patient = validPatient();
        patient.checkValid();
        Allergy drugAllergy;
        drugAllergy.name = "Penicillin";
        patient.addAllergy(drugAllergy);
        assert(patient.hasAllergy("Penicillin"));
        assert(!patient.hasAllergy("penicillin"));
        assert(!patient.needsUrgentHelp());
        patient.vitals.pulse = 121;
        assert(patient.needsUrgentHelp());
    }

    void testMedCardHistory() {
        ChronicDisease disease;
        disease.code = "E11";
        disease.startYear = 2020;
        assert(disease.yearsSick(2026) == 6);
        assert(disease.yearsSick(2019) == 0);
        disease.setRemission();
        assert(!disease.active);

        Vaccination vaccination;
        vaccination.vaccineName = "Flu";
        vaccination.yearDone = 2025;
        vaccination.validYears = 1;
        assert(vaccination.isValid(2026));
        assert(!vaccination.isValid(2027));
        vaccination.renew(2026);
        assert(vaccination.isValid(2027));

        BodyMetrics metrics;
        metrics.heightCm = 180.0;
        metrics.weightKg = 81.0;
        assert(approxEqual(metrics.bmi(), 25.0));
        assert(metrics.isOverweight());
        metrics.heightCm = 0.0;
        assert(approxEqual(metrics.bmi(), 0.0));

        MedCard card;
        card.cardId = "CARD-1";
        card.patient = validPatient();
        card.addDisease(disease);
        card.addVaccine(vaccination);
        assert(card.activeDiseasesCount() == 0);

        ChronicDisease activeDisease;
        card.addDisease(activeDisease);
        assert(card.activeDiseasesCount() == 1);
        assert(card.diseases.size() == 2);
        assert(card.vaccines.size() == 1);
    }

    void testClinicWard() {
        HospitalBed bed;
        bed.bedNumber = 7;
        bed.occupy();
        assert(bed.occupied);
        expectThrow<TimeConflictException>([&]() { bed.occupy(); });
        bed.freeBed();
        assert(!bed.occupied);

        WardRoom ward;
        ward.roomCode = "W-101";
        ward.totalBeds = 2;
        ward.freeBedsCount = 2;
        assert(ward.hasFreeBed());
        ward.admit();
        ward.admit();
        assert(!ward.hasFreeBed());
        expectThrow<TimeConflictException>([&]() { ward.admit(); });
        ward.release();
        assert(ward.hasFreeBed());
        ward.release();
        ward.release();
        assert(ward.freeBedsCount == 2);

        MedDevice device;
        device.serial = "D-1";
        device.useDevice(3);
        device.useDevice(-5);
        assert(device.hoursUsed == 3);
        device.breakDevice();
        expectThrow<DeviceErrorException>([&]() { device.useDevice(1); });
        device.repairDevice();
        device.useDevice(2);
        assert(device.hoursUsed == 5);

        Sterilizer sterilizer;
        sterilizer.deviceId = "S-1";
        expectThrow<HygieneErrorException>([&]() { sterilizer.runCycle(119); });
        assert(!sterilizer.cycleDone);
        sterilizer.runCycle(121);
        assert(sterilizer.tempC == 121);
        assert(sterilizer.cycleDone);
    }

    void testVisitsDiagnosis() {
        Symptom symptom;
        symptom.severity = 4;
        assert(symptom.isSevere());
        symptom.addDay();
        assert(symptom.days == 2);

        Diagnosis diagnosis;
        diagnosis.code = "J06";
        diagnosis.title = "Acute bronchitis";
        assert(diagnosis.fullText() == "J06: Acute bronchitis");

        Appointment appointment;
        expectThrow<InvalidPatientDataException>([&]() { appointment.complete(); });
        appointment.patient = validPatient();
        appointment.complete();
        assert(appointment.done);

        SickLeave sickLeave;
        sickLeave.patient = validPatient();
        sickLeave.extend(2);
        assert(sickLeave.days == 5);
        expectThrow<InvalidPatientDataException>([&]() { sickLeave.extend(0); });
        sickLeave.closeSheet();
        expectThrow<InvalidPatientDataException>([&]() { sickLeave.extend(1); });

        Referral referral;
        referral.patient = validPatient();
        referral.toSpecialty = "Cardiology";
        referral.useReferral();
        assert(referral.used);
        expectThrow<TimeConflictException>([&]() { referral.useReferral(); });
    }

    void testTreatmentPlan() {
        Procedure procedure;
        procedure.title = "Injections";
        procedure.count = 2;
        procedure.markOneDone();
        procedure.markOneDone();
        procedure.markOneDone();
        assert(procedure.doneCount == 2);
        assert(procedure.isFinished());

        TherapyCourse course;
        course.name = "Physiotherapy";
        course.daysTotal = 3;
        course.passDay();
        assert(course.leftDays() == 2);
        course.passDay();
        course.passDay();
        course.passDay();
        assert(course.daysPassed == 3);
        assert(course.leftDays() == 0);

        TreatmentPlan plan;
        plan.planId = "PLAN-1";
        plan.diagnosis.code = "J06";
        plan.addProcedure(procedure);
        plan.addCourse(course);
        assert(plan.proceduresCount() == 1);
        assert(plan.courses.size() == 1);

        RecoveryNote note;
        note.score = 6;
        assert(!note.isImproving());
        note.score = 7;
        assert(note.isImproving());
    }

    void testStaffDoctors() {
        MedLicense license;
        assert(!license.isValid(2026));
        license.number = "LIC-1";
        assert(license.isValid(2028));
        assert(!license.isValid(2029));
        assert(approxEqual(license.bonusRate(), 1.1));
        license.category = 3;
        assert(approxEqual(license.bonusRate(), 1.3));

        Staff staff;
        staff.salary = 2000.0;
        staff.license = license;
        assert(approxEqual(staff.totalSalary(), 2600.0));
        staff.setBusy(true);
        assert(staff.busy);

        Doctor doctor;
        doctor.maxPatients = 2;
        doctor.takePatient();
        assert(doctor.todayPatients == 1);
        assert(doctor.freeSlots() == 1);
        doctor.takePatient();
        expectThrow<DoctorBusyException>([&]() { doctor.takePatient(); });
        doctor.maxPatients = 5;
        doctor.setBusy(true);
        expectThrow<DoctorBusyException>([&]() { doctor.takePatient(); });

        Surgeon surgeon;
        surgeon.sterileReady = false;
        expectThrow<HygieneErrorException>([&]() { surgeon.doOperation(validPatient()); });
        surgeon.sterileReady = true;
        Patient latexPatient = validPatient("P-LATEX");
        Allergy latex;
        latex.name = "Latex";
        latexPatient.addAllergy(latex);
        expectThrow<AllergyRiskException>([&]() { surgeon.doOperation(latexPatient); });
        surgeon.doOperation(validPatient());
        assert(surgeon.operationsCount == 1);

        Nurse nurse;
        Patient patient = validPatient();
        Allergy penicillin;
        penicillin.name = "Penicillin";
        patient.addAllergy(penicillin);
        expectThrow<AllergyRiskException>([&]() { nurse.giveInjection(patient, "Penicillin", false); });
        nurse.canDoIV = false;
        expectThrow<BadPrescriptionException>([&]() { nurse.giveInjection(validPatient(), "Analgin", true); });
        nurse.canDoIV = true;
        nurse.giveInjection(validPatient(), "Analgin", true);
        assert(nurse.proceduresDone == 1);
    }

    void testOtherStaff() {
        Dentist dentist;
        dentist.treatTooth(1);
        dentist.treatTooth(32);
        assert(dentist.teethTreated == 2);
        expectThrow<InvalidPatientDataException>([&]() { dentist.treatTooth(0); });
        expectThrow<InvalidPatientDataException>([&]() { dentist.treatTooth(33); });

        Pharmacist pharmacist;
        expectThrow<BadPrescriptionException>([&]() { pharmacist.issueDrug("", false); });
        expectThrow<BadPrescriptionException>([&]() { pharmacist.issueDrug("Morphine", true); });
        pharmacist.issueDrug("Aspirin", false);
        pharmacist.canGiveStrong = true;
        pharmacist.issueDrug("Morphine", true);
        assert(pharmacist.issuedDrugs == 2);

        Receptionist receptionist;
        Doctor doctor;
        doctor.maxPatients = 1;
        receptionist.bookVisit(doctor);
        assert(receptionist.bookedCount == 1);
        assert(doctor.todayPatients == 1);
        expectThrow<DoctorBusyException>([&]() { receptionist.bookVisit(doctor); });
        assert(receptionist.bookedCount == 1);

        LabWorker worker;
        expectThrow<BadLabSampleException>([&]() { worker.checkSample(false); });
        worker.checkSample(true);
        assert(worker.testsDone == 1);
    }

    void testLabDiagnostics() {
        LabSample sample;
        expectThrow<BadLabSampleException>([&]() { sample.check(); });
        sample.sampleId = "S-1";
        sample.check();
        sample.spoil();
        expectThrow<BadLabSampleException>([&]() { sample.check(); });

        BloodTest blood;
        assert(blood.isNormal());
        blood.sugar = 6.1;
        assert(!blood.isNormal());
        blood.sugar = 5.0;
        blood.hemoglobin = 114.0;
        assert(!blood.isNormal());

        UrineTest urine;
        assert(urine.isClean());
        urine.proteinFound = true;
        assert(!urine.isClean());
        urine.proteinFound = false;
        urine.density = 1.031;
        assert(!urine.isClean());

        XRayScan scan;
        scan.area = "Chest";
        assert(scan.summary() == "Chest: normal");
        scan.hasFracture = true;
        assert(scan.summary() == "Chest: fracture");

        LabOrder order;
        order.orderId = "ORD-1";
        order.patient = validPatient();
        order.sample = sample;
        expectThrow<BadLabSampleException>([&]() { order.completeOrder(); });
        order.sample = LabSample();
        order.sample.sampleId = "S-2";
        order.completeOrder();
        assert(order.ready);
    }

    void testPharmacy() {
        Medicine medicine;
        medicine.code = "MED-1";
        medicine.title = "Paracetamol";
        medicine.price = 12.0;
        expectThrow<BadPrescriptionException>([&]() { medicine.costFor(0); });
        assert(approxEqual(medicine.costFor(3), 36.0));

        DrugStock stock;
        stock.medicine = medicine;
        stock.amount = 12;
        stock.minAmount = 10;
        assert(!stock.isLow());
        expectThrow<NoMedicineStockException>([&]() { stock.takeDrug(20); });
        stock.takeDrug(5);
        assert(stock.amount == 7);
        assert(stock.isLow());
        stock.addDrug(-10);
        assert(stock.amount == 7);
        stock.addDrug(10);
        assert(stock.amount == 17);

        Prescription prescription;
        prescription.id = "RX-1";
        prescription.patient = validPatient();
        prescription.medicine = medicine;
        prescription.verifyForPatient();
        prescription.packs = 0;
        expectThrow<BadPrescriptionException>([&]() { prescription.verifyForPatient(); });
        prescription.packs = 1;
        Allergy allergy;
        allergy.name = "Paracetamol";
        prescription.patient.addAllergy(allergy);
        expectThrow<AllergyRiskException>([&]() { prescription.verifyForPatient(); });

        DrugSupplier supplier;
        supplier.name = "Pharma";
        supplier.supplyStock(stock, 3);
        assert(stock.amount == 20);
        supplier.reliable = false;
        expectThrow<InsuranceDeniedException>([&]() { supplier.supplyStock(stock, 3); });
    }

    void testBilling() {
        Bill bill;
        bill.billId = "BILL-1";
        expectThrow<NoMoneyException>([&]() { bill.setAmount(-1.0); });
        bill.setAmount(120.0);
        assert(approxEqual(bill.amount, 120.0));
        bill.markPaid();
        assert(bill.paid);

        CashDesk desk;
        assert(approxEqual(desk.balance, 1000.0));
        expectThrow<NoMoneyException>([&]() { desk.acceptCash(0.0); });
        desk.acceptCash(500.0);
        assert(approxEqual(desk.balance, 1500.0));
        expectThrow<NoMoneyException>([&]() { desk.refundCash(2000.0); });
        desk.refundCash(500.0);
        assert(approxEqual(desk.balance, 1000.0));

        InsuranceClaim claim;
        claim.claimId = "CL-1";
        claim.patient = validPatient();
        claim.sum = 200.0;
        claim.processClaim();
        assert(claim.approved);
        assert(approxEqual(claim.patient.insurance.limit, 800.0));
        InsuranceClaim denied;
        denied.patient = validPatient();
        denied.sum = 2000.0;
        expectThrow<InsuranceDeniedException>([&]() { denied.processClaim(); });
        assert(!denied.approved);

        DiscountCard card;
        card.percent = 10;
        assert(approxEqual(card.applyDiscount(200.0), 180.0));
        expectThrow<NoMoneyException>([&]() { card.applyDiscount(-1.0); });
    }

    void testClinicSystem() {
        SanitaryCheck check;
        check.checkId = "SAN-1";
        check.failCheck(30);
        assert(check.score == 70);
        assert(check.passed);
        check.failCheck(15);
        assert(check.score == 55);
        assert(!check.passed);

        QualityReport report;
        report.reportId = "Q-1";
        report.resolveOne();
        assert(report.openComplaints() == 0);
        report.addComplaint();
        report.addComplaint();
        report.resolveOne();
        assert(report.openComplaints() == 1);
        report.resolveOne();
        report.resolveOne();
        assert(report.resolved == 2);
        assert(report.openComplaints() == 0);

        Department department;
        department.closeDept();
        assert(!department.open);
        department.openDept();
        assert(department.open);

        MedicalCenter center;
        center.centerName = "City Medical Center";
        center.addDepartment(department);
        assert(center.departments.size() == 1);
        center.addPatient(validPatient("P-1"));
        center.addPatient(validPatient("P-2"));
        assert(center.totalPatients() == 2);
        Patient& found = center.findPatientById("P-2");
        found.name = "Updated Name";
        assert(center.findPatientById("P-2").name == "Updated Name");
        expectThrow<PatientNotFoundException>([&]() { center.findPatientById("UNKNOWN"); });
        expectThrow<InvalidPatientDataException>([&]() { center.addPatient(Patient()); });
    }

}

int main() {
    testPatientCore();
    testMedCardHistory();
    testClinicWard();
    testVisitsDiagnosis();
    testTreatmentPlan();
    testStaffDoctors();
    testOtherStaff();
    testLabDiagnostics();
    testPharmacy();
    testBilling();
    testClinicSystem();

    std::cout << "All unit tests passed successfully!\n";
    return 0;
}
