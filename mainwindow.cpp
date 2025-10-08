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

    ui->WarriorBonus1_2->setText("Порыв к действию: В первый ход наносит двойной урон оружием");
    ui->BarbarianBonus1->setText("Ярость: +2 к урону в первые 3 хода, потом -1 к урону");
    ui->RogueBonus1->setText("Скрытая атака:+1 к урону если ловкость персонажа выше ловкости цели");

    ui->WarriorBonus2->setText("Щит: -3 к получаемому урону если сила персонажа выше силы атакующего");
    ui->BarbarianBonus2->setText("Каменная кожа: Получаемый урон снижается на значение выносливости");
    ui->RogueBonus2->setText("Ловкость +1");

    ui->WarriorBonus3->setText("Сила +1");
    ui->BarbarianBonus3->setText("Выносливость +1");
    ui->RogueBonus3->setText("Яд: Наносит дополнительные +1 урона на втором ходу, +2 на третьем и так далее.");
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

    // 3) Создаём BattleWindow как *независимое* окно (без this как parent)
    BattleWindow *battleWin = new BattleWindow(m_game); // parent = nullptr
    battleWin->setAttribute(Qt::WA_DeleteOnClose);      // удалится при закрытии
    battleWin->setWindowFlag(Qt::Window);               // явное топ-левел окно
    battleWin->show();

    // 4) Скрываем главное окно (оно больше не управляет жизненным циклом battleWin)
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
