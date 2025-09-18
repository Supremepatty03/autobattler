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

    void setWeapon(const QString name) {this->currentWeapon = createWeapon(name); }
    int getWeaponDamage () {return (currentWeapon->damage());}

    void addAgility (const int amount) {this->agility+=amount;}
    int getAgility() {return this->agility;}

    void addStrength (const int amount) {this->strength+=amount;}
    int getStrength() {return this->strength;}

    void addEndurance (const int amount) {this->endurance+=amount;}
    int getEndurance() {return this->endurance;}

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
    virtual std::unique_ptr<Weapon> startingWeapon() const = 0;

    virtual void applyLevelBonus(Character &c, int level) = 0;
    virtual void onAttack(Character &attacker, Character &target, BattleContext &ctx, int level) {}
    virtual void onDefense(Character& defender, Character& attacker, BattleContext& ctx, int level) {}
};

struct RogueClass : public CharacterClassBase
{
    QString name() const override { return ("Разбойник"); }
    int healthPerLevel() const override { return 4; }
    std::unique_ptr<Weapon> startingWeapon() const override { return (createWeapon("Кинжал")); }

    void applyLevelBonus(Character &c, int level) override
    {
        if (level == 2) {
            c.addAgility(1);
        }
    }

    void onAttack (Character &attacker, Character &target, BattleContext &ctx, int level) override
    {
        // скрытая атака на 1 уровне
        if (level >= 1 && attacker.getAgility() > target.getAgility()) {
            ctx.damage += 1;
           // std::cout << "Скрытая атака! +1 урон\n";                                TODO: ADD LOGGING TO COUT
        }
        // яд на 3 уровне
        if (level >= 3) {
            ctx.damage += (ctx.turn - 1);
            // std::cout << "Яд! +" << (ctx.turn - 1) << " урона\n";
        }
    }
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx, int level) override {
        return; // модификаторы защиты отсутствуют.
    }
};
struct WarriorClass : public CharacterClassBase{
    QString name() const override { return ("Воин"); }
    int healthPerLevel() const override { return 5; }
    std::unique_ptr<Weapon> startingWeapon() const override { return (createWeapon("Меч")); }

    void applyLevelBonus(Character &c, int level) override
    {
        if (level == 3) { c.addStrength(1); }
    }
    void onAttack(Character &attacker, Character &target, BattleContext &ctx, int level) override
    {
        // x2 атака в первый ход
        if (level >= 1 && ctx.turn == 1) {
            ctx.damage += attacker.getWeaponDamage();
            // std::cout << "Двойной урон от оружия!\n";                                TODO: ADD LOGGING TO COUT
        }
    }
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx, int level) override {
        // щит на 2 уровне
        if (level >=2 && defender.getStrength() > attacker.getStrength()){
            ctx.damage = std::max(0, ctx.damage-3);
        }
    }
};

struct BarbarianClass : public CharacterClassBase{
    QString name() const override { return ("Варвар"); }
    int healthPerLevel() const override { return 6; }
    std::unique_ptr<Weapon> startingWeapon() const override { return (createWeapon("Дубина")); }

    void applyLevelBonus(Character &c, int level) override
    {
        if (level == 3) { c.addEndurance(1); }
    }
    void onAttack(Character &attacker, Character &target, BattleContext &ctx, int level) override
    {
        // Ярость
        if (level >= 1 && ctx.turn <= 3) {
            ctx.damage += 2;
            // std::cout << "Ярость в действии! +2 урона\n";                                TODO: ADD LOGGING TO COUT
        }
        else if (level >= 1 && ctx.turn > 3) {
            ctx.damage -= 1;
            // std::cout << "Герой устал! -1 урон\n";
        }
    }
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx, int level) override {
        // перк на 2 уровне
        if (level >=2){
            ctx.damage = std::max(0, ctx.damage - defender.getEndurance());
        }
    }
};

class Player : public Character
{
    std::map<std::unique_ptr<CharacterClassBase>, int> classLevels;
};

#endif // CHARACTER_H
