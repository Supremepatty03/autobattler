#include "game.h"
#include "monsters.h"

Game::Game() {}

void Game::setClass(std::unique_ptr<CharacterClassBase> cls) {
    player_ = std::make_unique<Player>(std::move(cls));
}

void Game::startNextBattle() {
    if (!player_ || !player_->isAlive()) {
        std::cout << "Игра окончена! Игрок мёртв.\n";
        return;
    }

    if (wins_ >= winsToComplete_) {
        std::cout << "Вы уже выиграли игру!\n";
        return;
    }

    currentMonster_ = spawnRandomMonster();
    Battle battle;

    bool won = battle.run(*player_, *currentMonster_, true);
    if (won) {
        handleVictory(*player_, *currentMonster_);
        ++wins_;
    } else {
        std::cout << "Вы погибли!\n";
    }
}

std::unique_ptr<Monster> Game::spawnRandomMonster() {
    static std::vector<std::function<std::unique_ptr<Monster>()>> factories = {
        [] { return std::make_unique<Goblin>(); },
        [] { return std::make_unique<Skeleton>(); },
        [] { return std::make_unique<Slime>(); },
        [] { return std::make_unique<Ghost>(); },
        [] { return std::make_unique<Golem>(); },
        [] { return std::make_unique<Dragon>(); }
    };
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, factories.size() - 1);

    return factories[dist(gen)]();
}
Monster* Game::makeRandomMonster()
{
    currentMonster_ = spawnRandomMonster(); // ownership остается в Game
    return currentMonster_.get();
}

void Game::handleVictory(Player& player, Monster& monster) {
    std::cout << "Игрок победил монстра: "
              << monster.getName().toStdString() << "!\n";

    // --- 1. Дроп оружия ---
    QString drop = monster.getDropWeaponName();
    if (!drop.isEmpty()) {
        std::cout << "Монстр уронил оружие: "
                  << drop.toStdString() << "\n";

        // игрок подбирает оружие                               /// TODO: ВЫБОР БРАТЬ ЕГО ИЛИ НЕТ UI
        player.setWeapon(drop);
        std::cout << "Игрок теперь вооружён: "
                  << drop.toStdString() << "\n";
    }

    // --- 2. Повышение уровня ---
    player.levelUp();
    std::cout << "Игрок поднял уровень! Теперь уровень: "
              << player.getLevel() << "\n";

    // --- 3. Восстановление HP (по желанию) ---
    player.healFull();
    std::cout << "Игрок восстановил здоровье: "
              << player.getHp() << "/" << player.getMaxHp() << "\n";
}
