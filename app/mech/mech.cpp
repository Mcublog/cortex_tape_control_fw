/**
 * @file mech.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Синглтон механизма.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include "mech.hpp"

namespace
{
Crp42602y *g_mech = nullptr;
}

void Mech::init(MechIo &io)
{
    static Crp42602y instance(io);
    g_mech = &instance;
    g_mech->init();
}

bool Mech::ready()
{
    return g_mech != nullptr;
}

Crp42602y &Mech::instance()
{
    return *g_mech;
}
