#include "SQLiteOperatorRepository.h"
#include <stdexcept>

SQLiteOperatorRepository::SQLiteOperatorRepository(const std::string& databaseFile)
    : storage_(createStorage(databaseFile)) {
    storage_.on_open = [](sqlite3* db) {
        if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Cannot enable foreign key constraints");
        }
        };
    storage_.sync_schema();
}

int SQLiteOperatorRepository::add(const Operator& op) { return storage_.insert(op); }
std::optional<Operator> SQLiteOperatorRepository::findById(int id) { return storage_.get_optional<Operator>(id); }
std::vector<Operator> SQLiteOperatorRepository::findAll() { return storage_.get_all<Operator>(); }

bool SQLiteOperatorRepository::update(const Operator& op) {
    if (!storage_.get_optional<Operator>(op.id)) return false;
    storage_.update(op);
    return true;
}

bool SQLiteOperatorRepository::remove(int id) {
    if (!storage_.get_optional<Operator>(id)) return false;
    storage_.remove<Operator>(id);
    return true;
}