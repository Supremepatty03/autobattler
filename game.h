#ifndef GAME_H
#define GAME_H
#include "battle.h"

class Game {
public:
    Game();
    void run() {
        Player player = createPlayer();

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
    Player createPlayer();
    std::unique_ptr<Monster> spawnRandomMonster();
    void handleVictory(Player& player, Monster& monster);
};
#endif // GAME_H
