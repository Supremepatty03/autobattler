#ifndef GAME_H
#define GAME_H
#include "battle.h"

class Game {
public:
    Game();
    void run() {

        std::unique_ptr<WarriorClass> cls;                          // TODO: ADD UI CHOISE !!!
        Player player = createPlayer(std::move(cls));

        while (player.isAlive()) {
            std::unique_ptr<Monster> monster = spawnRandomMonster();

            Battle battle;
            bool won = battle.run(player, *monster, true);

            if (won) {
                handleVictory(player, *monster);
            } else {
                std::cout << "Вы погибли!\n";
                break;
            }
        }
    }

private:
    Player createPlayer(std::unique_ptr<CharacterClassBase> cls);
    std::unique_ptr<Monster> spawnRandomMonster();
    void handleVictory(Player& player, Monster& monster);
};
#endif // GAME_H
