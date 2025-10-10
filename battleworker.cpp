#include "battleworker.h"
#include "battle.h"
#include "monsters.h"
#include <QString>

void BattleWorker::doBattle()
{
    if (!m_player || !m_monster) { emit finished(false); return; }

    auto logger = [this](const QString &s){ emit logMessage(s); };

    // progress callback — эмитим сигнал hpUpdated
    auto progress = [this](int ph, int pmax, int mh, int mmax){
        emit hpUpdated(ph, pmax, mh, mmax);
    };

    Battle battle;
    bool won = battle.run(*m_player, *m_monster, /*logOutput=*/false, logger, progress);

    emit finished(won);
}
