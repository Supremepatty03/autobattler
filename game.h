#ifndef GAME_H
#define GAME_H
#include "battle.h"

class Game {
public:
    Game();
    void startNextBattle();
    void setClass (std::unique_ptr<CharacterClassBase> cls);
    Player* getPlayer() { return player_.get(); }
    const Player* getPlayer() const { return player_.get(); }

    Monster* getCurrentMonster() { return currentMonster_.get(); }
    const Monster* getCurrentMonster() const { return currentMonster_.get(); }
    Monster* makeRandomMonster();
private:
    std::unique_ptr<Monster> spawnRandomMonster();
    void handleVictory(Player& player, Monster& monster);
    std::unique_ptr<CharacterClassBase> chosenClass_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<Monster> currentMonster_;

    int winsToComplete_ = 5;
    int wins_ = 0;
};
#endif // GAME_H
