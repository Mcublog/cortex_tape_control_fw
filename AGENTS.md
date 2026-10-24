# AGENTS.md

Прошивка кассетного механизма CRP42602Y (M02753900D) на STM32F072CB.
`app/` — общий код, `arch/` + `bsp/` — платформенные реализации (stm32f0 / desktop),
`targets/` — сборки. Логика и тайминги механики описаны в `docs/mech/crp42602y.md`
(там же разбор оригинальной AVRTapeControl, с которой портирована машина состояний).

## Правила работы

- Коммиты — только по отдельному прямому запросу. Не коммитить самостоятельно,
  даже когда правки выглядят завершёнными и проверенными.
- Состав и объём тестов всегда согласовывать с пользователем заранее. Не добавлять
  новые кейсы и не расширять существующие по своей инициативе.

## Сборка

Toolchain-файла нет: без явных компиляторов CMake берёт host gcc и падает на `-mthumb`.

```bash
TC=~/.local/bin/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=$TC/arm-none-eabi-gcc \
  -DCMAKE_CXX_COMPILER=$TC/arm-none-eabi-g++ \
  -DCMAKE_ASM_COMPILER=$TC/arm-none-eabi-gcc \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build                                  # всё: fw + 5 hwtest + emu
cmake --build build --target fw_ctc_v1               # только прошивка
```

- Build types: `Debug` (`-Og -g3`), `Release` (`-Os -s`), `RelWithDebInfo` (`-Os -g3`, включает `FORCE_LOG`).
- `build/output/*.hex` — путь захардкожен в `cmake/utils.cmake`: артефакты всегда падают
  в `<repo>/build/output`, даже при сборке из другой директории.
- Прошивка: `JLink -CommanderScript build/targets/ctc_v1/fw_ctc_v1.jlink` (скрипт генерится CMake,
  hex берётся из `build/output/`).

## Тесты

`test/` не входит в корневую сборку (`add_subdirectory(test)` закомментирован) — собирается отдельно.
Сейчас один набор: `test/mech` — тайминги машины состояний CRP42602Y на host-компиляторе.

```bash
cmake -S test/mech -B build_mech_test -G Ninja
cmake --build build_mech_test
./build_mech_test/mech_test        # успех: "126 checks, 0 failures", exit code 0
```

`err> mech: transport error` в выводе — ожидаемые сообщения из негативных кейсов, не провал.

## Ловушки CMake

- Везде `file(GLOB_RECURSE ...)` без `CONFIGURE_DEPENDS`: после добавления/удаления файла
  нужен перезапуск `cmake -S . -B build ...`, иначе новые исходники не попадут в сборку.
- `app/version.h` генерируется из `cmake/version.h.in` (`configure_file` в `cmake/CMakeLists.txt`),
  но лежит в git: бамп версии — это правка `project(... VERSION ...)` в корневом `CMakeLists.txt`
  **и** `app/version.h` одним коммитом.
- `MAX_TIMERS=10` задаётся из cmake (`cmake/mcu_f0.cmake`, `cmake/desktop.cmake`); дефолт
  библиотеки — 5, а `application()` уже создаёт 5 таймеров.
- `targets/emu` принудительно ставит host `gcc/g++` даже при ARM-toolchain в кэше — эмулятор
  всегда x86-64.

## Архитектура

- Две точки входа: `targets/ctc_v1/Core/Src/main.c` (CubeMX-проект, пользовательский код только
  в блоках `USER CODE BEGIN/END`) и `targets/emu/main.cpp`.
- Кооперативный главный цикл в `app/application.cpp`: флаги 1/2/10/20/100 мс из SoftwareTimer;
  `Mech::instance().tick2ms()` должен идти каждые 2 мс, `tick20ms()` — каждые 20 мс
  (все константы механики заданы в тиках по 2 мс).
- `app/` собирается под обе платформы, поэтому изменения `app/interfaces/System.hpp` и
  `app/interfaces/MechIo.hpp` требуют реализации в `arch/desktop` + `bsp/desktop`
  **и** в `arch/stm32f0` + `bsp/ctc_v1`. Платформенные реализации подключаются через
  `isystem()` и `board_mech_io()`.
- hwtest-сборки (`hwtest/pcb_ctc_v1/*`) собирают `app/` **без** `app/application.cpp`
  и подменяют `application()` своей; новый код с побочными эффектами на старте может
  сломать их линковку.
- Протокол: COBS + CRC32, точка входа `Communication::process()` в
  `app/process/communication/process.cpp`. Новая команда = id и структура в `app/proto/commands.h`,
  запись в таблицу `m_command_list`, handler в `app/process/communication/handlers/`.
- Логи — SEGGER RTT через `libs/debug/log_libs.h`: `LOG_MODULE_NAME` + `LOG_MODULE_LEVEL`
  на каждый файл (`LOG_INFO` требует уровень > 2). На desktop дополнительно нужен `FORCE_LOG`.

## CLI и эмулятор

- `scripts/tapecli/` — Python-клиент протокола. Править только `src/`;
  `scripts/tapecli/build/lib/` — устаревшая копия, не трогать.
- `tapecli` отстаёт от прошивки: знает команды 0x00–0x02, `CMDID_MECH_*` нет. При добавлении
  команды в прошивку синхронизировать `scripts/tapecli/src/tapecli/protocol/commands/list.py`.
- `env/` — venv, собранный на Python 3.12 (системный теперь 3.14), поэтому `env/bin/tapecli`
  падает с `ModuleNotFoundError`. Пересоздавать через `python3.12 -m venv env`.
- E2E без платы: `scripts/nullmodem_tty.sh` (socat) поднимает `/dev/ttyS10` ↔ `/dev/ttyS11`;
  эмулятор открывает `/dev/ttyS11`, CLI — `/dev/ttyS10`.

## Конвенции

- C11 / C++20, `gnu`-расширения включены; стиль — `.vscode/settings.json`
  (Microsoft, column limit 90, indent 4).
- Документирующие комментарии к функциям в заголовках — **на русском**, doxygen-формат
  (`@brief` + пояснение, при необходимости параметры). Образцы: `app/mech/crp42602y.hpp`,
  `app/mech/mech.hpp`, `app/interfaces/MechIo.hpp`. Заголовки без комментариев есть
  (`app/io/igpio.h`, `app/proto/handlers.h`) — при правке такого файла комментарии
  к затронутым функциям добавляются на русском.
- Файловые шапки — doxygen `@file/@brief/@author/@copyright`, как в существующих файлах.
- Коммиты: `<scope>: <что сделано>` (`app`, `fw`, `hwtest`, `libs`, `tapecli`, `docs`).
  Основная работа — в ветке `develop`.
- `libs/SoftwareTimer` — git submodule (плюс `libs/Ring-Buffer`, `libs/cobs-c` — vendored):
  не править внутри без явного запроса, изменения идут через их репозитории.
