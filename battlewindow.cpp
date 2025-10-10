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
    setUpUI();

    if (auto* p = m_game.getPlayer()) {
        setPlayerInfo(p);
    } else {
        // заглушка
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

void BattleWindow::setUpUI(){
    ui->BattleLog->setReadOnly(true);
    ui->newGameButton->setVisible(false);
    ui->nextMonsterButton->setVisible(true);
    ui->swapWeaponButton->setVisible(false);
    ui->swapWeaponNegativeButton->setVisible(false);
    ui->classesPanel->setVisible(false);}


BattleWindow::~BattleWindow()
{
    delete ui;
}

void BattleWindow::setPlayerInfo(Character* playerChoise) {
    ui->PlayerTypesLabel->setText(playerChoise->getName());
    ui->PlayerWeaponLabel->setText(playerChoise->getWeaponName());
    ui->PlayerHP->setMaximum(playerChoise->getMaxHp());
    ui->PlayerHP->setValue(playerChoise->getHp());

}

void BattleWindow::setMonsterInfo(Monster* monster) {
    ui->MonsterTypeLabel->setText(monster->getName());
    ui->MonsterWeaponLabel->setText(monster->getWeaponName());
    ui->MonsterHP->setMaximum(monster->getMaxHp());
    ui->MonsterHP->setValue(monster->getHp());
}

void BattleWindow::appendLog(const QString& text) {
    ui->BattleLog->append(text);
}

void BattleWindow::on_swapWeaponButton_clicked() {
    auto uiLogger = [this](const QString& s) {
        ui->BattleLog->append(s);
    };
    bool ok = m_game.applyDropToPlayer(true, uiLogger);
    if (ok) {
        appendLog("Вы подобрали оружие.");
        if (auto* p = m_game.getPlayer()) setPlayerInfo(p);
    }
    ui->swapWeaponButton->setVisible(false);
    ui->swapWeaponNegativeButton->setVisible(false);
}

void BattleWindow::on_nextMonsterButton_clicked()
{
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

    setPlayerInfo(player);
    setMonsterInfo(monster);

    ui->nextMonsterButton->setEnabled(false);
    ui->swapWeaponButton->setEnabled(false);
    ui->swapWeaponNegativeButton->setEnabled(false);

    // создаём поток и worker
    QThread* thread = new QThread(this); // parent = this — чтобы поток удалился при закрытии окна
    BattleWorker* worker = new BattleWorker(player, monster);
    worker->moveToThread(thread);

    QPointer<QThread> threadPtr(thread);
    QPointer<BattleWorker> workerPtr(worker);

    // логирование: worker -> ui (queued)
    connect(worker, &BattleWorker::logMessage, this, [this](const QString &line){
        ui->BattleLog->append(line);
    }, Qt::QueuedConnection);

    // обновление HP-полос
    connect(worker, &BattleWorker::hpUpdated, this, [this](int pHp, int pMax, int mHp, int mMax){
        ui->PlayerHP->setMaximum(pMax);
        ui->PlayerHP->setValue(pHp);

        ui->MonsterHP->setMaximum(mMax);
        ui->MonsterHP->setValue(mHp);
    }, Qt::QueuedConnection);
    // когда worker закончит — сначала обработка результата (GUI-поток)
    connect(worker, &BattleWorker::finished, this,
            [this, threadPtr, workerPtr](bool won){
                if (won) {
                    appendLog("Бой завершён: Победа!");

                    // logger -> BattleLog
                    auto uiLogger = [this](const QString &s){
                        ui->BattleLog->append(s);
                    };

                    Player* p = m_game.getPlayer();
                    Monster* m = m_game.getCurrentMonster();
                    if (p && m) {
                        m_game.handleVictory(*p, *m, uiLogger);
                    }
                    if (m_game.getWins()>=m_game.getWinsToComplete()) {
                        appendLog("Поздравляем! Вы одержали все победы и прошли игру!");
                        ui->nextMonsterButton->setVisible(false);
                        ui->swapWeaponButton->setVisible(false);
                        ui->swapWeaponNegativeButton->setVisible(false);
                        ui->newGameButton->setVisible(true);
                        return;
                    }

                    QString drop = m_game.getLastDrop();
                    if (!drop.isEmpty()) {
                        ui->swapWeaponButton->setVisible(true);
                        ui->swapWeaponNegativeButton->setVisible(true);
                        ui->nextMonsterButton->setEnabled(false);
                    }
                    showClassChoicePanel();
                } else {
                    appendLog("Бой завершён: Поражение!");
                    ui->newGameButton->setVisible(true);
                    ui->nextMonsterButton->setEnabled(false);
                    ui->nextMonsterButton->setVisible(false);

                }

                // Обновить UI
                if (auto* p = m_game.getPlayer()) setPlayerInfo(p);
                if (auto* m = m_game.getCurrentMonster()) setMonsterInfo(m);
                else {
                    ui->MonsterTypeLabel->setText("Нет монстра");
                    ui->MonsterHP->setMaximum(1);
                    ui->MonsterHP->setValue(0);
                }

                ui->nextMonsterButton->setEnabled(true);
                ui->swapWeaponButton->setEnabled(true);
                ui->swapWeaponNegativeButton->setEnabled(true);

                if (threadPtr) threadPtr->quit();
            },
            Qt::QueuedConnection);

    // очистка worker и thread
    connect(thread, &QThread::finished, worker, &QObject::deleteLater, Qt::QueuedConnection);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater, Qt::QueuedConnection);

    connect(thread, &QThread::started, worker, &BattleWorker::doBattle, Qt::QueuedConnection);

    thread->start();

}
void BattleWindow::on_swapWeaponNegativeButton_clicked() {
    auto uiLogger = [this](const QString &s){
        ui->BattleLog->append(s);
    };
    m_game.applyDropToPlayer(false, uiLogger);

    ui->swapWeaponButton->setVisible(false);
    ui->swapWeaponNegativeButton->setVisible(false);
}
void BattleWindow::on_newGameButton_clicked() {
    emit requestReturnToMenu();
    this->close();
}

void BattleWindow::showClassChoicePanel()
{
    // Получаем игрока
    Player* p = m_game.getPlayer();
    if (!p) {
        ui->classesPanel->setVisible(false);
        return;
    }

    QLayout *lay = ui->classesPanel->layout();
    if (!lay) {
        auto *vbox = new QVBoxLayout(ui->classesPanel);
        vbox->setContentsMargins(4,4,4,4);
        vbox->setSpacing(6);
        ui->classesPanel->setLayout(vbox);
        lay = vbox;
    }

    QLayoutItem *child;
    while ((child = lay->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    const auto& classes = p->getClasses();
    bool any = false;

    for (const auto& clsPtr : classes) {
        if (!clsPtr) continue;
        QString cname = clsPtr->name();
        int lvl = p->getClassLevel(cname);

        if (lvl == p->getMaxClassLevel()) {continue;}

        // Создаём кнопку для повышения данного класса
        QString btnText = QString("%1 — Повысить (ур. %2)").arg(cname).arg(lvl);
        QPushButton *btn = new QPushButton(btnText, ui->classesPanel);
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        // При клике: повышаем уровень через Player::levelUpClass и обновляем UI/логи
        connect(btn, &QPushButton::clicked, this, [this, cname, btn]() {
            btn->setEnabled(false);

            auto uiLogger = [this](const QString &s){
                ui->BattleLog->append(s);
            };

            Player* pl = m_game.getPlayer();
            if (!pl) {
                uiLogger("Ошибка: игрок отсутствует.");
                ui->classesPanel->setVisible(false);
                ui->nextMonsterButton->setEnabled(true);
                ui->swapWeaponButton->setEnabled(true);
                ui->swapWeaponNegativeButton->setEnabled(true);
                return;
            }

            int newLevel = pl->levelUpClass(cname);
            if (newLevel > 0) {
                uiLogger(QString("Класс %1 повышен до уровня %2").arg(cname).arg(newLevel));
                setPlayerInfo(pl);
            } else {
                uiLogger(QString("Не удалось повысить класс %1 (возможно достигнут максимум или класс отсутствует)").arg(cname));
            }

            ui->classesPanel->setVisible(false);
            ui->nextMonsterButton->setEnabled(true);
            ui->swapWeaponButton->setEnabled(true);
            ui->swapWeaponNegativeButton->setEnabled(true);
        });

        lay->addWidget(btn);
        any = true;
    }

    ui->classesPanel->setVisible(any);
    if (any) {
        ui->nextMonsterButton->setEnabled(false);
        ui->swapWeaponButton->setEnabled(false);
        ui->swapWeaponNegativeButton->setEnabled(false);
    } else {
        ui->nextMonsterButton->setEnabled(true);
        ui->swapWeaponButton->setEnabled(true);
        ui->swapWeaponNegativeButton->setEnabled(true);
    }
}
