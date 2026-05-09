# MediCore — Hospital Management System

## OOP Project | Spring 2026 | BCS-2A

MUHAMMAD AHMAD MANZOOR

\---

## How to Compile

```bash
cd HospitalMS
make
```

This produces a binary named `medicore` in the `HospitalMS/` directory.

## How to Run

```bash
cd HospitalMS
./medicore
```

> \*\*Important:\*\* Always run from the `HospitalMS/` directory so relative paths to `data/\*.txt` resolve correctly.

## How to Clean

```bash
make clean
```

\---

## Project Structure

```
HospitalMS/
├── Makefile
├── README.md
├── data/                    ← all .txt data files live here
│   ├── admin.txt
│   ├── appointments.txt
│   ├── bills.txt
│   ├── discharged.txt
│   ├── doctors.txt
│   ├── patients.txt
│   ├── prescriptions.txt
│   └── security\_log.txt│   └── Validator.h
└── src/                     ← all .cpp implementation files
      └── main.cpp
```

\---

## Class Overview

|Class|Description|
|-|-|
|`Person`|Abstract base class; holds id, name, contact, password; two pure virtual methods: `showMenu()` and `getRole()`|
|`Patient`|Inherits Person; adds age, gender, balance; overloads `+=`, `-=`, `==`, `<<`|
|`Doctor`|Inherits Person; adds specialization, fee; overloads `==`, `<<`|
|`Admin`|Inherits Person; loaded from admin.txt|
|`Appointment`|Single appointment; overloads `==` (conflict check), `<<`|
|`Bill`|Charge from an appointment; status: unpaid/paid/cancelled|
|`Prescription`|Doctor's medicines + notes for a completed appointment|
|`Storage<T>`|Generic template container; static array `T data\[100]`; provides add/removeById/findById/getAll/size/update|
|`FileHandler`|Only class doing file I/O; load/append/update/delete for all entity types|
|`Validator`|Only class doing input validation; date, slot, contact, password, menu choice, float, int|
|`HospitalException`|Base exception class; char message\[200] + virtual what()|
|`FileNotFoundException`|Thrown when a required .txt file cannot be opened|
|`InsufficientFundsException`|Thrown when balance < required amount|
|`InvalidInputException`|Thrown on bad user input|
|`SlotUnavailableException`|Thrown when a booking slot is already taken|
|`PatientMenu`|All patient session logic (book, cancel, view, pay, top-up)|
|`DoctorMenu`|All doctor session logic (view, mark, prescribe)|
|`AdminMenu`|All admin session logic (add/remove doctor, discharge, reports)|

\---

## Login Credentials (default data)

### Patients

|ID|Password|
|-|-|
|1|pass123|
|2|pass234|

### Doctors

|ID|Password|Specialization|
|-|-|-|
|1|doc123|Cardiology|
|2|doc234|Neurology|
|3|doc345|Orthopedics|

### Admin

|ID|Password|
|-|-|
|1|admin123|

\---

## Key Design Decisions

* **No `std::string` or `std::vector`** — all strings are `char\[]`, all collections are `Storage<T>` with a fixed array of 100.
* **No `strcmp`, `strtok`, `strlen`** — all string operations are implemented manually in `Validator` and local helpers.
* **Case-insensitive specialization search** — uses manual `toLowerChar()` per character (no `<cctype>` tolower).
* **Dynamic allocation** — all user-facing pointers are into the `Storage<T>` arrays; no heap allocation is done without a corresponding delete path.
* **No memory leaks** — all `Storage<T>` objects are stack-allocated and automatically destroyed on exit. No `new`/`delete` is used directly.
* **Persistence** — every mutation immediately rewrites the relevant `.txt` file. On startup all files are re-read into Storage.
* **Security log** — every login attempt (success or failure) is appended to `security\_log.txt` with a timestamp.
* **Three-attempt lockout** — after 3 consecutive failed logins the session is locked and the event is logged.

