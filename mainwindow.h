#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "game.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void on_startGameButton_clicked();
    void on_radioBarbarian_clicked(bool checked);
    void on_radioWarrior_clicked(bool checked);
    void on_radioRogue_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    Game m_game;
};
#endif // MAINWINDOW_H
