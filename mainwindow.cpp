#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "character.h"
#include "battlewindow.h"
#include "ui_battlewindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUpUI();
}

void MainWindow::setUpUI(){
    ui->BarbarianHP->display(6);
    ui->WarriorHP->display(5);
    ui->RogueHP->display(4);
    ui->BarbarianWeapon->setText("Дубина");
    ui->WarriorWeapon->setText("Меч");
    ui->RogueWeapon->setText("Кинжал");

    ui->WarriorBonus1_2->setText("Порыв к действию:\nВ первый ход наносит\nдвойной урон оружием");
    ui->BarbarianBonus1->setText("Ярость:\n+2 к урону в первые 3 хода,\nпотом -1 к урону");
    ui->RogueBonus1->setText("Скрытая атака:\n+1 к урону если ловкость персонажа\n выше ловкости цели");

    ui->WarriorBonus2->setText("Щит: -3 к получаемому\nурону если сила персонажа\nвыше силы атакующего");
    ui->BarbarianBonus2->setText("Каменная кожа:\nПолучаемый урон снижается на\nзначение выносливости");
    ui->RogueBonus2->setText("Ловкость +1");

    ui->WarriorBonus3->setText("Сила +1");
    ui->BarbarianBonus3->setText("Выносливость +1");
    ui->RogueBonus3->setText("Яд: Наносит дополнительные\n+1 урона на втором ходу,\n+2 на третьем и так далее.");
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startGameButton_clicked(){
    qDebug() << "start pressed";
    std::unique_ptr<CharacterClassBase> userChoise;

    if (ui->radioWarrior->isChecked()){
        userChoise = std::make_unique<WarriorClass>();
    }
    else if (ui->radioBarbarian->isChecked()){
        userChoise = std::make_unique<BarbarianClass>();
    }
    else if (ui->radioRogue->isChecked()){
        userChoise = std::make_unique<RogueClass>();
    }
    else {ui->ErrorLabel->setText("Выберите персонажа!"); return;}


    m_game = Game();
    m_game.setClass(std::move(userChoise));
    qDebug() << "Game initialized; player created? " << (m_game.getPlayer() != nullptr);

    // BattleWindow независимое окно
    BattleWindow *battleWin = new BattleWindow(m_game);
    battleWin->setAttribute(Qt::WA_DeleteOnClose);
    battleWin->setWindowFlag(Qt::Window);

    connect(battleWin, &BattleWindow::requestReturnToMenu, this, [this, battleWin](){
        this->show();
        m_game.reset();
        battleWin->deleteLater();
    });

    battleWin->show();

    this->hide();

    qDebug() << "BattleWindow shown, main window hidden";
}

void MainWindow::on_radioBarbarian_clicked(bool checked){
    Q_UNUSED(checked);
    ui->radioRogue->setChecked(false);
    ui->radioWarrior->setChecked(false);
    ui->ErrorLabel->clear();
}
void MainWindow::on_radioWarrior_clicked(bool checked){
    Q_UNUSED(checked);
    ui->radioBarbarian->setChecked(false);
    ui->radioRogue->setChecked(false);
    ui->ErrorLabel->clear();
}
void MainWindow::on_radioRogue_clicked(bool checked){
    Q_UNUSED(checked);
    ui->radioBarbarian->setChecked(false);
    ui->radioWarrior->setChecked(false);
    ui->ErrorLabel->clear();
}
