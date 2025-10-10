#include "battle.h"

bool Battle::run(Character& player, Character& monster, bool logOutput,
         std::function<void(const QString&)> logger,std::function<void(int playerHp,int playerMax,int monsterHp,int monsterMax)> progress)
{
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

    logMsg(QString("Вам выпали следующие статы:"));
    logMsg(QString("Сила: %1").arg(player.getStrength()));
    logMsg(QString("Выносливость: %1").arg(player.getEndurance()));
    logMsg(QString("Ловкость: %1").arg(player.getAgility()));

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
            logMsg(QString("=== Ход %1 — атакует: %2 ===").arg(turnCounter).arg(attacker->getName()));
        }
        QThread::msleep(2000);
        // 1) шанс попадания
        int atkAgi = attacker->getAgility();

        int defAgi = defender->getAgility();

        int sumAgi = std::max(1, atkAgi + defAgi); // защита от нуля
        std::uniform_int_distribution<int> dist(1, sumAgi);
        int roll = dist(rng);
        bool miss = (roll <= defAgi);
        if (logOutput || logger) {
            logMsg(QString("%1 бросок попадания: %2 (порог уклонения цели=%3)").arg(attacker->getName()).arg(roll).arg(defAgi));
        }
        QThread::msleep(500);
        if (miss) {
            if (logOutput|| logger ) logMsg(QString("Промах! Ход переходит"));
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
                logMsg(QString("Попадание! Начальный урон: %1 (оружие=%2)").arg(ctx.damage).arg(ctx.attacker->getWeaponName()));
            }


            // 3. Эффекты атакующего
            if (auto* m = dynamic_cast<Monster*>(attacker)) {
                for (auto& t : m->getTraits()) {
                    t->onAttack(*attacker, *defender, ctx);
                }
                for (const auto &line : ctx.logs) { logMsg(line); }
                ctx.logs.clear();
            }
            else if (auto* p = dynamic_cast<Player*>(attacker)) {
                /*ctx.damage = */ attacker->attack(*defender, ctx);

                for (const auto &line : ctx.logs) { logMsg(line); }
                ctx.logs.clear();
            }


            // 4. Эффекты защитника
            if (auto* m = dynamic_cast<Monster*>(defender)) {
                for (auto& t : m->getTraits()) {
                    t->onDefense(*defender, *attacker, ctx);
                }
                for (const auto &line : ctx.logs) { logMsg(line); }
                ctx.logs.clear();
            }
            QThread::msleep(1000);
            if (logOutput || logger) {
                logMsg(QString("Итоговый урон после модификаторов: %1").arg(ctx.damage));
            }

            // 5. Применение урона
            if (ctx.damage > 0) {
                defender->defend(ctx);                  // применяются эффекты защитника
                for (const auto &line : ctx.logs) { logMsg(line); }
                ctx.logs.clear();
                if (logOutput || logger) {
                    logMsg(QString("%1 получил %2 урона. HP=%3/%4")
                               .arg(defender->getName())
                               .arg(ctx.damage)
                               .arg(defender->getHp())
                               .arg(defender->getMaxHp()));
                }
                if (progress) {
                    progress(player.getHp(), player.getMaxHp(), monster.getHp(), monster.getMaxHp());
                }
            }

            // 6. Проверка смерти
            if (!defender->isAlive()) {
                if (progress) {
                    progress(player.getHp(), player.getMaxHp(), monster.getHp(), monster.getMaxHp());
                }
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
    if (progress) {
        progress(player.getHp(), player.getMaxHp(), monster.getHp(), monster.getMaxHp());
    }
    return player.isAlive();
}
