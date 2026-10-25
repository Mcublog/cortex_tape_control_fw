# Changelog

Все заметные изменения прошивки **CortexTapeControl** (CRP42602Y на STM32F072CB).

Формат — [Keep a Changelog](https://keepachangelog.com/ru/1.1.0/), версионирование —
[SemVer](https://semver.org/lang/ru/). В список попадают изменения, заметные потребителю
прошивки: поведение механики, протокол, CLI и инструменты сборки.

## [0.2.0] — 2026-10-25

### Добавлено

* Машина состояний CRP42602Y: `app/mech/crp42602y.{hpp,cpp}` — порт AVRTapeControl
  с циклограммами на тайм-базе 2 мс. Режимы `Stop`, `Play Fwd/Rev`, `Rec Fwd/Rev`,
  `Fwind Fwd/Rev`, переходы через `SubToActive`/`SubWait*`, таймауты датчиков,
  проверка HOME в конце цикла, до 6 повторов возврата в STOP и режим `HALT`.
* Типы `mech::UserMode`, `Mode`, `Error`, `Direction`, `Features`, `Sensors`
  (`app/mech/types.hpp`) и интерфейс `MechIo` с реализациями для STM32 (`bsp/ctc_v1`)
  и desktop-эмулятора (`bsp/desktop`).
* Команды протокола: `CMDID_MECH_SET_MODE` (`0x03`), `CMDID_MECH_GET_STATE` (`0x04`),
  `CMDID_MECH_TRANSPORT` (`0x05`) и структура `mech_state_t` (режим, переход,
  счётчик повторов, ошибка, датчики, направление, состояние ведущего двигателя).
* Управление механизмом с лицевой панели (`app/events/every_10ms.cpp`): REC, PLAY,
  STOP, FF, REW; повторное нажатие PLAY меняет направление; запись откладывается
  до стабильного STOP.
* Пин `POWER_ON` (питание `VCC_MECH`/`VCC_SOLENOID`): `io_gpio_power()`, включается
  из `MechIo` при работе механики.
* Host-тесты машины состояний `test/mech` — 126 проверок таймингов, ошибок,
  повторов и `HALT`; собираются отдельно от прошивки.
* hwtest `hwtest_ctc_v1_analog_scale`; обновлены `hwtest_solenoid` (циклы play_a/rew_b)
  и `hwtest_buttons`.
* Документация: `docs/mech/crp42602y.md` (циклограммы, режимы, разбор AVRTapeControl),
  схемы платы v1.1 и v1.2, `AGENTS.md`, переработанный `README.md`.

### Изменено

* `tapecli` переведён на подкоманды: `version`, `state`, `stop`, `play-fwd`, `play-rev`,
  `rewind-fwd`, `rewind-rev`, `set-mode`, `solenoid`. JSON-файлы команд и устаревшие
  модули удалены.
* Протокол расширен новыми id и payload, диспетчеризация команд — в
  `app/process/communication/process.cpp`, обработчики — в `handlers/`.
* Главный цикл: добавлен 2-мс тик механики и обработка событий 10 мс
  (`app/application.cpp`).
* Интерфейсы `System` и `MechIo` расширены (питание механизма, тик 2 мс) с реализациями
  в `arch/stm32f0` + `bsp/ctc_v1` и `arch/desktop` + `bsp/desktop`.

### Исправлено

* Инициализация TIM2: явное присваивание `htim2.Instance` и снятие GPIO-конфигурации
  `SW3_Pin`, конфликтовавшей с `TIM2_CH3`.
* USB CDC: корректная обработка line coding (`GET`/`SET_LINE_CODING`).

### Совместимость

* Протокол несовместим с 0.1.0: добавлены команды `0x03`–`0x05`, набор полей
  `mech_state_t` отсутствует в старых клиентах. Для работы с 0.2.0 нужен обновлённый
  `tapecli`.

## [0.1.0] — 2025-11-30

### Добавлено

* Протокол COBS + CRC32 поверх USB CDC: `CMDID_GET_VERSION` (`0x01`) и
  `CMDID_SOLENOID_PULL` (`0x02`, `pull_time_ms` + `cooldown_ms`).
* Блокирующее управление соленоидом с выдержкой и паузой.
* Эмулятор `emu_ctc_v1` для сборки под host с заглушками ввода-вывода.
* `tapecli` — Python-клиент протокола: сканирование портов, serial-адаптер, COBS, CRC.
* Приём USB-данных через COBS-парсер в `usbd_cdc_if.c`.

### Исправлено

* Сборка эмулятора на host-компиляторе (принудительные host `gcc`/`g++`).

## [0.0.1] — 2025-10-29

Первая прошивка: каркас CubeMX-проекта STM32F072CB, индикация на сдвиговом регистре,
кнопки и входы, таймеры, начальные hwtest-сборки, распиновка разъёма механизма.

[0.2.0]: https://github.com/Mcublog/cortex_tape_control_fw/compare/0.1.0...0.2.0
[0.1.0]: https://github.com/Mcublog/cortex_tape_control_fw/compare/0.0.1...0.1.0
[0.0.1]: https://github.com/Mcublog/cortex_tape_control_fw/releases/tag/0.0.1
