#pragma once
#ifndef MONSTERS_H
#define MONSTERS_H

#include "character.h"

struct MonsterTrait {
    virtual ~MonsterTrait() = default;
    virtual void onAttack(Character& attacker, Character& target, BattleContext& ctx) {}
    virtual void onDefense(Character& defender, Character& attacker, BattleContext& ctx) {}
};

class Monster : public Character {
public:
    Monster(QString n, int str, int agi, int endu, int hpBase, QString dropWeaponName)
        : Character(std::move(n), str, agi, endu, hpBase),
        dropWeapon(std::move(dropWeaponName)) {}

    virtual ~Monster() = default;

    QString getDropWeaponName() const { return dropWeapon; }

    // В базовой реализации attack() — собираем ctx, вызываем трейты атакующего, затем
    // даём цельной стороне (defender) обработать ctx (включая её трейты), и затем применяем урон.
    // Но чтобы обеспечить гибкость, мы реализуем логику в менеджере боя (Battle).


    const std::vector<std::unique_ptr<MonsterTrait>>& getTraits() const {
        return traits;
    }

    std::vector<std::unique_ptr<MonsterTrait>>& getTraits() {
        return traits;
    }

protected:
    QString dropWeapon;
    std::vector<std::unique_ptr<MonsterTrait>> traits;
};

// ---------------------------
// Конкретные трейты и монстры
// ---------------------------

// Goblin — без особенностей
class Goblin : public Monster {
public:
    Goblin() : Monster("Гоблин", 1, 1, 1, 5, "Кинжал") {
        setWeapon("Кинжал");
    }

    void attack(Character& target, BattleContext& ctx) override {
        ctx.attacker = this;
        ctx.defender = &target;
        ctx.damage = getWeaponDamage() + getStrength();
        ctx.damageType = getWeaponType();
        // трейты атакующего монстра (у гоблина нет) — вызываются извне менеджером боя
    }
};

// 2) Skeleton — x2 урон от дробящего (Crushing)
class SkeletonWeakness : public MonsterTrait {
public:
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx) override {
        if (ctx.damageType == WeaponType::Crushing) {
            ctx.damage *= 2;
           // std::cout << defender.getName().toStdString() << " получает двойной урон от дробящего!\n";
        }
    }
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Скелет", 2, 2, 1, 10, "Дубина") {
        setWeapon("Дубина");
        traits.push_back(std::make_unique<SkeletonWeakness>());
    }

    void attack(Character& target, BattleContext& ctx) override {
        ctx.attacker = this;
        ctx.defender = &target;
        ctx.damage = getWeaponDamage() + getStrength();
        ctx.damageType = getWeaponType();
    }
};

// 3) Slime — иммунитет к рубящему
class SlimeImmunity : public MonsterTrait {
public:
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx) override {
        if (ctx.damageType == WeaponType::Chopping) {
            ctx.damage = 0;
         //   std::cout << defender.getName().toStdString() << " полностью иммунен к рубящему урону!\n";
        }
    }
};

class Slime : public Monster {
public:
    Slime() : Monster("Слайм", 1, 1, 2, 8, "Копье") {
        setWeapon("Копье");
        traits.push_back(std::make_unique<SlimeImmunity>());
    }

    void attack(Character& target, BattleContext& ctx) override {
        ctx.attacker = this;
        ctx.defender = &target;
        ctx.damage = getWeaponDamage() + getStrength();
        ctx.damageType = getWeaponType();
    }
};

// 4) Ghost — скрытая атака
class GhostSneakAttack : public MonsterTrait {
public:
    void onAttack(Character& attacker, Character& target, BattleContext& ctx) override {
        if (attacker.getAgility() > target.getAgility()) {
            ctx.damage += 1;
           // std::cout << attacker.getName().toStdString() << " наносит скрытую атаку! +1 урон.\n";
        }
    }
};

class Ghost : public Monster {
public:
    Ghost() : Monster("Призрак", 1, 3, 1, 6, "Меч") {
        setWeapon("Меч");
        traits.push_back(std::make_unique<GhostSneakAttack>());
    }

    void attack(Character& target, BattleContext& ctx) override {
        ctx.attacker = this;
        ctx.defender = &target;
        ctx.damage = getWeaponDamage() + getStrength();
        ctx.damageType = getWeaponType();
    }
};

// 5) Golem — "Каменная кожа"
class GolemStoneSkin : public MonsterTrait {
public:
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx) override {
        const int red = defender.getEndurance();
        ctx.damage = std::max(0, ctx.damage - red);
      //  std::cout << defender.getName().toStdString() << " снижает урон каменной кожей на " << red << "\n";
    }
};

class Golem : public Monster {
public:
    Golem() : Monster("Голем", 3, 1, 3, 10, "Топор") {
        setWeapon("Топор");
        traits.push_back(std::make_unique<GolemStoneSkin>());
    }

    void attack(Character& target, BattleContext& ctx) override {
        ctx.attacker = this;
        ctx.defender = &target;
        ctx.damage = getWeaponDamage() + getStrength();
        ctx.damageType = getWeaponType();
    }
};

// 6) Dragon — каждые 3 хода дышит огнём (+3 урона)
class DragonFireBreath : public MonsterTrait {
public:
    void onAttack(Character& attacker, Character& target, BattleContext& ctx) override {
        if (ctx.turn % 3 == 0) {
            ctx.damage += 3;
         //   std::cout << attacker.getName().toStdString() << " дышит огнём! +3 урона.\n";
        }
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Дракон", 3, 3, 3, 20, "Легендарный Меч") {
        setWeapon("Легендарный Меч");
        traits.push_back(std::make_unique<DragonFireBreath>());
    }

    void attack(Character& target, BattleContext& ctx) override {
        ctx.attacker = this;
        ctx.defender = &target;
        ctx.damage = getWeaponDamage() + getStrength();
        ctx.damageType = getWeaponType();
        // turn учитывается в ctx.turn, трейты используют его
    }
};
#endif // MONSTERS_H
