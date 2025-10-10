#pragma once
#ifndef BATTLEWORKER_H
#define BATTLEWORKER_H

#include <QObject>
#include <QString>

class Player;
class Monster;

class BattleWorker : public QObject
{
    Q_OBJECT
public:
    explicit BattleWorker(Player* player, Monster* monster, QObject* parent = nullptr)
        : QObject(parent), m_player(player), m_monster(monster) {}

public slots:
    // слот, который будет вызван в рабочем потоке
    void doBattle();

signals:
    void logMessage(const QString &text);
    void finished(bool won);
    void hpUpdated(int playerHp, int playerMax, int monsterHp, int monsterMax);

private:
    Player* m_player = nullptr;
    Monster* m_monster = nullptr;
};
#endif // BATTLEWORKER_H
