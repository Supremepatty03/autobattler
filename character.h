#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H

#include <Qstring>
#include <QDebug>
#include "weapons.h"
#include <map>
#include <iostream>
#include <random>

//#include "monsters.h"

struct MonsterTrait;

class Character;

struct BattleContext {
    int turn = 1;                         // текущий ход в бою (управляется классом Battle)
    int damage = 0;                       // вычисляемый итоговый урон
    WeaponType damageType = WeaponType::Stabbing; // тип урона
    Character* attacker = nullptr;        // кто атакует
    Character* defender = nullptr;        // кто защищается
    std::vector<QString> logs;

};

class Character
{
public:
    Character(QString n, int str, int agi, int endu, int hpBase)
        : name(std::move(n)),
        strength(str), agility(agi), endurance(endu),
        hp(hpBase), maxHp(hpBase), level(1) {}

    virtual ~Character() = default;
    virtual void attack(Character& target, BattleContext& ctx) = 0;
    virtual void takeDamage(int damage) { hp -= damage; if (hp < 0) hp = 0;}
    virtual bool isAlive() { return (hp > 0); }
    virtual void defend (BattleContext& ctx) { takeDamage(ctx.damage); }

    void setWeapon(const QString name) {this->currentWeapon = createWeapon(name); }
    int getWeaponDamage () {return (currentWeapon->damage());}
    WeaponType getWeaponType () {return (currentWeapon->type());}
    QString getWeaponName () {return (currentWeapon->name());}

    void addAgility (const int amount) {this->agility+=amount;}
    int getAgility() {return this->agility;}

    void addStrength (const int amount) {this->strength+=amount;}
    int getStrength() {return this->strength;}

    void addEndurance (const int amount) {this->endurance+=amount;}
    int getEndurance() {return this->endurance;}

    void healFull() { hp = maxHp; }


    // --- Здоровье ---
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    void setMaxHp(int newHp) { maxHp = newHp; hp = maxHp; }

    // --- Общие данные ---
    QString getName() const { return name; }
    int getLevel() const { return level; }
    void levelUp() { ++level; }

protected:
    QString name;

private:
    int strength;
    int agility;
    int endurance;
    int hp;
    int maxHp;
    int level;

    std::unique_ptr<Weapon> currentWeapon;
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
            ctx.logs.emplace_back(QString("Скрытая атака! +1 урон"));
        }
        // яд на 3 уровне
        if (level >= 3) {
            int extraDMG = ctx.turn - 1;
            ctx.damage += extraDMG;
            ctx.logs.emplace_back(QString("Яд: +%1 урона на этом ходе").arg(extraDMG));
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
            ctx.logs.emplace_back(QString("Двойной урон от оружия в первый ход!\n"));
        }
    }
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx, int level) override {
        // щит на 2 уровне
        if (level >=2 && defender.getStrength() > attacker.getStrength()){
            ctx.damage = std::max(0, ctx.damage-3);
            ctx.logs.emplace_back(QString("Сила атакующего меньше силы героя"));
            ctx.logs.emplace_back(QString("Сработал щит! -3 к урону"));
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
            ctx.logs.emplace_back(QString("Ярость в действии! +2 урона\n"));
        }
        else if (level >= 1 && ctx.turn > 3) {
            ctx.damage -= 1;
            ctx.logs.emplace_back(QString("Герой устал! -1 урон\n"));
        }
    }
    void onDefense(Character& defender, Character& attacker, BattleContext& ctx, int level) override {
        // перк на 2 уровне
        if (level >=2){
            ctx.damage = std::max(0, ctx.damage - defender.getEndurance());
        }
    }
};

class Player : public Character {
public:
    explicit Player(std::unique_ptr<CharacterClassBase> cls)
        : Character("Игрок", 0, 0, 0, 0)
    {
        QString chosenName = cls->name();
        addClass(std::move(cls));
        qDebug() << "Начальное хп:" << getHp();
        addOtherClassesZero(chosenName);
    }

    // Отключаем копирование
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    // --- Основная логика боя ---
    // attack заполняет ctx и применяет эффекты классов атакующего
    void attack(Character& target, BattleContext& ctx) override {
        ctx.attacker = this;
        ctx.defender = &target;
        ctx.damage = getWeaponDamage() + getStrength();
        ctx.damageType = getWeaponType();

        for (const auto& cls : classes_) {
            if (!cls) continue;
            int lvl = getClassLevel(cls->name());
            cls->onAttack(*this, target, ctx, lvl);
        }
    }

    // defend применяет эффекты onDefense классов, затем наносит урон
    virtual void defend (BattleContext& ctx) override {
        for (const auto& cls : classes_) {
            if (!cls) continue;
            int lvl = getClassLevel(cls->name());
            cls->onDefense(*this, *ctx.attacker, ctx, lvl);
        }
        takeDamage(ctx.damage);
    }

    // --- Работа с классами  ---
    // Добавляет класс (уровень 1) и применяет бонус первого уровня
    void addClass(std::unique_ptr<CharacterClassBase> cls) {
        if (!cls) return;
        QString cname = cls->name();
        if (hasClass(cname)) return;
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 3);

        addStrength(dist(rng));
        addAgility(dist(rng));
        addEndurance(dist(rng));

        classLevels_[cname] = 1;
        classes_.push_back(std::move(cls));

        // применяем начальные бонусы и выдаём стартовое оружие
        CharacterClassBase* raw = classes_.back().get();
        if (raw) {
            raw->applyLevelBonus(*this, 1);
            auto sw = raw->startingWeapon();
            if (sw) {
                setWeapon(sw->name());
            }
            setMaxHp(getMaxHp() + raw->healthPerLevel() + getEndurance());
            healFull();
        }
    }

    // Повысить уровень у выбранного класса по имени (или добавить, если нет)
    // Возвращает новый уровень или 0 при ошибке
    int levelUpClass(const QString& className) {
        // найти класс-объект
        auto it = std::find_if(classes_.begin(), classes_.end(),
                               [&](const std::unique_ptr<CharacterClassBase>& p) {
                                   return p && p->name() == className;
                               });
        if (it == classes_.end()) {
            // класс не найден
            return 0;
        }
        CharacterClassBase* cls = it->get();

        if (classLevels_[className]<maxClassLevel_){
            int newLevel = ++classLevels_[className];
            // бонусы уровня
            cls->applyLevelBonus(*this, newLevel);
            // здоровье за уровень
            setMaxHp(getMaxHp() + cls->healthPerLevel());
            healFull();
            return newLevel;
        }
        else return 0;
    }

    void levelUp() {
        if (classes_.empty()) {
            Character::levelUp();
            return;
        }
        QString cname = classes_.front()->name();
        levelUpClass(cname);
        Character::levelUp();
    }
    void addOtherClassesZero(const QString& chosenClassName)
    {
        // для добавления класса, если его ещё нет и он не выбран
        auto addIfNeeded = [&](std::unique_ptr<CharacterClassBase> cls) {
            if (!cls) return;
            QString name = cls->name();
            if (name == chosenClassName) return;      // пропускаем выбранный
            if (classLevels_.find(name) != classLevels_.end()) return; // уже есть — пропускаем

            // добавление класса с уровнем 0
            classLevels_[name] = 0;
            classes_.push_back(std::move(cls));
        };
        addIfNeeded(std::make_unique<WarriorClass>());
        addIfNeeded(std::make_unique<RogueClass>());
        addIfNeeded(std::make_unique<BarbarianClass>());
    }

    int getClassLevel(const QString& className) const {
        auto it = classLevels_.find(className);
        return (it == classLevels_.end()) ? 0 : it->second;
    }

    bool hasClass(const QString& className) const {
        return getClassLevel(className) > 0;
    }

    QString getClassSummary() const {
        QString res;
        for (const auto& cls : classes_) {
            if (!cls) continue;
            QString name = cls->name();
            int lvl = getClassLevel(name);
            res += name + " (уровень " + QString::number(lvl) + ")  ";
        }
        return res.trimmed();
    }

    // Доступ к списку классов (только чтение)
    const std::vector<std::unique_ptr<CharacterClassBase>>& getClasses() const {
        return classes_;
    }
    int getMaxClassLevel() {return maxClassLevel_;}
private:
    // vector классов плюс map уровней по имени
    std::vector<std::unique_ptr<CharacterClassBase>> classes_;
    std::map<QString, int> classLevels_;
    int maxClassLevel_ = 3;
};

#endif // CHARACTER_H
