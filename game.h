#ifndef GAME_H
#define GAME_H
#include "battle.h"

class Game {
public:
    Game();
    void startNextBattle(std::function<void(const QString&)> logger = {});
    void setClass (std::unique_ptr<CharacterClassBase> cls);
    Player* getPlayer() { return player_.get(); }
    const Player* getPlayer() const { return player_.get(); }

    QString getLastDrop() const { return lastDrop_; }
    void clearLastDrop() { lastDrop_.clear(); }
    bool applyDropToPlayer(bool take, std::function<void(const QString&)> logger = {});

    Monster* getCurrentMonster() { return currentMonster_.get(); }
    const Monster* getCurrentMonster() const { return currentMonster_.get(); }
    Monster* makeRandomMonster();
    void handleVictory(Player& player, Monster& monster, std::function<void(const QString&)> logger = {});

    int pendingLevelUps() const { return pendingLevelUps_; }
    bool applyPendingLevelUpToClass(const QString &className, std::function<void(const QString&)> logger = {});
    void reset();
    int getWins() {return wins_;}
    int getWinsToComplete() {return winsToComplete_;}
private:
    std::unique_ptr<Monster> spawnRandomMonster();
    std::unique_ptr<CharacterClassBase> chosenClass_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<Monster> currentMonster_;

    QString lastDrop_;

    int winsToComplete_ = 5;
    int wins_ = 0;
    int pendingLevelUps_ = 0;
};
#endif // GAME_H
