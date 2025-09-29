#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "character.h"

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
    else {ui->ErrorLabel->setText("Выберите персонажа!");}

    m_game = Game();
    m_game.setClass(std::move(userChoise));
}
