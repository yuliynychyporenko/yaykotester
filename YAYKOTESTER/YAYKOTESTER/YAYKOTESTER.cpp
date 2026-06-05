#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <windows.h>

using namespace std;

enum class EggColor {
    WHITE = 0, BROWN, BLUE, GREEN, DIRTY, UNKNOWN
};

enum class ShellState {
    INTACT = 0, SMALL_CRACK, LARGE_CRACK, BROKEN, DIRTY
};

enum class EggSize {
    SMALL = 0, MEDIUM, LARGE, XLARGE, JUMBO
};

enum class TestResult {
    APPROVED = 0, LIMITED, REJECTED, WASHING_NEEDED
};

class Date {
private:
    int day, month, year;
public:
    Date() {
        time_t now = time(0);
        tm ltm;
        localtime_s(&ltm, &now);
        day = ltm.tm_mday;
        month = 1 + ltm.tm_mon;
        year = 1900 + ltm.tm_year;
    }

    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    string toString() const {
        stringstream ss;
        ss << setw(2) << setfill('0') << day << "."
            << setw(2) << setfill('0') << month << "."
            << year;
        return ss.str();
    }

    void save(ofstream& file) const {
        file << day << " " << month << " " << year << endl;
    }

    void load(ifstream& file) {
        file >> day >> month >> year;
    }
};

class Egg {
private:
    int id;
    EggColor color;
    ShellState shellState;
    EggSize size;
    double weight;
    double freshnessScore;
    Date testDate;
    TestResult result;
    string notes;
    static int nextId;

public:
    Egg() : id(0), color(EggColor::UNKNOWN), shellState(ShellState::BROKEN),
        size(EggSize::SMALL), weight(0), freshnessScore(0), result(TestResult::REJECTED) {
    }

    Egg(EggColor c, ShellState s, EggSize sz, double w, double fresh, string n = "")
        : color(c), shellState(s), size(sz), weight(w), freshnessScore(fresh),
        notes(n), testDate(Date()) {
        id = ++nextId;
        result = evaluateQuality();
    }

    int getId() const { return id; }
    TestResult getResult() const { return result; }
    EggColor getColor() const { return color; }

    TestResult evaluateQuality() const {
        if (shellState == ShellState::BROKEN) return TestResult::REJECTED;
        if (shellState == ShellState::DIRTY || color == EggColor::DIRTY)
            return TestResult::WASHING_NEEDED;
        if (shellState == ShellState::LARGE_CRACK) return TestResult::LIMITED;
        if (shellState == ShellState::SMALL_CRACK && freshnessScore < 5.0)
            return TestResult::LIMITED;
        if (color == EggColor::UNKNOWN) return TestResult::REJECTED;
        if (freshnessScore < 4.0) return TestResult::LIMITED;
        if (color == EggColor::WHITE && shellState == ShellState::INTACT && freshnessScore >= 6.0)
            return TestResult::APPROVED;
        if (shellState == ShellState::INTACT && freshnessScore >= 5.0)
            return TestResult::APPROVED;
        return TestResult::LIMITED;
    }

    string colorToString() const {
        switch (color) {
        case EggColor::WHITE: return "Білий";
        case EggColor::BROWN: return "Коричневий";
        case EggColor::BLUE: return "Блакитний";
        case EggColor::GREEN: return "Зелений";
        case EggColor::DIRTY: return "Брудний";
        default: return "Невідомий";
        }
    }

    string shellToString() const {
        switch (shellState) {
        case ShellState::INTACT: return "Ціле";
        case ShellState::SMALL_CRACK: return "Маленька тріщина";
        case ShellState::LARGE_CRACK: return "Велика тріщина";
        case ShellState::BROKEN: return "Розбите";
        case ShellState::DIRTY: return "Забруднене";
        default: return "Невідомо";
        }
    }

    string sizeToString() const {
        switch (size) {
        case EggSize::SMALL: return "S (35-45г)";
        case EggSize::MEDIUM: return "M (45-55г)";
        case EggSize::LARGE: return "L (55-65г)";
        case EggSize::XLARGE: return "XL (65-75г)";
        case EggSize::JUMBO: return "Jumbo (75г+)";
        default: return "Невідомо";
        }
    }

    string resultToString() const {
        switch (result) {
        case TestResult::APPROVED: return "[OK] СХВАЛЕНО";
        case TestResult::LIMITED: return "[!] ОБМЕЖЕНО";
        case TestResult::REJECTED: return "[X] ВІДХИЛЕНО";
        case TestResult::WASHING_NEEDED: return "[~] ПОТРІБНЕ МИТТЯ";
        default: return "Невідомо";
        }
    }

    void save(ofstream& file) const {
        file << id << endl;
        file << static_cast<int>(color) << endl;
        file << static_cast<int>(shellState) << endl;
        file << static_cast<int>(size) << endl;
        file << weight << endl;
        file << freshnessScore << endl;
        testDate.save(file);
        file << static_cast<int>(result) << endl;
        file << notes << endl;
        file << "---" << endl;
    }

    void load(ifstream& file) {
        int c, s, sz, r;
        file >> id >> c >> s >> sz >> weight >> freshnessScore;
        color = static_cast<EggColor>(c);
        shellState = static_cast < ShellState > (s);
        size = static_cast<EggSize>(sz);
        testDate.load(file);
        file >> r;
        result = static_cast<TestResult>(r);
        file.ignore();
        getline(file, notes);
        string separator;
        getline(file, separator);
        if (id >= nextId) nextId = id;
    }

    void display() const {
        cout << "\n+======================================+" << endl;
        cout << "|         ЯЙЦЕ #" << setw(3) << id << "                  |" << endl;
        cout << "+======================================+" << endl;
        cout << "| Колір:        " << left << setw(25) << colorToString() << "|" << endl;
        cout << "| Стан:         " << setw(25) << shellToString() << "|" << endl;
        cout << "| Розмір:       " << setw(25) << sizeToString() << "|" << endl;
        cout << "| Вага:         " << setw(25) << (to_string(weight) + " г") << "|" << endl;
        cout << "| Свіжість:     " << setw(25) << (to_string(freshnessScore) + "/10") << "|" << endl;
        cout << "| Дата тесту:   " << setw(25) << testDate.toString() << "|" << endl;
        cout << "+======================================+" << endl;
        cout << "| РЕЗУЛЬТАТ:    " << setw(25) << resultToString() << "|" << endl;
        cout << "+======================================+" << endl;
        if (!notes.empty()) {
            cout << "Примітка: " << notes << endl;
        }
    }

    void displayShort() const {
        cout << "| " << setw(3) << id
            << " | " << setw(10) << colorToString()
            << " | " << setw(15) << shellToString()
            << " | " << setw(10) << sizeToString()
            << " | " << setw(6) << weight
            << " | " << setw(30) << resultToString() << " |" << endl;
    }
};

int Egg::nextId = 0;

class EggTestingSystem {
private:
    vector<Egg> eggs;
    const string FILENAME = "egg_tests.dat";

public:
    EggTestingSystem() { loadFromFile(); }
    ~EggTestingSystem() { saveToFile(); }

    void addEgg() {
        cout << "\n=== НОВЕ ТЕСТУВАННЯ ЯЙЦЯ ===" << endl;

        cout << "\nОберіть колір яйця:" << endl;
        cout << "1. Білий (Рекомендовано для продажу)" << endl;
        cout << "2. Коричневий" << endl;
        cout << "3. Блакитний" << endl;
        cout << "4. Зелений" << endl;
        cout << "5. Брудний (Потребує миття)" << endl;
        cout << "6. Невідомий" << endl;
        int colorChoice = getChoice(1, 6);
        EggColor color = static_cast<EggColor>(colorChoice - 1);

        cout << "\nОберіть стан шкаралупи:" << endl;
        cout << "1. Ціле" << endl;
        cout << "2. Маленька тріщина" << endl;
        cout << "3. Велика тріщина" << endl;
        cout << "4. Розбите" << endl;
        cout << "5. Забруднене" << endl;
        int shellChoice = getChoice(1, 5);
        ShellState shell = static_cast < ShellState > (shellChoice - 1);

        cout << "\nОберіть розмір:" << endl;
        cout << "1. S (35-45г)" << endl;
        cout << "2. M (45-55г)" << endl;
        cout << "3. L (55-65г)" << endl;
        cout << "4. XL (65-75г)" << endl;
        cout << "5. Jumbo (75г+)" << endl;
        int sizeChoice = getChoice(1, 5);
        EggSize size = static_cast<EggSize>(sizeChoice - 1);

        cout << "\nВведіть вагу яйця (грами): ";
        double weight = getDouble(10, 150);

        cout << "\nОцініть свіжість яйця (0.0 - 10.0):" << endl;
        cout << "10.0 - Ідеально свіже" << endl;
        cout << "7.0-9.0 - Дуже свіже" << endl;
        cout << "5.0-6.9 - Свіже" << endl;
        cout << "3.0-4.9 - Середня свіжість" << endl;
        cout << "0.0-2.9 - Несвіже" << endl;
        double freshness = getDouble(0.0, 10.0);

        cout << "\nДодаткові примітки (Enter для пропуску): ";
        cin.ignore();
        string notes;
        getline(cin, notes);

        Egg newEgg(color, shell, size, weight, freshness, notes);
        eggs.push_back(newEgg);

        cout << "\n[OK] Яйце додано до системи!" << endl;
        newEgg.display();
        giveRecommendations(newEgg);
    }

    void showAll() const {
        if (eggs.empty()) {
            cout << "\nСистема порожня. Додайте яйце." << endl;
            return;
        }
        cout << "\n=== СПИСОК ВСІХ ТЕСТОВАНИХ ЯЄЦЬ ===" << endl;
        printHeader();
        for (const auto& egg : eggs) egg.displayShort();
        printFooter();
    }

    void showDetails() const {
        if (eggs.empty()) {
            cout << "\nСистема порожня." << endl;
            return;
        }
        cout << "\nВведіть ID яйця: ";
        int id; cin >> id;
        auto it = find_if(eggs.begin(), eggs.end(), [id](const Egg& e) { return e.getId() == id; });
        if (it != eggs.end()) it->display();
        else cout << "\n[X] Яйце з ID " << id << " не знайдено." << endl;
    }

    void showStatistics() const {
        int total = 0, approved = 0, limited = 0, rejected = 0, washing = 0;
        for (const auto& egg : eggs) {
            total++;
            switch (egg.getResult()) {
            case TestResult::APPROVED: approved++; break;
            case TestResult::LIMITED: limited++; break;
            case TestResult::REJECTED: rejected++; break;
            case TestResult::WASHING_NEEDED: washing++; break;
            }
        }
        cout << "\n+======================================+" << endl;
        cout << "|         СТАТИСТИКА СИСТЕМИ           |" << endl;
        cout << "+======================================+" << endl;
        cout << "| Всього протестовано:  " << setw(15) << total << " |" << endl;
        cout << "| [OK] Схвалено:       " << setw(15) << approved << " |" << endl;
        cout << "| [!] Обмежено:        " << setw(15) << limited << " |" << endl;
        cout << "| [X] Відхилено:       " << setw(15) << rejected << " |" << endl;
        cout << "| [~] Потрібне миття:  " << setw(15) << washing << " |" << endl;
        cout << "+======================================+" << endl;
        if (total > 0) {
            double passRate = (approved * 100.0) / total;
            cout << "| Рівень придатності:   " << setw(14) << fixed << setprecision(1) << passRate << "% |" << endl;
        }
        cout << "+======================================+" << endl;
    }

    void filterByResult() const {
        cout << "\nОберіть результат для фільтрації:" << endl;
        cout << "1. Схвалено" << endl;
        cout << "2. Обмежено" << endl;
        cout << "3. Відхилено" << endl;
        cout << "4. Потрібне миття" << endl;
        int choice = getChoice(1, 4);
        TestResult filter = static_cast<TestResult>(choice - 1);
        cout << "\n=== РЕЗУЛЬТАТИ ФІЛЬТРАЦІЇ ===" << endl;
        printHeader();
        bool found = false;
        for (const auto& egg : eggs) {
            if (egg.getResult() == filter) {
                egg.displayShort();
                found = true;
            }
        }
        printFooter();
        if (!found) cout << "\nЯйця з таким результатом не знайдено." << endl;
    }

    void deleteEgg() {
        cout << "\nВведіть ID яйця для видалення: ";
        int id; cin >> id;
        auto it = remove_if(eggs.begin(), eggs.end(), [id](const Egg& e) { return e.getId() == id; });
        if (it != eggs.end()) {
            eggs.erase(it, eggs.end());
            cout << "\n[OK] Яйце #" << id << " видалено." << endl;
        }
        else cout << "\n[X] Яйце не знайдено." << endl;
    }

    void quickTest() {
        cout << "\n=== ШВИДКИЙ ТЕСТ (Демо) ===" << endl;
        eggs.push_back(Egg(EggColor::WHITE, ShellState::INTACT, EggSize::LARGE, 62.5, 9.2, "Ідеальне біле яйце"));
        eggs.push_back(Egg(EggColor::BROWN, ShellState::SMALL_CRACK, EggSize::MEDIUM, 48.0, 6.5, "Невелика тріщина"));
        eggs.push_back(Egg(EggColor::WHITE, ShellState::BROKEN, EggSize::LARGE, 60.0, 8.0, "Розбите при транспортуванні"));
        eggs.push_back(Egg(EggColor::DIRTY, ShellState::INTACT, EggSize::XLARGE, 70.0, 7.5, "Брудне, потрібне миття"));
        eggs.push_back(Egg(EggColor::BLUE, ShellState::INTACT, EggSize::JUMBO, 80.0, 9.5, "Порода Арокана"));
        cout << "\n[OK] Додано 5 демо-яєць!" << endl;
        showAll();
    }

    void showHelp() const {
        cout << "\n+==============================================+" << endl;
        cout << "|              ДОВІДКА ПО СИСТЕМІ              |" << endl;
        cout << "+==============================================+" << endl;
        cout << "| ПРАВИЛА КЛАСИФІКАЦІЇ ЯЄЦЬ:                  |" << endl;
        cout << "|                                              |" << endl;
        cout << "| [OK] СХВАЛЕНО (Можна продавати):            |" << endl;
        cout << "|    • Біле яйце, ціле, свіжість >= 6.0        |" << endl;
        cout << "|    • Будь-який колір, ціле, свіжість >= 5.0  |" << endl;
        cout << "|                                              |" << endl;
        cout << "| [!] ОБМЕЖЕНО (Знижка 50%):                   |" << endl;
        cout << "|    • Маленька тріщина + свіжість < 5.0       |" << endl;
        cout << "|    • Велика тріщина                          |" << endl;
        cout << "|    • Свіжість < 4.0                          |" << endl;
        cout << "|                                              |" << endl;
        cout << "| [X] ВІДХИЛЕНО (Утилізація):                  |" << endl;
        cout << "|    • Розбите яйце                            |" << endl;
        cout << "|    • Невідомий колір                         |" << endl;
        cout << "|                                              |" << endl;
        cout << "| [~] ПОТРІБНЕ МИТТЯ:                         |" << endl;
        cout << "|    • Брудний колір або забруднена шкаралупа  |" << endl;
        cout << "+==============================================+" << endl;
    }

private:
    int getChoice(int min, int max) const {
        int choice;
        while (true) {
            cout << "\nВаш вибір (" << min << "-" << max << "): ";
            cin >> choice;
            if (cin.fail() || choice < min || choice > max) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[X] Некоректний вибір. Спробуйте ще раз." << endl;
            }
            else {
                cin.ignore(10000, '\n');
                return choice;
            }
        }
    }

    double getDouble(double min, double max) const {
        double val;
        while (true) {
            cin >> val;
            if (cin.fail() || val < min || val > max) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[X] Некоректне значення. Діапазон " << min << "-" << max << ": ";
            }
            else {
                cin.ignore(10000, '\n');
                return val;
            }
        }
    }

    void printHeader() const {
        cout << "+-----+------------+-----------------+------------+--------+------------------------------+" << endl;
        cout << "| ID  | Колір      | Стан            | Розмір     | Вага   | Результат                    |" << endl;
        cout << "+-----+------------+-----------------+------------+--------+------------------------------+" << endl;
    }

    void printFooter() const {
        cout << "+-----+------------+-----------------+------------+--------+------------------------------+" << endl;
    }

    void giveRecommendations(const Egg& egg) const {
        cout << "\nРЕКОМЕНДАЦІЇ:" << endl;
        switch (egg.getResult()) {
        case TestResult::APPROVED:
            cout << "   • Можна розміщувати на полиці за повною ціною" << endl;
            cout << "   • Рекомендований термін зберігання: 28 днів" << endl;
            break;
        case TestResult::LIMITED:
            cout << "   • Рекомендується знижка 50%" << endl;
            cout << "   • Термін зберігання: 7 днів" << endl;
            cout << "   • Використовувати для термічної обробки" << endl;
            break;
        case TestResult::REJECTED:
            cout << "   • НЕ ПРИДАТНЕ ДЛЯ ВЖИВАННЯ!" << endl;
            cout << "   • Направити на утилізацію або переробку" << endl;
            break;
        case TestResult::WASHING_NEEDED:
            cout << "   • Провести миття у теплій воді (40°C)" << endl;
            cout << "   • Після миття провести повторне тестування" << endl;
            break;
        }
    }

    void saveToFile() const {
        ofstream file(FILENAME);
        if (!file.is_open()) return;
        file << eggs.size() << endl;
        for (const auto& egg : eggs) egg.save(file);
        file.close();
    }

    void loadFromFile() {
        ifstream file(FILENAME);
        if (!file.is_open()) return;
        size_t count;
        file >> count;
        file.ignore();
        for (size_t i = 0; i < count; i++) {
            Egg egg;
            egg.load(file);
            eggs.push_back(egg);
        }
        file.close();
    }
};

void showMenu() {
    cout << "\n+======================================+" << endl;
    cout << "|    СИСТЕМА АВТОМАТИЧНОГО ТЕСТУВАННЯ  |" << endl;
    cout << "|         ЯЄЦЬ ЯКОСТІ \"EGG-QA\"         |" << endl;
    cout << "+======================================+" << endl;
    cout << "|  1. Протестувати нове яйце           |" << endl;
    cout << "|  2. Показати всі яйця                |" << endl;
    cout << "|  3. Деталі яйця за ID                |" << endl;
    cout << "|  4. Статистика                       |" << endl;
    cout << "|  5. Фільтр за результатом            |" << endl;
    cout << "|  6. Видалити яйце                    |" << endl;
    cout << "|  7. Швидкий тест (Демо)              |" << endl;
    cout << "|  8. Довідка (Правила класифікації)   |" << endl;
    cout << "|  0. Вихід                            |" << endl;
    cout << "+======================================+" << endl;
    cout << "Ваш вибір: ";
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    EggTestingSystem system;

    cout << "+==============================================+" << endl;
    cout << "|                                              |" << endl;
    cout << "|     СИСТЕМА АВТОМАТИЧНОГО ТЕСТУВАННЯ        |" << endl;
    cout << "|              ЯЄЦЬ ЯКОСТІ \"EGG-QA\"          |" << endl;
    cout << "|                                              |" << endl;
    cout << "|  Автоматизована система контролю якості    |" << endl;
    cout << "|  курячих яєць для фермерських              |" << endl;
    cout << "|  господарств та виробництв.                |" << endl;
    cout << "|                                              |" << endl;
    cout << "+==============================================+" << endl;

    int choice;
    do {
        showMenu();
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            choice = -1;
        }
        switch (choice) {
        case 1: system.addEgg(); break;
        case 2: system.showAll(); break;
        case 3: system.showDetails(); break;
        case 4: system.showStatistics(); break;
        case 5: system.filterByResult(); break;
        case 6: system.deleteEgg(); break;
        case 7: system.quickTest(); break;
        case 8: system.showHelp(); break;
        case 0: cout << "\nДякуємо за використання системи EGG-QA!" << endl; break;
        default: cout << "\n[X] Некоректний вибір." << endl;
        }
    } while (choice != 0);

    return 0;
}