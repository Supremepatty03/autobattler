#pragma once
#ifndef BATTLEWINDOW_H
#define BATTLEWINDOW_H
#include "game.h"
#include <QWidget>

namespace Ui {
class BattleWindow;
}

class BattleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BattleWindow(Game& game, QWidget *parent = nullptr);
    ~BattleWindow();
    void setUpUI();

    // методы для обновления интерфейса
    void setPlayerInfo(Character* playerChoise);
    void setMonsterInfo(Monster* monster);
    void appendLog(const QString& text);

private slots:
    void on_swapWeaponButton_clicked();
    void on_swapWeaponNegativeButton_clicked();
    void on_newGameButton_clicked();
    void on_nextMonsterButton_clicked();
signals:
    void requestReturnToMenu();
private:
    Ui::BattleWindow *ui;
    Game& m_game;
    void showClassChoicePanel();
};

#endif // BATTLEWINDOW_H
