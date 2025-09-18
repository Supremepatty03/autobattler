#ifndef WEAPONS_H
#define WEAPONS_H

#include <Qstring>

enum class WeaponType { Stabbing, Chopping, Crushing };

struct Weapon
{
    virtual ~Weapon() = default;

    virtual QString name() const = 0;
    int damage() const { return m_damage; }
    WeaponType type() const { return m_type; }

protected:
    Weapon(int damage, WeaponType type)
        : m_damage(damage)
        , m_type(type)
    {}

private:
    int m_damage;
    WeaponType m_type;
};

struct Sword : public Weapon
{
    Sword(int damage = 3)
        : Weapon(damage, WeaponType::Chopping)
    {}
    QString name() const override { return "Меч"; }
};

struct Club : public Weapon
{
    Club(int damage = 3)
        : Weapon(damage, WeaponType::Crushing)
    {}
    QString name() const override { return "Дубина"; }
};

struct Dagger : public Weapon
{
    Dagger(int damage = 2)
        : Weapon(damage, WeaponType::Stabbing)
    {}
    QString name() const override { return "Кинжал"; }
};

struct Axe : public Weapon
{
    Axe(int damage = 4)
        : Weapon(damage, WeaponType::Chopping)
    {}
    QString name() const override { return "Топор"; }
};

struct Spear : public Weapon
{
    Spear(int damage = 3)
        : Weapon(damage, WeaponType::Stabbing)
    {}
    QString name() const override { return "Копье"; }
};

struct LegendarySword : public Sword
{
    LegendarySword()
        : Sword(10)
    {}
    QString name() const override { return "Легендарный Меч"; }
};

std::unique_ptr<Weapon> createWeapon(const QString &name);

#endif // WEAPONS_H
