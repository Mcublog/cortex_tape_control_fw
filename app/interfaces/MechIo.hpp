
#ifndef APP_INTERFACES_MECH_IO_HPP
#define APP_INTERFACES_MECH_IO_HPP

/**
 * @brief Низкоуровневый интерфейс к лентопротяжному механизму.
 *
 * Геттеры датчиков возвращают логическое состояние: true — датчик активен.
 * Выходы также логические: true — сигнал активен.
 */
class MechIo
{
  public:
    virtual ~MechIo() = default;

    virtual void solenoid(bool on) = 0;
    virtual void capstan(bool on) = 0;
    virtual void mute(bool on) = 0;
    virtual void rec_en(bool on) = 0;

    virtual bool home() = 0;
    virtual bool tacho() = 0;
    virtual bool tape_in() = 0;
    virtual bool no_rec_fwd() = 0;
    virtual bool no_rec_rev() = 0;
};

#endif // APP_INTERFACES_MECH_IO_HPP
