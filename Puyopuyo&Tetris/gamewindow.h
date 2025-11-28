#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QTimerEvent>
#include <QKeyEvent>
#include "Games.h"
#include "GameOverWindow.h"

const int BLOCK_SIZE=45;
const int MARGIN=5;
const int SPPED_MS = 1000;

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    GameWindow(QString gamename, QWidget *parent = nullptr);
    ~GameWindow();

    // 그리기 함수
    void paintEvent(QPaintEvent* event);
    // keyboard 조작 함수
    void keyPressEvent(QKeyEvent* event);
    // timer
    void timerEvent(QTimerEvent* event);
    // 상화에 따른 그리기 함수
    void drawNext();
    int timer;

private:
    Game* p_game;
    QString GAMENAME;
    GameoverWindow* gameoverwindow;
};

#endif // GAMEWINDOW_H
