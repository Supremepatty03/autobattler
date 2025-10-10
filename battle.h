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
#include <QObject>
#include <QDebug>
#include <QThread>

enum class BattleResult { AttackerWon, DefenderWon };


class Battle
{
public:
    Battle()
        : rng(std::random_device{}())
    {}

    bool run(Character& player, Character& monster, bool logOutput = true,
             std::function<void(const QString&)> logger = {},std::function<void(int playerHp,int playerMax,int monsterHp,int monsterMax)> progress = {});


private:
    std::mt19937 rng;
    int wins_ = 0;
    int winsToComlete = 5;
};

#endif // BATTLE_H
