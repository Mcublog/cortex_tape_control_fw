/**
 * @file crp42602y.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Машина состояний механизма CRP42602Y.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include "crp42602y.hpp"

//>>---------------------- Log control
#define LOG_MODULE_NAME mech
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

namespace
{

// Временные метки CRP42602Y, один тик = 2 мс.
constexpr uint8_t kTimStop = 23;       // 46 ms
constexpr uint8_t kTimWaitHead = 12;   // 24 ms
constexpr uint8_t kTimHeadDir = 24;    // 48 ms
constexpr uint8_t kTimWaitPinch = 52;  // 104 ms
constexpr uint8_t kTimPinchEn = 73;    // 146 ms
constexpr uint8_t kTimWaitTakeup = 128; // 256 ms
constexpr uint8_t kTimTakeupDir = 144; // 288 ms
constexpr uint8_t kTimWaitMode = 184;  // 368 ms
constexpr uint8_t kTimActive = 210;    // 420 ms
constexpr uint8_t kTimWaitStop = 160;  // 320 ms

// Таймауты тахометра, один тик = 20 мс.
constexpr uint8_t kTachoStopMax = 12;  // 240 ms
constexpr uint8_t kTachoPlayMax = 50;  // 1000 ms
constexpr uint8_t kTachoFwindMax = 10; // 200 ms

constexpr uint8_t kModeRepMax = 6;
constexpr uint16_t kIdleCapNoTape = 7500;  // 15 с, один тик = 2 мс
constexpr uint16_t kIdleCapTapeIn = 60000; // 120 с
constexpr uint16_t kIdleCapMax = kIdleCapTapeIn;
constexpr uint16_t kTachoTimerMax = 240;

bool target_head_rev(mech::Mode mode)
{
    return (mode == mech::Mode::PbRev) || (mode == mech::Mode::RcRev) ||
           (mode == mech::Mode::FwFwdHdRev) || (mode == mech::Mode::FwRevHdRev);
}

bool target_takeup_fwd(mech::Mode mode)
{
    return (mode == mech::Mode::PbFwd) || (mode == mech::Mode::RcFwd) ||
           (mode == mech::Mode::FwFwd) || (mode == mech::Mode::FwFwdHdRev);
}

bool target_play_rec(mech::Mode mode)
{
    return (mode == mech::Mode::PbFwd) || (mode == mech::Mode::PbRev) ||
           (mode == mech::Mode::RcFwd) || (mode == mech::Mode::RcRev);
}

bool target_rec(mech::Mode mode)
{
    return (mode == mech::Mode::RcFwd) || (mode == mech::Mode::RcRev);
}

} // namespace

Crp42602y::Crp42602y(MechIo &io) : m_io(io)
{
}

void Crp42602y::init()
{
    // Безопасное состояние выходов до первого тика.
    out_solenoid(false);
    out_capstan(false);
    out_mute(true);
    out_rec_en(false);

    m_user_mode = mech::UserMode::Stop;
    m_mode = mech::Mode::Stop;
    m_target_mode = mech::Mode::ToInit;
    m_last_play_dir = mech::Direction::Fwd;

    m_trans_timer = 0;
    m_idle_time = 0;
    m_tacho_timer = 0;
    m_retries = 0;
    m_error = 0;

    sample_sensors();
    m_tacho_level = m_io.tacho();
}

void Crp42602y::set_features(const mech::Features &features)
{
    m_features = features;
}

const mech::Features &Crp42602y::features() const
{
    return m_features;
}

mech::UserMode Crp42602y::user_mode() const
{
    return m_user_mode;
}

mech::Mode Crp42602y::mode() const
{
    return m_mode;
}

mech::Mode Crp42602y::target_mode() const
{
    return m_target_mode;
}

mech::Direction Crp42602y::last_play_dir() const
{
    return m_last_play_dir;
}

uint8_t Crp42602y::transition_left() const
{
    return m_trans_timer;
}

uint8_t Crp42602y::retries() const
{
    return m_retries;
}

uint8_t Crp42602y::error() const
{
    return m_error;
}

mech::Sensors Crp42602y::sensors() const
{
    return m_sensors;
}

bool Crp42602y::capstan_on() const
{
    return m_capstan_state;
}

bool Crp42602y::is_active_mode(mech::Mode mode)
{
    return (mode >= mech::Mode::PbFwd) && (mode <= mech::Mode::FwRevHdRev);
}

bool Crp42602y::is_play_rec_mode(mech::Mode mode)
{
    return target_play_rec(mode);
}

bool Crp42602y::is_fast_wind_mode(mech::Mode mode)
{
    return (mode == mech::Mode::FwFwd) || (mode == mech::Mode::FwRev) ||
           (mode == mech::Mode::FwFwdHdRev) || (mode == mech::Mode::FwRevHdRev);
}

bool Crp42602y::is_rec_mode(mech::Mode mode)
{
    return target_rec(mode);
}

void Crp42602y::out_solenoid(bool on)
{
    if (m_solenoid_state != on)
    {
        m_solenoid_state = on;
        m_io.solenoid(on);
    }
}

void Crp42602y::out_capstan(bool on)
{
    if (m_capstan_state != on)
    {
        m_capstan_state = on;
        m_io.capstan(on);
    }
}

void Crp42602y::out_mute(bool on)
{
    if (m_mute_state != on)
    {
        m_mute_state = on;
        m_io.mute(on);
    }
}

void Crp42602y::out_rec_en(bool on)
{
    if (m_rec_en_state != on)
    {
        m_rec_en_state = on;
        m_io.rec_en(on);
    }
}

void Crp42602y::sample_sensors()
{
    m_sensors.home = m_io.home();
    m_sensors.tape_in = m_io.tape_in();
    m_sensors.no_rec_fwd = m_io.no_rec_fwd();
    m_sensors.no_rec_rev = m_io.no_rec_rev();
}

void Crp42602y::tick20ms()
{
    sample_sensors();

    if (m_tacho_timer < kTachoTimerMax)
        m_tacho_timer++;
}

void Crp42602y::tick2ms()
{
    const bool tacho = m_io.tacho();
    m_sensors.tacho = tacho;

    if (tacho != m_tacho_level)
    {
        m_tacho_level = tacho;
        m_tacho_timer = 0;
    }

    state_machine();
}

void Crp42602y::state_machine()
{
    // Защита от выхода индекса режима за границы.
    if ((m_mode >= mech::Mode::Max) || (m_target_mode >= mech::Mode::Max))
        set_error(mech::Error::LogicFault);

    // Нет кассеты: принудительный STOP и сброс направления.
    if (!m_sensors.tape_in)
    {
        m_last_play_dir = mech::Direction::Fwd;
        m_user_mode = mech::UserMode::Stop;

        if ((m_target_mode != mech::Mode::Halt) &&
            (m_target_mode != mech::Mode::ToInit))
            m_target_mode = mech::Mode::Stop;

        out_mute(true);
        out_rec_en(false);
    }

    if (m_trans_timer == 0)
    {
        if (m_mode == mech::Mode::Halt)
        {
            static_halt();
        }
        else if (m_target_mode != m_mode)
        {
            target2mode();
        }
        else if (user_to_transport(m_user_mode, m_last_play_dir) !=
                 m_target_mode)
        {
            user2target();
        }
        else
        {
            static_mode();
        }

        // Таймаут простоя, только в стабильном состоянии.
        if (!m_sensors.tape_in)
        {
            if (m_idle_time >= kIdleCapNoTape)
                out_capstan(false);
        }
        else
        {
            if (m_idle_time >= kIdleCapTapeIn)
                out_capstan(false);
        }
    }
    else
    {
        m_trans_timer--;
        m_idle_time = 0;
        cyclogram();

        if (m_trans_timer == 0)
            m_tacho_timer = 0;
    }

    if ((m_trans_timer == 1) || (m_target_mode != m_mode))
    {
        LOG_DEBUG("MODE|>t%03u<|u%u}%u>%u|0x%02x", m_trans_timer,
                  static_cast<uint8_t>(m_user_mode),
                  static_cast<uint8_t>(m_mode),
                  static_cast<uint8_t>(m_target_mode), m_sensors.to_byte());
    }
}

mech::Mode Crp42602y::user_to_transport(mech::UserMode mode,
                                         mech::Direction dir) const
{
    switch (mode)
    {
    case mech::UserMode::PlayFwd:
        return mech::Mode::PbFwd;
    case mech::UserMode::PlayRev:
        return mech::Mode::PbRev;
    case mech::UserMode::RecFwd:
        return mech::Mode::RcFwd;
    case mech::UserMode::RecRev:
        return mech::Mode::RcRev;
    case mech::UserMode::FwindFwd:
        return (dir == mech::Direction::Fwd) ? mech::Mode::FwFwd
                                             : mech::Mode::FwFwdHdRev;
    case mech::UserMode::FwindRev:
        return (dir == mech::Direction::Fwd) ? mech::Mode::FwRev
                                             : mech::Mode::FwRevHdRev;
    case mech::UserMode::Stop:
    default:
        return mech::Mode::Stop;
    }
}

void Crp42602y::set_error(mech::Error err)
{
    m_target_mode = mech::Mode::Halt;
    m_mode = mech::Mode::Halt;
    m_error |= static_cast<uint8_t>(err);
    LOG_ERROR("transport error: 0x%02x", m_error);
}

void Crp42602y::static_halt()
{
    m_target_mode = mech::Mode::Halt;
    m_user_mode = mech::UserMode::Stop;

    out_mute(true);
    out_rec_en(false);

    m_trans_timer = 0;

    if (!m_sensors.home)
    {
        out_capstan(true);
        m_trans_timer = kTimWaitStop;
        m_mode = mech::Mode::SubToHalt;
    }
    else
    {
        out_solenoid(false);
        out_capstan(false);
    }
}

void Crp42602y::target2mode()
{
    out_mute(true);
    out_rec_en(false);

    // Для любого перехода мотор должен быть запущен.
    if (!m_capstan_state)
        m_target_mode = mech::Mode::ToInit;

    m_idle_time = 0;
    m_tacho_timer = 0;

    if (m_target_mode == mech::Mode::ToInit)
    {
        m_trans_timer = kTimWaitStop;
        m_mode = mech::Mode::SubInit;
        m_target_mode = mech::Mode::Stop;
    }
    else if (m_target_mode == mech::Mode::Stop)
    {
        m_trans_timer = kTimWaitStop;
        m_mode = mech::Mode::SubToStop;
    }
    else if (m_target_mode == mech::Mode::Halt)
    {
        m_mode = mech::Mode::Halt;
    }
    else
    {
        if ((m_target_mode >= mech::Mode::PbFwd) &&
            (m_target_mode <= mech::Mode::FwRevHdRev))
        {
            // Датчики запрета записи.
            if (m_target_mode == mech::Mode::RcFwd && m_sensors.no_rec_fwd)
                m_target_mode = mech::Mode::PbFwd;
            else if (m_target_mode == mech::Mode::RcRev &&
                     m_sensors.no_rec_rev)
                m_target_mode = mech::Mode::PbRev;

            m_trans_timer = kTimActive;
            m_mode = mech::Mode::SubToActive;
        }
        else
        {
            m_target_mode = mech::Mode::Stop;
            m_user_mode = mech::UserMode::Stop;
        }

        m_error = 0;
    }
}

void Crp42602y::user2target()
{
    if (m_target_mode != mech::Mode::Stop)
    {
        // Любая смена активного режима идёт через STOP.
        m_target_mode = mech::Mode::Stop;
    }
    else
    {
        m_target_mode = user_to_transport(m_user_mode, m_last_play_dir);
    }
}

void Crp42602y::static_mode()
{
    if (m_mode == mech::Mode::Stop)
    {
        if (m_idle_time < kIdleCapMax)
            m_idle_time++;

        out_mute(true);
        out_rec_en(false);

        if (!m_sensors.home)
        {
            m_retries++;

            if (m_retries >= kModeRepMax)
            {
                m_trans_timer = kTimWaitStop;
                m_mode = mech::Mode::SubInit;
                m_target_mode = mech::Mode::Halt;
            }
            else
            {
                m_trans_timer = kTimWaitStop;
                m_mode = mech::Mode::SubToStop;
            }
        }
        else
        {
            m_retries = 0;

            if (m_features.stop_tacho)
            {
                if (m_capstan_state)
                {
                    if (m_tacho_timer > kTachoStopMax)
                        set_error(mech::Error::BadDrive);
                }
                else
                {
                    m_tacho_timer = 0;
                }
            }
        }
    }
    else if (is_play_rec_mode(m_mode))
    {
        m_idle_time = 0;

        if (m_tacho_timer > kTachoPlayMax)
        {
            out_mute(true);
            out_rec_en(false);

            m_target_mode = mech::Mode::Stop;
            m_last_play_dir = mech::Direction::Fwd;
            m_user_mode = mech::UserMode::Stop;

            if (m_features.reverse_enable)
            {
                if (m_features.auto_reverse)
                {
                    if (m_mode == mech::Mode::PbFwd)
                    {
                        m_user_mode = mech::UserMode::PlayRev;
                    }
                    else if (m_mode == mech::Mode::PbRev)
                    {
                        if (m_features.play_loop)
                            m_user_mode = mech::UserMode::PlayFwd;
                    }
                    else if (m_mode == mech::Mode::RcFwd)
                    {
                        if (!m_sensors.no_rec_rev)
                            m_user_mode = mech::UserMode::RecRev;
                        else if (m_features.pb_f2rew)
                            m_user_mode = mech::UserMode::FwindRev;
                        else
                            m_last_play_dir = mech::Direction::Rev;
                    }
                    else if ((m_mode == mech::Mode::RcRev) &&
                             m_features.play_loop &&
                             !m_sensors.no_rec_fwd)
                    {
                        m_user_mode = mech::UserMode::RecFwd;
                    }
                }
                else
                {
                    if ((m_mode == mech::Mode::PbFwd) ||
                        (m_mode == mech::Mode::RcFwd))
                    {
                        if (m_features.pb_f2rew)
                            m_user_mode = mech::UserMode::FwindRev;
                        else
                            m_last_play_dir = mech::Direction::Rev;
                    }
                }
            }
            else if (m_features.pb_f2rew)
            {
                m_user_mode = mech::UserMode::FwindRev;
            }
        }
        else
        {
            out_mute(false);

            if (target_rec(m_mode))
                out_rec_en(true);
        }

        // Механизм неожиданно оказался в HOME.
        if (m_sensors.home)
        {
            m_mode = mech::Mode::Stop;
            m_target_mode = mech::Mode::Stop;
            m_trans_timer = kTimWaitStop;
            m_user_mode = mech::UserMode::Stop;
        }
    }
    else if (is_fast_wind_mode(m_mode))
    {
        out_mute(true);
        out_rec_en(false);
        m_idle_time = 0;

        if (m_tacho_timer > kTachoFwindMax)
        {
            m_target_mode = mech::Mode::Stop;
            m_last_play_dir = mech::Direction::Fwd;
            m_user_mode = mech::UserMode::Stop;

            if ((m_mode == mech::Mode::FwFwd) ||
                (m_mode == mech::Mode::FwFwdHdRev))
            {
                if (m_features.ff_2rew)
                    m_user_mode = mech::UserMode::FwindRev;
                else if (m_features.reverse_enable)
                    m_last_play_dir = mech::Direction::Rev;
            }
        }

        if (m_sensors.home)
        {
            m_mode = mech::Mode::Stop;
            m_target_mode = mech::Mode::Stop;
            m_trans_timer = kTimWaitStop;
            m_user_mode = mech::UserMode::Stop;
        }
    }
}

void Crp42602y::cyclogram()
{
    switch (m_mode)
    {
    case mech::Mode::SubInit:
        out_capstan(true);
        out_solenoid(false);
        out_mute(true);
        out_rec_en(false);

        if (m_trans_timer == 0)
            m_mode = m_target_mode;
        break;

    case mech::Mode::SubToStop:
        out_capstan(true);
        out_solenoid(true);

        if (m_trans_timer <= (kTimWaitStop - kTimStop))
        {
            out_solenoid(false);
            m_mode = mech::Mode::SubWaitStop;
        }
        break;

    case mech::Mode::SubWaitStop:
        out_solenoid(false);

        if (m_trans_timer == 0)
        {
            m_mode = mech::Mode::Stop;

            if (!m_sensors.home)
            {
                m_retries++;

                if (m_retries >= kModeRepMax)
                {
                    set_error(mech::Error::NoCtrl);
                }
                else
                {
                    m_trans_timer = kTimWaitStop;
                    m_mode = mech::Mode::SubToStop;
                }
            }
            else
            {
                m_retries = 0;
            }
        }
        break;

    case mech::Mode::SubToActive:
        out_capstan(true);
        out_solenoid(true);

        if ((m_target_mode == mech::Mode::PbFwd) ||
            (m_target_mode == mech::Mode::RcFwd))
            m_last_play_dir = mech::Direction::Fwd;
        else if ((m_target_mode == mech::Mode::PbRev) ||
                 (m_target_mode == mech::Mode::RcRev))
            m_last_play_dir = mech::Direction::Rev;

        m_mode = mech::Mode::SubAct;
        break;

    case mech::Mode::SubAct:
        if (m_trans_timer < (kTimActive - kTimWaitHead))
        {
            m_mode = mech::Mode::SubWaitDir;

            if (m_solenoid_state && target_head_rev(m_target_mode))
                out_solenoid(true);
            else
                out_solenoid(false);

            if (target_rec(m_target_mode))
                out_rec_en(true);
        }
        break;

    case mech::Mode::SubWaitDir:
        if (m_trans_timer < (kTimActive - kTimHeadDir))
        {
            m_mode = mech::Mode::SubHdDirSel;
            out_solenoid(target_head_rev(m_target_mode));
        }
        break;

    case mech::Mode::SubHdDirSel:
        if (m_trans_timer < (kTimActive - kTimWaitPinch))
        {
            m_mode = mech::Mode::SubWaitPinch;
            out_solenoid(false);
        }
        break;

    case mech::Mode::SubWaitPinch:
        if (m_trans_timer < (kTimActive - kTimPinchEn))
        {
            m_mode = mech::Mode::SubPinchSel;
            out_solenoid(target_play_rec(m_target_mode));
        }
        break;

    case mech::Mode::SubPinchSel:
        if (m_trans_timer < (kTimActive - kTimWaitTakeup))
        {
            m_mode = mech::Mode::SubWaitTakeup;

            if (m_solenoid_state && target_takeup_fwd(m_target_mode))
                out_solenoid(true);
            else
                out_solenoid(false);
        }
        break;

    case mech::Mode::SubWaitTakeup:
        if (m_trans_timer < (kTimActive - kTimTakeupDir))
        {
            m_mode = mech::Mode::SubTuDirSel;
            out_solenoid(target_takeup_fwd(m_target_mode));
        }
        break;

    case mech::Mode::SubTuDirSel:
        if (m_trans_timer < (kTimActive - kTimWaitMode))
        {
            m_mode = mech::Mode::SubWaitRun;
            out_solenoid(false);
        }
        break;

    case mech::Mode::SubWaitRun:
        if (m_trans_timer == 0)
        {
            out_solenoid(false);
            m_mode = m_target_mode;

            if (m_sensors.home)
            {
                set_error(mech::Error::NoCtrl);
                m_last_play_dir = mech::Direction::Fwd;
            }
            else
            {
                if (is_play_rec_mode(m_mode))
                    out_mute(false);

                m_retries = 0;
            }
        }
        break;

    case mech::Mode::SubToHalt:
        out_capstan(true);
        out_solenoid(true);
        m_mode = mech::Mode::Halt;
        break;

    case mech::Mode::Halt:
        if (m_trans_timer <= (kTimWaitStop - kTimStop))
            out_solenoid(false);
        break;

    default:
        break;
    }
}

bool Crp42602y::request(mech::UserMode mode)
{
    if ((m_mode == mech::Mode::Halt) || (m_target_mode == mech::Mode::Halt) ||
        (m_mode == mech::Mode::SubToHalt))
        return false;

    // Выход из записи разрешён только через STOP.
    const bool rec_requested = (m_user_mode == mech::UserMode::RecFwd) ||
                               (m_user_mode == mech::UserMode::RecRev);

    if ((is_rec_mode(m_mode) || is_rec_mode(m_target_mode) || rec_requested) &&
        (mode != mech::UserMode::Stop))
        return false;

    // Запись разрешена только из стабильного STOP.
    if ((mode == mech::UserMode::RecFwd) || (mode == mech::UserMode::RecRev))
    {
        if ((m_mode != mech::Mode::Stop) || (m_target_mode != mech::Mode::Stop) ||
            (m_trans_timer != 0))
            return false;

        if ((mode == mech::UserMode::RecFwd) && m_sensors.no_rec_fwd)
            return false;

        if ((mode == mech::UserMode::RecRev) && m_sensors.no_rec_rev)
            return false;
    }

    m_user_mode = mode;
    return true;
}
