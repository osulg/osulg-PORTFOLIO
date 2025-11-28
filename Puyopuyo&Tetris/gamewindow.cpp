#include "GameWindow.h"

GameWindow::GameWindow(QString gamename, QWidget *parent) : QWidget(parent)
{
    timer=startTimer(SPPED_MS);
    GAMENAME = gamename;

    if (GAMENAME == "Puyopuyo")
    {
        p_game = new PuyopuyoGame();
        resize(500, 800);
    }
    else if (GAMENAME == "Tetris")
    {
        p_game = new TetrisGame();
        resize(700, 1100);
    }
    else if (GAMENAME == "PuyopuyoTetris")
    {
        p_game = new PuyopuyoTetrisGame();
        resize(700, 900);
    }
    setWindowTitle(gamename);
}

GameWindow::~GameWindow()
{
    delete p_game;
    delete gameoverwindow;
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    int ROW = p_game->ROW;
    int COL = p_game->COL;
    int LENGTH = p_game->LENGTH;

    string** board = p_game->board;
    string* next = p_game->next;

    double axis_row = p_game->axis_row;
    double axis_col = p_game->axis_col;

    int score = p_game->score;

    QPainter painter(this);
    painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    painter.drawRect(MARGIN,MARGIN,COL*BLOCK_SIZE,ROW*BLOCK_SIZE);

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial",14));
    painter.drawText(QRect(MARGIN*3+COL*BLOCK_SIZE,MARGIN,BLOCK_SIZE,BLOCK_SIZE),Qt::AlignCenter,"NEXT");


    for(int k = 0; k < LENGTH; k++)
    {
        std::string current_color = next[k];


        int current_col = MARGIN*10+COL*(BLOCK_SIZE/1)+0*(BLOCK_SIZE/1);
        int current_row = MARGIN*10+0*(BLOCK_SIZE/2) + 100 * k + 100;
        int one = (BLOCK_SIZE/2);

        if (next[k].length() == 2)
        {
            for (int l = 0; l < 2; l++)
            {
                // 뿌요에서 설정한 것과 같이 문자를 변경
                if (next[k][l] == 'R')
                {
                    painter.setBrush(QBrush(QColor(255, 0, 0),Qt::SolidPattern));
                    painter.drawEllipse(current_col, current_row + l * one, one, one);
                }

                if (next[k][l] == 'Y')
                {
                    painter.setBrush(QBrush(QColor(255, 255, 0),Qt::SolidPattern));
                    painter.drawEllipse(current_col, current_row + l * one, one, one);
                }

                if (next[k][l] == 'G')
                {
                    painter.setBrush(QBrush(QColor(0, 255, 0),Qt::SolidPattern));
                    painter.drawEllipse(current_col, current_row + l * one, one, one);
                }

                if (next[k][l] == 'B')
                {
                    painter.setBrush(QBrush(QColor(0, 0, 255),Qt::SolidPattern));
                    painter.drawEllipse(current_col, current_row + l * one, one, one);
                }

                if (next[k][l] == 'P')
                {
                    painter.setBrush(QBrush(QColor(255, 0, 255),Qt::SolidPattern));
                    painter.drawEllipse(current_col, current_row + l * one, one, one);
                }
            }
        }

        if (next[k] == "O")
        {
            painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
            painter.drawRect(current_col, current_row, one, one);
            painter.drawRect(current_col, current_row + one, one, one);
            painter.drawRect(current_col + one, current_row, one, one);
            painter.drawRect(current_col + one, current_row + one, one, one);
        }


        else if (next[k] == "T")
        {
            painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
            painter.drawRect(current_col + one, current_row, one, one);
            painter.drawRect(current_col, current_row + one, one, one);
            painter.drawRect(current_col + one, current_row + one, one, one);
            painter.drawRect(current_col + 2 * one, current_row + one, one, one);
        }


        else if (next[k] == "S")
        {
            painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
            painter.drawRect(current_col + one, current_row, one, one);
            painter.drawRect(current_col + 2 * one, current_row, one, one);
            painter.drawRect(current_col, current_row + one, one, one);
            painter.drawRect(current_col + one, current_row + one, one, one);
        }


        else if (next[k] == "Z")
        {
            painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
            painter.drawRect(current_col, current_row, one, one);
            painter.drawRect(current_col + one, current_row, one, one);
            painter.drawRect(current_col + one, current_row + one, one, one);
            painter.drawRect(current_col + 2 * one, current_row + one, one, one);
        }


        else if (next[k] == "J")
        {
            painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
            painter.drawRect(current_col, current_row, one, one);
            painter.drawRect(current_col, current_row + one, one, one);
            painter.drawRect(current_col + one, current_row + one, one, one);
            painter.drawRect(current_col + 2 * one, current_row + one, one, one);
        }


        else if (next[k] == "L")
        {
            painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
            painter.drawRect(current_col + 2 * one, current_row, one, one);
            painter.drawRect(current_col, current_row + one, one, one);
            painter.drawRect(current_col + one, current_row + one, one, one);
            painter.drawRect(current_col + 2 * one, current_row + one, one, one);
        }


        else if (next[k] == "I")
        {
            painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
            painter.drawRect(current_col, current_row + one, one, one);
            painter.drawRect(current_col + one, current_row + one, one, one);
            painter.drawRect(current_col + 2 * one, current_row + one, one, one);
            painter.drawRect(current_col + 3 * one, current_row + one, one, one);
        }
    }


    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial",12));
    if (GAMENAME == "Puyopuyo")
        painter.drawText(QRect(MARGIN*3+COL*BLOCK_SIZE,MARGIN*2+4*BLOCK_SIZE,BLOCK_SIZE*4,BLOCK_SIZE*4 + 800),Qt::AlignCenter,"current chain : " + QString::number(score));
    if (GAMENAME == "Tetris")
        painter.drawText(QRect(MARGIN*3+COL*BLOCK_SIZE,MARGIN*2+4*BLOCK_SIZE,BLOCK_SIZE*4,BLOCK_SIZE*4 + 800),Qt::AlignCenter,"current line : " + QString::number(score));
    if (GAMENAME == "PuyopuyoTetris")
        painter.drawText(QRect(MARGIN*3+COL*BLOCK_SIZE,MARGIN*2+4*BLOCK_SIZE,BLOCK_SIZE*4,BLOCK_SIZE*4 + 800),Qt::AlignCenter,"current chain&line : " + QString::number(score));



    for(int i=0; i<ROW; i++)
        for(int j=0; j<COL; j++)
        {
                painter.setBrush(QBrush(QColor(255, 255, 255),Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
        }

    for(int i=0;i<ROW;i++)
    {
        for(int j=0;j<COL;j++)
        {
            if(board[i][j] == "BlackPuyo")
            {
                painter.setBrush(QBrush(QColor(0, 0, 0),Qt::SolidPattern));
                painter.drawEllipse(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }

            if(board[i][j] == "BlackBlock")
            {
                painter.setBrush(QBrush(QColor(0, 0, 0),Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }

            else if(board[i][j] == "Gray")
            {
                painter.setBrush(QBrush(QColor(128, 128, 128),Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }

            else if(board[i][j] == "White")
            {
                painter.setBrush(QBrush(QColor(255, 255, 255),Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }
            else if(board[i][j] == "silver")
            {
                painter.setBrush(QBrush(QColor(192, 192, 192),Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }


            // 뿌요에서 설정한 것과 같이 문자열 변경
            else if(board[i][j] == "R")
            {
                painter.setBrush(QBrush((QColor(255, 0, 0)),Qt::SolidPattern));
                painter.drawEllipse(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }

            else if(board[i][j] == "Y")
            {
                painter.setBrush(QBrush((QColor(255, 255, 0)),Qt::SolidPattern));
                painter.drawEllipse(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }

            else if(board[i][j] == "G")
            {
                painter.setBrush(QBrush((QColor(0, 255, 0)),Qt::SolidPattern));
                painter.drawEllipse(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }

            else if(board[i][j] == "B")
            {
                painter.setBrush(QBrush((QColor(0, 0, 255)),Qt::SolidPattern));
                painter.drawEllipse(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }

            else if(board[i][j] == "P")
            {
                painter.setBrush(QBrush((QColor(255, 0, 255)),Qt::SolidPattern));
                painter.drawEllipse(j*BLOCK_SIZE+MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }
        }
    }

    painter.setBrush(QBrush(QColor(0, 0, 0),Qt::SolidPattern));
    painter.drawRect(axis_col*BLOCK_SIZE+MARGIN + BLOCK_SIZE/4,axis_row*BLOCK_SIZE+MARGIN + BLOCK_SIZE/4,BLOCK_SIZE/2,BLOCK_SIZE/2);
}

// keyBoard 조작 함수
void GameWindow::keyPressEvent(QKeyEvent *event)
{
    // 게임 종료
    if (p_game->gameover == true)
        return;

    // 키보드에 따른 조작
    switch(event->key())
    {
    case Qt::Key_Down:
        p_game->moveDown();
        drawNext();
        break;
    case Qt::Key_Left:
        p_game->moveLeft();
        drawNext();
        break;
    case Qt::Key_Right:
        p_game->moveRight();
        drawNext();
        break;
    case Qt::Key_Space:
        p_game->HardDrop();
        drawNext();

        break;
    case Qt::Key_Z:
        p_game->rotate_counter();
        drawNext();
        break;
    case Qt::Key_X:
        p_game->rotate();
        drawNext();
        break;
    default:
        break;
    }
}

// 타이머 관련 함수
void GameWindow::timerEvent(QTimerEvent* event)
{

    // 아래로 내려가기 때문에 moveDown 사용
    if(event->timerId()==timer){
        p_game->moveDown();
        drawNext();

        if (p_game->gameover) {
            // 게임이 종료되면 타이머 중지
            killTimer(timer);

            // 게임 오버 창 표시
            gameoverwindow = new GameoverWindow;
            gameoverwindow->show();
        }

    }
}

// 상화별 그리기 함수
void GameWindow::drawNext()
{
    int ROW = p_game->ROW;
    int COL = p_game->COL;
    double AXIS_ROW = p_game->axis_row;
    double AXIS_COL = p_game->axis_col;

    string** board = p_game->board;
    string* next = p_game->next;

    int LENGTH = p_game->LENGTH;
    int score =  p_game->score;


    // 게임 종료의 경우 모든 블럭 검은색으로 설정
    if (p_game->gameover == true)
    {
        for (int i = 0; i < ROW; i++)
            for (int j = 0; j < COL; j++)
            {
                if (board[i][j] == "R" || board[i][j] == "Y" || board[i][j] == "G" || board[i][j] == "B" || board[i][j] == "P")
                    board[i][j] = "BlackPuyo";

                else if (board[i][j] == "Gray")
                    board[i][j] = "BlackBlock";
            }
        gameoverwindow = new GameoverWindow;
        gameoverwindow->show();

        return;
    }

    // 초기화를 next[0], 각 게임에서의 행과 열의 축으로 설정
    string current = next[0];
    p_game->initializeGame(AXIS_ROW, AXIS_COL, current, score);

    update();
}
