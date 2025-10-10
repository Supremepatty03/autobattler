#include "game.h"
#include "monsters.h"

Game::Game() {}

void Game::setClass(std::unique_ptr<CharacterClassBase> cls) {
    player_ = std::make_unique<Player>(std::move(cls));
}
void Game::reset()
{
    // Сброс внутренних данных
    player_.reset();
    currentMonster_.reset();
    lastDrop_.clear();
    pendingLevelUps_ = 0;
    wins_ = 0;
    chosenClass_.reset();
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

    currentMonster_ = spawnRandomMonster();
    if (!currentMonster_) {
        log("Ошибка: не удалось заспавнить монстра.");
        return;
    }

    log(QString("Новый монстр: %1").arg(currentMonster_->getName()));

    Battle battle;
    bool won = battle.run(*player_, *currentMonster_, /*logOutput=*/false, logger);

    if (won) {
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
    currentMonster_ = spawnRandomMonster(); // ownership в Game
    return currentMonster_.get();
}

void Game::handleVictory(Player& player, Monster& monster,
                         std::function<void(const QString&)> logger)
{
    ++wins_;
    auto log = [&](const QString &s){
        if (logger) logger(s);
        else std::cout << s.toStdString() << std::endl;
    };

    log(QString("Игрок победил монстра: %1").arg(monster.getName()));

    // --- Дроп оружия ---
    QString drop = monster.getDropWeaponName();
    lastDrop_.clear();
    if (!drop.isEmpty()) {
        log(QString("Монстр уронил оружие: %1").arg(drop));
        lastDrop_ = drop;
        log(QString("Ожидается выбор игрока: взять или оставить (%1)").arg(drop));
        log(QString("Урон вашего оружия: %1 / Урон дропа: %2").arg(player_->getWeaponDamage()).arg(monster.getWeaponDamage()));
    }

    pendingLevelUps_ += 1;
    log(QString("Игрок получил право на повышение класса"));
}

bool Game::applyDropToPlayer(bool take, std::function<void(const QString&)> logger)
{
    auto log = [&](const QString &s){
        if (logger) logger(s);
        else std::cout << s.toStdString() << std::endl;
    };

    if (lastDrop_.isEmpty()) {
        log("Нет последнего дропа для подбора.");
        return false;
    }

    if (!take) {
        log(QString("Игрок отбросил: %1").arg(lastDrop_));
        lastDrop_.clear();
        return false;
    }

    // взять предмет
    if (!player_) {
        log("Ошибка: игрока нет.");
        return false;
    }

    player_->setWeapon(lastDrop_);
    log(QString("Игрок подобрал: %1").arg(lastDrop_));
    lastDrop_.clear();
    return true;
}
bool Game::applyPendingLevelUpToClass(const QString &className, std::function<void(const QString&)> logger)
{
    auto log = [&](const QString &s){
        if (logger) logger(s);
        else std::cout << s.toStdString() << std::endl;
    };

    if (pendingLevelUps_ <= 0) {
        log("Нет доступных повышений для применения.");
        return false;
    }
    if (!player_) {
        log("Нет игрока.");
        return false;
    }

    // повысить указанный класс у игрока
    int newLvl = player_->levelUpClass(className);
    if (newLvl <= 0) {
        log(QString("Не удалось повысить класс %1").arg(className));
        return false;
    }

    --pendingLevelUps_;
    player_->levelUp();    // если хочешь, чтобы общий уровень считался отдельно — можно вызвать Character::levelUp()
    player_->healFull();   // восстанавливаем здоровье после повышения
    log(QString("Класс %1 повышен до %2; pending=%3").arg(className).arg(newLvl).arg(pendingLevelUps_));
    log(QString("Игрок восстановил здоровье: %1/%2").arg(player_->getHp()).arg(player_->getMaxHp()));
    return true;
}
