# Лабораторна робота %8 — Система обліку роботи кол-центру
# Варіант 2

Консольний застосунок на C++20 із використанням SQLite, `sqlite_orm` та патерну Repository.

## Збирання та запуск
1. Переконайтеся, що встановлено `vcpkg`, `CMake` та компілятор C++20 (MSVC).
2. Задайте змінну середовища `VCPKG_ROOT` або вкажіть шлях до `vcpkg.cmake` у `CMakePresets.json`.
3. Відкрийте папку проєкту у Visual Studio (Open a local folder) або виконайте в терміналі:
   ```bash
   cmake --preset default
   cmake --build build
   ./build/CallCenterORM.exe