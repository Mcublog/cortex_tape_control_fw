#ifndef ITIMER_H
#define ITIMER_H

typedef void (*timer_systick_callback_t)(void);

#ifdef __cplusplus
extern "C" {
#endif

    void timer_init(timer_systick_callback_t systick_callback);

    void timer_irq_enable(void);
    void timer_irq_disable(void);

#ifdef __cplusplus
}
#endif


#endif // ITIMER_H