#include "battleworker.h"
#include "battle.h"
#include "monsters.h"
#include <QString>

void BattleWorker::doBattle()
{
    if (!m_player || !m_monster) {
        emit finished(false);
        return;
    }

    // лямбда-логгер, который шлёт сообщения в UI через сигнал
    auto logger = [this](const QString &s){
        emit logMessage(s);
    };

    // Выполним бой (в отдельном потоке). Метод run синхронно вернёт результат.
    Battle battle;
    bool won = battle.run(*m_player, *m_monster, /*logOutput=*/false, logger);

    emit finished(won);
}
