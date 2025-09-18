#ifndef CHARACTER_H
#define CHARACTER_H

#include <Qstring>
#include "weapons.h"
#include <map>

class Character
{
public:
    Character();
    virtual void attack(Character &) = 0;
    virtual void takeDamage(int damage) { hp -= damage; }
    virtual bool isAlive() { return (hp > 0); }

    void setWeapon(const QString name) { currentWeapon = createWeapon(name); }

private:
    int strength;
    int agility;
    int endurance;
    int hp;
    int maxHp;
    int level;

    std::unique_ptr<Weapon> currentWeapon;
};

struct BattleContext
{
    int turn = 1;
    int damage = 0;
};

struct CharacterClassBase
{
    virtual ~CharacterClassBase() = default;

    virtual QString name() const = 0;
    virtual int healthPerLevel() const = 0;
    virtual Weapon *startingWeapon() const = 0;

    virtual void applyLevelBonus(Character &c, int level) = 0;
    virtual void onAttack(Character &attacker, Character &target, BattleContext &ctx, int level) {}
};

struct RogueClass : public CharacterClassBase
{
    QString name() const override { return ("Разбойник"); }
    int healthPerLevel() const override { return 4; }
    Weapon *startingWeapon() const override { return {"Кинжал", 2, WeaponType::Chopping}; }

    void applyLevelBonus(Character &c, int level) override
    {
        if (level == 2) {
            c.agility += 1;
        }
    }

    void onAttack(Character &attacker, Character &target, BattleContext &ctx, int level) override
    {
        // скрытая атака на 1 уровне
        if (level >= 1 && attacker.agility > target.agility) {
            ctx.damage += 1;
            std::cout << "Скрытая атака! +1 урон\n";
        }
        // яд на 3 уровне
        if (level >= 3) {
            ctx.damage += (ctx.turn - 1);
            std::cout << "Яд! +" << (ctx.turn - 1) << " урона\n";
        }
    }
};

class Player : public Character
{
    //std::map<CharacterClass, int> classLevels;
};

#endif // CHARACTER_H
