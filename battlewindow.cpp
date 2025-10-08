#include "battlewindow.h"
#include "ui_battlewindow.h"
#include "game.h"
#include <QThread>
#include <QPointer>
#include "battleworker.h"

BattleWindow::BattleWindow(Game& game, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BattleWindow)
    , m_game(game)
{
    qDebug() << "BattleWindow pressed";
    ui->setupUi(this);
    ui->BattleLog->setReadOnly(true);
    ui->newGameButton->setVisible(false);
    ui->nextMonsterButton->setVisible(true);
    ui->swapWeaponButton->setVisible(false);
    ui->swapWeaponNegativeButton->setVisible(false);

    if (auto* p = m_game.getPlayer()) {
        setPlayerInfo(p);
    } else {
        // игрока ещё нет — показать заглушку и/или включить кнопку "Start game"
        ui->PlayerTypesLabel->setText("Игрок не создан");
        ui->PlayerWeaponLabel->setText("-");
        ui->PlayerHP->setMaximum(1);
        ui->PlayerHP->setValue(0);
    }
    qDebug() << "BattleWindow pressed2";
    if (auto* m = m_game.getCurrentMonster()) {
        setMonsterInfo(m);
    } else {
        // заглушка для монстра
        ui->MonsterTypeLabel->setText("Нет монстра");
        ui->MonsterHP->setMaximum(1);
        ui->MonsterHP->setValue(0);
    }
    qDebug() << "BattleWindow pressed3";
}

BattleWindow::~BattleWindow()
{
    delete ui;
}

void BattleWindow::setPlayerInfo(Character* playerChoise) {
    ui->PlayerTypesLabel->setText(playerChoise->getName());
    ui->PlayerWeaponLabel->setText(playerChoise->getWeaponName());
    ui->PlayerHP->setValue(playerChoise->getMaxHp());
}

void BattleWindow::setMonsterInfo(Monster* monster) {
    ui->MonsterTypeLabel->setText(monster->getName());
    ui->MonsterWeaponLabel->setText(monster->getWeaponName());
    ui->MonsterHP->setValue(monster->getMaxHp());
}

void BattleWindow::appendLog(const QString& text) {
    ui->BattleLog->append(text);
}

void BattleWindow::on_swapWeaponButton_clicked() {

//todo
}

void BattleWindow::on_nextMonsterButton_clicked()
{
    // 1) подготовка: получаем player и monster
    Player* player = m_game.getPlayer();
    Monster* monster = m_game.makeRandomMonster();

    if (!player) {
        appendLog("Нет игрока!");
        return;
    }
    if (!monster) {
        appendLog("Не удалось заспавнить монстра.");
        return;
    }

    // Показываем базовую информацию (после проверки, чтобы не дерефать nullptr)
    setPlayerInfo(player);
    setMonsterInfo(monster);

    // Блокируем кнопки, чтобы не запускать параллельный бой
    ui->nextMonsterButton->setEnabled(false);
    ui->swapWeaponButton->setEnabled(false);
    ui->swapWeaponNegativeButton->setEnabled(false);

    // 3) создаём поток и worker
    QThread* thread = new QThread(this); // parent = this — чтобы поток удалился при закрытии окна
    BattleWorker* worker = new BattleWorker(player, monster);
    worker->moveToThread(thread);

    QPointer<QThread> threadPtr(thread);
    QPointer<BattleWorker> workerPtr(worker);

    // 4) логирование: worker -> ui (queued)
    connect(worker, &BattleWorker::logMessage, this, [this](const QString &line){
        ui->BattleLog->append(line);
    }, Qt::QueuedConnection);

    // 5) когда worker закончит — сначала обработка результата (GUI-поток)
    connect(worker, &BattleWorker::finished, this,
            [this, threadPtr, workerPtr](bool won){
                // Выполняется в GUI-потоке — безопасно обновлять UI и вызывать handleVictory
                if (won) {
                    appendLog("Бой завершён: Победа!");
                    // Рекомендуется вызвать handleVictory в GUI-потоке (если он взаимодействует с UI)
                    auto uiLogger = [this](const QString &s){
                        ui->BattleLog->append(s);

                    };

                    Player* p = m_game.getPlayer();
                    Monster* m = m_game.getCurrentMonster();
                    if (p && m) {
                        m_game.handleVictory(*p, *m, uiLogger);
                    }
                } else {
                    appendLog("Бой завершён: Поражение!");
                }

                // Обновить UI (HP/оружие и т.д.)
                if (auto* p = m_game.getPlayer()) setPlayerInfo(p);
                if (auto* m = m_game.getCurrentMonster()) setMonsterInfo(m);
                else {
                    ui->MonsterTypeLabel->setText("Нет монстра");
                    ui->MonsterHP->setMaximum(1);
                    ui->MonsterHP->setValue(0);
                }

                // Включаем кнопки обратно
                ui->nextMonsterButton->setEnabled(true);
                ui->swapWeaponButton->setEnabled(true);
                ui->swapWeaponNegativeButton->setEnabled(true);

                // Попросим поток завершиться (worker уже вернул finished)
                if (threadPtr) threadPtr->quit();
            },
            Qt::QueuedConnection);

    // 6) очистка: удалить worker и thread после завершения потока
    connect(thread, &QThread::finished, worker, &QObject::deleteLater, Qt::QueuedConnection);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater, Qt::QueuedConnection);

    // 7) при старте потока — вызвать doBattle()
    connect(thread, &QThread::started, worker, &BattleWorker::doBattle, Qt::QueuedConnection);

    // 8) стартуем поток
    thread->start();
}
void BattleWindow::on_swapWeaponNegativeButton_clicked() {}
void BattleWindow::on_newGameButton_clicked() {}

