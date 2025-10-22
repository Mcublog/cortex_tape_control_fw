/**
 * @file io_mock.c
 * @author Viacheslav (Viacheslav@mcublog.ru)
 * @brief IO mocking module
 * @version 0.1
 * @date 2022-09-26
 *
 * @copyright Mcublog Copyright (c) 2022
 *
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "app/utils/idelay.h"
#include "io_mock.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME iomock
#define LOG_MODULE_LEVEL (3)
#include "log_libs.h"
//<<----------------------

//>>---------------------- Locals
#define IOMOCK_EDGE_DETECTING_PERIOD_MS (10)
#define IOMOCK_EDGE_DETECTING_ERROR_PERIOD_MS (1000)

sem_t m_mutex;
//<<----------------------

//>>---------------------- Public functions
/**
 * @brief Read bool value ('1'/'0') from file
 *
 * @param filename
 * @return true
 * @return false
 */
bool iomock_read_bool(const char *filename)
{
    bool result = false;
    sem_wait(&m_mutex);
    FILE *fp = fopen(filename, "r"); // read mode
    if (fp == NULL)
    {
        LOG_ERROR("Error while opening: %s", filename);
        sem_post(&m_mutex);
        return false;
    }
    char ch = 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        if ('1' == ch)
        {
            result = true;
            break;
        }
        else if ('0' == ch)
        {
            result = false;
            break;
        }
    }
    fclose(fp);
    sem_post(&m_mutex);
    return result;
}

/**
 * @brief Write bool state
 *
 * @param filename
 * @param value
 */
void iomock_write_bool(const char *filename, bool value)
{
    sem_wait(&m_mutex);
    bool result = false;
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        sem_post(&m_mutex);
        LOG_ERROR("Error while opening: %s", filename);
        return;
    }
    char ch = value ? '1' : '0';
    fputc(ch, fp);
    fclose(fp);
    sem_post(&m_mutex);
    return;
}

/**
 * @brief Write somthing data
 *
 * @param filename
 * @param data
 * @param size
 */
void iomock_write_data(const char *filename, const void *data, uint32_t size)
{
    sem_wait(&m_mutex);
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        LOG_ERROR("Error while opening: %s", filename);
        return;
    }
    fwrite(data, size, 1, fp);
    fclose(fp);
    sem_post(&m_mutex);
    return;
}

/**
 * @brief Read somthing
 *
 * @param filename
 * @param data
 * @param size
 */
int iomock_read_data(const char *filename, void *data, uint32_t size)
{
    sem_wait(&m_mutex);
    int ret = -1;
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        LOG_ERROR("Error while opening: %s", filename);
        sem_post(&m_mutex);
        return ret;
    }
    if (fread(data, size, 1, fp) != 1)
    {
        if (feof(fp))
            LOG_ERROR("Premature end of file: %s", filename);
        else
            LOG_ERROR("File %s read error", filename);
    }
    else
        ret = 0;
    fclose(fp);
    sem_post(&m_mutex);
    return ret;
}

/**
 * @brief Edge detection function
 *
 * @param vargp pointer to iomock_handlers_t struct
 * @return void*
 */
void *iomock_edge_detecting(void *vargp)
{
    iomock_handlers_t *mock_handlers = (iomock_handlers_t *)(vargp);
    bool old_state, new_state;
    old_state = new_state = mock_handlers->state_reading();
    while (1)
    {
        if (mock_handlers == NULL)
        {
            LOG_ERROR("mock_handlers is NULL");
            delay_ms(IOMOCK_EDGE_DETECTING_ERROR_PERIOD_MS);
            continue;
        }
        new_state = mock_handlers->state_reading();
        if (old_state == false && new_state == true)
        {
            LOG_DEBUG("rising edge detected");
            mock_handlers->irg_handler(IOMOCK_IRQ_RISING);
        }
        else if (old_state == true && new_state == false)
        {
            LOG_DEBUG("falling edge detected");
            mock_handlers->irg_handler(IOMOCK_IRQ_FALLING);
        }
        old_state = new_state;
        delay_ms(IOMOCK_EDGE_DETECTING_PERIOD_MS);
    }
    return NULL;
}

/**
 * @brief
 *
 * @param filename
 * @return true
 * @return false
 */
bool iomock_file_is_exist(const char *filename)
{
    FILE *file;
    bool res = false;

    sem_wait(&m_mutex);
    if ((file = fopen(filename, "r")) == NULL)
    {
        if (errno == ENOENT)
        {
            LOG_INFO("File: %s doesn't exist", filename);
        }
        else
        {
            // Check for other errors too, like EACCES and EISDIR
            LOG_ERROR("Some other error occured");
        }
    }
    else
    {
        fclose(file);
        res = true;
    }
    sem_post(&m_mutex);
    return res;
}

/**
 * @brief Create file, if it already exists do nothing
 * But force_init is forcing init
 *
 * @param filename
 * @param init pointer to init data, optional
 * @param size in bytes, optional
 * @param force_init
 * @return true
 * @return false
 */
bool iomock_file_create(const char *filename, const void *init, const uint32_t size,
                        bool force_init)
{
    if (iomock_file_is_exist(filename))
    {
        LOG_INFO("File: %s already exists", filename);
        if (force_init == false)
            return true;
    }

    sem_wait(&m_mutex);
    FILE *fp = fopen(filename, "w+b");
    if (fp == NULL)
    {
        LOG_ERROR("Some other error occured");
        return true;
    }
    if (init != NULL && size != 0)
        fwrite(init, size, 1, fp);
    fclose(fp);
    sem_post(&m_mutex);
    return true;
}

/**
 * @brief Remove file
 *
 * @param filename
 * @return true
 * @return false
 */
bool iomock_file_remove(const char *filename)
{
    if (iomock_file_is_exist(filename) == false)
    {
        LOG_INFO("File: %s is not exist", filename);
        return true;
    }
    if (remove(filename) == 0)
        return true;
    LOG_ERROR("Do not remove; %s", filename);
    return false;
}

void iomock_init(void)
{
    sem_init(&m_mutex, 0, 1);
}
//<<----------------------
