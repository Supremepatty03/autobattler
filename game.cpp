#include "game.h"
#include "monsters.h"

Game::Game() {}

void Game::setClass(std::unique_ptr<CharacterClassBase> cls) {
    player_ = std::make_unique<Player>(std::move(cls));
}

void Game::startNextBattle(std::function<void(const QString&)> logger)
{
    auto log = [&](const QString &s){
        if (logger) logger(s);
        else std::cout << s.toStdString() << std::endl;
    };

    if (!player_ || !player_->isAlive()) {
        log("Игра окончена! Игрок мёртв.");
        return;
    }

    if (wins_ >= winsToComplete_) {
        log(QString("Вы уже выиграли игру! Побед: %1").arg(winsToComplete_));
        return;
    }

    // создаём и сохраняем текущего монстра
    currentMonster_ = spawnRandomMonster();
    if (!currentMonster_) {
        log("Ошибка: не удалось заспавнить монстра.");
        return;
    }

    // логим старт боя
    log(QString("Новый монстр: %1").arg(currentMonster_->getName()));

    // запускаем бой — прокидываем logger дальше в Battle::run
    Battle battle;
    bool won = battle.run(*player_, *currentMonster_, /*logOutput=*/false, logger);

    if (won) {
        // handleVictory будет логировать через тот же logger
        handleVictory(*player_, *currentMonster_, logger);
        ++wins_;
    } else {
        log("Вы погибли!");
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

void Game::handleVictory(Player& player, Monster& monster,
                         std::function<void(const QString&)> logger)
{
    auto log = [&](const QString &s){
        if (logger) logger(s);
        else std::cout << s.toStdString() << std::endl;
    };

    log(QString("Игрок победил монстра: %1").arg(monster.getName()));

    // --- 1. Дроп оружия ---
    QString drop = monster.getDropWeaponName();
    if (!drop.isEmpty()) {
        log(QString("Монстр уронил оружие: %1").arg(drop));
        // выдаём игроку оружие (в твоей логике: автоматом)
        player.setWeapon(drop);
        log(QString("Игрок теперь вооружён: %1").arg(drop));
    }

    // --- 2. Повышение уровня ---
    player.levelUp();
    log(QString("Игрок поднял уровень! Теперь уровень: %1").arg(player.getLevel()));

    // --- 3. Восстановление HP ---
    player.healFull();
    log(QString("Игрок восстановил здоровье: %1/%2")
            .arg(player.getHp()).arg(player.getMaxHp()));
}
