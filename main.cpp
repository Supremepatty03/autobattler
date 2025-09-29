#include "mainwindow.h"
#include "game.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Game game;
    game.run();
    return 0;
}
