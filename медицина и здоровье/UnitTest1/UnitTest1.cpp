#include "pch.h"
#include "CppUnitTest.h"

#include <cmath>
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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MedicalCppUnitTests
{
    bool approxEqual(double a, double b, double eps = 1e-6) {
        return std::fabs(a - b) <= eps;
    }

    Patient validPatient(const std::string& id = "P-1") {
        Patient p;
        p.id = id;
        p.name = "Ivanov Ivan";
        p.age = 35;
        p.contact.phone = "1234567";
        p.insurance.limit = 1000.0;
        return p;
    }

    TEST_CLASS(PatientCoreTests)
    {
    public:
        TEST_METHOD(Contact_Allergy_Vitals_Checks)
        {
            Contact contact;
            Assert::IsFalse(contact.hasPhone());
            contact.phone = "1234567";
            contact.city = "Minsk";
            Assert::IsTrue(contact.hasPhone());
            Assert::AreEqual(std::string("Minsk, 1234567"), contact.info());

            Allergy allergy;
            Assert::IsFalse(allergy.isDangerous());
            allergy.level = 3;
            allergy.raiseLevel();
            Assert::IsTrue(allergy.isDangerous());
            allergy.raiseLevel();
            allergy.raiseLevel();
            Assert::AreEqual(5, allergy.level);

            Vitals vitals;
            Assert::IsFalse(vitals.hasFever());
            vitals.temp = 37.5;
            Assert::IsTrue(vitals.hasFever());
            vitals.diaPressure = 90;
            Assert::IsTrue(vitals.highPressure());
            vitals.temp = 39.0;
            Assert::IsTrue(vitals.isBad());
        }

        TEST_METHOD(Insurance_And_Patient_Validation)
        {
            Insurance insurance;
            insurance.limit = 500.0;
            Assert::IsTrue(insurance.canCover(300.0));
            Assert::IsFalse(insurance.canCover(600.0));
            insurance.spend(300.0);
            Assert::AreEqual(200.0, insurance.limit, 0.001);
            Assert::ExpectException<InsuranceDeniedException>([&]() {
                insurance.spend(500.0);
            });

            Patient patient;
            Assert::ExpectException<InvalidPatientDataException>([&]() {
                patient.checkValid();
            });

            patient = validPatient();
            patient.checkValid();
            Allergy allergy;
            allergy.name = "Penicillin";
            patient.addAllergy(allergy);
            Assert::IsTrue(patient.hasAllergy("Penicillin"));
            Assert::IsFalse(patient.needsUrgentHelp());
            patient.vitals.pulse = 121;
            Assert::IsTrue(patient.needsUrgentHelp());
        }
    };

    TEST_CLASS(MedicalHistoryTests)
    {
    public:
        TEST_METHOD(Disease_Vaccination_Metrics_And_Card)
        {
            ChronicDisease disease;
            disease.startYear = 2020;
            Assert::AreEqual(6, disease.yearsSick(2026));
            Assert::AreEqual(0, disease.yearsSick(2019));
            disease.setRemission();
            Assert::IsFalse(disease.active);

            Vaccination vaccination;
            vaccination.yearDone = 2025;
            vaccination.validYears = 1;
            Assert::IsTrue(vaccination.isValid(2026));
            Assert::IsFalse(vaccination.isValid(2027));
            vaccination.renew(2026);
            Assert::IsTrue(vaccination.isValid(2027));

            BodyMetrics metrics;
            metrics.heightCm = 180.0;
            metrics.weightKg = 81.0;
            Assert::AreEqual(25.0, metrics.bmi(), 0.001);
            Assert::IsTrue(metrics.isOverweight());

            MedCard card;
            card.patient = validPatient();
            card.addDisease(disease);
            card.addVaccine(vaccination);
            Assert::AreEqual(0, card.activeDiseasesCount());
            ChronicDisease activeDisease;
            card.addDisease(activeDisease);
            Assert::AreEqual(1, card.activeDiseasesCount());
        }
    };

    TEST_CLASS(WardDeviceTests)
    {
    public:
        TEST_METHOD(Beds_Ward_Device_And_Sterilizer)
        {
            HospitalBed bed;
            bed.occupy();
            Assert::IsTrue(bed.occupied);
            Assert::ExpectException<TimeConflictException>([&]() { bed.occupy(); });
            bed.freeBed();
            Assert::IsFalse(bed.occupied);

            WardRoom ward;
            ward.totalBeds = 2;
            ward.freeBedsCount = 2;
            ward.admit();
            ward.admit();
            Assert::IsFalse(ward.hasFreeBed());
            Assert::ExpectException<TimeConflictException>([&]() { ward.admit(); });
            ward.release();
            Assert::IsTrue(ward.hasFreeBed());

            MedDevice device;
            device.useDevice(3);
            device.useDevice(-5);
            Assert::AreEqual(3, device.hoursUsed);
            device.breakDevice();
            Assert::ExpectException<DeviceErrorException>([&]() { device.useDevice(1); });
            device.repairDevice();
            device.useDevice(2);
            Assert::AreEqual(5, device.hoursUsed);

            Sterilizer sterilizer;
            Assert::ExpectException<HygieneErrorException>([&]() { sterilizer.runCycle(119); });
            sterilizer.runCycle(121);
            Assert::IsTrue(sterilizer.cycleDone);
            Assert::AreEqual(121, sterilizer.tempC);
        }
    };

    TEST_CLASS(VisitsTreatmentTests)
    {
    public:
        TEST_METHOD(Symptoms_Diagnosis_Appointment_SickLeave_Referral)
        {
            Symptom symptom;
            symptom.severity = 4;
            Assert::IsTrue(symptom.isSevere());
            symptom.addDay();
            Assert::AreEqual(2, symptom.days);

            Diagnosis diagnosis;
            diagnosis.code = "J06";
            diagnosis.title = "Bronchitis";
            Assert::AreEqual(std::string("J06: Bronchitis"), diagnosis.fullText());

            Appointment appointment;
            Assert::ExpectException<InvalidPatientDataException>([&]() { appointment.complete(); });
            appointment.patient = validPatient();
            appointment.complete();
            Assert::IsTrue(appointment.done);

            SickLeave sickLeave;
            sickLeave.extend(2);
            Assert::AreEqual(5, sickLeave.days);
            Assert::ExpectException<InvalidPatientDataException>([&]() { sickLeave.extend(0); });
            sickLeave.closeSheet();
            Assert::ExpectException<InvalidPatientDataException>([&]() { sickLeave.extend(1); });

            Referral referral;
            referral.useReferral();
            Assert::IsTrue(referral.used);
            Assert::ExpectException<TimeConflictException>([&]() { referral.useReferral(); });
        }

        TEST_METHOD(Procedures_Courses_Plan_And_Recovery)
        {
            Procedure procedure;
            procedure.count = 2;
            procedure.markOneDone();
            procedure.markOneDone();
            procedure.markOneDone();
            Assert::AreEqual(2, procedure.doneCount);
            Assert::IsTrue(procedure.isFinished());

            TherapyCourse course;
            course.daysTotal = 3;
            course.passDay();
            Assert::AreEqual(2, course.leftDays());
            course.passDay();
            course.passDay();
            course.passDay();
            Assert::AreEqual(0, course.leftDays());

            TreatmentPlan plan;
            plan.addProcedure(procedure);
            plan.addCourse(course);
            Assert::AreEqual(1, plan.proceduresCount());
            Assert::AreEqual(1, static_cast<int>(plan.courses.size()));

            RecoveryNote note;
            Assert::IsFalse(note.isImproving());
            note.score = 7;
            Assert::IsTrue(note.isImproving());
        }
    };

    TEST_CLASS(StaffTests)
    {
    public:
        TEST_METHOD(License_Staff_Doctor_Surgeon_Nurse)
        {
            MedLicense license;
            Assert::IsFalse(license.isValid(2026));
            license.number = "LIC-1";
            Assert::IsTrue(license.isValid(2028));
            Assert::AreEqual(1.1, license.bonusRate(), 0.001);

            Staff staff;
            staff.salary = 2000.0;
            staff.license = license;
            Assert::AreEqual(2200.0, staff.totalSalary(), 0.001);
            staff.setBusy(true);
            Assert::IsTrue(staff.busy);

            Doctor doctor;
            doctor.maxPatients = 2;
            doctor.takePatient();
            Assert::AreEqual(1, doctor.freeSlots());
            doctor.takePatient();
            Assert::ExpectException<DoctorBusyException>([&]() { doctor.takePatient(); });

            Surgeon surgeon;
            surgeon.sterileReady = false;
            Assert::ExpectException<HygieneErrorException>([&]() {
                surgeon.doOperation(validPatient());
            });
            surgeon.sterileReady = true;
            Patient latexPatient = validPatient("P-LATEX");
            Allergy latex;
            latex.name = "Latex";
            latexPatient.addAllergy(latex);
            Assert::ExpectException<AllergyRiskException>([&]() {
                surgeon.doOperation(latexPatient);
            });
            surgeon.doOperation(validPatient());
            Assert::AreEqual(1, surgeon.operationsCount);

            Nurse nurse;
            nurse.canDoIV = false;
            Assert::ExpectException<BadPrescriptionException>([&]() {
                nurse.giveInjection(validPatient(), "Analgin", true);
            });
            nurse.canDoIV = true;
            nurse.giveInjection(validPatient(), "Analgin", true);
            Assert::AreEqual(1, nurse.proceduresDone);
        }

        TEST_METHOD(Dentist_Pharmacist_Receptionist_LabWorker)
        {
            Dentist dentist;
            dentist.treatTooth(32);
            Assert::AreEqual(1, dentist.teethTreated);
            Assert::ExpectException<InvalidPatientDataException>([&]() { dentist.treatTooth(33); });

            Pharmacist pharmacist;
            Assert::ExpectException<BadPrescriptionException>([&]() { pharmacist.issueDrug("", false); });
            Assert::ExpectException<BadPrescriptionException>([&]() { pharmacist.issueDrug("Morphine", true); });
            pharmacist.issueDrug("Aspirin", false);
            Assert::AreEqual(1, pharmacist.issuedDrugs);

            Receptionist receptionist;
            Doctor doctor;
            doctor.maxPatients = 1;
            receptionist.bookVisit(doctor);
            Assert::AreEqual(1, receptionist.bookedCount);
            Assert::ExpectException<DoctorBusyException>([&]() { receptionist.bookVisit(doctor); });

            LabWorker worker;
            Assert::ExpectException<BadLabSampleException>([&]() { worker.checkSample(false); });
            worker.checkSample(true);
            Assert::AreEqual(1, worker.testsDone);
        }
    };

    TEST_CLASS(DiagnosticsPharmacyTests)
    {
    public:
        TEST_METHOD(Samples_Tests_And_Orders)
        {
            LabSample sample;
            Assert::ExpectException<BadLabSampleException>([&]() { sample.check(); });
            sample.sampleId = "S-1";
            sample.check();
            sample.spoil();
            Assert::ExpectException<BadLabSampleException>([&]() { sample.check(); });

            BloodTest blood;
            Assert::IsTrue(blood.isNormal());
            blood.sugar = 6.1;
            Assert::IsFalse(blood.isNormal());

            UrineTest urine;
            Assert::IsTrue(urine.isClean());
            urine.proteinFound = true;
            Assert::IsFalse(urine.isClean());

            XRayScan scan;
            scan.area = "Chest";
            Assert::AreEqual(std::string("Chest: normal"), scan.summary());
            scan.hasFracture = true;
            Assert::AreEqual(std::string("Chest: fracture"), scan.summary());

            LabOrder order;
            order.sample = sample;
            Assert::ExpectException<BadLabSampleException>([&]() { order.completeOrder(); });
            order.sample = LabSample();
            order.sample.sampleId = "S-2";
            order.completeOrder();
            Assert::IsTrue(order.ready);
        }

        TEST_METHOD(Medicine_Stock_Prescription_Supplier)
        {
            Medicine medicine;
            medicine.price = 12.0;
            Assert::ExpectException<BadPrescriptionException>([&]() { medicine.costFor(0); });
            Assert::AreEqual(36.0, medicine.costFor(3), 0.001);

            DrugStock stock;
            stock.amount = 12;
            Assert::ExpectException<NoMedicineStockException>([&]() { stock.takeDrug(20); });
            stock.takeDrug(5);
            Assert::IsTrue(stock.isLow());
            stock.addDrug(10);
            Assert::AreEqual(17, stock.amount);

            Prescription prescription;
            prescription.patient = validPatient();
            prescription.medicine.title = "Paracetamol";
            prescription.verifyForPatient();
            prescription.packs = 0;
            Assert::ExpectException<BadPrescriptionException>([&]() {
                prescription.verifyForPatient();
            });

            DrugSupplier supplier;
            supplier.supplyStock(stock, 3);
            Assert::AreEqual(20, stock.amount);
            supplier.reliable = false;
            Assert::ExpectException<InsuranceDeniedException>([&]() {
                supplier.supplyStock(stock, 3);
            });
        }
    };

    TEST_CLASS(BillingCompanyTests)
    {
    public:
        TEST_METHOD(Bills_Cash_Claims_And_Discounts)
        {
            Bill bill;
            Assert::ExpectException<NoMoneyException>([&]() { bill.setAmount(-1.0); });
            bill.setAmount(120.0);
            bill.markPaid();
            Assert::IsTrue(bill.paid);

            CashDesk desk;
            Assert::ExpectException<NoMoneyException>([&]() { desk.acceptCash(0.0); });
            desk.acceptCash(500.0);
            Assert::AreEqual(1500.0, desk.balance, 0.001);
            Assert::ExpectException<NoMoneyException>([&]() { desk.refundCash(2000.0); });
            desk.refundCash(500.0);
            Assert::AreEqual(1000.0, desk.balance, 0.001);

            InsuranceClaim claim;
            claim.patient = validPatient();
            claim.sum = 200.0;
            claim.processClaim();
            Assert::IsTrue(claim.approved);
            Assert::AreEqual(800.0, claim.patient.insurance.limit, 0.001);

            DiscountCard card;
            card.percent = 10;
            Assert::AreEqual(180.0, card.applyDiscount(200.0), 0.001);
            Assert::ExpectException<NoMoneyException>([&]() { card.applyDiscount(-1.0); });
        }

        TEST_METHOD(Sanitary_Quality_Department_And_Center)
        {
            SanitaryCheck check;
            check.failCheck(45);
            Assert::IsFalse(check.passed);

            QualityReport report;
            report.resolveOne();
            report.addComplaint();
            report.addComplaint();
            report.resolveOne();
            Assert::AreEqual(1, report.openComplaints());

            Department department;
            department.closeDept();
            Assert::IsFalse(department.open);
            department.openDept();
            Assert::IsTrue(department.open);

            MedicalCenter center;
            center.addDepartment(department);
            center.addPatient(validPatient("P-1"));
            center.addPatient(validPatient("P-2"));
            Assert::AreEqual(2, center.totalPatients());
            Assert::AreEqual(std::string("Ivanov Ivan"), center.findPatientById("P-1").name);
            Assert::ExpectException<PatientNotFoundException>([&]() {
                center.findPatientById("UNKNOWN");
            });
            Assert::ExpectException<InvalidPatientDataException>([&]() {
                center.addPatient(Patient());
            });
        }
    };
}
