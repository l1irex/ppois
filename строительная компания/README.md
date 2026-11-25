# Construction Company Domain Model

A C++17 object model of the "Construction company" subject area. The project
implements a class hierarchy for projects and construction sites, work schedules
and stages, estimates and contracts, payments, budgets and taxes, employees and
qualifications, heavy equipment, materials and warehouses, safety inspections,
quality control, and a top-level company class. It demonstrates public
inheritance, composition and aggregation, validation through domain methods, and
a dedicated exception hierarchy.

## Requirements

- Visual Studio 2022 with the MSVC v145 toolset (or another C++17 compiler)
- Windows API (`windows.h`) for UTF-8 console output in the demo
- No third-party libraries are required

## Build

Open `строительная компания.slnx` in Visual Studio 2022, select `x64` and
`Debug` or `Release`, then build the solution (`Ctrl+Shift+B`). From a Developer
Command Prompt the same build can be started with:

```sh
msbuild "строительная компания.slnx" /p:Configuration=Release /p:Platform=x64
```

This builds:

- `строительная компания` — an interactive demonstration program (`main.cpp`)
- `UnitTest1` — a Visual Studio unit-test project included in the solution

## Run the demonstration

Run the `строительная компания` executable. The program shows a menu with nine
commands:

```sh
x64\Release\строительная компания.exe
```

| Command | Action |
|---:|---|
| 1 | Add a construction project with a client and a site |
| 2 | Start a project by ID |
| 3 | Spend project budget by ID |
| 4 | Hire an employee through `HRManager` |
| 5 | Write materials off the cement stock |
| 6 | Assign an excavator to a site with an operator |
| 7 | Register a safety violation during an inspection |
| 8 | Print the company report |
| 0 | Exit |

All domain errors are caught in `main()` and printed as `Ошибка: <message>`.

## Run the tests

The self-contained assert-based test suite is `ConstructionTests/ConstructionTests.cpp`.
It is a separate console project (not part of `.slnx`); build it with:

```sh
msbuild "ConstructionTests\ConstructionTests.vcxproj" /p:Configuration=Release /p:Platform=x64
```

`ConstructionTests` also contains a small auxiliary `main.cpp`. Exclude it from
the build (or compile `ConstructionTests.cpp` alone) so that only one `main()`
is linked:

```sh
cl /std:c++17 /EHsc /I"строительная компания" ConstructionTests\ConstructionTests.cpp
```

Run the resulting executable:

```sh
ConstructionTests.exe
```

On success it prints `All unit tests passed successfully!`. The tests cover
geography and clients, projects and schedules, finance and contracts, staff,
equipment, materials, safety, quality, and company lookups.

## Requirement coverage

| Requirement | Required | Implemented |
|---|---:|---:|
| Classes | 50 | 53 domain classes |
| Fields | 150 | 249 |
| Unique domain behaviors | 100 | 202 |
| Associations | 30 | 60+ |
| Custom exceptions | 12 | 13 |

The behavior count includes every domain operation with logic (validation,
calculation, state transition, compatibility check, aggregation, and so on).
Constructors and trivial accessors are not counted separately; the model has no
constructors, all objects are configured through public fields and initialized
by in-class defaults.

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Public inheritance | `ResidentialProject`, `CommercialProject`, `InfrastructureProject : public Project` |
| Public inheritance | `Worker`, `Foreman`, `Engineer`, `Accountant`, `HRManager`, `Operator`, `Mechanic : public Employee` |
| Public inheritance | `Excavator`, `Crane`, `ConcreteMixer : public Equipment` |
| Exception inheritance | 12 concrete exceptions derive from `ConstructionException`, which derives from `std::runtime_error` |
| Composition | `Project` owns `Client`, `Site`, and `WorkSchedule`; `DeliveryOrder` owns `Supplier`, `Material`, and `Site`; `Salary` owns `Employee` |
| Aggregation | `ConstructionCompany` keeps collections of `Client`, `Project`, `Site`, and `Employee`; `Foreman` and `HRManager` keep collections of workers/employees |
| Association | `Equipment` references `Site`, `Operator`, and `MaintenanceRecord`; `MaterialStock` references `Material` and `WarehouseZone` |
| Validation in behavior | `Project::checkProject`, `Estimate::addItem`, `Contract::sign`, `Operator::checkPermit`, `SafetyViolation::checkCritical` |
| Not used | Virtual methods, multiple/virtual inheritance, and namespaces — the model deliberately uses plain public inheritance and global class names |

## Namespaces

No namespaces are declared in this project: every domain class and every
exception class lives in the global namespace. The only qualified name is the
standard-library base `std::runtime_error` of `ConstructionException`.

---

## Exception hierarchy

All exceptions derive from `ConstructionException`, which derives from
`std::runtime_error`. Every exception stores a message and is thrown from the
domain method that detects the invalid state.

| Exception | Meaning |
|---|---|
| `ConstructionException` | Root of the hierarchy |
| `InsufficientBudgetException` | Project budget, company budget, or account balance is too low |
| `InvalidProjectDataException` | Invalid project, site, estimate item, apartment, or work data |
| `ProjectNotFoundException` | Project with the requested ID does not exist |
| `InvalidContractException` | Contract/client data invalid or contract operation rejected |
| `InsufficientMaterialException` | Not enough material in stock, delivery, or mixer drum |
| `EquipmentUnavailableException` | Equipment is in repair, already assigned, or wind-locked |
| `EquipmentOverloadException` | Load, depth, height, or rack weight exceeds the limit |
| `InvalidQualificationException` | Missing or expired permit, license, or qualification level |
| `SafetyViolationException` | Critical unfixed safety violation (severity 4 or 5) |
| `QualityDefectException` | Critical unfixed defect or work stage not accepted |
| `InvalidScheduleException` | Negative progress, invalid day count, unfinished schedule |
| `InvalidPaymentException` | Invalid sum, overpayment, double payment, or blocked account |

---

## Core value and support classes

### GeoPoint
- Fields: `x`, `y`, `height`
- Methods: `isValid()`, `distanceTo(const GeoPoint&)`, `heightDiff(const GeoPoint&)`
- Behavior: validates coordinate bounds `[-1000, 1000]`, computes planar distance with the Pythagorean formula, and computes height difference.

### Address
- Fields: `country`, `city`, `street`, `zipCode`, `point`
- Methods: `isFilled()`, `fullAddress()`, `distanceTo(const Address&)`
- Behavior: checks that city and street are present, formats a readable address, and delegates distance calculation to `GeoPoint`.

### Client
- Fields: `id`, `name`, `inn`, `creditLimit`, `active`
- Methods: `checkData()`, `canPay(double)`, `addCreditLimit(double)`, `disableClient()`
- Behavior: validates name and INN, checks payment against the credit limit and active status, increases the limit with validation, and deactivates the client.

### Vendor
- Fields: `id`, `name`, `workType`, `rating`
- Methods: `changeRating(double)`, `isGoodVendor()`, `getInfo()`
- Behavior: adjusts rating with clamping to `[0, 5]`, checks the reliability threshold `3.5`, and formats vendor info.

---

## Project and schedule hierarchy

### Apartment
- Fields: `number`, `floor`, `area`, `sold`
- Methods: `calcPrice(double)`, `sell()`
- Behavior: computes price from area and price per square meter with validation, and prevents selling an apartment twice.

### LeaseUnit
- Fields: `code`, `area`, `rentPerMeter`, `rented`
- Methods: `calcMonthRent()`, `rentOut()`
- Behavior: computes monthly rent and prevents renting the same unit twice.

### RoadSection
- Fields: `code`, `lengthKm`, `lanes`, `donePercent`
- Methods: `addProgress(double)`, `calcArea()`
- Behavior: accumulates progress with a `100%` cap and computes approximate road surface area from length, lanes, and lane width.

### WorkStage
- Fields: `id`, `name`, `weight`, `accepted`
- Methods: `acceptStage()`, `rejectStage()`, `calcContribution(double)`
- Behavior: accepts or rejects a stage and computes its weighted contribution to total progress.

### Task : has `WorkStage` collection
- Fields: `id`, `title`, `days`, `progress`, `done`, `stages`
- Methods: `addProgress(double)`, `addStage(const WorkStage&)`, `allStagesAccepted()`, `leftDays()`, `finishTask()`
- Behavior: caps progress at `100%`, checks all stages, estimates remaining days, and refuses to finish while stages are unaccepted.

### WorkSchedule
- Fields: `id`, `planDays`, `passedDays`, `late`, `tasks`
- Methods: `addTask(const Task&)`, `passDays(int)`, `getProgress()`, `countDoneTasks()`, `addPlanDays(int)`
- Behavior: tracks elapsed days, detects lateness against planned days and progress, averages task progress, and clears the late flag when the plan is extended.

### Site
- Fields: `id`, `name`, `address`, `areaGa`, `permit`, `open`
- Methods: `givePermit()`, `openSite()`, `closeSite()`, `distToSite(const Site&)`, `getSummary()`
- Behavior: issues a building permit only for a filled address and positive area, opens only with a permit, closes, computes distance to another site, and formats a summary.

### Project
- Fields: `id`, `name`, `status`, `budgetPlan`, `budgetSpent`, `client`, `site`, `schedule`
- Methods: `checkProject()`, `spendMoney(double)`, `leftBudget()`, `spentPercent()`, `startProject()`, `finishProject()`
- Behavior: validates project/client data, spends money with budget control, computes remaining budget and spending percentage, starts only with a valid open site, and finishes only with a `100%` schedule.

### ResidentialProject : public Project
- Fields: `floors`, `parkingCount`, `hasLift`, `apartments`
- Methods: `addApartment(const Apartment&)`, `totalArea()`, `countUnsold()`
- Behavior: manages apartments, sums their area, and counts unsold units.

### CommercialProject : public Project
- Fields: `levelClass`, `parkingCount`, `hasGate`, `units`
- Methods: `addUnit(const LeaseUnit&)`, `totalMonthRent()`, `rentedPercent()`
- Behavior: manages lease units, sums monthly rent, and computes the rented percentage.

### InfrastructureProject : public Project
- Fields: `typeName`, `speedLimit`, `ecoCheck`, `sections`
- Methods: `addSection(const RoadSection&)`, `totalLength()`, `avgProgress()`
- Behavior: manages road sections, sums their length, and computes average progress.

---

## Finance and contracts

### CostItem
- Fields: `code`, `name`, `count`, `price`
- Methods: `total()`, `multiplyCount(double)`, `isOk()`
- Behavior: computes line cost, scales quantity with validation, and checks item validity.

### Estimate
- Fields: `id`, `title`, `markup`, `reserve`, `approved`, `items`
- Methods: `addItem(const CostItem&)`, `baseSum()`, `totalSum()`, `approve()`, `extraSum()`
- Behavior: validates and stores items, computes base and final sums with markup and reserve, and approves only non-empty estimates.

### Invoice
- Fields: `number`, `sum`, `paidSum`, `paid`, `client`
- Methods: `payPart(double)`, `leftToPay()`, `cancel()`, `getState()`
- Behavior: accepts partial payments without overpayment, computes the outstanding amount, cancels only unpaid invoices, and reports `Paid`, `PartPaid`, or `Unpaid`.

### PaymentSchedule
- Fields: `code`, `parts`, `penaltyPercent`, `invoices`
- Methods: `addInvoice(const Invoice&)`, `totalInvoicesSum()`, `calcPenalty(double, int)`
- Behavior: stores invoices, sums their amounts, and computes penalties from debt, rate, and days.

### Contract
- Fields: `number`, `amount`, `months`, `signedContract`, `client`, `schedule`
- Methods: `sign()`, `changeAmount(double)`, `monthSum()`, `cancelContract()`
- Behavior: signs only with valid client and positive amount/term, changes amount while keeping it positive, and computes a monthly installment.

### BankAccount
- Fields: `accountNumber`, `bankName`, `balance`, `blocked`
- Methods: `putMoney(double)`, `takeMoney(double)`, `sendTo(BankAccount&, double)`, `block()`, `unblock()`
- Behavior: credits and debits with block and amount checks, transfers money between accounts, and toggles the blocked state.

### Payment
- Fields: `id`, `sum`, `payType`, `done`, `client`
- Methods: `makePayment(Invoice&, BankAccount&)`, `rollbackPayment(BankAccount&, Invoice&)`, `isBankTransfer()`, `info()`
- Behavior: performs a payment through an account and invoice, rolls it back, recognizes bank transfers, and formats payment info.

### ExpenseRecord
- Fields: `id`, `sum`, `type`, `refundable`
- Methods: `check()`, `realCost()`, `setRefundable(bool)`
- Behavior: validates the record and returns zero real cost for refundable expenses.

### TaxCalculator
- Fields: `ndsPercent`, `profitTaxPercent`, `salaryTaxPercent`
- Methods: `calcNds(double)`, `calcProfitTax(double)`, `calcSalaryTax(double)`
- Behavior: computes VAT, profit tax, and payroll tax with input validation.

### ProfitAnalyzer
- Fields: `income`, `expenses`, `targetPercent`
- Methods: `profit()`, `realPercent()`, `isProfitOk()`, `needIncome(double)`
- Behavior: computes profit and margin, checks the target margin, and computes the income needed for a requested margin below `100%`.

### Budget
- Fields: `id`, `totalMoney`, `spentMoney`, `records`
- Methods: `addRecord(const ExpenseRecord&)`, `freeMoney()`, `addMoney(double)`, `almostEmpty()`, `nonRefundableSum()`, `clearRecords()`
- Behavior: validates and records expenses within the limit, tracks free money, detects a budget spent at `90%` or more, sums non-refundable costs, and resets records.

---

## Staff hierarchy

### Qualification
- Fields: `title`, `level`, `validYears`, `isExpired`
- Methods: `checkMinLevel(int)`, `renewQualification(int)`, `expire()`
- Behavior: checks that a qualification is not expired and meets the required grade, renews it, and expires it.

### Employee
- Fields: `id`, `fullName`, `position`, `baseSalary`, `workedDays`, `active`
- Methods: `addWorkedDays(int)`, `calcMonthlyPay(int)`, `fireEmployee()`, `info()`
- Behavior: records worked days, computes salary proportional to a norm of days, deactivates the employee, and formats employee info.

### Salary
- Fields: `period`, `grossAmount`, `bonus`, `taxPercent`, `paid`, `employee`
- Methods: `addBonus(double)`, `calcNetSalary()`, `payOut()`, `totalWithBonus()`
- Behavior: adds a non-negative bonus, computes net pay after tax, pays only an active employee once, and computes gross pay with bonus.

### Worker : public Employee
- Fields: `specialty`, `hoursWorked`, `hourlyRate`, `hasHelmetAndVest`, `qualification`
- Methods: `logHours(int)`, `calcPieceworkPay()`, `checkSafetyGear()`, `canDoTask(int)`
- Behavior: logs hours and derives worked days, computes piecework pay, requires helmet and vest, and checks task qualification.

### Foreman : public Employee
- Fields: `brigadeName`, `brigadeSize`, `onSite`, `workers`
- Methods: `addWorker(const Worker&)`, `checkAllBrigadeGear()`, `countQualifiedWorkers(int)`, `sendToSite()`
- Behavior: manages a brigade, checks the safety gear of every worker, counts qualified workers, and sends the brigade to the site.

### Engineer : public Employee
- Fields: `specialization`, `licenseNumber`, `inspectedObjects`, `licenseValid`, `assignedSite`
- Methods: `verifyLicense()`, `assignSite(const Site&)`, `canApproveDrawings()`, `revokeLicense()`, `getSiteSummary()`
- Behavior: validates the license, assigns a site only with a valid license, checks drawing-approval rights, revokes the license, and reports the assigned site.

### Accountant : public Employee
- Fields: `certNumber`, `processedDocs`, `canApprovePayments`
- Methods: `registerDoc()`, `calcTaxForSum(double, double)`, `checkPaymentLimit(double, double)`, `resetDocCounter()`, `blockPayments()`
- Behavior: registers documents only while active, computes tax for a sum, checks payment approval limits, and blocks payments.

### HRManager : public Employee
- Fields: `hiresCount`, `dismissalsCount`, `staffList`
- Methods: `hireEmployee(const Employee&)`, `dismissEmployee(const std::string&)`, `countActiveStaff()`, `checkWorkerQualification(const Worker&, int)`, `averageSalary()`
- Behavior: hires employees, dismisses by ID with validation, counts active staff, checks worker qualification, and computes average salary.

---

## Equipment hierarchy

### Operator : public Employee
- Fields: `permitCode`, `operatingHours`, `permitActive`, `qualification`
- Methods: `checkPermit(int)`, `addOperatingHours(int)`, `suspendPermit()`, `isReadyForShift()`
- Behavior: validates the equipment permit and grade, records operating hours, suspends the permit, and checks readiness for a shift.

### Mechanic : public Employee
- Fields: `workshop`, `repairsCount`, `certified`
- Methods: `performRepair(int)`, `canInspectHeavyMachinery()`, `revokeCertification()`, `mechanicStatus()`
- Behavior: performs repairs only while active and certified, checks heavy-machinery inspection rights, revokes certification, and reports status.

### MaintenanceRecord
- Fields: `recordId`, `description`, `cost`, `motorHoursAtService`, `completed`, `mechanic`
- Methods: `completeService(double)`, `isWarrantyEligible()`, `shortInfo()`
- Behavior: completes service with a mechanic and nonnegative cost, checks warranty eligibility below `500` motor hours, and formats short info.

### Equipment
- Fields: `code`, `model`, `maxLoadTons`, `motorHours`, `available`, `inRepair`, `currentSite`, `assignedOperator`, `history`
- Methods: `checkAvailability()`, `checkLoad(double)`, `assignToSite(const Site&, const Operator&)`, `releaseFromWork(int)`, `sendToRepair(const MaintenanceRecord&)`
- Behavior: checks availability and load, assigns to a site with a permitted operator, releases after work while counting motor hours, and sends the machine to repair.

### Excavator : public Equipment
- Fields: `bucketVolumeCubicM`, `maxDigDepthM`, `hasHydraulicHammer`
- Methods: `calcDiggingCapacity(int)`, `digTrench(double, double)`, `attachHammer()`
- Behavior: computes hourly digging capacity, digs a trench with availability/load/depth checks, and attaches a hydraulic hammer.

### Crane : public Equipment
- Fields: `boomLengthM`, `maxHeightM`, `windLockEngaged`
- Methods: `liftCargo(double, double)`, `toggleWindLock(bool)`, `maxMomentTonMeters()`, `canReach(double)`
- Behavior: lifts cargo with load, wind-lock, and height checks, toggles the wind lock, computes maximum load moment, and checks reachability.

### ConcreteMixer : public Equipment
- Fields: `drumVolumeCubicM`, `currentConcreteVolume`, `drumRotating`
- Methods: `loadConcrete(double)`, `pourConcrete(double)`, `freeDrumCapacity()`
- Behavior: loads concrete without overloading the drum, pours concrete with an insufficiency check, and computes remaining drum capacity.

---

## Materials and warehouse

### Material
- Fields: `id`, `name`, `unit`, `price`, `weightPerUnit`
- Methods: `calcCost(double)`, `calcWeight(double)`, `isValid()`, `shortName()`
- Behavior: computes cost and weight for a positive quantity, validates material data, and formats a short name.

### Supplier
- Fields: `id`, `name`, `phone`, `rating`, `active`
- Methods: `checkActive()`, `changeRating(double)`, `isReliable()`, `blockSupplier()`
- Behavior: checks that the supplier is active, adjusts rating with clamping, checks reliability, and blocks the supplier.

### DeliveryOrder
- Fields: `orderId`, `quantity`, `deliveryCost`, `delivered`, `supplier`, `material`, `targetSite`
- Methods: `confirmDelivery()`, `fullOrderPrice()`, `totalCargoWeight()`, `cancelOrder()`
- Behavior: confirms delivery with quantity and supplier checks, computes material cost plus delivery, computes cargo weight, and cancels an undelivered order.

### Rack
- Fields: `code`, `maxWeightKg`, `currentWeightKg`
- Methods: `putWeight(double)`, `takeWeight(double)`, `freeWeight()`
- Behavior: loads and unloads weight with overload validation and reports free capacity.

### WarehouseZone
- Fields: `name`, `shelvesCount`, `covered`, `racks`
- Methods: `addRack(const Rack&)`, `totalFreeCapacity()`, `canStoreWeight(double)`
- Behavior: manages racks, sums free capacity, and checks whether a weight can be stored.

### MaterialStock
- Fields: `stockId`, `totalQty`, `reservedQty`, `minQty`, `material`, `zone`
- Methods: `freeQty()`, `reserve(double)`, `writeOff(double)`, `addStock(double)`, `needOrder()`
- Behavior: computes free quantity, reserves and writes off stock with insufficiency checks, adds stock, and detects an order requirement at the minimum level.

### MaterialRequirement
- Fields: `reqId`, `requiredQty`, `fulfilled`, `material`, `task`
- Methods: `calcNeedCost()`, `fulfillFromStock(MaterialStock&)`, `missingQty(const MaterialStock&)`
- Behavior: computes need cost, fulfills the requirement from stock once, and computes missing quantity.

---

## Safety, quality and company

### SafetyViolation
- Fields: `id`, `description`, `severity`, `fixed`, `worker`
- Methods: `checkCritical()`, `markFixed()`, `info()`
- Behavior: throws on an unfixed violation of severity `4` or `5`, marks it fixed, and formats severity info.

### SafetyInspection
- Fields: `id`, `date`, `passed`, `site`, `inspector`, `violations`
- Methods: `addViolation(const SafetyViolation&)`, `openViolationsCount()`, `fixAllViolations()`, `summary()`
- Behavior: registers violations, fails the inspection for severity `3` or higher, counts open violations, fixes all of them, and formats a summary.

### DefectReport
- Fields: `id`, `defectText`, `severity`, `fixed`, `stage`, `task`
- Methods: `checkDefect()`, `fixDefect()`, `shortDesc()`
- Behavior: throws on a critical unfixed defect, marks it fixed, and formats a short description.

### QualityControl
- Fields: `controlId`, `checkedCount`, `approved`, `stage`, `defects`
- Methods: `addDefect(const DefectReport&)`, `unfixedCount()`, `approveStageIfClean()`, `defectRate()`
- Behavior: registers defects, rejects the control for severity `3` or higher, accepts the stage only without unfixed defects, and computes defect rate.

### ConstructionCompany
- Fields: `name`, `inn`, `mainBudget`, `clients`, `projects`, `sites`, `employees`
- Methods: `addClient(const Client&)`, `addProject(const Project&)`, `findProjectById(const std::string&)`, `addSite(const Site&)`, `hireEmployee(const Employee&)`, `totalPlannedBudget()`
- Behavior: adds validated clients and projects, looks up a project by ID or throws `ProjectNotFoundException`, adds sites and employees, and sums planned budgets across projects.

---

## Associations

A class is associated with another class when it stores it as a field or
accepts/returns it as a method parameter. The model contains more than 60 such
associations; the field-level ones are listed below.

| # | Association | Kind |
|---:|---|---|
| 1 | `Address` → `GeoPoint` via `point` | field |
| 2 | `Project` → `Client` via `client` | field |
| 3 | `Project` → `Site` via `site` | field |
| 4 | `Project` → `WorkSchedule` via `schedule` | field |
| 5 | `Site` → `Address` via `address` | field |
| 6 | `WorkSchedule` → `Task` via `tasks` | field collection |
| 7 | `Task` → `WorkStage` via `stages` | field collection |
| 8 | `ResidentialProject` → `Apartment` via `apartments` | field collection |
| 9 | `CommercialProject` → `LeaseUnit` via `units` | field collection |
| 10 | `InfrastructureProject` → `RoadSection` via `sections` | field collection |
| 11 | `Estimate` → `CostItem` via `items` | field collection |
| 12 | `Contract` → `Client` via `client` | field |
| 13 | `Contract` → `PaymentSchedule` via `schedule` | field |
| 14 | `PaymentSchedule` → `Invoice` via `invoices` | field collection |
| 15 | `Invoice` → `Client` via `client` | field |
| 16 | `Payment` → `Client` via `client` | field |
| 17 | `Salary` → `Employee` via `employee` | field |
| 18 | `Worker` → `Qualification` via `qualification` | field |
| 19 | `Foreman` → `Worker` via `workers` | field collection |
| 20 | `Engineer` → `Site` via `assignedSite` | field |
| 21 | `HRManager` → `Employee` via `staffList` | field collection |
| 22 | `Operator` → `Qualification` via `qualification` | field |
| 23 | `MaintenanceRecord` → `Mechanic` via `mechanic` | field |
| 24 | `Equipment` → `Site` via `currentSite` | field |
| 25 | `Equipment` → `Operator` via `assignedOperator` | field |
| 26 | `Equipment` → `MaintenanceRecord` via `history` | field collection |
| 27 | `DeliveryOrder` → `Supplier` via `supplier` | field |
| 28 | `DeliveryOrder` → `Material` via `material` | field |
| 29 | `DeliveryOrder` → `Site` via `targetSite` | field |
| 30 | `WarehouseZone` → `Rack` via `racks` | field collection |
| 31 | `MaterialStock` → `Material` via `material` | field |
| 32 | `MaterialStock` → `WarehouseZone` via `zone` | field |
| 33 | `MaterialRequirement` → `Material` via `material` | field |
| 34 | `MaterialRequirement` → `Task` via `task` | field |
| 35 | `SafetyViolation` → `Worker` via `worker` | field |
| 36 | `SafetyInspection` → `Site` via `site` | field |
| 37 | `SafetyInspection` → `Engineer` via `inspector` | field |
| 38 | `SafetyInspection` → `SafetyViolation` via `violations` | field collection |
| 39 | `DefectReport` → `WorkStage` via `stage` | field |
| 40 | `DefectReport` → `Task` via `task` | field |
| 41 | `QualityControl` → `WorkStage` via `stage` | field |
| 42 | `QualityControl` → `DefectReport` via `defects` | field collection |
| 43 | `ConstructionCompany` → `Budget` via `mainBudget` | field |
| 44 | `ConstructionCompany` → `Client` via `clients` | field collection |
| 45 | `ConstructionCompany` → `Project` via `projects` | field collection |
| 46 | `ConstructionCompany` → `Site` via `sites` | field collection |
| 47 | `ConstructionCompany` → `Employee` via `employees` | field collection |
| 48 | `Budget` → `ExpenseRecord` via `records` | field collection |
| 49 | `Payment::makePayment(Invoice&, BankAccount&)` | parameter |
| 50 | `Payment::rollbackPayment(BankAccount&, Invoice&)` | parameter |
| 51 | `Equipment::assignToSite(const Site&, const Operator&)` | parameter |
| 52 | `Engineer::assignSite(const Site&)` | parameter |
| 53 | `Foreman::addWorker(const Worker&)` | parameter |
| 54 | `HRManager::checkWorkerQualification(const Worker&, int)` | parameter |
| 55 | `Task::addStage(const WorkStage&)` | parameter |
| 56 | `WorkSchedule::addTask(const Task&)` | parameter |
| 57 | `Estimate::addItem(const CostItem&)` | parameter |
| 58 | `PaymentSchedule::addInvoice(const Invoice&)` | parameter |
| 59 | `Budget::addRecord(const ExpenseRecord&)` | parameter |
| 60 | `ResidentialProject::addApartment(const Apartment&)` | parameter |
| 61 | `CommercialProject::addUnit(const LeaseUnit&)` | parameter |
| 62 | `InfrastructureProject::addSection(const RoadSection&)` | parameter |
| 63 | `MaterialRequirement::fulfillFromStock(MaterialStock&)` | parameter |
| 64 | `SafetyInspection::addViolation(const SafetyViolation&)` | parameter |
| 65 | `QualityControl::addDefect(const DefectReport&)` | parameter |
| 66 | `ConstructionCompany::addClient/addProject/addSite/hireEmployee` | parameter |
| 67 | `Site::distToSite(const Site&)` | parameter |

---

## Project layout

- `строительная компания/Exceptions.h` — exception hierarchy
- `строительная компания/CoreGeo.h` — coordinates, addresses, clients, vendors
- `строительная компания/ProjectsSchedule.h` — projects, sites, schedules, tasks, stages
- `строительная компания/FinanceContracts.h` — estimates, contracts, invoices, payments, budgets, taxes
- `строительная компания/Staff.h` — employees, salaries, qualifications, HR
- `строительная компания/Equipment.h` — machinery, operators, mechanics, maintenance
- `строительная компания/Materials.h` — materials, suppliers, deliveries, warehouses, stock
- `строительная компания/QualitySafetyCompany.h` — safety, quality, and the company class
- `строительная компания/main.cpp` — interactive demonstration program
- `ConstructionTests/ConstructionTests.cpp` — self-contained test suite
- `ConstructionTests/main.cpp` — auxiliary material-stock checks
- `UnitTest1/UnitTest1.cpp` — Visual Studio unit-test project stub
- `строительная компания.slnx` — solution configuration (x64 and x86)
