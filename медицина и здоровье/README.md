# Medical Center Domain Model

A C++20 object model of the "Medical center" subject area. The project implements
a class hierarchy for staff and doctors, patients and medical cards, visits and
diagnoses, wards and medical devices, treatment plans, laboratory diagnostics,
pharmacy stock and prescriptions, billing and insurance, quality control, and a
top-level medical center class. It demonstrates public inheritance, composition
and aggregation, validation through domain methods, and a dedicated exception
hierarchy.

## Requirements

- Visual Studio 2022 with the MSVC v145 toolset (or another C++20 compiler)
- Windows API (`windows.h`) for UTF-8 console output in the demo
- No third-party libraries are required

## Build

Open `медицина и здоровье.slnx` in Visual Studio 2022, select `x64` and
`Debug` or `Release`, then build the solution (`Ctrl+Shift+B`). From a Developer
Command Prompt the same build can be started with:

```sh
msbuild "медицина и здоровье.slnx" /p:Configuration=Release /p:Platform=x64
```

This builds:

- `медицина и здоровье` — an interactive demonstration program (`main.cpp`)
- `UnitTest1` — a Visual Studio unit-test project included in the solution

## Run the demonstration

Run the `медицина и здоровье` executable. The program shows a menu with ten
commands:

```sh
x64\Release\медицина и здоровье.exe
```

| Command | Action |
|---:|---|
| 1 | Add a department with a name and room count |
| 2 | Register a patient by ID, name, age, and phone |
| 3 | Add an allergy to a patient found by ID |
| 4 | Book a patient with the doctor |
| 5 | Issue a prescription and check the patient's allergies |
| 6 | Issue medicine from the pharmacy stock |
| 7 | Occupy a bed in the ward |
| 8 | Accept a cash payment in the cash desk |
| 9 | Print the medical center report |
| 0 | Exit |

All domain errors are caught in `main()` and printed as `Ошибка: <message>`.

## Run the tests

The self-contained assert-based test suite is `MedicalTests/MedicalTests.cpp`.
It is a separate console project (not part of `.slnx`); build it with:

```sh
msbuild "MedicalTests\MedicalTests.vcxproj" /p:Configuration=Release /p:Platform=x64
```

The suite has its own `main()`, so compile `MedicalTests.cpp` alone when using
the compiler directly:

```sh
cl /std:c++20 /EHsc /I"медицина и здоровье" MedicalTests\MedicalTests.cpp
```

Run the resulting executable:

```sh
MedicalTests.exe
```

On success it prints `All unit tests passed successfully!`. The tests cover
patient core data, medical history, visits and diagnoses, wards and devices,
treatment plans, staff, laboratory diagnostics, pharmacy, billing, and the
medical center.

`UnitTest1/UnitTest1.cpp` is a native Visual Studio test project. Build it in
Visual Studio with the **Desktop development with C++** and **C++ native unit
test support** components installed, then run it from Test Explorer.

## Requirement coverage

| Requirement | Required | Implemented |
|---|---:|---:|
| Classes | 50 | 48 domain classes + 14 exception classes = 62 |
| Fields | 150 | 154 |
| Unique domain behaviors | 100 | 103 |
| Associations | 30 | 38 |
| Custom exceptions | 12 | 14 |

The behavior count includes every domain operation with logic (validation,
calculation, state transition, compatibility check, aggregation, and so on).
Constructors and trivial accessors are not counted separately; the model has no
constructors, all objects are configured through public fields and initialized
by in-class defaults. The exact distribution of the 103 unique behaviors is
given in [Domain behavior catalog](#domain-behavior-catalog).

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Public inheritance | `Doctor : public Staff`, `Surgeon : public Doctor`, `Nurse : public Staff` |
| Public inheritance | `Dentist : public Doctor`, `Pharmacist : public Staff`, `Receptionist : public Staff`, `LabWorker : public Staff` |
| Exception inheritance | 13 concrete exceptions derive from `MedicalException`, which derives from `std::exception` |
| Composition | `Patient` owns `Contact`, `Vitals`, `Insurance`, and allergies; `MedCard` owns `Patient`, `BodyMetrics`, diseases, and vaccines |
| Composition | `TreatmentPlan` owns `Diagnosis`, procedures, and courses; `MedicalCenter` owns patients, departments, and `CashDesk` |
| Aggregation | `Prescription` references `Patient` and `Medicine`; `LabOrder` references `Patient` and `LabSample`; `DrugStock` references `Medicine` |
| Association | `Appointment` references `Patient` and `Doctor`; `Bill` and `InsuranceClaim` reference `Patient`; `Staff` references `MedLicense` |
| Validation in behavior | `Patient::checkValid`, `Doctor::takePatient`, `Surgeon::doOperation`, `Prescription::verifyForPatient`, `Sterilizer::runCycle`, `MedicalCenter::findPatientById` |
| Not used | Virtual domain methods, multiple/virtual inheritance, enums, and namespaces — the only override is `MedicalException::what()` |

## Namespaces

No namespace contains domain or exception classes: every domain class and every
exception class lives in the global namespace. `main.cpp` declares an unnamed
namespace around `printMenu()`; otherwise all standard-library names are
qualified explicitly.

---

## Exception hierarchy

All exceptions derive from `MedicalException`, which derives from
`std::exception`. Every exception stores a message and is thrown from the domain
method that detects the invalid state.

| Exception | Meaning |
|---|---|
| `MedicalException` | Root of the hierarchy |
| `PatientNotFoundException` | Patient with the requested ID does not exist |
| `InvalidPatientDataException` | Invalid patient, tooth, sick leave, or appointment data |
| `DoctorBusyException` | Doctor is busy or has no free appointment slots |
| `TimeConflictException` | Reused referral, occupied bed, or full ward |
| `AllergyRiskException` | Drug or latex allergy detected |
| `BadPrescriptionException` | Empty drug, missing IV permission, invalid packs, or bad quantity |
| `NoMedicineStockException` | Not enough medicine in stock or invalid issue quantity |
| `DrugConflictException` | Drug interaction conflict (declared for domain extension) |
| `BadLabSampleException` | Spoiled or empty laboratory sample |
| `InsuranceDeniedException` | Inactive/insufficient insurance or unreliable supplier |
| `NoMoneyException` | Negative bill, invalid cash operation, or negative price |
| `DeviceErrorException` | Attempt to use a broken medical device |
| `HygieneErrorException` | Non-sterile operating conditions or low sterilization temperature |

---

## Core value and support classes

### Contact
- Fields: `phone`, `email`, `city`
- Methods: `hasPhone()`, `hasEmail()`, `info()`
- Behavior: checks that the phone has at least seven characters, accepts an email only when it is at least five characters long and contains `@`, and formats city and phone.

### Allergy
- Fields: `name`, `reaction`, `level`
- Methods: `isDangerous()`, `raiseLevel()`, `lowerLevel()`
- Behavior: treats level `4` or `5` as dangerous, caps the level at `5`, and never lowers it below `1`.

### Vitals
- Fields: `temp`, `pulse`, `sysPressure`, `diaPressure`
- Methods: `hasFever()`, `highPressure()`, `isBad()`, `isStable()`
- Behavior: detects fever at `37.5`, high blood pressure at `140/90`, and critical state at `39.0`, pulse above `120`, or systolic pressure `170` or more; `isStable()` is true only when none of these conditions holds.

### Insurance
- Fields: `number`, `company`, `limit`, `active`
- Methods: `canCover(double)`, `spend(double)`, `topUp(double)`
- Behavior: checks active insurance and available limit, then spends from the limit or throws `InsuranceDeniedException`; `topUp` increases the limit only for an active policy and a positive amount.

### Patient
- Fields: `id`, `name`, `age`, `contact`, `vitals`, `insurance`, `allergies`
- Methods: `checkValid()`, `addAllergy(const Allergy&)`, `hasAllergy(const std::string&)`, `allergyLevel(const std::string&)`, `needsUrgentHelp()`
- Behavior: validates ID, name, and age, manages allergies, returns the stored allergy level (`0` when absent), and delegates urgency to `Vitals`.

---

## Medical history and visits

### ChronicDisease
- Fields: `code`, `title`, `startYear`, `active`
- Methods: `yearsSick(int)`, `setRemission()`, `relapse()`
- Behavior: computes years since diagnosis, moves the disease into remission, and returns it to the active state on relapse.

### Vaccination
- Fields: `vaccineName`, `yearDone`, `validYears`
- Methods: `isValid(int)`, `renew(int)`, `yearsLeft(int)`
- Behavior: checks whether the vaccination is still valid, renews it in the given year, and reports how many valid years remain.

### BodyMetrics
- Fields: `heightCm`, `weightKg`, `bloodGroup`
- Methods: `bmi()`, `isOverweight()`, `idealWeight()`
- Behavior: computes body mass index, detects a BMI of `25` or more, and calculates the ideal weight for a BMI of `22`.

### MedCard
- Fields: `cardId`, `patient`, `metrics`, `diseases`, `vaccines`
- Methods: `addDisease(const ChronicDisease&)`, `addVaccine(const Vaccination&)`, `activeDiseasesCount()`, `validVaccinesCount(int)`
- Behavior: stores medical history entries, counts active diseases, and counts vaccinations that are still valid in a given year.

### Symptom
- Fields: `name`, `days`, `severity`
- Methods: `isSevere()`, `addDay()`, `isLong()`
- Behavior: treats severity `4` or `5` as severe, increments the duration, and flags a symptom lasting `7` days or more as long.

### Diagnosis
- Fields: `code`, `title`, `mainDiag`
- Methods: `fullText()`
- Behavior: formats the diagnosis code and title.

### Appointment
- Fields: `id`, `patient`, `doctor`, `date`, `done`
- Methods: `complete()`, `reschedule(const std::string&)`
- Behavior: validates the patient before completing the appointment and moves an unfinished appointment to a non-empty new date.

### SickLeave
- Fields: `number`, `patient`, `days`, `closed`
- Methods: `extend(int)`, `closeSheet()`
- Behavior: extends an open sick leave with a positive number of days and closes it.

### Referral
- Fields: `id`, `patient`, `toSpecialty`, `used`
- Methods: `useReferral()`
- Behavior: allows a referral to be used only once.

---

## Wards, devices, and treatment

### HospitalBed
- Fields: `bedNumber`, `occupied`
- Methods: `occupy()`, `freeBed()`
- Behavior: prevents occupying an already occupied bed and frees it.

### WardRoom
- Fields: `roomCode`, `totalBeds`, `freeBedsCount`
- Methods: `hasFreeBed()`, `admit()`, `release()`, `occupancyRate()`
- Behavior: admits a patient only when a free bed exists, releases beds up to total capacity, and reports the share of occupied beds.

### MedDevice
- Fields: `serial`, `name`, `working`, `hoursUsed`
- Methods: `useDevice(int)`, `breakDevice()`, `repairDevice()`, `needsService()`
- Behavior: refuses to use a broken device, records positive usage hours, toggles working state, and flags service after `100` hours.

### Sterilizer
- Fields: `deviceId`, `tempC`, `cycleDone`
- Methods: `runCycle(int)`
- Behavior: requires a target temperature of at least `120` degrees and records a completed cycle.

### Procedure
- Fields: `title`, `count`, `doneCount`
- Methods: `markOneDone()`, `isFinished()`, `leftCount()`
- Behavior: records completed procedures without exceeding the planned count and reports how many are left.

### TherapyCourse
- Fields: `name`, `daysTotal`, `daysPassed`
- Methods: `passDay()`, `leftDays()`, `progressPercent()`
- Behavior: advances the course up to its total duration, reports remaining days, and computes completion percentage capped at `100`.

### TreatmentPlan
- Fields: `planId`, `diagnosis`, `procedures`, `courses`
- Methods: `addProcedure(const Procedure&)`, `addCourse(const TherapyCourse&)`, `proceduresCount()`, `coursesCount()`
- Behavior: manages procedures and therapy courses and counts both collections.

### RecoveryNote
- Fields: `date`, `note`, `score`
- Methods: `isImproving()`, `isCritical()`
- Behavior: treats a recovery score of `7` or more as improvement and `3` or less as a critical condition.

---

## Staff hierarchy

### MedLicense
- Fields: `number`, `expireYear`, `category`
- Methods: `isValid(int)`, `bonusRate()`
- Behavior: validates a license number and expiration year and computes the category bonus.

### Staff
- Fields: `id`, `name`, `salary`, `busy`, `license`
- Methods: `totalSalary()`, `setBusy(bool)`
- Behavior: computes salary with the license bonus and changes the busy flag.

### Doctor : public Staff
- Fields: `specialty`, `maxPatients`, `todayPatients`
- Methods: `takePatient()`, `freeSlots()`
- Behavior: accepts a patient only when not busy and below the daily limit, and reports free slots.

### Surgeon : public Doctor
- Fields: `operationsCount`, `sterileReady`
- Methods: `doOperation(const Patient&)`
- Behavior: requires sterile conditions and checks the patient for a latex allergy before an operation.

### Nurse : public Staff
- Fields: `proceduresDone`, `canDoIV`
- Methods: `giveInjection(const Patient&, const std::string&, bool)`
- Behavior: checks drug allergy and IV permission before recording an injection.

### Dentist : public Doctor
- Fields: `teethTreated`, `hasXRay`
- Methods: `treatTooth(int)`
- Behavior: accepts tooth numbers from `1` to `32` and records treatment.

### Pharmacist : public Staff
- Fields: `issuedDrugs`, `canGiveStrong`
- Methods: `issueDrug(const std::string&, bool)`
- Behavior: rejects an empty drug name or a strong drug without permission, otherwise records issuance.

### Receptionist : public Staff
- Fields: `bookedCount`
- Methods: `bookVisit(Doctor&)`
- Behavior: books through `Doctor::takePatient()` and counts successful bookings.

### LabWorker : public Staff
- Fields: `testsDone`
- Methods: `checkSample(bool)`
- Behavior: rejects a bad sample and records a successful test.

---

## Laboratory diagnostics

### LabSample
- Fields: `sampleId`, `type`, `valid`
- Methods: `spoil()`, `check()`
- Behavior: invalidates a sample and checks that it has an ID and is not spoiled.

### BloodTest
- Fields: `hemoglobin`, `leukocytes`, `sugar`
- Methods: `isNormal()`
- Behavior: checks hemoglobin of at least `115` and sugar between `3.3` and `6.0`.

### UrineTest
- Fields: `density`, `proteinFound`
- Methods: `isClean()`
- Behavior: checks density between `1.005` and `1.030` and absence of protein.

### XRayScan
- Fields: `area`, `hasFracture`, `comment`
- Methods: `summary()`
- Behavior: formats the scan area with a fracture or normal result.

### LabOrder
- Fields: `orderId`, `patient`, `sample`, `ready`
- Methods: `completeOrder()`
- Behavior: validates the sample and marks the order ready.

---

## Pharmacy and billing

### Medicine
- Fields: `code`, `title`, `price`, `prescriptionOnly`
- Methods: `costFor(int)`
- Behavior: computes cost for a positive quantity or throws `BadPrescriptionException`.

### DrugStock
- Fields: `medicine`, `amount`, `minAmount`
- Methods: `isLow()`, `takeDrug(int)`, `addDrug(int)`
- Behavior: detects low stock, issues medicine with insufficiency checks, and adds positive quantities.

### Prescription
- Fields: `id`, `patient`, `medicine`, `packs`
- Methods: `verifyForPatient()`
- Behavior: validates the patient and packs, and checks the medicine title against patient allergies.

### DrugSupplier
- Fields: `name`, `phone`, `reliable`
- Methods: `supplyStock(DrugStock&, int)`
- Behavior: rejects unreliable suppliers and delegates restocking to `DrugStock`.

### Bill
- Fields: `billId`, `patient`, `amount`, `paid`
- Methods: `setAmount(double)`, `markPaid()`
- Behavior: rejects a negative amount and marks the bill as paid.

### CashDesk
- Fields: `balance`
- Methods: `acceptCash(double)`, `refundCash(double)`
- Behavior: accepts positive cash and refunds only an amount covered by the balance.

### InsuranceClaim
- Fields: `claimId`, `patient`, `sum`, `approved`
- Methods: `processClaim()`
- Behavior: spends from patient insurance and approves the claim on success.

### DiscountCard
- Fields: `cardCode`, `percent`
- Methods: `applyDiscount(double)`
- Behavior: rejects a negative price and applies the percentage discount.

---

## Quality and medical center

### SanitaryCheck
- Fields: `checkId`, `score`, `passed`
- Methods: `failCheck(int)`
- Behavior: subtracts a penalty and fails the check below `60` points.

### QualityReport
- Fields: `reportId`, `complaints`, `resolved`
- Methods: `addComplaint()`, `resolveOne()`, `openComplaints()`
- Behavior: registers complaints, prevents resolving more than registered, and reports open complaints.

### Department
- Fields: `name`, `roomsCount`, `open`
- Methods: `closeDept()`, `openDept()`
- Behavior: toggles the department's open state.

### MedicalCenter
- Fields: `centerName`, `patients`, `departments`, `cashDesk`
- Methods: `addPatient(const Patient&)`, `findPatientById(const std::string&)`, `addDepartment(const Department&)`, `totalPatients()`
- Behavior: adds validated patients, looks up a patient by ID or throws `PatientNotFoundException`, adds departments, and counts patients.

---

## Domain behavior catalog

The 103 unique domain behaviors are distributed across the model as follows.

| Class | Behaviors | Count |
|---|---|---:|
| `Contact` | `hasPhone`, `hasEmail`, `info` | 3 |
| `Allergy` | `isDangerous`, `raiseLevel`, `lowerLevel` | 3 |
| `Vitals` | `hasFever`, `highPressure`, `isBad`, `isStable` | 4 |
| `Insurance` | `canCover`, `spend`, `topUp` | 3 |
| `Patient` | `checkValid`, `addAllergy`, `hasAllergy`, `allergyLevel`, `needsUrgentHelp` | 5 |
| `ChronicDisease` | `yearsSick`, `setRemission`, `relapse` | 3 |
| `Vaccination` | `isValid`, `renew`, `yearsLeft` | 3 |
| `BodyMetrics` | `bmi`, `isOverweight`, `idealWeight` | 3 |
| `MedCard` | `addDisease`, `addVaccine`, `activeDiseasesCount`, `validVaccinesCount` | 4 |
| `Symptom` | `isSevere`, `addDay`, `isLong` | 3 |
| `Diagnosis` | `fullText` | 1 |
| `Appointment` | `complete`, `reschedule` | 2 |
| `SickLeave` | `extend`, `closeSheet` | 2 |
| `Referral` | `useReferral` | 1 |
| `HospitalBed` | `occupy`, `freeBed` | 2 |
| `WardRoom` | `hasFreeBed`, `admit`, `release`, `occupancyRate` | 4 |
| `MedDevice` | `useDevice`, `breakDevice`, `repairDevice`, `needsService` | 4 |
| `Sterilizer` | `runCycle` | 1 |
| `Procedure` | `markOneDone`, `isFinished`, `leftCount` | 3 |
| `TherapyCourse` | `passDay`, `leftDays`, `progressPercent` | 3 |
| `TreatmentPlan` | `addProcedure`, `addCourse`, `proceduresCount`, `coursesCount` | 4 |
| `RecoveryNote` | `isImproving`, `isCritical` | 2 |
| `MedLicense` | `isValid`, `bonusRate` | 2 |
| `Staff` | `totalSalary`, `setBusy` | 2 |
| `Doctor` | `takePatient`, `freeSlots` | 2 |
| `Surgeon` | `doOperation` | 1 |
| `Nurse` | `giveInjection` | 1 |
| `Dentist` | `treatTooth` | 1 |
| `Pharmacist` | `issueDrug` | 1 |
| `Receptionist` | `bookVisit` | 1 |
| `LabWorker` | `checkSample` | 1 |
| `LabSample` | `spoil`, `check` | 2 |
| `BloodTest` | `isNormal` | 1 |
| `UrineTest` | `isClean` | 1 |
| `XRayScan` | `summary` | 1 |
| `LabOrder` | `completeOrder` | 1 |
| `Medicine` | `costFor` | 1 |
| `DrugStock` | `isLow`, `takeDrug`, `addDrug` | 3 |
| `Prescription` | `verifyForPatient` | 1 |
| `DrugSupplier` | `supplyStock` | 1 |
| `Bill` | `setAmount`, `markPaid` | 2 |
| `CashDesk` | `acceptCash`, `refundCash` | 2 |
| `InsuranceClaim` | `processClaim` | 1 |
| `DiscountCard` | `applyDiscount` | 1 |
| `SanitaryCheck` | `failCheck` | 1 |
| `QualityReport` | `addComplaint`, `resolveOne`, `openComplaints` | 3 |
| `Department` | `closeDept`, `openDept` | 2 |
| `MedicalCenter` | `addPatient`, `findPatientById`, `addDepartment`, `totalPatients` | 4 |
| **Total** | | **103** |

---

## Associations

A class is associated with another class when it stores it as a field or
accepts/returns it as a method parameter. The model contains 38 such
associations; the field-level ones are listed below.

| # | Association | Kind |
|---:|---|---|
| 1 | `Patient` → `Contact` via `contact` | field |
| 2 | `Patient` → `Vitals` via `vitals` | field |
| 3 | `Patient` → `Insurance` via `insurance` | field |
| 4 | `Patient` → `Allergy` via `allergies` | field collection |
| 5 | `MedCard` → `Patient` via `patient` | field |
| 6 | `MedCard` → `BodyMetrics` via `metrics` | field |
| 7 | `MedCard` → `ChronicDisease` via `diseases` | field collection |
| 8 | `MedCard` → `Vaccination` via `vaccines` | field collection |
| 9 | `Appointment` → `Patient` via `patient` | field |
| 10 | `Appointment` → `Doctor` via `doctor` | field |
| 11 | `SickLeave` → `Patient` via `patient` | field |
| 12 | `Referral` → `Patient` via `patient` | field |
| 13 | `TreatmentPlan` → `Diagnosis` via `diagnosis` | field |
| 14 | `TreatmentPlan` → `Procedure` via `procedures` | field collection |
| 15 | `TreatmentPlan` → `TherapyCourse` via `courses` | field collection |
| 16 | `Patient::addAllergy(const Allergy&)` | parameter |
| 17 | `MedCard::addDisease(const ChronicDisease&)` | parameter |
| 18 | `MedCard::addVaccine(const Vaccination&)` | parameter |
| 19 | `TreatmentPlan::addProcedure(const Procedure&)` | parameter |
| 20 | `TreatmentPlan::addCourse(const TherapyCourse&)` | parameter |
| 21 | `Staff` → `MedLicense` via `license` | field |
| 22 | `LabOrder` → `Patient` via `patient` | field |
| 23 | `LabOrder` → `LabSample` via `sample` | field |
| 24 | `DrugStock` → `Medicine` via `medicine` | field |
| 25 | `Prescription` → `Patient` via `patient` | field |
| 26 | `Prescription` → `Medicine` via `medicine` | field |
| 27 | `Bill` → `Patient` via `patient` | field |
| 28 | `InsuranceClaim` → `Patient` via `patient` | field |
| 29 | `MedicalCenter` → `Patient` via `patients` | field collection |
| 30 | `MedicalCenter` → `Department` via `departments` | field collection |
| 31 | `MedicalCenter` → `CashDesk` via `cashDesk` | field |
| 32 | `Surgeon::doOperation(const Patient&)` | parameter |
| 33 | `Nurse::giveInjection(const Patient&, ...)` | parameter |
| 34 | `DrugSupplier::supplyStock(DrugStock&, int)` | parameter |
| 35 | `Receptionist::bookVisit(Doctor&)` | parameter |
| 36 | `MedicalCenter::addPatient(const Patient&)` | parameter |
| 37 | `MedicalCenter::findPatientById(...)` returns `Patient&` | result |
| 38 | `MedicalCenter::addDepartment(const Department&)` | parameter |

---

## Project layout

- `медицина и здоровье/HealthExceptions.h` — exception hierarchy
- `медицина и здоровье/PatientCore.h` — contacts, allergies, vitals, insurance, patients
- `медицина и здоровье/MedCardHistory.h` — diseases, vaccinations, body metrics, medical cards
- `медицина и здоровье/VisitsDiagnosis.h` — symptoms, diagnoses, appointments, sick leaves, referrals
- `медицина и здоровье/ClinicWard.h` — beds, wards, medical devices, sterilizer
- `медицина и здоровье/TreatmentPlan.h` — procedures, therapy courses, plans, recovery notes
- `медицина и здоровье/StaffDoctors.h` — licenses, staff, doctors, surgeons, nurses
- `медицина и здоровье/OtherStaff.h` — dentists, pharmacists, receptionists, lab workers
- `медицина и здоровье/LabDiagnostics.h` — samples, blood/urine tests, X-ray scans, orders
- `медицина и здоровье/PharmacyStock.h` — medicines, stock, prescriptions, suppliers
- `медицина и здоровье/BillingPay.h` — bills, cash desk, claims, discounts
- `медицина и здоровье/ClinicSystem.h` — sanitary checks, quality reports, departments, medical center
- `медицина и здоровье/main.cpp` — interactive demonstration program
- `MedicalTests/MedicalTests.cpp` — self-contained test suite
- `UnitTest1/UnitTest1.cpp` — Visual Studio native unit-test project
- `медицина и здоровье.slnx` — solution configuration (x64 and x86)
