#include "battlewindow.h"
#include "ui_battlewindow.h"
#include "game.h"
#include <QThread>
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
        appendLog("Нет монстра!");
        return;
    }

    ui->nextMonsterButton->setEnabled(false);
    ui->swapWeaponButton->setEnabled(false);
    ui->swapWeaponNegativeButton->setEnabled(false);

    // 3) создаём поток и worker
    QThread* thread = new QThread(this); // parent = this — чтобы удалить, если окно закроется
    BattleWorker* worker = new BattleWorker(player, monster);
    worker->moveToThread(thread);

    // 4) подключаем сигналы worker -> UI
    // лог
    connect(worker, &BattleWorker::logMessage, this, [this](const QString &line){
        // Этот слот вызывается в GUI-потоке (queued connection) — безопасно обновлять UI
        ui->BattleLog->append(line);
        //ui->BattleLog->verticalScrollBar()->setValue(ui->BattleLog->verticalScrollBar()->maximum());
    });

    // завершение боя
    connect(worker, &BattleWorker::finished, this, [this, thread, worker](bool won){
        // Обновляем UI по результату (этот код выполняется в GUI-потоке)
        if (won) appendLog("Бой завершён: Победа!");
        else appendLog("Бой завершён: Поражение!");

        // Включаем кнопки обратно (или показываем next monster / new game)
        ui->nextMonsterButton->setEnabled(true);
        ui->swapWeaponButton->setEnabled(true);
        ui->swapWeaponNegativeButton->setEnabled(true);
        ui->newGameButton->setVisible(false); // пример

        // Остановим поток и удалим worker/thread корректно
        thread->quit();
        // thread->wait() не вызываем прямо здесь — оно может блокировать GUI, но можно дождаться finished и потом в следующем шаге удалить.
        // Мы подключим finished -> thread->deleteLater и thread->finished -> worker->deleteLater ниже.
    });

    // 5) очистка объектов после завершения потока
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    // 6) запуск: когда поток стартует, вызываем doBattle (queued)
    connect(thread, &QThread::started, worker, &BattleWorker::doBattle);

    thread->start();
}
void BattleWindow::on_swapWeaponNegativeButton_clicked() {}
void BattleWindow::on_newGameButton_clicked() {}

