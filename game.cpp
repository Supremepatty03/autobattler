#include "game.h"
#include "monsters.h"

Game::Game() {}

void Game::setClass (std::unique_ptr<CharacterClassBase> cls){
    chosenClass_ = std::move(cls);
    if (player_) {
        player_->addClass(std::move(chosenClass_));
    }
}
Player Game::createPlayer(std::unique_ptr<CharacterClassBase> cls) {
    return Player(std::move(cls));
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
