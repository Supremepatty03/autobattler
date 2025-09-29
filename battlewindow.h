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
    explicit BattleWindow(QWidget *parent = nullptr);
    ~BattleWindow();

    // методы для обновления интерфейса
    void setPlayerInfo(Character* playerChoise);
    void setMonsterInfo(Monster* monster);
    void appendLog(const QString& text);

private slots:
    void on_swapWeaponButton_clicked();
    void on_swapWeaponNegativeButton_clicked();
    void on_newGameButton_clicked();
    void on_nextMonsterButton_clicked();

private:
    Ui::BattleWindow *ui;
};

#endif // BATTLEWINDOW_H
