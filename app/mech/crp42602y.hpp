
#ifndef APP_MECH_CRP42602Y_HPP
#define APP_MECH_CRP42602Y_HPP

#include <cstdint>

#include "app/interfaces/MechIo.hpp"
#include "app/mech/types.hpp"

/**
 * @brief Машина состояний механизма CRP42602Y (M02753900D).
 *
 * Реализация портирована из Fagear/AVRTapeControl mech_crp42602y.c.
 * Тайм-база: tick2ms() вызывается каждые 2 мс, tick20ms() — каждые 20 мс.
 */
class Crp42602y
{
  public:
    explicit Crp42602y(MechIo &io);

    void init();

    void tick2ms();
    void tick20ms();

    /**
     * @brief Запросить новый пользовательский режим.
     *
     * Повторный запрос перемотки в текущем направлении разворачивает его
     * на противоположное. Если лента уже кончилась в запрошенном направлении
     * (был автостоп по таймауту тахометра), запрос play или перемотки в эту
     * сторону подменяется на противоположный.
     *
     * @return false, если запрос запрещён (HALT, защита записи и т.п.)
     */
    bool request(mech::UserMode mode);

    void set_features(const mech::Features &features);
    const mech::Features &features() const;

    mech::UserMode user_mode() const;
    mech::Mode mode() const;
    mech::Mode target_mode() const;
    mech::Direction last_play_dir() const;
    uint8_t transition_left() const;
    uint8_t retries() const;
    uint8_t error() const;
    mech::Sensors sensors() const;
    bool capstan_on() const;

    static bool is_active_mode(mech::Mode mode);
    static bool is_play_rec_mode(mech::Mode mode);
    static bool is_fast_wind_mode(mech::Mode mode);
    static bool is_rec_mode(mech::Mode mode);

  private:
    // Выходы с кэшированием состояния.
    void out_solenoid(bool on);
    void out_capstan(bool on);
    void out_mute(bool on);
    void out_rec_en(bool on);

    void sample_sensors();

    void state_machine();
    void cyclogram();
    void static_mode();
    void static_halt();
    void target2mode();
    void user2target();

    void set_error(mech::Error err);
    mech::Mode user_to_transport(mech::UserMode mode,
                                 mech::Direction dir) const;

    /**
     * @brief Проверить, что перемотка в заданном направлении уже идёт или запрошена.
     *
     * Приоритет проверки: последний запрос пользователя, затем целевая мода,
     * затем текущая мода. Благодаря этому повторное нажатие во время перехода
     * возвращает прежнее направление, а не игнорируется.
     *
     * @param mode запрошенный пользовательский режим перемотки
     */
    bool wind_active(mech::UserMode mode) const;

    /**
     * @brief Развернуть запрос play или перемотки, если лента кончилась в эту сторону.
     *
     * @param mode запрошенный пользовательский режим
     * @return противоположное направление, если оно уже упиралось в конец ленты,
     * иначе исходный запрос
     */
    mech::UserMode apply_tape_end(mech::UserMode mode) const;

    /**
     * @brief Развернуть повторный запрос перемотки на противоположное направление.
     *
     * @param mode запрошенный пользовательский режим
     * @return противоположная перемотка, если в текущем направлении уже мотаем,
     * иначе исходный запрос
     */
    mech::UserMode apply_wind_toggle(mech::UserMode mode) const;

    MechIo &m_io;

    mech::Features m_features;
    mech::Sensors m_sensors;

    mech::UserMode m_user_mode = mech::UserMode::Stop;
    mech::Mode m_mode = mech::Mode::Stop;
    mech::Mode m_target_mode = mech::Mode::ToInit;
    mech::Direction m_last_play_dir = mech::Direction::Fwd;
    mech::Direction m_eof_dir = mech::Direction::Fwd;

    // Лента кончилась в направлении m_eof_dir (автостоп по таймауту тахометра).
    bool m_eof = false;

    uint8_t m_trans_timer = 0;
    uint16_t m_idle_time = 0;
    uint16_t m_tacho_timer = 0;
    uint8_t m_retries = 0;
    uint8_t m_error = 0;

    bool m_tacho_level = false;

    bool m_solenoid_state = false;
    bool m_capstan_state = false;
    bool m_mute_state = false;
    bool m_rec_en_state = false;
};

#endif // APP_MECH_CRP42602Y_HPP
