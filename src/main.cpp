#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <sqlite3.h>
#include "Database.h"
#include "SQLiteOperatorRepository.h"

using namespace sqlite_orm;

void prepareTestData(auto& storage) {
    storage.sync_schema();
    std::vector<Operator> operators = {
        {0, "Олена", "Support", "Day"},
        {0, "Іван", "Sales", "Day"},
        {0, "Марія", "Support", "Night"},
        {0, "Петро", "Technical", "Day"},
        {0, "Анна", "Sales", "Night"}
    };

    std::vector<int> opIds;
    for (const auto& op : operators) opIds.push_back(storage.insert(op));

    std::vector<Call> calls = {
        {0, opIds[0], "2026-11-01", "+380501111111", 120},
        {0, opIds[0], "2026-11-01", "+380501111112", 45},
        {0, opIds[0], "2026-11-02", "+380501111113", 300},
        {0, opIds[1], "2026-11-01", "+380672222221", 600},
        {0, opIds[1], "2026-11-02", "+380672222222", 150},
        {0, opIds[1], "2026-11-03", "+380672222223", 90},
        {0, opIds[2], "2026-11-01", "+380633333331", 200},
        {0, opIds[2], "2026-11-02", "+380633333332", 180},
        {0, opIds[2], "2026-11-03", "+380633333333", 400},
        {0, opIds[3], "2026-11-01", "+380994444441", 1200},
        {0, opIds[3], "2026-11-02", "+380994444442", 850},
        {0, opIds[3], "2026-11-03", "+380994444443", 320},
        {0, opIds[4], "2026-11-01", "+380975555551", 45},
        {0, opIds[4], "2026-11-02", "+380975555552", 60},
        {0, opIds[4], "2026-11-03", "+380975555553", 110}
    };
    for (const auto& c : calls) storage.insert(c);
}

void runRawSQLiteDemo() {
    std::cout << "\nЗавдання 2: SQLite C API vs ORM\n";
    sqlite3* db = nullptr;
    sqlite3_open("raw.db", &db);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Operator (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, department TEXT, shiftType TEXT);", nullptr, nullptr, nullptr);

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, "INSERT INTO Operator (name, department, shiftType) VALUES (?, ?, ?);", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, "Максим", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, "Support", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, "Day", -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    std::cout << "Запис додано через сирий C API.\n";
    sqlite3_close(db);
}

void runQueriesAndTransactionsDemo() {
    std::filesystem::remove("call_center.db");
    auto storage = createStorage("call_center.db");
    storage.on_open = [](sqlite3* db) { sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr); };
    storage.open_forever();

    prepareTestData(storage);

    std::cout << "\nЗавдання 5: Запити\n";

    auto nightShift = storage.get_all<Operator>(where(c(&Operator::shiftType) == "Night"));
    std::cout << "Нічна зміна: " << nightShift.size() << " операторів\n";

    auto longestCalls = storage.get_all<Call>(order_by(&Call::durationSeconds).desc(), limit(3));
    std::cout << "Найдовший дзвінок: " << longestCalls[0].durationSeconds << " секунд\n";

    auto avgDuration = storage.avg(&Call::durationSeconds);
    std::cout << "Середня тривалість дзвінка: " << avgDuration << " секунд\n";

    auto supportCalls = storage.select(
        columns(&Operator::name, &Call::clientPhone, &Call::durationSeconds),
        inner_join<Call>(on(c(&Operator::id) == &Call::operatorId)),
        where(c(&Operator::department) == "Support")
    );
    std::cout << "Дзвінків у відділі Support: " << supportCalls.size() << "\n";

    std::cout << "\nЗавдання 6: Транзакції\n";

    storage.transaction([&]() {
        int newOpId = storage.insert(Operator{ 0, "Олег", "Sales", "Day" });
        storage.insert(Call{ 0, newOpId, "2026-11-18", "+380000000000", 120 });
        return true;
        });
    std::cout << "Успішна транзакція збережена. Кількість операторів: " << storage.count<Operator>() << "\n";

    try {
        storage.transaction([&]() {
            storage.insert(Operator{ 0, "Тест Відкат", "Support", "Night" });
            storage.insert(Call{ 0, -1, "2026-11-18", "+380111111111", 50 });
            return true;
            });
    }
    catch (...) {
        std::cout << "Транзакція скасована через помилку Foreign Key.\n";
    }

    auto rollbackCheck = storage.get_all<Operator>(where(c(&Operator::name) == "Тест Відкат"));
    std::cout << "Запис 'Тест Відкат' існує: " << std::boolalpha << !rollbackCheck.empty() << "\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try {
        runRawSQLiteDemo();
        runQueriesAndTransactionsDemo();
    }
    catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << '\n';
    }
    return 0;
}