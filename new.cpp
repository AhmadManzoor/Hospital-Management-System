//
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <string>
//#include <ctime>
//#include <cmath>
//#include <cassert>
//#include <cctype>
//#include <algorithm>  // only for non-sort uses (e.g. std::transform on single char)
//
//// ─── SFML headers ────────────────────────────────────────────
//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>
//#include <SFML/System.hpp>
//
//using namespace std;
//
//// ═══════════════════════════════════════════════════════════════
////  SECTION 1 — Custom Exception Hierarchy
//
///*
// * HospitalException — base class for all custom exceptions.
// */
//class HospitalException {
//protected:
//    char message[200]; // fixed buffer for the error description
//public:
//    // Constructor: copy msg into the buffer (safe, manual)
//    explicit HospitalException(const string& msg) {
//        int i = 0;
//        for (; i < 199 && i < (int)msg.size(); ++i)
//            message[i] = msg[i];
//        message[i] = '\0';
//    }
//    // Returns the stored error message — virtual so subclasses can override
//    virtual const char* what() const { return message; }
//    virtual ~HospitalException() {}
//};
//
///* Thrown when a required data file cannot be opened on startup */
//class FileNotFoundException : public HospitalException {
//public:
//    explicit FileNotFoundException(const string& filename)
//        : HospitalException("File not found: " + filename) {
//    }
//};
//
///* Thrown when a patient's balance is below the required amount */
//class InsufficientFundsException : public HospitalException {
//public:
//    InsufficientFundsException()
//        : HospitalException("Insufficient funds. Please top up your balance.") {
//    }
//};
//
///* Thrown when user-supplied input fails validation rules */
//class InvalidInputException : public HospitalException {
//public:
//    explicit InvalidInputException(const string& detail)
//        : HospitalException("Invalid input: " + detail) {
//    }
//};
//
///* Thrown when a patient tries to book an already-occupied time slot */
//class SlotUnavailableException : public HospitalException {
//public:
//    SlotUnavailableException()
//        : HospitalException("That time slot is already taken. Please choose another.") {
//    }
//};
//
//// ═══════════════════════════════════════════════════════════════
////  SECTION 2 — Storage<T>  (generic fixed-array container)
//// ═══════════════════════════════════════════════════════════════
//
///*
// * Storage<T> — a generic template container backed by a fixed array.
// * Capacity is 200 elements; no vectors, no dynamic allocation of the
// * array itself (the array lives inside the object).
// *
// * T must have:
// *   int  getId()  const
// *   bool isActive() const   (soft-delete flag)
// */
//template<typename T>
//class Storage {
//private:
//    T    data[200]; // fixed backing array — no vectors allowed
//    int  count;     // number of records currently stored (including soft-deleted)
//public:
//    Storage() : count(0) {}
//
//    // Add a new element; returns false if capacity exceeded
//    bool add(const T& item) {
//        if (count >= 200) return false;
//        data[count++] = item;
//        return true;
//    }
//
//    // Find element by integer ID; returns nullptr if not found or inactive
//    T* findById(int id) {
//        for (int i = 0; i < count; ++i)
//            if (data[i].getId() == id && data[i].isActive())
//                return &data[i];
//        return nullptr;
//    }
//
//    // Remove (soft-delete) element by ID; returns true on success
//    bool removeById(int id) {
//        for (int i = 0; i < count; ++i) {
//            if (data[i].getId() == id && data[i].isActive()) {
//                data[i].deactivate(); // mark as deleted
//                return true;
//            }
//        }
//        return false;
//    }
//
//    // Return pointer to the raw array (for iteration)
//    T* getAll() { return data; }
//
//    // Total stored records (active + inactive)
//    int size() const { return count; }
//
//    // Clears all records (used when reloading from file)
//    void clear() { count = 0; }
//};
//
//// ═══════════════════════════════════════════════════════════════
////  SECTION 3 — Validator
//// ═══════════════════════════════════════════════════════════════
//
///*
// * Validator — the ONLY class that contains input-validation logic.
// * All methods are static so no instance is needed.
// */
//class Validator {
//public:
//    // ── Positive float check ────────────────────────────────────
//    static bool isPositiveFloat(const string& s) {
//        if (s.empty()) return false;
//        bool dotSeen = false;
//        int  digits = 0;
//        for (int i = 0; i < (int)s.size(); ++i) {
//            char c = s[i];
//            if (c == '.' && !dotSeen) { dotSeen = true; continue; }
//            if (c < '0' || c > '9')  return false;
//            ++digits;
//        }
//        if (digits == 0) return false;
//        // Must be > 0
//        double val = stod(s);
//        return val > 0.0;
//    }
//
//    // ── Positive integer check ──────────────────────────────────
//    static bool isPositiveInt(const string& s) {
//        if (s.empty()) return false;
//        for (char c : s) if (c < '0' || c > '9') return false;
//        return stoi(s) > 0;
//    }
//
//    // ── 11-digit all-numeric contact number ─────────────────────
//    static bool isValidContact(const string& s) {
//        if (s.size() != 11) return false;
//        for (char c : s) if (c < '0' || c > '9') return false;
//        return true;
//    }
//
//    // ── Password: minimum 6 characters ─────────────────────────
//    static bool isValidPassword(const string& s) {
//        return s.size() >= 6;
//    }
//
//    // ── Date: DD-MM-YYYY, month 01-12, day 01-31, year >= current
//    static bool isValidDate(const string& s) {
//        if (s.size() != 10) return false;
//        if (s[2] != '-' || s[5] != '-') return false;
//        // Extract parts
//        string dd = s.substr(0, 2);
//        string mm = s.substr(3, 2);
//        string yyyy = s.substr(6, 4);
//        // All digits?
//        for (char c : dd)   if (c < '0' || c > '9') return false;
//        for (char c : mm)   if (c < '0' || c > '9') return false;
//        for (char c : yyyy) if (c < '0' || c > '9') return false;
//        int day = stoi(dd);
//        int mon = stoi(mm);
//        int year = stoi(yyyy);
//        if (day < 1 || day > 31) return false;
//        if (mon < 1 || mon > 12) return false;
//        // Year must be current year or later
//        time_t now = time(nullptr);
//        struct tm* t = localtime(&now);
//        int curYear = t->tm_year + 1900;
//        if (year < curYear) return false;
//        return true;
//    }
//
//    // ── One of the 8 fixed time slots ───────────────────────────
//    static bool isValidSlot(const string& s) {
//        const string slots[8] = {
//            "09:00","10:00","11:00","12:00",
//            "13:00","14:00","15:00","16:00"
//        };
//        for (int i = 0; i < 8; ++i)
//            if (s == slots[i]) return true;
//        return false;
//    }
//
//    // ── Menu choice in range [lo, hi] ───────────────────────────
//    static bool isMenuChoice(const string& s, int lo, int hi) {
//        if (s.empty()) return false;
//        for (char c : s) if (c < '0' || c > '9') return false;
//        int v = stoi(s);
//        return v >= lo && v <= hi;
//    }
//
//    // ── Non-empty string with max length ────────────────────────
//    static bool isValidName(const string& s, int maxLen = 100) {
//        return !s.empty() && (int)s.size() <= maxLen;
//    }
//
//    // ── Positive balance top-up ─────────────────────────────────
//    static bool isValidTopUp(const string& s) {
//        return isPositiveFloat(s);
//    }
//};
//
//// ═══════════════════════════════════════════════════════════════
////  SECTION 4 — Entity Classes (forward declarations where needed)
//// ═══════════════════════════════════════════════════════════════
//
//// ── Forward-declare Appointment so Patient/Doctor can reference it ──
//class Appointment;
//
///* ────────────────────────────────────────────────────────────────
// * Person — Abstract base class for Patient, Doctor, Admin.
// * Holds identity attributes common to all users.
// * Has two pure virtual methods: getRole() and displayInfo().
// * ────────────────────────────────────────────────────────────────*/
//class Person {
//protected:
//    int    id;
//    string name;
//    string contact;
//    string password;
//    bool   active; // soft-delete flag used by Storage<T>
//public:
//    Person() : id(0), active(true) {}
//    Person(int i, const string& n, const string& c, const string& p)
//        : id(i), name(n), contact(c), password(p), active(true) {
//    }
//
//    // Pure virtual methods — every concrete user type must implement these
//    virtual string getRole()    const = 0;
//    virtual void   displayInfo()const = 0;
//
//    // Common accessors
//    int    getId()      const { return id; }
//    string getName()    const { return name; }
//    string getContact() const { return contact; }
//    string getPassword()const { return password; }
//    bool   isActive()   const { return active; }
//    void   deactivate() { active = false; }
//
//    // Setters
//    void setName(const string& n) { name = n; }
//    void setContact(const string& c) { contact = c; }
//    void setPassword(const string& p) { password = p; }
//
//    virtual ~Person() {}
//};
//
///* ────────────────────────────────────────────────────────────────
// * Patient — a registered patient.
// * Operator overloads:
// *   += : add to balance
// *   -= : deduct from balance
// *   == : compare by patient ID
// *   << : formatted console output
// * ────────────────────────────────────────────────────────────────*/
//class Patient : public Person {
//private:
//    int    age;
//    char   gender; // 'M' or 'F'
//    double balance;
//public:
//    Patient() : Person(), age(0), gender('M'), balance(0.0) {}
//    Patient(int i, const string& n, int a, char g,
//        const string& c, const string& p, double b)
//        : Person(i, n, c, p), age(a), gender(g), balance(b) {
//    }
//
//    // ── Pure virtuals from Person ───────────────────────────────
//    string getRole()    const override { return "Patient"; }
//    void   displayInfo()const override {
//        cout << "Patient [" << id << "] " << name
//            << " | Age: " << age
//            << " | Gender: " << gender
//            << " | Balance: PKR " << balance << "\n";
//    }
//
//    // ── Accessors ───────────────────────────────────────────────
//    int    getAge()    const { return age; }
//    char   getGender() const { return gender; }
//    double getBalance()const { return balance; }
//    void   setBalance(double b) { balance = b; }
//
//    // ── Operator overloads ──────────────────────────────────────
//
//    // += : add to patient balance (top-up, refund)
//    Patient& operator+=(double amount) {
//        balance += amount;
//        return *this;
//    }
//
//    // -= : deduct from patient balance (fee, bill payment)
//    Patient& operator-=(double amount) {
//        balance -= amount;
//        return *this;
//    }
//
//    // == : compare two patients by ID
//    bool operator==(const Patient& other) const {
//        return id == other.id;
//    }
//
//    // << : formatted output to ostream
//    friend ostream& operator<<(ostream& os, const Patient& p) {
//        os << "Patient ID: " << p.id
//            << " | Name: " << p.name
//            << " | Age: " << p.age
//            << " | Gender: " << p.gender
//            << " | Contact: " << p.contact
//            << " | Balance: PKR " << p.balance;
//        return os;
//    }
//};
//
///* ────────────────────────────────────────────────────────────────
// * Doctor — a hospital doctor with specialization and fee.
// * Operator overloads:
// *   == : compare by doctor ID
// *   << : formatted console output
// * ────────────────────────────────────────────────────────────────*/
//class Doctor : public Person {
//private:
//    string specialization;
//    double fee;
//public:
//    Doctor() : Person(), fee(0.0) {}
//    Doctor(int i, const string& n, const string& spec,
//        const string& c, const string& p, double f)
//        : Person(i, n, c, p), specialization(spec), fee(f) {
//    }
//
//    // ── Pure virtuals from Person ───────────────────────────────
//    string getRole()    const override { return "Doctor"; }
//    void   displayInfo()const override {
//        cout << "Doctor [" << id << "] " << name
//            << " | Spec: " << specialization
//            << " | Fee: PKR " << fee << "\n";
//    }
//
//    // ── Accessors ───────────────────────────────────────────────
//    string getSpecialization() const { return specialization; }
//    double getFee()            const { return fee; }
//
//    // == : compare by doctor ID
//    bool operator==(const Doctor& other) const {
//        return id == other.id;
//    }
//
//    // << : formatted output
//    friend ostream& operator<<(ostream& os, const Doctor& d) {
//        os << "Doctor ID: " << d.id
//            << " | Name: " << d.name
//            << " | Spec: " << d.specialization
//            << " | Contact: " << d.contact
//            << " | Fee: PKR " << d.fee;
//        return os;
//    }
//};
//
///* ────────────────────────────────────────────────────────────────
// * Admin — the system administrator loaded from admin.txt.
// * ────────────────────────────────────────────────────────────────*/
//class Admin : public Person {
//public:
//    Admin() : Person() {}
//    Admin(int i, const string& n, const string& p)
//        : Person(i, n, "", p) {
//    }
//
//    string getRole()    const override { return "Admin"; }
//    void   displayInfo()const override {
//        cout << "Admin [" << id << "] " << name << "\n";
//    }
//};
//
///* ────────────────────────────────────────────────────────────────
// * Appointment — a single scheduled appointment.
// * Operator overloads:
// *   == : detect scheduling conflict (same doctor, date, slot,
// *         neither cancelled)
// *   << : formatted output
// * ────────────────────────────────────────────────────────────────*/
//class Appointment {
//private:
//    int    id;
//    int    patientId;
//    int    doctorId;
//    string date;      // DD-MM-YYYY
//    string timeSlot;  // e.g. "09:00"
//    string status;    // pending | completed | cancelled | no-show
//    bool   active;    // soft-delete flag for Storage<T>
//public:
//    Appointment()
//        : id(0), patientId(0), doctorId(0),
//        status("pending"), active(true) {
//    }
//    Appointment(int i, int pid, int did,
//        const string& d, const string& ts, const string& st)
//        : id(i), patientId(pid), doctorId(did),
//        date(d), timeSlot(ts), status(st), active(true) {
//    }
//
//    // ── Accessors ───────────────────────────────────────────────
//    int    getId()        const { return id; }
//    int    getPatientId() const { return patientId; }
//    int    getDoctorId()  const { return doctorId; }
//    string getDate()      const { return date; }
//    string getTimeSlot()  const { return timeSlot; }
//    string getStatus()    const { return status; }
//    bool   isActive()     const { return active; }
//    void   deactivate() { active = false; }
//    void   setStatus(const string& s) { status = s; }
//
//    // == : conflict check — same doctor, same date, same slot,
//    //      and neither appointment is cancelled
//    bool operator==(const Appointment& other) const {
//        if (doctorId != other.doctorId) return false;
//        if (date != other.date)     return false;
//        if (timeSlot != other.timeSlot) return false;
//        if (status == "cancelled")    return false;
//        if (other.status == "cancelled")return false;
//        return true;
//    }
//
//    // << : formatted output
//    friend ostream& operator<<(ostream& os, const Appointment& a) {
//        os << "Appt ID: " << a.id
//            << " | Patient: " << a.patientId
//            << " | Doctor: " << a.doctorId
//            << " | Date: " << a.date
//            << " | Slot: " << a.timeSlot
//            << " | Status: " << a.status;
//        return os;
//    }
//};
//
///* ────────────────────────────────────────────────────────────────
// * Bill — a financial charge tied to an appointment.
// * ────────────────────────────────────────────────────────────────*/
//class Bill {
//private:
//    int    id;
//    int    patientId;
//    int    appointmentId;
//    double amount;
//    string status; // unpaid | paid | cancelled
//    string date;   // DD-MM-YYYY
//    bool   active;
//public:
//    Bill()
//        : id(0), patientId(0), appointmentId(0),
//        amount(0.0), status("unpaid"), active(true) {
//    }
//    Bill(int i, int pid, int aid, double amt,
//        const string& st, const string& d)
//        : id(i), patientId(pid), appointmentId(aid),
//        amount(amt), status(st), date(d), active(true) {
//    }
//
//    int    getId()            const { return id; }
//    int    getPatientId()     const { return patientId; }
//    int    getAppointmentId() const { return appointmentId; }
//    double getAmount()        const { return amount; }
//    string getStatus()        const { return status; }
//    string getDate()          const { return date; }
//    bool   isActive()         const { return active; }
//    void   deactivate() { active = false; }
//    void   setStatus(const string& s) { status = s; }
//};
//
///* ────────────────────────────────────────────────────────────────
// * Prescription — medicines and notes for a completed appointment.
// * ────────────────────────────────────────────────────────────────*/
//class Prescription {
//private:
//    int    id;
//    int    appointmentId;
//    int    patientId;
//    int    doctorId;
//    string date;
//    string medicines; // semicolon-separated: "Paracetamol 500mg;Amox 250mg"
//    string notes;
//    bool   active;
//public:
//    Prescription()
//        : id(0), appointmentId(0), patientId(0),
//        doctorId(0), active(true) {
//    }
//    Prescription(int i, int aid, int pid, int did,
//        const string& d, const string& m, const string& n)
//        : id(i), appointmentId(aid), patientId(pid), doctorId(did),
//        date(d), medicines(m), notes(n), active(true) {
//    }
//
//    int    getId()            const { return id; }
//    int    getAppointmentId() const { return appointmentId; }
//    int    getPatientId()     const { return patientId; }
//    int    getDoctorId()      const { return doctorId; }
//    string getDate()          const { return date; }
//    string getMedicines()     const { return medicines; }
//    string getNotes()         const { return notes; }
//    bool   isActive()         const { return active; }
//    void   deactivate() { active = false; }
//};
//
//// ═══════════════════════════════════════════════════════════════
////  SECTION 5 — FileHandler
////  The ONLY class permitted to perform file I/O.
//// ═══════════════════════════════════════════════════════════════
//
///*
// * FileHandler — centralises all file operations.
// * Each load*() method reads a .txt file and populates a Storage<T>.
// * Each save*() / update*() / delete*() method rewrites the relevant file.
// *
// * CSV format: fields separated by commas, one record per line,
// * no spaces around commas.
// */
//class FileHandler {
//public:
//    // ── Helper: get today's date as DD-MM-YYYY ──────────────────
//    static string todayString() {
//        time_t now = time(nullptr);
//        struct tm* t = localtime(&now);
//        char buf[11];
//        strftime(buf, sizeof(buf), "%d-%m-%Y", t);
//        return string(buf);
//    }
//
//    // ── Helper: get current timestamp for security log ──────────
//    static string nowTimestamp() {
//        time_t now = time(nullptr);
//        struct tm* t = localtime(&now);
//        char buf[25];
//        strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", t);
//        return string(buf);
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Ensure a file exists (creates empty file if absent)
//    // ────────────────────────────────────────────────────────────
//    static void ensureFile(const string& filename) {
//        ifstream f(filename);
//        if (!f.is_open()) {
//            ofstream out(filename); // create empty
//            out.close();
//        }
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Seed default admin if admin.txt is empty
//    // ────────────────────────────────────────────────────────────
//    static void seedAdmin() {
//        ensureFile("admin.txt");
//        ifstream f("admin.txt");
//        string line;
//        bool hasContent = false;
//        while (getline(f, line))
//            if (!line.empty()) { hasContent = true; break; }
//        f.close();
//        if (!hasContent) {
//            ofstream out("admin.txt");
//            out << "1,Admin,admin123\n";
//            out.close();
//        }
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Load patients from patients.txt into storage
//    //  Format: id,name,age,gender,contact,password,balance
//    // ────────────────────────────────────────────────────────────
//    static void loadPatients(Storage<Patient>& store) {
//        store.clear();
//        ensureFile("patients.txt");
//        ifstream f("patients.txt");
//        string line;
//        while (getline(f, line)) {
//            if (line.empty()) continue;
//            // Parse CSV manually using stringstream
//            stringstream ss(line);
//            string tok;
//            vector<string> fields; // local use only
//            // Actually parse by comma position to strictly avoid vector:
//            string f0, f1, f2, f3, f4, f5, f6;
//            getline(ss, f0, ','); // id
//            getline(ss, f1, ','); // name
//            getline(ss, f2, ','); // age
//            getline(ss, f3, ','); // gender
//            getline(ss, f4, ','); // contact
//            getline(ss, f5, ','); // password
//            getline(ss, f6, ','); // balance
//
//            if (f0.empty()) continue;
//            int    id = stoi(f0);
//            int    age = stoi(f2);
//            char   g = f3.empty() ? 'M' : f3[0];
//            double bal = stod(f6);
//            Patient p(id, f1, age, g, f4, f5, bal);
//            store.add(p);
//        }
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Save ALL active patients back to patients.txt
//    // ────────────────────────────────────────────────────────────
//    static void saveAllPatients(Storage<Patient>& store) {
//        ofstream f("patients.txt");
//        Patient* all = store.getAll();
//        for (int i = 0; i < store.size(); ++i) {
//            if (!all[i].isActive()) continue;
//            f << all[i].getId() << ","
//                << all[i].getName() << ","
//                << all[i].getAge() << ","
//                << all[i].getGender() << ","
//                << all[i].getContact() << ","
//                << all[i].getPassword() << ","
//                << all[i].getBalance() << "\n";
//        }
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Append a single patient record to patients.txt
//    // ────────────────────────────────────────────────────────────
//    static void appendPatient(const Patient& p) {
//        ofstream f("patients.txt", ios::app);
//        f << p.getId() << ","
//            << p.getName() << ","
//            << p.getAge() << ","
//            << p.getGender() << ","
//            << p.getContact() << ","
//            << p.getPassword() << ","
//            << p.getBalance() << "\n";
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Load doctors
//    //  Format: id,name,specialization,contact,password,fee
//    // ────────────────────────────────────────────────────────────
//    static void loadDoctors(Storage<Doctor>& store) {
//        store.clear();
//        ensureFile("doctors.txt");
//        ifstream f("doctors.txt");
//        string line;
//        while (getline(f, line)) {
//            if (line.empty()) continue;
//            stringstream ss(line);
//            string f0, f1, f2, f3, f4, f5;
//            getline(ss, f0, ',');
//            getline(ss, f1, ',');
//            getline(ss, f2, ',');
//            getline(ss, f3, ',');
//            getline(ss, f4, ',');
//            getline(ss, f5, ',');
//            if (f0.empty()) continue;
//            Doctor d(stoi(f0), f1, f2, f3, f4, stod(f5));
//            store.add(d);
//        }
//        f.close();
//    }
//
//    static void saveAllDoctors(Storage<Doctor>& store) {
//        ofstream f("doctors.txt");
//        Doctor* all = store.getAll();
//        for (int i = 0; i < store.size(); ++i) {
//            if (!all[i].isActive()) continue;
//            f << all[i].getId() << ","
//                << all[i].getName() << ","
//                << all[i].getSpecialization() << ","
//                << all[i].getContact() << ","
//                << all[i].getPassword() << ","
//                << all[i].getFee() << "\n";
//        }
//        f.close();
//    }
//
//    static void appendDoctor(const Doctor& d) {
//        ofstream f("doctors.txt", ios::app);
//        f << d.getId() << ","
//            << d.getName() << ","
//            << d.getSpecialization() << ","
//            << d.getContact() << ","
//            << d.getPassword() << ","
//            << d.getFee() << "\n";
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Load admin  — Format: id,name,password
//    // ────────────────────────────────────────────────────────────
//    static Admin loadAdmin() {
//        seedAdmin();
//        ifstream f("admin.txt");
//        string line;
//        while (getline(f, line)) {
//            if (line.empty()) continue;
//            stringstream ss(line);
//            string f0, f1, f2;
//            getline(ss, f0, ',');
//            getline(ss, f1, ',');
//            getline(ss, f2, ',');
//            if (!f0.empty())
//                return Admin(stoi(f0), f1, f2);
//        }
//        return Admin(1, "Admin", "admin123"); // fallback
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Load appointments
//    //  Format: id,patientId,doctorId,date,timeSlot,status
//    // ────────────────────────────────────────────────────────────
//    static void loadAppointments(Storage<Appointment>& store) {
//        store.clear();
//        ensureFile("appointments.txt");
//        ifstream f("appointments.txt");
//        string line;
//        while (getline(f, line)) {
//            if (line.empty()) continue;
//            stringstream ss(line);
//            string f0, f1, f2, f3, f4, f5;
//            getline(ss, f0, ',');
//            getline(ss, f1, ',');
//            getline(ss, f2, ',');
//            getline(ss, f3, ',');
//            getline(ss, f4, ',');
//            getline(ss, f5, ',');
//            if (f0.empty()) continue;
//            Appointment a(stoi(f0), stoi(f1), stoi(f2), f3, f4, f5);
//            store.add(a);
//        }
//        f.close();
//    }
//
//    static void saveAllAppointments(Storage<Appointment>& store) {
//        ofstream f("appointments.txt");
//        Appointment* all = store.getAll();
//        for (int i = 0; i < store.size(); ++i) {
//            if (!all[i].isActive()) continue;
//            f << all[i].getId() << ","
//                << all[i].getPatientId() << ","
//                << all[i].getDoctorId() << ","
//                << all[i].getDate() << ","
//                << all[i].getTimeSlot() << ","
//                << all[i].getStatus() << "\n";
//        }
//        f.close();
//    }
//
//    static void appendAppointment(const Appointment& a) {
//        ofstream f("appointments.txt", ios::app);
//        f << a.getId() << ","
//            << a.getPatientId() << ","
//            << a.getDoctorId() << ","
//            << a.getDate() << ","
//            << a.getTimeSlot() << ","
//            << a.getStatus() << "\n";
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Load bills
//    //  Format: id,patientId,appointmentId,amount,status,date
//    // ────────────────────────────────────────────────────────────
//    static void loadBills(Storage<Bill>& store) {
//        store.clear();
//        ensureFile("bills.txt");
//        ifstream f("bills.txt");
//        string line;
//        while (getline(f, line)) {
//            if (line.empty()) continue;
//            stringstream ss(line);
//            string f0, f1, f2, f3, f4, f5;
//            getline(ss, f0, ',');
//            getline(ss, f1, ',');
//            getline(ss, f2, ',');
//            getline(ss, f3, ',');
//            getline(ss, f4, ',');
//            getline(ss, f5, ',');
//            if (f0.empty()) continue;
//            Bill b(stoi(f0), stoi(f1), stoi(f2), stod(f3), f4, f5);
//            store.add(b);
//        }
//        f.close();
//    }
//
//    static void saveAllBills(Storage<Bill>& store) {
//        ofstream f("bills.txt");
//        Bill* all = store.getAll();
//        for (int i = 0; i < store.size(); ++i) {
//            if (!all[i].isActive()) continue;
//            f << all[i].getId() << ","
//                << all[i].getPatientId() << ","
//                << all[i].getAppointmentId() << ","
//                << all[i].getAmount() << ","
//                << all[i].getStatus() << ","
//                << all[i].getDate() << "\n";
//        }
//        f.close();
//    }
//
//    static void appendBill(const Bill& b) {
//        ofstream f("bills.txt", ios::app);
//        f << b.getId() << ","
//            << b.getPatientId() << ","
//            << b.getAppointmentId() << ","
//            << b.getAmount() << ","
//            << b.getStatus() << ","
//            << b.getDate() << "\n";
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Load prescriptions
//    //  Format: id,appointmentId,patientId,doctorId,date,medicines,notes
//    // ────────────────────────────────────────────────────────────
//    static void loadPrescriptions(Storage<Prescription>& store) {
//        store.clear();
//        ensureFile("prescriptions.txt");
//        ifstream f("prescriptions.txt");
//        string line;
//        while (getline(f, line)) {
//            if (line.empty()) continue;
//            stringstream ss(line);
//            string f0, f1, f2, f3, f4, f5, f6;
//            getline(ss, f0, ',');
//            getline(ss, f1, ',');
//            getline(ss, f2, ',');
//            getline(ss, f3, ',');
//            getline(ss, f4, ',');
//            getline(ss, f5, ',');
//            getline(ss, f6); // notes may contain commas? spec says max 300 chars
//            if (f0.empty()) continue;
//            Prescription p(stoi(f0), stoi(f1), stoi(f2), stoi(f3), f4, f5, f6);
//            store.add(p);
//        }
//        f.close();
//    }
//
//    static void saveAllPrescriptions(Storage<Prescription>& store) {
//        ofstream f("prescriptions.txt");
//        Prescription* all = store.getAll();
//        for (int i = 0; i < store.size(); ++i) {
//            if (!all[i].isActive()) continue;
//            f << all[i].getId() << ","
//                << all[i].getAppointmentId() << ","
//                << all[i].getPatientId() << ","
//                << all[i].getDoctorId() << ","
//                << all[i].getDate() << ","
//                << all[i].getMedicines() << ","
//                << all[i].getNotes() << "\n";
//        }
//        f.close();
//    }
//
//    static void appendPrescription(const Prescription& p) {
//        ofstream f("prescriptions.txt", ios::app);
//        f << p.getId() << ","
//            << p.getAppointmentId() << ","
//            << p.getPatientId() << ","
//            << p.getDoctorId() << ","
//            << p.getDate() << ","
//            << p.getMedicines() << ","
//            << p.getNotes() << "\n";
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Security log — append a line
//    //  Format: timestamp,role,entered_id,result
//    // ────────────────────────────────────────────────────────────
//    static void logSecurity(const string& role,
//        const string& enteredId,
//        const string& result) {
//        ensureFile("security_log.txt");
//        ofstream f("security_log.txt", ios::app);
//        f << nowTimestamp() << ","
//            << role << ","
//            << enteredId << ","
//            << result << "\n";
//        f.close();
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Read security log as one big string (for Admin view)
//    // ────────────────────────────────────────────────────────────
//    static string readSecurityLog() {
//        ensureFile("security_log.txt");
//        ifstream f("security_log.txt");
//        string result, line;
//        while (getline(f, line))
//            result += line + "\n";
//        return result.empty() ? "No security events logged." : result;
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Discharge patient — copies records to discharged.txt
//    //  then removes from active files
//    // ────────────────────────────────────────────────────────────
//    static void dischargePatient(int patientId,
//        Storage<Patient>& patients,
//        Storage<Appointment>& appointments,
//        Storage<Bill>& bills,
//        Storage<Prescription>& prescriptions) {
//        ensureFile("discharged.txt");
//        ofstream dis("discharged.txt", ios::app);
//
//        // Write patient row
//        Patient* p = patients.findById(patientId);
//        if (p) {
//            dis << "PATIENT," << p->getId() << ","
//                << p->getName() << ","
//                << p->getAge() << ","
//                << p->getGender() << ","
//                << p->getContact() << ","
//                << p->getPassword() << ","
//                << p->getBalance() << "\n";
//            p->deactivate();
//        }
//
//        // Write and deactivate appointments
//        Appointment* apps = appointments.getAll();
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getPatientId() != patientId) continue;
//            dis << "APPOINTMENT," << apps[i].getId() << ","
//                << apps[i].getPatientId() << ","
//                << apps[i].getDoctorId() << ","
//                << apps[i].getDate() << ","
//                << apps[i].getTimeSlot() << ","
//                << apps[i].getStatus() << "\n";
//            apps[i].deactivate();
//        }
//
//        // Write and deactivate bills
//        Bill* blist = bills.getAll();
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getPatientId() != patientId) continue;
//            dis << "BILL," << blist[i].getId() << ","
//                << blist[i].getPatientId() << ","
//                << blist[i].getAppointmentId() << ","
//                << blist[i].getAmount() << ","
//                << blist[i].getStatus() << ","
//                << blist[i].getDate() << "\n";
//            blist[i].deactivate();
//        }
//
//        // Write and deactivate prescriptions
//        Prescription* rxs = prescriptions.getAll();
//        for (int i = 0; i < prescriptions.size(); ++i) {
//            if (!rxs[i].isActive()) continue;
//            if (rxs[i].getPatientId() != patientId) continue;
//            dis << "PRESCRIPTION," << rxs[i].getId() << ","
//                << rxs[i].getAppointmentId() << ","
//                << rxs[i].getPatientId() << ","
//                << rxs[i].getDoctorId() << ","
//                << rxs[i].getDate() << ","
//                << rxs[i].getMedicines() << ","
//                << rxs[i].getNotes() << "\n";
//            rxs[i].deactivate();
//        }
//        dis.close();
//
//        // Persist changes to files
//        saveAllPatients(patients);
//        saveAllAppointments(appointments);
//        saveAllBills(bills);
//        saveAllPrescriptions(prescriptions);
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Helper: compute next available ID across a Storage
//    // ────────────────────────────────────────────────────────────
//    template<typename T>
//    static int nextId(Storage<T>& store) {
//        int maxId = 0;
//        T* all = store.getAll();
//        for (int i = 0; i < store.size(); ++i) {
//            if (all[i].isActive() && all[i].getId() > maxId)
//                maxId = all[i].getId();
//        }
//        return maxId + 1;
//    }
//
//    // ────────────────────────────────────────────────────────────
//    //  Helper: parse a DD-MM-YYYY date into a tm struct
//    //  Returns a time_t for difftime comparisons
//    // ────────────────────────────────────────────────────────────
//    static time_t parseDate(const string& d) {
//        // d = "DD-MM-YYYY"
//        if (d.size() < 10) return 0;
//        int day = stoi(d.substr(0, 2));
//        int mon = stoi(d.substr(3, 2));
//        int year = stoi(d.substr(6, 4));
//        struct tm t = {};
//        t.tm_mday = day;
//        t.tm_mon = mon - 1;
//        t.tm_year = year - 1900;
//        t.tm_isdst = -1;
//        return mktime(&t);
//    }
//
//    // Days between two DD-MM-YYYY date strings (positive = d2 is later)
//    static double daysBetween(const string& d1, const string& d2) {
//        time_t t1 = parseDate(d1);
//        time_t t2 = parseDate(d2);
//        return difftime(t2, t1) / 86400.0;
//    }
//};
//
////  SECTION 6 — Sorting helpers (manual sorts — no library sort)
//
//static string dateToSortKey(const string& d) {
//    // Convert DD-MM-YYYY → YYYYMMDD for lexicographic sorting
//    if (d.size() < 10) return d;
//    return d.substr(6, 4) + d.substr(3, 2) + d.substr(0, 2);
//}
//
//static void sortAppointmentsByDateAsc(Appointment** arr, int n) {
//    // Insertion sort ascending
//    for (int i = 1; i < n; ++i) {
//        Appointment* key = arr[i];
//        int j = i - 1;
//        while (j >= 0 && dateToSortKey(arr[j]->getDate())
//    > dateToSortKey(key->getDate())) {
//            arr[j + 1] = arr[j];
//            --j;
//        }
//        arr[j + 1] = key;
//    }
//}
//
//static void sortAppointmentsByDateDesc(Appointment** arr, int n) {
//    sortAppointmentsByDateAsc(arr, n);
//    // Reverse
//    for (int i = 0, j = n - 1; i < j; ++i, --j)
//        swap(arr[i], arr[j]);
//}
//
//static void sortPrescriptionsByDateDesc(Prescription** arr, int n) {
//    // Insertion sort descending
//    for (int i = 1; i < n; ++i) {
//        Prescription* key = arr[i];
//        int j = i - 1;
//        while (j >= 0 && dateToSortKey(arr[j]->getDate())
//            < dateToSortKey(key->getDate())) {
//            arr[j + 1] = arr[j];
//            --j;
//        }
//        arr[j + 1] = key;
//    }
//}
//
//// Sort appointment pointers by time slot ascending (lexicographic on "HH:MM")
//static void sortAppointmentsBySlotAsc(Appointment** arr, int n) {
//    for (int i = 1; i < n; ++i) {
//        Appointment* key = arr[i];
//        int j = i - 1;
//        while (j >= 0 && arr[j]->getTimeSlot() > key->getTimeSlot()) {
//            arr[j + 1] = arr[j];
//            --j;
//        }
//        arr[j + 1] = key;
//    }
//}
//
////SECTION 7 — Case-insensitive string compare (manual)
//static string toLowerStr(const string& s) {
//    string out = s;
//    for (int i = 0; i < (int)out.size(); ++i)
//        out[i] = (char)tolower((unsigned char)out[i]);
//    return out;
//}
//
//static bool equalIgnoreCase(const string& a, const string& b) {
//    return toLowerStr(a) == toLowerStr(b);
//}
//
////  SECTION 8 — SFML GUI helpers
//
// // Colour palette for MediCore
//const sf::Color BG_DARK = sf::Color(15, 23, 42);        // deep navy
//const sf::Color PANEL_BG = sf::Color(30, 41, 59);        // slate panel
//const sf::Color ACCENT = sf::Color(56, 189, 248);      // sky blue
//const sf::Color ACCENT2 = sf::Color(99, 102, 241);      // indigo
//const sf::Color TEXT_MAIN = sf::Color(226, 232, 240);     // light slate
//const sf::Color TEXT_DIM = sf::Color(100, 116, 139);     // muted slate
//const sf::Color SUCCESS = sf::Color(52, 211, 153);      // emerald
//const sf::Color WARNING = sf::Color(251, 191, 36);      // amber
//const sf::Color DANGER = sf::Color(248, 113, 113);     // red
//const sf::Color INPUT_BG = sf::Color(51, 65, 85);        // darker slate
//const sf::Color BTN_HOVER = sf::Color(71, 85, 105);       // button hover
//
//struct GuiCtx {
//    sf::RenderWindow window;
//    sf::Font         font;
//    bool             fontLoaded;
//
//    // Output buffer — lines shown in the left panel
//    string outputLines[500];
//    int    lineCount;
//
//    // Right panel state
//    string promptText;   // current input prompt
//    string inputBuffer;  // what the user is typing
//    bool   inputActive;  // is the input field active?
//
//    // Menu buttons
//    string menuItems[20];
//    int    menuCount;
//    int    hoveredMenu;  // -1 = none
//
//    // Scroll offset for output panel (in pixels)
//    float scrollY;
//
//    GuiCtx() : lineCount(0), inputActive(false),
//        menuCount(0), hoveredMenu(-1), scrollY(0.0f),
//        fontLoaded(false) {
//    }
//};
//
//// Global gui context (one per program run — not a global variable
//// in the OOP sense; it is passed by reference throughout).
//// We declare it here for access in HospitalSystem below.
//static GuiCtx g_gui;
//
//// ── Draw the entire SFML frame ──────────────────────────────────
//static void drawFrame(GuiCtx& gui) {
//    if (!gui.window.isOpen()) return;
//    gui.window.clear(BG_DARK);
//
//    float W = (float)gui.window.getSize().x;
//    float H = (float)gui.window.getSize().y;
//    float leftW = W * 0.58f;
//    float rightX = leftW + 4.0f;
//    float rightW = W - rightX;
//
//    // ── Left panel background ───────────────────────────────────
//    sf::RectangleShape leftPanel(sf::Vector2f(leftW, H));
//    leftPanel.setFillColor(PANEL_BG);
//    gui.window.draw(leftPanel);
//
//    // ── Header bar ─────────────────────────────────────────────
//    sf::RectangleShape header(sf::Vector2f(W, 48.0f));
//    header.setFillColor(sf::Color(17, 24, 39));
//    gui.window.draw(header);
//
//    if (gui.fontLoaded) {
//        sf::Text title("MediCore Hospital Management System", gui.font, 18);
//        title.setFillColor(ACCENT);
//        title.setPosition(16, 13);
//        gui.window.draw(title);
//    }
//
//    // ── Output lines in left panel ─────────────────────────────
//    if (gui.fontLoaded) {
//        float lineH = 18.0f;
//        float startY = 56.0f - gui.scrollY;
//        // Create a clip rectangle via sf::View
//        sf::View clipView(sf::FloatRect(0, 56, leftW, H - 56));
//        clipView.setViewport(sf::FloatRect(0, 56.0f / H, leftW / W, (H - 56.0f) / H));
//        gui.window.setView(clipView);
//
//        for (int i = 0; i < gui.lineCount; ++i) {
//            float y = startY + i * lineH;
//            if (y + lineH < 56 || y > H) continue; // clip
//            sf::Text line(gui.outputLines[i], gui.font, 13);
//            // Colour coding: lines starting with specific prefixes
//            const string& s = gui.outputLines[i];
//            if (s.size() > 0 && s[0] == '>') line.setFillColor(ACCENT);
//            else if (s.find("Error") != string::npos ||
//                s.find("error") != string::npos ||
//                s.find("Cannot") != string::npos)
//                line.setFillColor(DANGER);
//            else if (s.find("successfully") != string::npos ||
//                s.find("Success") != string::npos)
//                line.setFillColor(SUCCESS);
//            else if (s.find("Warning") != string::npos ||
//                s.find("OVERDUE") != string::npos)
//                line.setFillColor(WARNING);
//            else if (s.size() > 2 && s[0] == '=' && s[1] == '=')
//                line.setFillColor(ACCENT2);
//            else
//                line.setFillColor(TEXT_MAIN);
//            line.setPosition(10, y);
//            gui.window.draw(line);
//        }
//        gui.window.setView(gui.window.getDefaultView());
//    }
//
//    // ── Right panel ─────────────────────────────────────────────
//    sf::RectangleShape rightPanel(sf::Vector2f(rightW, H - 48.0f));
//    rightPanel.setPosition(rightX, 48.0f);
//    rightPanel.setFillColor(sf::Color(17, 24, 39));
//    gui.window.draw(rightPanel);
//
//    if (gui.fontLoaded) {
//        float rx = rightX + 12.0f;
//        float ry = 64.0f;
//
//        // Prompt label
//        if (!gui.promptText.empty()) {
//            sf::Text prompt(gui.promptText, gui.font, 14);
//            prompt.setFillColor(ACCENT);
//            prompt.setPosition(rx, ry);
//            gui.window.draw(prompt);
//            ry += 26.0f;
//
//            // Input box
//            if (gui.inputActive) {
//                sf::RectangleShape box(sf::Vector2f(rightW - 24.0f, 32.0f));
//                box.setPosition(rx - 4.0f, ry - 4.0f);
//                box.setFillColor(INPUT_BG);
//                box.setOutlineColor(ACCENT);
//                box.setOutlineThickness(1.5f);
//                gui.window.draw(box);
//
//                string display = gui.inputBuffer + "|";
//                sf::Text inp(display, gui.font, 14);
//                inp.setFillColor(TEXT_MAIN);
//                inp.setPosition(rx, ry);
//                gui.window.draw(inp);
//                ry += 44.0f;
//            }
//        }
//
//        // Menu buttons
//        sf::Vector2i mouse = sf::Mouse::getPosition(gui.window);
//        for (int i = 0; i < gui.menuCount; ++i) {
//            float bx = rx - 4.0f;
//            float by = ry + i * 40.0f;
//            float bw = rightW - 24.0f;
//            float bh = 34.0f;
//
//            bool hovered = (mouse.x >= bx && mouse.x <= bx + bw &&
//                mouse.y >= by && mouse.y <= by + bh);
//
//            sf::RectangleShape btn(sf::Vector2f(bw, bh));
//            btn.setPosition(bx, by);
//            btn.setFillColor(hovered ? BTN_HOVER : sf::Color(30, 41, 59));
//            btn.setOutlineColor(hovered ? ACCENT : TEXT_DIM);
//            btn.setOutlineThickness(1.0f);
//            gui.window.draw(btn);
//
//            sf::Text label(gui.menuItems[i], gui.font, 13);
//            label.setFillColor(hovered ? ACCENT : TEXT_MAIN);
//            label.setPosition(bx + 10.0f, by + 8.0f);
//            gui.window.draw(label);
//        }
//    }
//
//    // ── Divider ─────────────────────────────────────────────────
//    sf::RectangleShape divider(sf::Vector2f(4.0f, H - 48.0f));
//    divider.setPosition(leftW, 48.0f);
//    divider.setFillColor(sf::Color(56, 189, 248, 60));
//    gui.window.draw(divider);
//
//    gui.window.display();
//}
//
//// Append a line to the output panel
//static void printLine(GuiCtx& gui, const string& line) {
//    if (gui.lineCount < 499) {
//        gui.outputLines[gui.lineCount++] = line;
//        // Auto-scroll down
//        gui.scrollY = max(0.0f, gui.lineCount * 18.0f - (float)(gui.window.getSize().y - 60));
//    }
//}
//
//static void printSep(GuiCtx& gui) {
//    printLine(gui, "══════════════════════════════════════════");
//}
//
//// Block until user types something and presses Enter.
//// Returns the string they typed.
//static string promptInput(GuiCtx& gui, const string& prompt) {
//    gui.promptText = prompt;
//    gui.inputBuffer = "";
//    gui.inputActive = true;
//    gui.menuCount = 0;
//
//    while (gui.window.isOpen()) {
//        sf::Event ev;
//        while (gui.window.pollEvent(ev)) {
//            if (ev.type == sf::Event::Closed)
//                gui.window.close();
//            if (ev.type == sf::Event::KeyPressed &&
//                ev.key.code == sf::Keyboard::Return) {
//                string result = gui.inputBuffer;
//                gui.inputBuffer = "";
//                gui.inputActive = false;
//                gui.promptText = "";
//                // Echo input to output panel
//                printLine(gui, "> " + result);
//                drawFrame(gui);
//                return result;
//            }
//            if (ev.type == sf::Event::KeyPressed &&
//                ev.key.code == sf::Keyboard::BackSpace) {
//                if (!gui.inputBuffer.empty())
//                    gui.inputBuffer.pop_back();
//            }
//            if (ev.type == sf::Event::TextEntered) {
//                char c = (char)ev.text.unicode;
//                if (c >= 32 && c < 127 && gui.inputBuffer.size() < 200)
//                    gui.inputBuffer += c;
//            }
//            if (ev.type == sf::Event::MouseWheelScrolled) {
//                gui.scrollY -= ev.mouseWheelScroll.delta * 20.0f;
//                gui.scrollY = max(0.0f, gui.scrollY);
//            }
//        }
//        drawFrame(gui);
//    }
//    return "";
//}
//
//// Show a numbered menu; returns 0-based index of chosen item
//// or -1 if window closed.
//static int showMenu(GuiCtx& gui, const string items[], int count) {
//    gui.menuCount = count;
//    gui.inputActive = false;
//    gui.promptText = "Choose an option:";
//    for (int i = 0; i < count; ++i)
//        gui.menuItems[i] = items[i];
//
//    while (gui.window.isOpen()) {
//        sf::Event ev;
//        while (gui.window.pollEvent(ev)) {
//            if (ev.type == sf::Event::Closed)
//                gui.window.close();
//            if (ev.type == sf::Event::MouseButtonPressed &&
//                ev.mouseButton.button == sf::Mouse::Left) {
//                float rx = (float)gui.window.getSize().x * 0.58f + 8.0f;
//                float ry = 90.0f;
//                float bw = (float)gui.window.getSize().x * 0.42f - 24.0f;
//                float bh = 34.0f;
//                int mx = ev.mouseButton.x;
//                int my = ev.mouseButton.y;
//                for (int i = 0; i < count; ++i) {
//                    float by = ry + i * 40.0f;
//                    if (mx >= rx - 4 && mx <= rx - 4 + bw &&
//                        my >= by && my <= by + bh) {
//                        gui.menuCount = 0;
//                        gui.promptText = "";
//                        printLine(gui, "> " + items[i]);
//                        drawFrame(gui);
//                        return i;
//                    }
//                }
//            }
//            if (ev.type == sf::Event::MouseWheelScrolled) {
//                gui.scrollY -= ev.mouseWheelScroll.delta * 20.0f;
//                gui.scrollY = max(0.0f, gui.scrollY);
//            }
//            // Also allow keyboard number shortcuts
//            if (ev.type == sf::Event::KeyPressed) {
//                int code = ev.key.code;
//                // sf::Keyboard::Num1 = 26, Num0 = 35 in SFML 2.5
//                // Use TextEntered instead for reliability
//            }
//            if (ev.type == sf::Event::TextEntered) {
//                char c = (char)ev.text.unicode;
//                if (c >= '1' && c <= '9') {
//                    int idx = c - '1';
//                    if (idx < count) {
//                        gui.menuCount = 0;
//                        gui.promptText = "";
//                        printLine(gui, "> " + items[idx]);
//                        drawFrame(gui);
//                        return idx;
//                    }
//                }
//                if (c == '0' && count >= 10) {
//                    gui.menuCount = 0;
//                    gui.promptText = "";
//                    printLine(gui, "> " + items[9]);
//                    drawFrame(gui);
//                    return 9;
//                }
//            }
//        }
//        drawFrame(gui);
//    }
//    return -1;
//}
//
////  SECTION 9 — HospitalSystem  (main controller)
//class HospitalSystem {
//private:
//    // ── Persistent data stores ───────────────────────────────────
//    Storage<Patient>      patients;
//    Storage<Doctor>       doctors;
//    Storage<Appointment>  appointments;
//    Storage<Bill>         bills;
//    Storage<Prescription> prescriptions;
//    Admin                 admin;
//
//    GuiCtx& gui; // reference to the SFML gui context
//
//    // ── Convenience: print to both console and GUI ───────────────
//    void print(const string& s) {
//        printLine(gui, s);
//        cout << s << "\n";
//        drawFrame(gui);
//    }
//
//    void printSep() {
//        ::printSep(gui);
//        cout << string(44, '=') << "\n";
//        drawFrame(gui);
//    }
//
//    // ── Reload all stores from files ─────────────────────────────
//    void reloadAll() {
//        FileHandler::loadPatients(patients);
//        FileHandler::loadDoctors(doctors);
//        FileHandler::loadAppointments(appointments);
//        FileHandler::loadBills(bills);
//        FileHandler::loadPrescriptions(prescriptions);
//        admin = FileHandler::loadAdmin();
//    }
//
//    // ─────────────────────────────────────────────────────────────
//    //  LOGIN subsystem
//    //  Returns true if credentials match; increments failCount.
//    //  After 3 failures, logs to security_log.txt.
//    // ─────────────────────────────────────────────────────────────
//    bool loginPatient(int& outId) {
//        int fails = 0;
//        while (fails < 3 && gui.window.isOpen()) {
//            string sid = promptInput(gui, "Enter Patient ID:");
//            string pwd = promptInput(gui, "Enter Password:");
//            // Find patient
//            bool found = false;
//            Patient* all = patients.getAll();
//            for (int i = 0; i < patients.size(); ++i) {
//                if (!all[i].isActive()) continue;
//                if (all[i].getId() == stoi(sid) &&
//                    all[i].getPassword() == pwd) {
//                    outId = all[i].getId();
//                    FileHandler::logSecurity("Patient", sid, "SUCCESS");
//                    return true;
//                }
//            }
//            ++fails;
//            print("Invalid credentials. Attempt " +
//                to_string(fails) + " of 3.");
//            FileHandler::logSecurity("Patient", sid, "FAILED");
//        }
//        print("Account locked. Contact admin.");
//        FileHandler::logSecurity("Patient", "-", "LOCKED");
//        return false;
//    }
//
//    bool loginDoctor(int& outId) {
//        int fails = 0;
//        while (fails < 3 && gui.window.isOpen()) {
//            string sid = promptInput(gui, "Enter Doctor ID:");
//            string pwd = promptInput(gui, "Enter Password:");
//            Doctor* all = doctors.getAll();
//            for (int i = 0; i < doctors.size(); ++i) {
//                if (!all[i].isActive()) continue;
//                if (all[i].getId() == stoi(sid) &&
//                    all[i].getPassword() == pwd) {
//                    outId = all[i].getId();
//                    FileHandler::logSecurity("Doctor", sid, "SUCCESS");
//                    return true;
//                }
//            }
//            ++fails;
//            print("Invalid credentials. Attempt " +
//                to_string(fails) + " of 3.");
//            FileHandler::logSecurity("Doctor", sid, "FAILED");
//        }
//        print("Account locked. Contact admin.");
//        FileHandler::logSecurity("Doctor", "-", "LOCKED");
//        return false;
//    }
//
//    bool loginAdmin() {
//        int fails = 0;
//        while (fails < 3 && gui.window.isOpen()) {
//            string sid = promptInput(gui, "Enter Admin ID:");
//            string pwd = promptInput(gui, "Enter Password:");
//            if (admin.getId() == stoi(sid) &&
//                admin.getPassword() == pwd) {
//                FileHandler::logSecurity("Admin", sid, "SUCCESS");
//                return true;
//            }
//            ++fails;
//            print("Invalid credentials. Attempt " +
//                to_string(fails) + " of 3.");
//            FileHandler::logSecurity("Admin", sid, "FAILED");
//        }
//        print("Account locked. Contact admin.");
//        FileHandler::logSecurity("Admin", "-", "LOCKED");
//        return false;
//    }
//
//    // ─────────────────────────────────────────────────────────────
//    //  PATIENT MENU functions
//    // ─────────────────────────────────────────────────────────────
//
//    // 1. Book Appointment
//    void patientBookAppointment(int patientId) {
//        printSep();
//        print("== Book Appointment ==");
//
//        // Find patient
//        Patient* pt = patients.findById(patientId);
//        if (!pt) { print("Patient not found."); return; }
//
//        // Get specialization
//        string spec = promptInput(gui, "Enter specialization (e.g. Cardiology):");
//
//        // Display matching doctors (case-insensitive)
//        bool found = false;
//        Doctor* docs = doctors.getAll();
//        print("── Doctors with specialization: " + spec + " ──");
//        for (int i = 0; i < doctors.size(); ++i) {
//            if (!docs[i].isActive()) continue;
//            if (equalIgnoreCase(docs[i].getSpecialization(), spec)) {
//                print("  ID: " + to_string(docs[i].getId()) +
//                    " | " + docs[i].getName() +
//                    " | Fee: PKR " + to_string((int)docs[i].getFee()));
//                found = true;
//            }
//        }
//        if (!found) {
//            print("No doctors available for that specialization.");
//            return;
//        }
//
//        // Choose doctor
//        string sdid = promptInput(gui, "Enter Doctor ID:");
//        int did = 0;
//        try { did = stoi(sdid); }
//        catch (...) { print("Doctor not found."); return; }
//        Doctor* doc = doctors.findById(did);
//        if (!doc) { print("Doctor not found."); return; }
//        if (!equalIgnoreCase(doc->getSpecialization(), spec)) {
//            print("Doctor not found."); return;
//        }
//
//        // Get date with up to 3 attempts
//        string date;
//        int dateAttempts = 0;
//        while (dateAttempts < 3) {
//            date = promptInput(gui, "Enter date (DD-MM-YYYY):");
//            if (Validator::isValidDate(date)) break;
//            try {
//                throw InvalidInputException("Invalid date. Use format DD-MM-YYYY.");
//            }
//            catch (InvalidInputException& e) {
//                print(e.what());
//            }
//            ++dateAttempts;
//            if (dateAttempts == 3) { print("Too many invalid attempts."); return; }
//        }
//
//        // Show available slots
//        const string allSlots[8] = {
//            "09:00","10:00","11:00","12:00",
//            "13:00","14:00","15:00","16:00"
//        };
//        print("── Available slots on " + date + " for Dr. " + doc->getName() + " ──");
//        Appointment* apps = appointments.getAll();
//        bool slotTaken[8] = {};
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getDoctorId() != did) continue;
//            if (apps[i].getDate() != date) continue;
//            if (apps[i].getStatus() == "cancelled") continue;
//            for (int s = 0; s < 8; ++s)
//                if (apps[i].getTimeSlot() == allSlots[s])
//                    slotTaken[s] = true;
//        }
//        for (int s = 0; s < 8; ++s) {
//            if (!slotTaken[s]) print("  " + allSlots[s]);
//        }
//
//        // Choose slot (loop on SlotUnavailableException)
//        string slot;
//        while (true) {
//            slot = promptInput(gui, "Enter time slot (e.g. 09:00):");
//            if (!Validator::isValidSlot(slot)) {
//                print("Invalid slot. Choose from: 09:00 10:00 11:00 12:00 13:00 14:00 15:00 16:00");
//                continue;
//            }
//            // Check if taken
//            bool taken = false;
//            for (int i = 0; i < appointments.size(); ++i) {
//                if (!apps[i].isActive()) continue;
//                if (apps[i].getDoctorId() == did &&
//                    apps[i].getDate() == date &&
//                    apps[i].getTimeSlot() == slot &&
//                    apps[i].getStatus() != "cancelled") {
//                    taken = true; break;
//                }
//            }
//            if (taken) {
//                try { throw SlotUnavailableException(); }
//                catch (SlotUnavailableException& e) {
//                    print(e.what());
//                    // Re-display available slots
//                    print("── Available slots ──");
//                    for (int s = 0; s < 8; ++s)
//                        if (!slotTaken[s]) print("  " + allSlots[s]);
//                }
//                continue;
//            }
//            break;
//        }
//
//        // Check balance
//        double fee = doc->getFee();
//        if (pt->getBalance() < fee) {
//            try { throw InsufficientFundsException(); }
//            catch (InsufficientFundsException& e) { print(e.what()); return; }
//        }
//
//        // Deduct fee, create appointment & bill
//        *pt -= fee;
//        FileHandler::saveAllPatients(patients);
//
//        int newApptId = FileHandler::nextId(appointments);
//        Appointment newAppt(newApptId, patientId, did, date, slot, "pending");
//        appointments.add(newAppt);
//        FileHandler::appendAppointment(newAppt);
//
//        int newBillId = FileHandler::nextId(bills);
//        string today = FileHandler::todayString();
//        Bill newBill(newBillId, patientId, newApptId, fee, "unpaid", today);
//        bills.add(newBill);
//        FileHandler::appendBill(newBill);
//
//        print("Appointment booked successfully. Appointment ID: " +
//            to_string(newApptId));
//        print("Bill generated. Bill ID: " + to_string(newBillId));
//        print("New balance: PKR " + to_string((int)pt->getBalance()));
//    }
//
//    // 2. Cancel Appointment
//    void patientCancelAppointment(int patientId) {
//        printSep();
//        print("== Cancel Appointment ==");
//
//        Patient* pt = patients.findById(patientId);
//        if (!pt) return;
//
//        // Display pending appointments
//        Appointment* apps = appointments.getAll();
//        bool hasPending = false;
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getPatientId() != patientId) continue;
//            if (apps[i].getStatus() != "pending") continue;
//            // Find doctor name
//            Doctor* doc = doctors.findById(apps[i].getDoctorId());
//            string dname = doc ? doc->getName() : "Unknown";
//            print("  ID: " + to_string(apps[i].getId()) +
//                " | Dr. " + dname +
//                " | " + apps[i].getDate() +
//                " | " + apps[i].getTimeSlot());
//            hasPending = true;
//        }
//        if (!hasPending) {
//            print("You have no pending appointments."); return;
//        }
//
//        string said = promptInput(gui, "Enter Appointment ID to cancel:");
//        int aid = 0;
//        try { aid = stoi(said); }
//        catch (...) { print("Invalid appointment ID."); return; }
//
//        // Validate ownership and pending status
//        Appointment* appt = appointments.findById(aid);
//        if (!appt || appt->getPatientId() != patientId ||
//            appt->getStatus() != "pending") {
//            print("Invalid appointment ID."); return;
//        }
//
//        // Find doctor fee for refund
//        Doctor* doc = doctors.findById(appt->getDoctorId());
//        double fee = doc ? doc->getFee() : 0.0;
//
//        // Update appointment to cancelled
//        appt->setStatus("cancelled");
//        FileHandler::saveAllAppointments(appointments);
//
//        // Refund
//        *pt += fee;
//        FileHandler::saveAllPatients(patients);
//
//        // Cancel corresponding bill
//        Bill* blist = bills.getAll();
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getAppointmentId() == aid) {
//                blist[i].setStatus("cancelled");
//                break;
//            }
//        }
//        FileHandler::saveAllBills(bills);
//
//        print("Appointment cancelled. PKR " +
//            to_string((int)fee) + " refunded to your balance.");
//        print("New balance: PKR " + to_string((int)pt->getBalance()));
//    }
//
//    // 3. View My Appointments (sorted by date ascending)
//    void patientViewAppointments(int patientId) {
//        printSep();
//        print("== My Appointments ==");
//
//        // Collect pointers
//        Appointment* apps = appointments.getAll();
//        Appointment* filtered[200];
//        int cnt = 0;
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getPatientId() == patientId)
//                filtered[cnt++] = &apps[i];
//        }
//        if (cnt == 0) { print("No appointments found."); return; }
//
//        sortAppointmentsByDateAsc(filtered, cnt);
//
//        print("ID   | Doctor                 | Spec          | Date       | Slot  | Status");
//        print(string(85, '-'));
//        for (int i = 0; i < cnt; ++i) {
//            Doctor* doc = doctors.findById(filtered[i]->getDoctorId());
//            string dname = doc ? doc->getName() : "Unknown";
//            string spec = doc ? doc->getSpecialization() : "N/A";
//            print(to_string(filtered[i]->getId()) + "    | " +
//                dname + string(max(0, 22 - (int)dname.size()), ' ') + "| " +
//                spec + string(max(0, 13 - (int)spec.size()), ' ') + "| " +
//                filtered[i]->getDate() + " | " +
//                filtered[i]->getTimeSlot() + " | " +
//                filtered[i]->getStatus());
//        }
//    }
//
//    // 4. View My Medical Records (sorted by date descending)
//    void patientViewMedicalRecords(int patientId) {
//        printSep();
//        print("== My Medical Records ==");
//
//        Prescription* rxs = prescriptions.getAll();
//        Prescription* filtered[200];
//        int cnt = 0;
//        for (int i = 0; i < prescriptions.size(); ++i) {
//            if (!rxs[i].isActive()) continue;
//            if (rxs[i].getPatientId() == patientId)
//                filtered[cnt++] = &rxs[i];
//        }
//        if (cnt == 0) { print("No medical records found."); return; }
//
//        sortPrescriptionsByDateDesc(filtered, cnt);
//
//        for (int i = 0; i < cnt; ++i) {
//            Doctor* doc = doctors.findById(filtered[i]->getDoctorId());
//            string dname = doc ? doc->getName() : "Unknown";
//            print("── Date: " + filtered[i]->getDate() +
//                " | Dr. " + dname + " ─-");
//            print("   Medicines: " + filtered[i]->getMedicines());
//            print("   Notes:     " + filtered[i]->getNotes());
//        }
//    }
//
//    // 5. View My Bills
//    void patientViewBills(int patientId) {
//        printSep();
//        print("== My Bills ==");
//
//        Bill* blist = bills.getAll();
//        double totalUnpaid = 0.0;
//        bool hasBills = false;
//
//        print("Bill ID | Appt ID | Amount (PKR) | Status    | Date");
//        print(string(60, '-'));
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getPatientId() != patientId) continue;
//            hasBills = true;
//            print(to_string(blist[i].getId()) + "       | " +
//                to_string(blist[i].getAppointmentId()) + "       | " +
//                to_string((int)blist[i].getAmount()) + "           | " +
//                blist[i].getStatus() + "  | " +
//                blist[i].getDate());
//            if (blist[i].getStatus() == "unpaid")
//                totalUnpaid += blist[i].getAmount();
//        }
//        if (!hasBills) { print("No bills found."); return; }
//        print("Total outstanding (unpaid): PKR " + to_string((int)totalUnpaid));
//    }
//
//    // 6. Pay Bill
//    void patientPayBill(int patientId) {
//        printSep();
//        print("== Pay Bill ==");
//
//        Patient* pt = patients.findById(patientId);
//        if (!pt) return;
//
//        Bill* blist = bills.getAll();
//        bool hasUnpaid = false;
//        print("Unpaid Bills:");
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getPatientId() != patientId) continue;
//            if (blist[i].getStatus() != "unpaid") continue;
//            print("  Bill ID: " + to_string(blist[i].getId()) +
//                " | Appt: " + to_string(blist[i].getAppointmentId()) +
//                " | Amount: PKR " + to_string((int)blist[i].getAmount()) +
//                " | " + blist[i].getDate());
//            hasUnpaid = true;
//        }
//        if (!hasUnpaid) { print("No unpaid bills."); return; }
//
//        string sbid = promptInput(gui, "Enter Bill ID to pay:");
//        int bid = 0;
//        try { bid = stoi(sbid); }
//        catch (...) { print("Invalid Bill ID."); return; }
//
//        Bill* bill = bills.findById(bid);
//        if (!bill || bill->getPatientId() != patientId ||
//            bill->getStatus() != "unpaid") {
//            print("Invalid Bill ID."); return;
//        }
//
//        if (pt->getBalance() < bill->getAmount()) {
//            try { throw InsufficientFundsException(); }
//            catch (InsufficientFundsException& e) { print(e.what()); return; }
//        }
//
//        *pt -= bill->getAmount();
//        bill->setStatus("paid");
//        FileHandler::saveAllPatients(patients);
//        FileHandler::saveAllBills(bills);
//
//        print("Bill paid successfully. Remaining balance: PKR " +
//            to_string((int)pt->getBalance()));
//    }
//
//    // 7. Top Up Balance
//    void patientTopUp(int patientId) {
//        printSep();
//        print("== Top Up Balance ==");
//
//        Patient* pt = patients.findById(patientId);
//        if (!pt) return;
//
//        int attempts = 0;
//        while (attempts < 3 && gui.window.isOpen()) {
//            string sAmt = promptInput(gui, "Enter amount to add (PKR):");
//            if (!Validator::isValidTopUp(sAmt)) {
//                try { throw InvalidInputException("Amount must be a positive number."); }
//                catch (InvalidInputException& e) { print(e.what()); }
//                ++attempts;
//                continue;
//            }
//            double amt = stod(sAmt);
//            *pt += amt;
//            FileHandler::saveAllPatients(patients);
//            print("Balance updated. New balance: PKR " +
//                to_string((int)pt->getBalance()));
//            return;
//        }
//        print("Too many invalid attempts. Returning to menu.");
//    }
//
//    // ── Patient main menu loop ───────────────────────────────────
//    void runPatientMenu(int patientId) {
//        Patient* pt = patients.findById(patientId);
//        if (!pt) return;
//
//        while (gui.window.isOpen()) {
//            printSep();
//            print("Welcome, " + pt->getName() +
//                " | Balance: PKR " + to_string((int)pt->getBalance()));
//            printSep();
//
//            const string items[8] = {
//                "1. Book Appointment",
//                "2. Cancel Appointment",
//                "3. View My Appointments",
//                "4. View My Medical Records",
//                "5. View My Bills",
//                "6. Pay Bill",
//                "7. Top Up Balance",
//                "8. Logout"
//            };
//            int choice = showMenu(gui, items, 8);
//            // Refresh pointer after any file reload
//            reloadAll();
//            pt = patients.findById(patientId);
//            if (!pt) return;
//
//            if (choice == 0) patientBookAppointment(patientId);
//            else if (choice == 1) patientCancelAppointment(patientId);
//            else if (choice == 2) patientViewAppointments(patientId);
//            else if (choice == 3) patientViewMedicalRecords(patientId);
//            else if (choice == 4) patientViewBills(patientId);
//            else if (choice == 5) patientPayBill(patientId);
//            else if (choice == 6) patientTopUp(patientId);
//            else break; // Logout
//        }
//    }
//
//    // ─────────────────────────────────────────────────────────────
//    //  DOCTOR MENU functions
//    // ─────────────────────────────────────────────────────────────
//
//    // 1. View Today's Appointments
//    void doctorViewToday(int doctorId) {
//        printSep();
//        print("== Today's Appointments ==");
//        string today = FileHandler::todayString();
//
//        Appointment* apps = appointments.getAll();
//        Appointment* filtered[200];
//        int cnt = 0;
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getDoctorId() != doctorId) continue;
//            if (apps[i].getDate() != today)    continue;
//            filtered[cnt++] = &apps[i];
//        }
//        if (cnt == 0) { print("No appointments scheduled for today."); return; }
//
//        sortAppointmentsBySlotAsc(filtered, cnt);
//
//        print("Appt ID | Patient         | Slot  | Status");
//        print(string(55, '-'));
//        for (int i = 0; i < cnt; ++i) {
//            Patient* pt = patients.findById(filtered[i]->getPatientId());
//            string pname = pt ? pt->getName() : "Unknown";
//            print(to_string(filtered[i]->getId()) + "       | " +
//                pname + string(max(0, 15 - (int)pname.size()), ' ') + "| " +
//                filtered[i]->getTimeSlot() + " | " +
//                filtered[i]->getStatus());
//        }
//    }
//
//    // 2. Mark Appointment Complete
//    void doctorMarkComplete(int doctorId) {
//        printSep();
//        print("== Mark Appointment Complete ==");
//        string today = FileHandler::todayString();
//
//        // Show today's pending
//        Appointment* apps = appointments.getAll();
//        bool any = false;
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getDoctorId() != doctorId) continue;
//            if (apps[i].getDate() != today)    continue;
//            if (apps[i].getStatus() != "pending") continue;
//            Patient* pt = patients.findById(apps[i].getPatientId());
//            print("  ID: " + to_string(apps[i].getId()) +
//                " | " + (pt ? pt->getName() : "Unknown") +
//                " | " + apps[i].getTimeSlot());
//            any = true;
//        }
//        if (!any) { print("No pending appointments today."); return; }
//
//        string said = promptInput(gui, "Enter Appointment ID:");
//        int aid = 0;
//        try { aid = stoi(said); }
//        catch (...) { print("Invalid ID."); return; }
//        Appointment* appt = appointments.findById(aid);
//        if (!appt || appt->getDoctorId() != doctorId ||
//            appt->getStatus() != "pending" || appt->getDate() != today) {
//            print("Invalid appointment ID."); return;
//        }
//        appt->setStatus("completed");
//        FileHandler::saveAllAppointments(appointments);
//        print("Appointment marked as completed.");
//    }
//
//    // 3. Mark Appointment No-Show
//    void doctorMarkNoShow(int doctorId) {
//        printSep();
//        print("== Mark Appointment No-Show ==");
//        string today = FileHandler::todayString();
//
//        Appointment* apps = appointments.getAll();
//        bool any = false;
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getDoctorId() != doctorId) continue;
//            if (apps[i].getDate() != today)    continue;
//            if (apps[i].getStatus() != "pending") continue;
//            Patient* pt = patients.findById(apps[i].getPatientId());
//            print("  ID: " + to_string(apps[i].getId()) +
//                " | " + (pt ? pt->getName() : "Unknown") +
//                " | " + apps[i].getTimeSlot());
//            any = true;
//        }
//        if (!any) { print("No pending appointments today."); return; }
//
//        string said = promptInput(gui, "Enter Appointment ID:");
//        int aid = 0;
//        try { aid = stoi(said); }
//        catch (...) { print("Invalid ID."); return; }
//        Appointment* appt = appointments.findById(aid);
//        if (!appt || appt->getDoctorId() != doctorId ||
//            appt->getStatus() != "pending" || appt->getDate() != today) {
//            print("Invalid appointment ID."); return;
//        }
//        appt->setStatus("no-show");
//        FileHandler::saveAllAppointments(appointments);
//
//        // Cancel bill (no refund)
//        Bill* blist = bills.getAll();
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getAppointmentId() == aid) {
//                blist[i].setStatus("cancelled");
//                break;
//            }
//        }
//        FileHandler::saveAllBills(bills);
//        print("Appointment marked as no-show.");
//    }
//
//    // 4. Write Prescription
//    void doctorWritePrescription(int doctorId) {
//        printSep();
//        print("== Write Prescription ==");
//
//        string said = promptInput(gui, "Enter Appointment ID:");
//        int aid = 0;
//        try { aid = stoi(said); }
//        catch (...) { print("Invalid ID."); return; }
//        Appointment* appt = appointments.findById(aid);
//        if (!appt || appt->getDoctorId() != doctorId ||
//            appt->getStatus() != "completed") {
//            print("Invalid: appointment must belong to you and be completed."); return;
//        }
//
//        // Check if prescription already exists
//        Prescription* rxs = prescriptions.getAll();
//        for (int i = 0; i < prescriptions.size(); ++i) {
//            if (!rxs[i].isActive()) continue;
//            if (rxs[i].getAppointmentId() == aid) {
//                print("Prescription already written for this appointment."); return;
//            }
//        }
//
//        string medicines = promptInput(gui, "Enter medicines (e.g. Paracetamol 500mg;Amox 250mg):");
//        // Truncate to 499 chars
//        if (medicines.size() > 499) medicines = medicines.substr(0, 499);
//
//        string notes = promptInput(gui, "Enter notes (max 300 chars):");
//        if (notes.size() > 300) notes = notes.substr(0, 300);
//
//        int newId = FileHandler::nextId(prescriptions);
//        string today = FileHandler::todayString();
//        Prescription rx(newId, aid, appt->getPatientId(), doctorId,
//            today, medicines, notes);
//        prescriptions.add(rx);
//        FileHandler::appendPrescription(rx);
//        print("Prescription saved. ID: " + to_string(newId));
//    }
//
//    // 5. View Patient Medical History (must be own patient)
//    void doctorViewPatientHistory(int doctorId) {
//        printSep();
//        print("== Patient Medical History ==");
//
//        string spid = promptInput(gui, "Enter Patient ID:");
//        int pid = 0;
//        try { pid = stoi(spid); }
//        catch (...) { print("Access denied."); return; }
//
//        // Check patient exists
//        Patient* pt = patients.findById(pid);
//        if (!pt) { print("Access denied. You can only view records of your own patients."); return; }
//
//        // Check has at least one completed appointment with this doctor
//        bool hasAppt = false;
//        Appointment* apps = appointments.getAll();
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getPatientId() != pid) continue;
//            if (apps[i].getDoctorId() != doctorId) continue;
//            if (apps[i].getStatus() == "completed") { hasAppt = true; break; }
//        }
//        if (!hasAppt) {
//            print("Access denied. You can only view records of your own patients."); return;
//        }
//
//        // Show prescriptions written by this doctor for this patient
//        Prescription* rxs = prescriptions.getAll();
//        Prescription* filtered[200];
//        int cnt = 0;
//        for (int i = 0; i < prescriptions.size(); ++i) {
//            if (!rxs[i].isActive()) continue;
//            if (rxs[i].getPatientId() != pid)      continue;
//            if (rxs[i].getDoctorId() != doctorId) continue;
//            filtered[cnt++] = &rxs[i];
//        }
//        if (cnt == 0) { print("No prescriptions found for this patient."); return; }
//
//        sortPrescriptionsByDateDesc(filtered, cnt);
//        print("Records for patient: " + pt->getName());
//        for (int i = 0; i < cnt; ++i) {
//            print("── " + filtered[i]->getDate() + " ──");
//            print("   Medicines: " + filtered[i]->getMedicines());
//            print("   Notes:     " + filtered[i]->getNotes());
//        }
//    }
//
//    // ── Doctor main menu loop ────────────────────────────────────
//    void runDoctorMenu(int doctorId) {
//        Doctor* doc = doctors.findById(doctorId);
//        if (!doc) return;
//
//        while (gui.window.isOpen()) {
//            printSep();
//            print("Welcome, Dr. " + doc->getName() +
//                " | Specialization: " + doc->getSpecialization());
//            printSep();
//
//            const string items[6] = {
//                "1. View Today's Appointments",
//                "2. Mark Appointment Complete",
//                "3. Mark Appointment No-Show",
//                "4. Write Prescription",
//                "5. View Patient Medical History",
//                "6. Logout"
//            };
//            int choice = showMenu(gui, items, 6);
//            reloadAll();
//            doc = doctors.findById(doctorId);
//            if (!doc) return;
//
//            if (choice == 0) doctorViewToday(doctorId);
//            else if (choice == 1) doctorMarkComplete(doctorId);
//            else if (choice == 2) doctorMarkNoShow(doctorId);
//            else if (choice == 3) doctorWritePrescription(doctorId);
//            else if (choice == 4) doctorViewPatientHistory(doctorId);
//            else break; // Logout
//        }
//    }
//    
//    //  ADMIN MENU functions
//
//    // 1. Add Doctor
//    void adminAddDoctor() {
//        printSep();
//        print("== Add Doctor ==");
//
//        string name = promptInput(gui, "Doctor name (max 50 chars):");
//        if (!Validator::isValidName(name, 50)) { print("Invalid name."); return; }
//
//        string spec = promptInput(gui, "Specialization (max 50 chars):");
//        if (!Validator::isValidName(spec, 50)) { print("Invalid specialization."); return; }
//
//        string contact = promptInput(gui, "Contact (11 digits):");
//        if (!Validator::isValidContact(contact)) { print("Invalid contact number."); return; }
//
//        string password = promptInput(gui, "Password (min 6 chars):");
//        if (!Validator::isValidPassword(password)) { print("Invalid password."); return; }
//
//        string sfee = promptInput(gui, "Consultation fee (PKR):");
//        if (!Validator::isPositiveFloat(sfee)) { print("Invalid fee."); return; }
//        double fee = stod(sfee);
//
//        int newId = FileHandler::nextId(doctors);
//        Doctor d(newId, name, spec, contact, password, fee);
//        doctors.add(d);
//        FileHandler::appendDoctor(d);
//        print("Doctor added successfully. ID: " + to_string(newId));
//    }
//
//    // 2. Remove Doctor
//    void adminRemoveDoctor() {
//        printSep();
//        print("== Remove Doctor ==");
//
//        Doctor* docs = doctors.getAll();
//        print("ID  | Name                   | Specialization   | Fee");
//        print(string(60, '-'));
//        for (int i = 0; i < doctors.size(); ++i) {
//            if (!docs[i].isActive()) continue;
//            print(to_string(docs[i].getId()) + "   | " +
//                docs[i].getName() + string(max(0, 22 - (int)docs[i].getName().size()), ' ') + "| " +
//                docs[i].getSpecialization() + string(max(0, 16 - (int)docs[i].getSpecialization().size()), ' ') + "| " +
//                to_string((int)docs[i].getFee()));
//        }
//
//        string sdid = promptInput(gui, "Enter Doctor ID to remove:");
//        int did = 0;
//        try { did = stoi(sdid); }
//        catch (...) { print("Invalid ID."); return; }
//
//        // Check for pending appointments
//        Appointment* apps = appointments.getAll();
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getDoctorId() == did &&
//                apps[i].getStatus() == "pending") {
//                print("Cannot remove doctor with pending appointments. "
//                    "Cancel or reassign them first.");
//                return;
//            }
//        }
//
//        if (!doctors.removeById(did)) { print("Doctor not found."); return; }
//        FileHandler::saveAllDoctors(doctors);
//        print("Doctor removed.");
//    }
//
//    // 3. View All Patients
//    void adminViewAllPatients() {
//        printSep();
//        print("== All Patients ==");
//        Patient* all = patients.getAll();
//        Bill* blist = bills.getAll();
//        print("ID  | Name            | Age | G | Contact     | Balance  | Unpaid Bills");
//        print(string(75, '-'));
//        for (int i = 0; i < patients.size(); ++i) {
//            if (!all[i].isActive()) continue;
//            // Count unpaid bills
//            int unpaid = 0;
//            for (int b = 0; b < bills.size(); ++b) {
//                if (!blist[b].isActive()) continue;
//                if (blist[b].getPatientId() == all[i].getId() &&
//                    blist[b].getStatus() == "unpaid") ++unpaid;
//            }
//            print(to_string(all[i].getId()) + "   | " +
//                all[i].getName() + string(max(0, 15 - (int)all[i].getName().size()), ' ') + "| " +
//                to_string(all[i].getAge()) + "  | " +
//                all[i].getGender() + " | " +
//                all[i].getContact() + " | " +
//                to_string((int)all[i].getBalance()) + "    | " +
//                to_string(unpaid));
//        }
//    }
//
//    // 4. View All Doctors
//    void adminViewAllDoctors() {
//        printSep();
//        print("== All Doctors ==");
//        Doctor* all = doctors.getAll();
//        print("ID  | Name                   | Specialization     | Contact     | Fee");
//        print(string(75, '-'));
//        for (int i = 0; i < doctors.size(); ++i) {
//            if (!all[i].isActive()) continue;
//            print(to_string(all[i].getId()) + "   | " +
//                all[i].getName() + string(max(0, 22 - (int)all[i].getName().size()), ' ') + "| " +
//                all[i].getSpecialization() + string(max(0, 18 - (int)all[i].getSpecialization().size()), ' ') + "| " +
//                all[i].getContact() + " | PKR " +
//                to_string((int)all[i].getFee()));
//        }
//    }
//
//    // 5. View All Appointments (sorted date descending)
//    void adminViewAllAppointments() {
//        printSep();
//        print("== All Appointments ==");
//        Appointment* apps = appointments.getAll();
//        Appointment* sorted[200];
//        int cnt = 0;
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            sorted[cnt++] = &apps[i];
//        }
//        sortAppointmentsByDateDesc(sorted, cnt);
//        print("ID  | Patient         | Doctor          | Date       | Slot  | Status");
//        print(string(75, '-'));
//        for (int i = 0; i < cnt; ++i) {
//            Patient* pt = patients.findById(sorted[i]->getPatientId());
//            Doctor* doc = doctors.findById(sorted[i]->getDoctorId());
//            string pname = pt ? pt->getName() : "Unknown";
//            string dname = doc ? doc->getName() : "Unknown";
//            print(to_string(sorted[i]->getId()) + "   | " +
//                pname + string(max(0, 15 - (int)pname.size()), ' ') + "| " +
//                dname + string(max(0, 15 - (int)dname.size()), ' ') + "| " +
//                sorted[i]->getDate() + " | " +
//                sorted[i]->getTimeSlot() + " | " +
//                sorted[i]->getStatus());
//        }
//    }
//
//    // 6. View Unpaid Bills (flag overdue > 7 days)
//    void adminViewUnpaidBills() {
//        printSep();
//        print("== Unpaid Bills ==");
//        string today = FileHandler::todayString();
//        Bill* blist = bills.getAll();
//        bool any = false;
//        print("Bill ID | Patient         | Amount (PKR) | Date");
//        print(string(60, '-'));
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getStatus() != "unpaid") continue;
//            Patient* pt = patients.findById(blist[i].getPatientId());
//            string pname = pt ? pt->getName() : "Unknown";
//            double daysDiff = FileHandler::daysBetween(blist[i].getDate(), today);
//            string dateStr = blist[i].getDate();
//            if (daysDiff > 7) dateStr += " [OVERDUE]";
//            print(to_string(blist[i].getId()) + "       | " +
//                pname + string(max(0, 15 - (int)pname.size()), ' ') + "| PKR " +
//                to_string((int)blist[i].getAmount()) + "       | " +
//                dateStr);
//            any = true;
//        }
//        if (!any) print("No unpaid bills.");
//    }
//
//    // 7. Discharge Patient
//    void adminDischargePatient() {
//        printSep();
//        print("== Discharge Patient ==");
//        string spid = promptInput(gui, "Enter Patient ID:");
//        int pid = 0;
//        try { pid = stoi(spid); }
//        catch (...) { print("Invalid ID."); return; }
//
//        Patient* pt = patients.findById(pid);
//        if (!pt) { print("Patient not found."); return; }
//
//        // Check unpaid bills
//        Bill* blist = bills.getAll();
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getPatientId() == pid &&
//                blist[i].getStatus() == "unpaid") {
//                print("Cannot discharge patient with unpaid bills."); return;
//            }
//        }
//
//        // Check pending appointments
//        Appointment* apps = appointments.getAll();
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getPatientId() == pid &&
//                apps[i].getStatus() == "pending") {
//                print("Cannot discharge patient with pending appointments."); return;
//            }
//        }
//
//        FileHandler::dischargePatient(pid, patients, appointments, bills, prescriptions);
//        reloadAll();
//        print("Patient discharged and archived successfully.");
//    }
//
//    // 8. View Security Log
//    void adminViewSecurityLog() {
//        printSep();
//        print("== Security Log ==");
//        string log = FileHandler::readSecurityLog();
//        // Print each line
//        stringstream ss(log);
//        string line;
//        while (getline(ss, line)) print(line);
//    }
//
//    // 9. Daily Report
//    void adminGenerateDailyReport() {
//        printSep();
//        string today = FileHandler::todayString();
//        print("== Daily Report for " + today + " ==");
//
//        // Appointment stats
//        int total = 0, pending = 0, completed = 0, noshow = 0, cancelled = 0;
//        double revenue = 0.0;
//        Appointment* apps = appointments.getAll();
//        for (int i = 0; i < appointments.size(); ++i) {
//            if (!apps[i].isActive()) continue;
//            if (apps[i].getDate() != today) continue;
//            ++total;
//            const string& st = apps[i].getStatus();
//            if (st == "pending")   ++pending;
//            else if (st == "completed") ++completed;
//            else if (st == "no-show")   ++noshow;
//            else if (st == "cancelled") ++cancelled;
//        }
//        print("Total appointments today: " + to_string(total) +
//            " (Pending: " + to_string(pending) +
//            " Completed: " + to_string(completed) +
//            " No-show: " + to_string(noshow) +
//            " Cancelled: " + to_string(cancelled) + ")");
//
//        // Revenue from paid bills today
//        Bill* blist = bills.getAll();
//        for (int i = 0; i < bills.size(); ++i) {
//            if (!blist[i].isActive()) continue;
//            if (blist[i].getDate() != today)  continue;
//            if (blist[i].getStatus() != "paid")  continue;
//            revenue += blist[i].getAmount();
//        }
//        print("Revenue collected today (paid bills): PKR " + to_string((int)revenue));
//
//        // Patients with outstanding unpaid bills
//        print("── Patients with outstanding unpaid bills ──");
//        Patient* pall = patients.getAll();
//        for (int i = 0; i < patients.size(); ++i) {
//            if (!pall[i].isActive()) continue;
//            double owed = 0.0;
//            for (int b = 0; b < bills.size(); ++b) {
//                if (!blist[b].isActive()) continue;
//                if (blist[b].getPatientId() == pall[i].getId() &&
//                    blist[b].getStatus() == "unpaid")
//                    owed += blist[b].getAmount();
//            }
//            if (owed > 0)
//                print("  " + pall[i].getName() + " | Owed: PKR " + to_string((int)owed));
//        }
//
//        // Doctor-wise summary for today
//        print("── Doctor-wise summary today ──");
//        Doctor* docs = doctors.getAll();
//        for (int d = 0; d < doctors.size(); ++d) {
//            if (!docs[d].isActive()) continue;
//            int dc = 0, dp = 0, dn = 0;
//            for (int i = 0; i < appointments.size(); ++i) {
//                if (!apps[i].isActive()) continue;
//                if (apps[i].getDoctorId() != docs[d].getId()) continue;
//                if (apps[i].getDate() != today)           continue;
//                const string& st = apps[i].getStatus();
//                if (st == "completed") ++dc;
//                else if (st == "pending") ++dp;
//                else if (st == "no-show") ++dn;
//            }
//            if (dc + dp + dn > 0)
//                print("  Dr. " + docs[d].getName() +
//                    " | Completed: " + to_string(dc) +
//                    " | Pending: " + to_string(dp) +
//                    " | No-show: " + to_string(dn));
//        }
//    }
//
//    // ── Admin main menu loop ─────────────────────────────────────
//    void runAdminMenu() {
//        while (gui.window.isOpen()) {
//            printSep();
//            print("Admin Panel — MediCore");
//            printSep();
//
//            const string items[10] = {
//                "1. Add Doctor",
//                "2. Remove Doctor",
//                "3. View All Patients",
//                "4. View All Doctors",
//                "5. View All Appointments",
//                "6. View Unpaid Bills",
//                "7. Discharge Patient",
//                "8. View Security Log",
//                "9. Generate Daily Report",
//                "10. Logout"
//            };
//            int choice = showMenu(gui, items, 10);
//            reloadAll();
//
//            if (choice == 0) adminAddDoctor();
//            else if (choice == 1) adminRemoveDoctor();
//            else if (choice == 2) adminViewAllPatients();
//            else if (choice == 3) adminViewAllDoctors();
//            else if (choice == 4) adminViewAllAppointments();
//            else if (choice == 5) adminViewUnpaidBills();
//            else if (choice == 6) adminDischargePatient();
//            else if (choice == 7) adminViewSecurityLog();
//            else if (choice == 8) adminGenerateDailyReport();
//            else break; // Logout
//        }
//    }
//
//    //  REGISTRATION for new patients
//    void registerPatient() {
//        printSep();
//        print("== Register New Patient ==");
//
//        string name = promptInput(gui, "Full name:");
//        string sage = promptInput(gui, "Age:");
//        if (!Validator::isPositiveInt(sage)) { print("Invalid age."); return; }
//        int age = stoi(sage);
//
//        string sgender = promptInput(gui, "Gender (M/F):");
//        char gender = sgender.empty() ? 'M' : (char)toupper(sgender[0]);
//        if (gender != 'M' && gender != 'F') { print("Invalid gender."); return; }
//
//        string contact = promptInput(gui, "Contact (11 digits):");
//        if (!Validator::isValidContact(contact)) { print("Invalid contact."); return; }
//
//        string password = promptInput(gui, "Password (min 6 chars):");
//        if (!Validator::isValidPassword(password)) { print("Invalid password."); return; }
//
//        int newId = FileHandler::nextId(patients);
//        Patient p(newId, name, age, gender, contact, password, 0.0);
//        patients.add(p);
//        FileHandler::appendPatient(p);
//        print("Registration successful. Your Patient ID is: " + to_string(newId));
//        print("You can now log in with your ID and password.");
//    }
//
//public:
//    explicit HospitalSystem(GuiCtx& g) : gui(g) {}
//
//    void run() {
//        // Load all data from files
//        reloadAll();
//        FileHandler::ensureFile("security_log.txt");
//
//        printSep();
//        print("Welcome to MediCore Hospital Management System");
//        printSep();
//
//        // ── Main login loop ─────────────────────────────────────
//        while (gui.window.isOpen()) {
//            print("");
//            print("Login as:");
//
//            const string roleItems[5] = {
//                "1. Patient",
//                "2. Doctor",
//                "3. Admin",
//                "4. Register as New Patient",
//                "5. Exit"
//            };
//            int role = showMenu(gui, roleItems, 5);
//            if (!gui.window.isOpen()) break;
//
//            if (role == 4) break; // Exit
//
//            if (role == 3) {
//                // Register new patient
//                registerPatient();
//                reloadAll();
//                continue;
//            }
//
//            if (role == 0) {
//                // Patient login
//                int pid = 0;
//                if (loginPatient(pid)) {
//                    reloadAll();
//                    runPatientMenu(pid);
//                    reloadAll();
//                }
//            }
//            else if (role == 1) {
//                // Doctor login
//                int did = 0;
//                if (loginDoctor(did)) {
//                    reloadAll();
//                    runDoctorMenu(did);
//                    reloadAll();
//                }
//            }
//            else if (role == 2) {
//                // Admin login
//                if (loginAdmin()) {
//                    reloadAll();
//                    runAdminMenu();
//                    reloadAll();
//                }
//            }
//        }
//
//        print("Thank you for using MediCore. Goodbye!");
//        drawFrame(gui);
//        // Give user a moment to read the farewell message
//        sf::sleep(sf::seconds(1.5f));
//    }
//};
//
//// ═══════════════════════════════════════════════════════════════
////  SECTION 10 — main()
////  A sequence of function/method calls only — no logic here.
//// ═══════════════════════════════════════════════════════════════
//
//int main() {
//    // ── Initialise SFML window ──────────────────────────────────
//    g_gui.window.create(
//        sf::VideoMode(1200, 720),
//        "MediCore Hospital Management System",
//        sf::Style::Close | sf::Style::Titlebar
//    );
//    g_gui.window.setFramerateLimit(60);
//
//    // ── Load font — try a few common system paths ───────────────
//    const char* fontPaths[] = {
//        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
//        "/usr/share/fonts/TTF/DejaVuSans.ttf",
//        "C:/Windows/Fonts/arial.ttf",
//        "/System/Library/Fonts/Helvetica.ttc",
//        "DejaVuSans.ttf",
//        "arial.ttf"
//    };
//    for (int i = 0; i < 6; ++i) {
//        if (g_gui.font.loadFromFile(fontPaths[i])) {
//            g_gui.fontLoaded = true;
//            break;
//        }
//    }
//    if (!g_gui.fontLoaded) {
//        // Without a font, SFML text won't render; fall back to console
//        cout << "[Warning] No font found. GUI text will not render correctly.\n"
//            << "          Place DejaVuSans.ttf in the working directory.\n";
//    }
//
//    // ── Draw initial frame ──────────────────────────────────────
//    drawFrame(g_gui);
//
//    // ── Run the hospital system ─────────────────────────────────
//    HospitalSystem system(g_gui);
//    system.run();
//
//    // ── Clean shutdown ──────────────────────────────────────────
//    if (g_gui.window.isOpen())
//        g_gui.window.close();
//
//    return 0;
//}
