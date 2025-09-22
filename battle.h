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

    bool run(Character& player, Character& monster, bool logOutput = true){
        Character* attacker = &player;
        Character* defender = &monster;
        if (monster.getAgility() > player.getAgility()){
            attacker = &monster;
            defender = &player;
        }
        int turnCounter = 1;

        if (logOutput) {
            std::cout << "Бой: " << player.getName().toStdString()
                << " vs " << monster.getName().toStdString() << "\n";
            std::cout << "Первый ход: " << attacker->getName().toStdString() << "\n";
        }

        while (player.isAlive() && monster.isAlive()){
            if (logOutput) {
                std::cout << "=== Ход " << turnCounter << " — атакует: "
                          << attacker->getName().toStdString() << " ===\n";
            }
            // 1) шанс попадания
            int atkAgi = attacker->getAgility();
            int defAgi = defender->getAgility();
            int sumAgi = std::max(1, atkAgi + defAgi); // защита от нуля
            std::uniform_int_distribution<int> dist(1, sumAgi);
            int roll = dist(rng);
            bool miss = (roll <= defAgi);
            if (logOutput) {
                std::cout << attacker->getName().toStdString() << " бросок попадания: "
                          << roll << " (порог уклонения цели=" << defAgi << ")\n";
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

                if (logOutput) {
                    std::cout << "Попадание! Начальный урон: " << ctx.damage
                              << " (оружие=" << static_cast<int>(ctx.damageType) << ")\n";
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

                if (logOutput) {
                    std::cout << "Итоговый урон после модификаторов: " << ctx.damage << "\n";
                }

                // 5. Применение урона
                if (ctx.damage > 0) {
                    defender->defend(ctx);
                    if (logOutput) {
                        std::cout << defender->getName().toStdString()
                        << " получил " << ctx.damage
                        << " урона. HP=" << defender->getHp()
                        << "/" << defender->getMaxHp() << "\n";
                    }
                }

                // 6. Проверка смерти
                if (!defender->isAlive()) {
                    if (logOutput) {
                        std::cout << defender->getName().toStdString()
                        << " пал! Победил "
                        << attacker->getName().toStdString() << "\n";
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
