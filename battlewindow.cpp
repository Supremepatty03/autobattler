#include "battlewindow.h"
#include "ui_battlewindow.h"
#include "game.h"

BattleWindow::BattleWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BattleWindow)
{
    ui->setupUi(this);
    // Дополнительно: настройки UI (например, readonly)
    // ui->battleLog->setReadOnly(true); // если такой виджет есть
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


}
void BattleWindow::on_swapWeaponNegativeButton_clicked() {}
void BattleWindow::on_newGameButton_clicked() {}
void BattleWindow::on_nextMonsterButton_clicked() {}
