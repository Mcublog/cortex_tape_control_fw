
#ifndef IO_MOCK_H
#define IO_MOCK_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    IOMOCK_IRQ_RISING = 0,
    IOMOCK_IRQ_FALLING
} iomock_irq_edge_t;

typedef void (*iomock_irq_handler_t)(iomock_irq_edge_t);
typedef bool (*iomock_reading_t)(void);

typedef struct
{
    iomock_irq_handler_t irg_handler;
    iomock_reading_t state_reading;
} iomock_handlers_t;

#ifdef __cplusplus
extern "C"
{
#endif

    void iomock_init(void);

    bool iomock_read_bool(const char *filename);
    void iomock_write_bool(const char *filename, bool value);
    void iomock_write_data(const char *filename, const void *data, uint32_t size);
    int iomock_read_data(const char *filename, void *data, uint32_t size);

    void *iomock_edge_detecting(void *vargp);

    bool iomock_file_is_exist(const char *filename);
    bool iomock_file_create(const char *filename, const void *init, const uint32_t size,
                        bool force_init);

    bool iomock_file_remove(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // IO_MOCK_H