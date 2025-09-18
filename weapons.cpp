#include "weapons.h"

std::unique_ptr<Weapon> createWeapon(const QString &name)
{
    if (name == "Меч")
        return std::make_unique<Sword>();
    if (name == "Дубина")
        return std::make_unique<Club>();
    if (name == "Кинжал")
        return std::make_unique<Dagger>();
    if (name == "Топор")
        return std::make_unique<Axe>();
    if (name == "Копье")
        return std::make_unique<Spear>();
    if (name == "Легендарный Меч")
        return std::make_unique<LegendarySword>();
    return nullptr;
}
