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
