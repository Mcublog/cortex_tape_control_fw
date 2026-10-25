/**
 * @file mech_test.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Проверка таймингов машины состояний CRP42602Y на хосте.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include <cstdint>
#include <cstdio>
#include <utility>
#include <vector>

#include "app/interfaces/MechIo.hpp"
#include "app/mech/crp42602y.hpp"

namespace
{

int g_failures = 0;
int g_checks = 0;

#define CHECK(cond)                                                            \
    do                                                                         \
    {                                                                          \
        g_checks++;                                                            \
        if (!(cond))                                                           \
        {                                                                      \
            g_failures++;                                                      \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);        \
        }                                                                      \
    } while (0)

#define CHECK_EQ(a, b)                                                         \
    do                                                                         \
    {                                                                          \
        g_checks++;                                                            \
        const auto _a = (a);                                                   \
        const auto _b = (b);                                                   \
        if (!(_a == _b))                                                       \
        {                                                                      \
            g_failures++;                                                      \
            std::printf("FAIL %s:%d: %s != %s (%u != %u)\n", __FILE__,         \
                        __LINE__, #a, #b, (unsigned)_a, (unsigned)_b);         \
        }                                                                      \
    } while (0)

struct Event
{
    int tick;
    bool value;
};

class FakeIo final : public MechIo
{
  public:
    bool home_state = true;
    bool tacho_state = false;
    bool tape_state = true;
    bool no_rec_fwd_state = false;
    bool no_rec_rev_state = false;

    bool solenoid_state = false;
    bool capstan_state = false;

    int tick = 0;
    std::vector<Event> solenoid_events;

    void solenoid(bool on) override
    {
        if (solenoid_state != on)
        {
            solenoid_state = on;
            solenoid_events.push_back({tick, on});
        }
    }

    void capstan(bool on) override
    {
        capstan_state = on;
    }

    void mute(bool) override
    {
    }

    void rec_en(bool) override
    {
    }

    bool home() override
    {
        return home_state;
    }

    bool tacho() override
    {
        return tacho_state;
    }

    bool tape_in() override
    {
        return tape_state;
    }

    bool no_rec_fwd() override
    {
        return no_rec_fwd_state;
    }

    bool no_rec_rev() override
    {
        return no_rec_rev_state;
    }
};

void step(Crp42602y &mech, FakeIo &io, int ticks)
{
    for (int i = 0; i < ticks; ++i)
    {
        if ((io.tick % 10) == 0)
            mech.tick20ms();

        mech.tick2ms();
        io.tick++;
    }
}

void bring_to_stop(Crp42602y &mech, FakeIo &io)
{
    io.home_state = true;
    io.tape_state = true;
    io.solenoid_events.clear();
    mech.init();
    step(mech, io, 200);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));
    CHECK(mech.capstan_on());
    io.solenoid_events.clear();
}

int start_active(Crp42602y &mech, FakeIo &io)
{
    io.home_state = false;
    const int start = io.tick;
    io.solenoid_events.clear();
    return start;
}

bool in_intervals(int tick,
                  const std::vector<std::pair<int, int>> &intervals)
{
    for (const auto &i : intervals)
    {
        if ((tick >= i.first) && (tick <= i.second))
            return true;
    }
    return false;
}

void check_solenoid(FakeIo &io, int start,
                    const std::vector<std::pair<int, int>> &intervals,
                    int last_tick)
{
    bool state = false;
    size_t ev = 0;

    for (int r = 0; r <= last_tick; ++r)
    {
        while ((ev < io.solenoid_events.size()) &&
               ((io.solenoid_events[ev].tick - start) == r))
        {
            state = io.solenoid_events[ev].value;
            ev++;
        }

        const bool expected = in_intervals(r, intervals);
        if (state != expected)
        {
            g_failures++;
            std::printf("FAIL solenoid at t=%d ms: %d, expected %d\n", r * 2,
                        state ? 1 : 0, expected ? 1 : 0);
            return;
        }
    }

    g_checks++;
}

void test_mode(mech::UserMode user, mech::Mode expected,
               const std::vector<std::pair<int, int>> &pulses)
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    const int start = start_active(mech, io);
    CHECK(mech.request(user));

    step(mech, io, 215);

    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(expected));
    CHECK_EQ(mech.error(), 0);
    CHECK(!io.solenoid_state);
    check_solenoid(io, start, pulses, 210);
}

void test_init()
{
    FakeIo io;
    Crp42602y mech(io);

    io.home_state = true;
    io.tape_state = true;
    mech.init();

    step(mech, io, 1);
    CHECK(!mech.capstan_on());
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::SubInit));

    step(mech, io, 1);
    CHECK(mech.capstan_on());

    step(mech, io, 158);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::SubInit));

    step(mech, io, 1);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));
    CHECK(mech.capstan_on());
}

void test_stop_cycle()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    // Сначала в PLAY FWD.
    io.home_state = false;
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::PbFwd));

    // Теперь STOP: HOME появляется к концу цикла.
    io.home_state = true;
    io.solenoid_events.clear();
    const int start = io.tick;
    CHECK(mech.request(mech::UserMode::Stop));

    step(mech, io, 10);
    CHECK(io.solenoid_state);

    step(mech, io, 15);
    CHECK(!io.solenoid_state);

    step(mech, io, 140);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    check_solenoid(io, start, {{2, 23}}, 164);
}

void test_active_failure_halt()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    // HOME не сбрасывается, значит механизм не переключился.
    io.home_state = true;
    CHECK(mech.request(mech::UserMode::PlayFwd));

    step(mech, io, 215);

    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Halt));
    CHECK_EQ(mech.error(),
             static_cast<uint8_t>(mech::Error::NoCtrl));

    // В HALT команды игнорируются.
    CHECK(!mech.request(mech::UserMode::Stop));

    // HOME уже активен, машина стоит.
    step(mech, io, 5);
    CHECK(!mech.capstan_on());
    CHECK(!io.solenoid_state);
}

void test_stop_retries_halt()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    io.home_state = false;
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::PbFwd));

    // HOME так и не появился: 6 попыток STOP, затем HALT.
    CHECK(mech.request(mech::UserMode::Stop));
    step(mech, io, 1000);

    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Halt));
    CHECK((mech.error() & static_cast<uint8_t>(mech::Error::NoCtrl)) != 0);
    CHECK(mech.retries() >= 6);
}

void test_intermediate_stop()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    io.home_state = false;
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 215);

    // Смена активного режима обязана пройти через STOP.
    io.home_state = true;
    CHECK(mech.request(mech::UserMode::FwindFwd));

    int guard = 0;
    while ((mech.mode() != mech::Mode::Stop) && (guard++ < 400))
        step(mech, io, 1);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    io.home_state = false;
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::FwFwd));
}

void test_last_play_dir()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    // PLAY REV оставляет головку в REV.
    const int rev_start = start_active(mech, io);
    CHECK(mech.request(mech::UserMode::PlayRev));
    step(mech, io, 215);
    check_solenoid(io, rev_start, {{2, 53}, {75, 129}}, 210);
    CHECK_EQ(static_cast<uint8_t>(mech.last_play_dir()),
             static_cast<uint8_t>(mech::Direction::Rev));

    // Перемотка вперёд должна выбрать FW_FWD_HD_REV.
    io.home_state = true;
    CHECK(mech.request(mech::UserMode::FwindFwd));

    int guard = 0;
    while ((mech.mode() != mech::Mode::Stop) && (guard++ < 400))
        step(mech, io, 1);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    io.home_state = false;
    const int fw_start = io.tick;
    io.solenoid_events.clear();
    step(mech, io, 215);

    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::FwFwdHdRev));
    check_solenoid(io, fw_start, {{2, 53}, {146, 185}}, 210);
}

void test_record_guard()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    io.no_rec_fwd_state = true;
    step(mech, io, 10);
    CHECK(!mech.request(mech::UserMode::RecFwd));
    io.no_rec_fwd_state = false;
    step(mech, io, 10);
    CHECK(mech.request(mech::UserMode::RecFwd));

    io.home_state = false;
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::RcFwd));

    // Из записи только в STOP.
    CHECK(!mech.request(mech::UserMode::PlayFwd));
    CHECK(mech.request(mech::UserMode::Stop));

    io.home_state = true;
    step(mech, io, 165);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    // Запись из активного PLAY запрещена.
    io.home_state = false;
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 215);
    CHECK(!mech.request(mech::UserMode::RecFwd));
}

void test_play_autostop()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    io.home_state = false;
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::PbFwd));

    // Тахометр молчит: через ~1000 мс должен начаться STOP.
    step(mech, io, 520);
    CHECK(mech.target_mode() == mech::Mode::Stop);

    io.home_state = true;
    step(mech, io, 170);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));
}

void test_idle_timeout()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);
    CHECK(mech.capstan_on());

    // Кассета есть: 120 с.
    step(mech, io, 59950);
    CHECK(mech.capstan_on());
    step(mech, io, 100);
    CHECK(!mech.capstan_on());

    // Запрос PLAY после power-save начинается с INIT.
    io.home_state = true;
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 2);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::SubInit));
    CHECK(!mech.capstan_on());

    step(mech, io, 1);
    CHECK(mech.capstan_on());
}

void test_fw_autostop()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    io.home_state = false;
    CHECK(mech.request(mech::UserMode::FwindFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::FwFwd));

    step(mech, io, 120);
    CHECK(mech.target_mode() == mech::Mode::Stop);

    io.home_state = true;
    step(mech, io, 170);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));
}

void test_play_eof_reverse()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    io.home_state = false;
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::PbFwd));

    // Тахометр молчит: конец ленты в направлении вперёд, автостоп.
    step(mech, io, 520);
    CHECK(mech.target_mode() == mech::Mode::Stop);

    io.home_state = true;
    step(mech, io, 170);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    // Нажатие play вперёд на конце ленты запускает воспроизведение назад.
    const int rev_start = start_active(mech, io);
    CHECK(mech.request(mech::UserMode::PlayFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::PbRev));
    CHECK_EQ(static_cast<uint8_t>(mech.last_play_dir()),
             static_cast<uint8_t>(mech::Direction::Rev));
    check_solenoid(io, rev_start, {{2, 53}, {75, 129}}, 210);

    // Обратный конец ленты: нажатие play назад даёт воспроизведение вперёд.
    step(mech, io, 520);
    CHECK(mech.target_mode() == mech::Mode::Stop);

    io.home_state = true;
    step(mech, io, 170);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    io.home_state = false;
    CHECK(mech.request(mech::UserMode::PlayRev));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::PbFwd));
    CHECK_EQ(static_cast<uint8_t>(mech.last_play_dir()),
             static_cast<uint8_t>(mech::Direction::Fwd));
}

void test_wind_reverse()
{
    FakeIo io;
    Crp42602y mech(io);
    bring_to_stop(mech, io);

    // Середина ленты: повторная перемотка вперёд разворачивает направление.
    io.home_state = false;
    CHECK(mech.request(mech::UserMode::FwindFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::FwFwd));

    CHECK(mech.request(mech::UserMode::FwindFwd));
    io.home_state = true;

    int guard = 0;
    while ((mech.mode() != mech::Mode::Stop) && (guard++ < 400))
        step(mech, io, 1);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    io.home_state = false;
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::FwRev));

    // Повторная перемотка назад возвращает перемотку вперёд.
    CHECK(mech.request(mech::UserMode::FwindRev));
    io.home_state = true;

    guard = 0;
    while ((mech.mode() != mech::Mode::Stop) && (guard++ < 400))
        step(mech, io, 1);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    io.home_state = false;
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::FwFwd));

    // Перемотка вперёд до конца ленты: нажатие вперёд мотает назад.
    step(mech, io, 120);
    CHECK(mech.target_mode() == mech::Mode::Stop);

    io.home_state = true;
    step(mech, io, 170);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::Stop));

    // Головка остаётся в REV, как и при явном запросе перемотки назад.
    io.home_state = false;
    CHECK(mech.request(mech::UserMode::FwindFwd));
    step(mech, io, 215);
    CHECK_EQ(static_cast<uint8_t>(mech.mode()),
             static_cast<uint8_t>(mech::Mode::FwRevHdRev));
}

} // namespace

int main()
{
    test_init();

    test_mode(mech::UserMode::PlayFwd, mech::Mode::PbFwd,
              {{2, 13}, {75, 185}});
    test_mode(mech::UserMode::PlayRev, mech::Mode::PbRev,
              {{2, 53}, {75, 129}});
    test_mode(mech::UserMode::RecFwd, mech::Mode::RcFwd,
              {{2, 13}, {75, 185}});
    test_mode(mech::UserMode::RecRev, mech::Mode::RcRev,
              {{2, 53}, {75, 129}});
    test_mode(mech::UserMode::FwindFwd, mech::Mode::FwFwd,
              {{2, 13}, {146, 185}});
    test_mode(mech::UserMode::FwindRev, mech::Mode::FwRev, {{2, 13}});

    // Режим FW_REV_HD_REV получаем после PLAY REV.
    {
        FakeIo io;
        Crp42602y mech(io);
        bring_to_stop(mech, io);
        io.home_state = false;
        CHECK(mech.request(mech::UserMode::PlayRev));
        step(mech, io, 215);

        io.home_state = true;
        CHECK(mech.request(mech::UserMode::FwindRev));

        int guard = 0;
        while ((mech.mode() != mech::Mode::Stop) && (guard++ < 400))
            step(mech, io, 1);
        CHECK_EQ(static_cast<uint8_t>(mech.mode()),
                 static_cast<uint8_t>(mech::Mode::Stop));

        io.home_state = false;
        const int start = io.tick;
        io.solenoid_events.clear();
        step(mech, io, 215);
        CHECK_EQ(static_cast<uint8_t>(mech.mode()),
                 static_cast<uint8_t>(mech::Mode::FwRevHdRev));
        check_solenoid(io, start, {{2, 53}}, 210);
    }

    test_stop_cycle();
    test_active_failure_halt();
    test_stop_retries_halt();
    test_intermediate_stop();
    test_last_play_dir();
    test_record_guard();
    test_play_autostop();
    test_fw_autostop();
    test_play_eof_reverse();
    test_wind_reverse();
    test_idle_timeout();

    std::printf("%d checks, %d failures\n", g_checks, g_failures);
    return (g_failures == 0) ? 0 : 1;
}
