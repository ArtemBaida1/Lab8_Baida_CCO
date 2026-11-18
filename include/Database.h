#pragma once
#include <string>
#include <sqlite_orm/sqlite_orm.h>
#include "Operator.h"
#include "Call.h"

inline auto createStorage(const std::string& filename) {
    using namespace sqlite_orm;
    return make_storage(filename,
        make_table("Operator",
            make_column("id", &Operator::id, primary_key().autoincrement()),
            make_column("name", &Operator::name),
            make_column("department", &Operator::department),
            make_column("shiftType", &Operator::shiftType)
        ),
        make_table("Call",
            make_column("id", &Call::id, primary_key().autoincrement()),
            make_column("operatorId", &Call::operatorId),
            make_column("callDate", &Call::callDate),
            make_column("clientPhone", &Call::clientPhone),
            make_column("durationSeconds", &Call::durationSeconds),
            foreign_key(&Call::operatorId).references(&Operator::id)
        )
    );
}