#ifndef GAMES_H
#define GAMES_H

#include <string>
#include <ctime>
#include <QQueue>
#include <queue>
#include <cstdlib>
#include <cmath>

using namespace std;

// Game class
class Game
{
public:
    // 생산자
    Game(int row, int col, int length); // 행, 열 길이 초기화

    // 소멸자
    ~Game();

    // 순수 가상 함수 구현 -> 하위 class에 구현
    // ketBoard 관련 함수
    virtual void moveDown() = 0;
    virtual void moveLeft() = 0;
    virtual void moveRight()= 0;
    virtual void HardDrop() = 0;
    virtual void rotate() = 0;
    virtual void rotate_counter() = 0;
    virtual void initializeGame(int row, int col, string& next, int score)=0;

    // 좌표 회전 함수
    void rotatePoint(int& x, int& y, int px, int py, int angle);



public:
    int ROW; // 행
    int COL; // 열
    int LENGTH; // 길이

    double axis_row; // 회전 축 행 좌표
    double axis_col; // 회전 축 열 좌표

    int score; // 점수

    bool gameover; // 게임오버

    string** board; // 게임 보드
    string* next; // 다음 블록 (Puyo, Tetromino)
};

// PuyopuyoGame class : Game 하위 class

class PuyopuyoGame : public Game
{
public:
    // 생산자
    PuyopuyoGame();
    // 소멸자
    ~PuyopuyoGame();

    // 게임 초기화 함수
    void initializeGame(int row, int col, string &next, int score) override;

    // keyBoard 관련 함수
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    void HardDrop() override;
    void rotate() override;
    void rotate_counter() override;


private:
    queue<string*>nextQueue; // 다음 블럭을 저장할 큐
    string puyo_color; // Puyo 색상
    string puyo_shape; // Puyo 모양

    // 램덤 Puyo 색상 생성 함수
    string createRandomPuyo();

    // Puyo의 모양 및 축 고정 함수
    void fixPuyoShapeAndAxis();

    // 다음 Puyo 표시 함수
    void displayNextPuyo();

    // 새로운 Puyo 생성 함수
    void createNewPuyo();

    // 색설정
    void setColorNum(int row, int col, string color);

    // 연결된 Puyo 삭제 함수
    void removeConnectedPuyos();

    // Puyo 연결성 검사
    void checkPuyo(int row, int col, string color, vector<vector<bool>>& visited, queue<pair<int, int>>& puyoQueue);

    // 중력 적용 함수
    void applyGravity();

};


// TetrisGame class : Game 하위 class
class TetrisGame : public Game
{
public:
    // 생산자
    TetrisGame();
    // 소멸자
    ~TetrisGame();

    // 게임 초기화 함수
    void initializeGame(int row, int col, string &next, int score) override;

    //keyBoard 관련 함수
    bool isValidMove(int row, int col); // 유효한 이동인지 확인하는 함수
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    void HardDrop() override;
    void rotate() override;
    void rotate_counter() override;

private:
    std::queue<std::string> nextQueue; // 다음 블럭을 저장할 큐
    string tetris_shape; // Tetromino의 모양
    int t_row, t_col; // Tetromino의 행과 열

    // 랜덤한 Tetromino 모양 생성 함수
    string createRandomTetris();

    // Tetromino 모양 및 축 고정 함수
    void fixTetrisShapeAndAxis();

    // 다음 Tetromino 표시
    void displayNextTetris();

    // 새로운 Tetromino 생성 함수
    void createNewTetris();

    // 큐 채우기 함수
    void fillQueue();
};


// PuyopuyoTetrisGame class : Game 하위 class
class PuyopuyoTetrisGame : public Game
{
public:
    // 생산자
    PuyopuyoTetrisGame();
    // 소멸자
    ~PuyopuyoTetrisGame();

    // 게임 초기화 함수
    void initializeGame(int row, int col, string &next, int score) override;

    // keyBoard 관련 함수
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    void HardDrop() override;
    void rotate() override;
    void rotate_counter() override;

private:
    queue<string*>nextQueue; // 다음 블럭을 저장할 큐

    string puyo_color; // Puyo 색상
    string puyo_shape; // Puyo 모양

    string tetris_shape; // Tetromino의 모양
    int t_row, t_col; // Tetromino의 행과 열


};


#endif // GAMES_H
