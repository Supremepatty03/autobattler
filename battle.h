#pragma once
#ifndef BATTLE_H
#define BATTLE_H

#include <random>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cassert>
#include "character.h"
#include "monsters.h"



enum class BattleResult { AttackerWon, DefenderWon };


class Battle
{
public:
    Battle()
        : rng(std::random_device{}())
    {}

    bool run(Character& player, Character& monster, bool logOutput = true, std::function<void(const QString&)> logger = {}){
        auto logMsg = [&](const QString &s)
        {
            if (logger) {
                logger(s);
            } else if (logOutput) {
                // fallback в консоль
                std::cout << s.toStdString() << std::endl;
            }
        };
        Character* attacker = &player;
        Character* defender = &monster;
        if (monster.getAgility() > player.getAgility()){
            attacker = &monster;
            defender = &player;
        }
        int turnCounter = 1;

        if (logOutput || logger) {
            logMsg(QString("Бой: %1 vs %2").arg(player.getName()).arg(monster.getName()));
            logMsg(QString("Первый ход: %1").arg(attacker->getName()));
        }

        while (player.isAlive() && monster.isAlive()){
            if (logOutput || logger) {
                logMsg(QString("=== Ход %1 — атакует: %2 ===")
                           .arg(turnCounter).arg(attacker->getName()));
            }
            // 1) шанс попадания
            int atkAgi = attacker->getAgility();
            int defAgi = defender->getAgility();
            int sumAgi = std::max(1, atkAgi + defAgi); // защита от нуля
            std::uniform_int_distribution<int> dist(1, sumAgi);
            int roll = dist(rng);
            bool miss = (roll <= defAgi);
            if (logOutput || logger) {
                logMsg(QString("%1 бросок попадания: %2 (порог уклонения цели=%3)")
                           .arg(attacker->getName()).arg(roll).arg(defAgi));
            }

            if (miss) {
                if (logOutput) std::cout << "Промах! Ход переходит.\n";
            }
            else // 2. Попадание
            {
                BattleContext ctx;
                ctx.turn = turnCounter;
                ctx.attacker = attacker;
                ctx.defender = defender;
                ctx.damage = attacker->getWeaponDamage() + attacker->getStrength();
                ctx.damageType = attacker->getWeaponType();

                if (logOutput || logger) {
                    logMsg(QString("Попадание! Начальный урон: %1 (оружие=%2)")
                               .arg(ctx.damage).arg(static_cast<int>(ctx.damageType)));
                }

                // 3. Эффекты атакующего
                if (auto* m = dynamic_cast<Monster*>(attacker)) {
                    for (auto& t : m->getTraits()) {
                        t->onAttack(*attacker, *defender, ctx);
                    }
                }

                // 4. Эффекты защитника
                if (auto* m = dynamic_cast<Monster*>(defender)) {
                    for (auto& t : m->getTraits()) {
                        t->onDefense(*defender, *attacker, ctx);
                    }
                }

                if (logOutput || logger) {
                    logMsg(QString("Итоговый урон после модификаторов: %1").arg(ctx.damage));
                }

                // 5. Применение урона
                if (ctx.damage > 0) {
                    defender->defend(ctx);
                    if (logOutput || logger) {
                        logMsg(QString("%1 получил %2 урона. HP=%3/%4")
                                   .arg(defender->getName())
                                   .arg(ctx.damage)
                                   .arg(defender->getHp())
                                   .arg(defender->getMaxHp()));
                    }
                }

                // 6. Проверка смерти
                if (!defender->isAlive()) {
                    if (logOutput || logger) {
                        logMsg(QString("%1 пал! Победил %2").arg(defender->getName()).arg(attacker->getName()));
                    }
                    return (attacker == &player);
                }
            }

            // 7. Смена ролей
            std::swap(attacker, defender);
            ++turnCounter;
        }

        return player.isAlive();
    }

private:
    std::mt19937 rng;
};

#endif // BATTLE_H
