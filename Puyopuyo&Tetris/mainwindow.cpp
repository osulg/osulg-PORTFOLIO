#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QLabel"
#include "GameWindow.h"
#include "GameoverWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, &MainWindow::buttonClicked, this, &MainWindow::generateGame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonPuyopuyo_clicked()
{
    emit buttonClicked(QString("Puyopuyo"));
}

void MainWindow::on_pushButtonTetris_clicked()
{
    emit buttonClicked(QString("Tetris"));
}

void MainWindow::on_pushButtonPuyopuyoTetris_clicked()
{
    emit buttonClicked(QString("PuyopuyoTetris"));
}

void MainWindow::generateGame(QString gamename)
{
    GameWindow* gamewindow = new GameWindow(gamename);
    gamewindow->show();

    //GameoverWindow* gameoverwindow = new GameoverWindow;
    //gameoverwindow->show();
    this->close();
}
