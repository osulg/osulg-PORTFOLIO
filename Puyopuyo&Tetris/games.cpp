#include "games.h"

// Game class 생산자
Game::Game(int row, int col, int length)
{
    ROW = row; // 행
    COL = col; // 열
    LENGTH = length; // 길이 (다음 블럭 수)

    // board에 메모리 할당
    board = new string*[ROW];
    for (int i = 0; i < ROW; i++)
        board[i] = new string[COL];

    // 다음 블럭 배열에 대한 메모리 할당
    next = new string[length];

    gameover = false; // 게임 오버 상태를 false로 초기화
}

// Game class 소멸자
Game::~Game()
{
    delete[] next; // next 배열 해제

    // board 배열 해제
    for (int i = 0; i < ROW; i++)
        delete[] board[i];
    delete[] board;
}


// PuyopuyoGame initialize
void PuyopuyoGame::initializeGame(int row, int col, string& next, int score){
    axis_row = row;
    axis_col = col;
    this->next[0] = next;
    this->score = score;
}

// TetrisGame initialize
void TetrisGame::initializeGame(int row, int col, string& next, int score){
    axis_row = row;
    axis_col = col;
    this->next[0] = next;
    this->score = score;
}

// PuyopuyiTetrisGame initialize
void PuyopuyoTetrisGame::initializeGame(int row, int col, string& next, int score){
    axis_row = row;
    axis_col = col;
    this->next[0] = next;
    this->score = score;
}

//keyBoard 관련 함수에 대한 내용
// PuyopuyoGame : moveDown (아래로 이동)
void PuyopuyoGame::moveDown() {
    int current_row = axis_row - 1; // 현재 행 위치 (축-1로 고정)
    int current_col = axis_col; // 현재 열 위치(축으로 고정)

    // 이동 가능한지 확인
    // 뿌요의 모양을 봤을 때, 아래쪽 뿌요에 축이 있기 때문에 이를 고려해 설정
    if (axis_row + 1 >= ROW || board[int(axis_row) + 1][int(axis_col)] != "0") {
        board[current_row][current_col] = puyo_shape[0];
        board[int(axis_row)][int(axis_col)] = puyo_shape[1];

        // gameover 상태
        for (int col = 0; col < COL; col++) {
            if (board[2][col] != "0") {
                gameover = true; // gameover
                break;
            }
        }

        // 새로운 뿌요 생성
        createNewPuyo();
        // 다음 뿌요 표시
        displayNextPuyo();
        return;
    }

    // 이동 전 뿌요 초기화
    board[current_row][current_col] = "0";
    board[int(axis_row)][int(axis_col)] = "0";

    // 아래로 이동 (각 뿌요에 대한 행의 위치 추가)
    axis_row++;
    current_row++;

    // 새로운 위치에 뿌요 위치
    board[current_row][current_col] = puyo_shape[0];
    board[int(axis_row)][int(axis_col)] = puyo_shape[1];

    // 연결된 뿌요 제거
    removeConnectedPuyos();
}

// TetrisGame : moveDown (아래로 이동)
void TetrisGame::moveDown() {
    int current_row = axis_row + 1; //
    tetris_shape = next[0];

    // 테트로미노가 아래로 이동 가능한지 확인
    if (isValidMove(current_row, axis_col)) {
        // 이동 전 테트로미노 초기화
        board[int(axis_row)][int(axis_col)] = "White";

        // 새로운 위치에 테트로미노를 표시
        axis_row = current_row;
        board[int(axis_row)][int(axis_col)] = tetris_shape;

    // 이동이 불가능하면
    } else {
        // 이동이 불가능하면 현재 위치에 테트로미노를 고정
        board[int(axis_row)][int(axis_col)] = tetris_shape;

        // 새로운 테트로미노를 생성하고 다음 블록을 표시
        createNewTetris();
    }
}

// 테트로미노의 이동이 유효한지 확인하는 함수
bool TetrisGame::isValidMove(int row, int col) {
    // 테트로미노의 위치가 보드의 크기 범위를 벗어나면
    if (row < 0 || row >= ROW || col < 0 || col >= COL) {
        return false; // 이동 불가
    }

    // 이동하려는 위치에 다른 블록이 있다면
    if (board[row][col] != "White") {
        return false; // 이동 불가
    }

    return true; // 이외의 경우에는 이동 가능
}


void PuyopuyoTetrisGame::moveDown() {
    ;
}


// moveLeft
// PuyopuyoGame : moveLeft (왼쪽로 이동)
void PuyopuyoGame::moveLeft() {
    int current_row = axis_row - 1; // 현재 행 위치 (축-1로 고정)
    int current_col = axis_col; // 현재 열 위치 (축으로 고정)

    // 이동 불가능해 아무런 동작 안함
    if (axis_col - 1 < 0 || board[int(axis_row)][int(axis_col) - 1] != "0" || board[current_row][current_col-1]=="Gray") {
        return;
    }

    // 이동 전 위치 초기화
    board[current_row][current_col] = "0";
    board[int(axis_row)][int(axis_col)] = "0";

    // 왼쪽으로 이동
    axis_col--;
    current_col--;

    // 새로운 위치에 뿌요 위치
    board[current_row][current_col] = puyo_shape[0];
    board[int(axis_row)][int(axis_col)] = puyo_shape[1];

}

void TetrisGame::moveLeft(){

}
void PuyopuyoTetrisGame::moveLeft(){

}


// moveRight
// PuyopuyoGame : moveRight (오른쪽으로 이동)
void PuyopuyoGame::moveRight() {
    int current_row = axis_row - 1; // 현재 열 위치 (축-1로 고정)
    int current_col = axis_col; // 현재 열 위치 (축으로 고정)

    // 이동 불가능해 아무런 동작 안함
    if (axis_col + 1 >= COL || board[int(axis_row)][int(axis_col) + 1] != "0" || board[current_row][current_col+1]=="Gray") {
        return;
    }

    // 이동 전 위치 초기화
    board[current_row][current_col] = "0";
    board[int(axis_row)][int(axis_col)] = "0";

    // 왼쪽으로 이동
    axis_col++;
    current_col++;

    // 새로운 위치에 뿌요 위치
    board[current_row][current_col] = puyo_shape[0];
    board[int(axis_row)][int(axis_col)] = puyo_shape[1];

}


void TetrisGame::moveRight(){

}
void PuyopuyoTetrisGame::moveRight(){

}


// HardDrop
// PuyopuyoGame : HarDorp (최대한 아래로 이동)
void PuyopuyoGame::HardDrop(){
    int current_row = axis_row - 1; // 현재 열 위치 (축-1로 고정)
    int current_col = axis_col; // 현재 열 위치 (축으로 고정)

    // 무한 반복 -> 최대한 아래로 내려가는 것으로 moveDown의 알고리즘 사용
    while (true) {
         // 이동 불가능해 아무런 동작 안함
        if (axis_row + 1 >= ROW || board[int(axis_row) + 1][int(axis_col)] != "0") {
            board[current_row][current_col] = puyo_shape[0];
            board[int(axis_row)][int(axis_col)] = puyo_shape[1];

            board[current_row][current_col] = puyo_shape[0];
            board[int(axis_row)][int(axis_col)] = puyo_shape[1];

            for (int col = 0; col < COL; col++) {
                if (board[2][col] != "0") {
                    gameover = true;
                    break;
                }
            }

            return;
        }

        // 이동 전 위치 초기화
        board[current_row][current_col] = "0";
        board[int(axis_row)][int(axis_col)] = "0";

        // 아래로 이동 (각 뿌요에 대한 행의 위치 추가)
        axis_row++;
        current_row++;

        // 새로운 위치에 뿌요 위치
        board[current_row][current_col] = "-" + puyo_shape[0];
        board[int(axis_row)][int(axis_col)] = "-" + puyo_shape[1];

    }
}

void TetrisGame::HardDrop(){

}

void PuyopuyoTetrisGame::HardDrop(){

}

// 점에 대한 회전 함수
void Game::rotatePoint(int& x, int& y, int px, int py, int angle) {
    // 라디안 계산
    int s = sin(angle * M_PI / 180); // sin값 계산
    int c = cos(angle * M_PI / 180); // cos값 계산

    // 원점 기준으로 이동
    x -= px;
    y -= py;

    // 변환
    int xnew = x * c - y * s;
    int ynew = x * s + y * c;

    // 새로운 위치 설정(되돌아 오기)
    x = xnew + px;
    y = ynew + py;
}

// rotate
// PuyopuyoGame : rotate (축 기준 시계방향 회전)

void PuyopuyoGame::rotate(){
    int pivot_row = axis_row; // 축의 행 위치
    int pivot_col = axis_col; // 축의 열 위치
    int current_row = axis_row - 1; // 현재 뿌요의 행 위치
    int current_col = axis_col; // 현재 뿌요의 열 위치

    // 새로운 행과 열로 설정
    int new_row = current_row;
    int new_col = current_col;

    // 회전
    rotatePoint(new_row, new_col, pivot_row, pivot_col, -90);

    // 회전 가능하면 회전 및 이전 위치 지우기
    if (new_row >= 0 && new_row < ROW && new_col >= 0 && new_col < COL && board[new_row][new_col] == "0") {
        board[current_row][current_col] = "0";
        board[new_row][new_col] = puyo_shape[0];
        board[pivot_row][pivot_col] = puyo_shape[1];
    }
}

void TetrisGame::rotate(){

}
void PuyopuyoTetrisGame::rotate(){

}

// rotate_counter
// PuyopuyoGame : rotate (축 기준 반시계방향 회전)
void PuyopuyoGame::rotate_counter() {
    int pivot_row = axis_row; // 축의 행 위치
    int pivot_col = axis_col; // 축의 열 위치
    int current_row = axis_row - 1; // 현재 뿌요의 행 위치
    int current_col = axis_col; // 현재 뿌요의 열 위치

    // 새로운 행과 열로 설정
    int new_row = current_row;
    int new_col = current_col;

    // 회전
    rotatePoint(new_row, new_col, pivot_row, pivot_col, 90);

    // 회전 가능하면 회전 및 이전 위치 지우기
    if (new_row >= 0 && new_row < ROW && new_col >= 0 && new_col < COL && board[new_row][new_col] == "0") {
        board[current_row][current_col] = "0";
        board[new_row][new_col] = puyo_shape[0];
        board[pivot_row][pivot_col] = puyo_shape[1];
    }
}


void TetrisGame::rotate_counter(){

}
void PuyopuyoTetrisGame::rotate_counter(){

}


// PuyopuyoGame //

//
string PuyopuyoGame::createRandomPuyo(){
    const string Puyo_color[5]={"R", "Y", "G", "B", "P"}; // 뿌요 색 배열

    srand((time(nullptr)));

    // 첫번째 뿌요의 색상
    int randomIndex1 = rand()%5;
    string selectPuyo1 = Puyo_color[randomIndex1];

    // 두 번쨰 뿌요의 색상
    int randomIndex2 = rand()%5;
    string selectPuyo2 = Puyo_color[randomIndex2];

    return selectPuyo1+selectPuyo2; // "R" + "Y" = "RY" 이런 형태로 반환
}

// 뿌요의 축과 색 고정
void PuyopuyoGame::fixPuyoShapeAndAxis() {
    // 뿌요의 모양 고정
    puyo_shape = next[0]; // 모양(색)은 next[0]

    // 행 및 열의 축
    axis_row = 1;
    axis_col = 2;
}

// 다음 뿌요 표시
void PuyopuyoGame::displayNextPuyo() {
    // next[0]의 뿌요를 보드에 표시하는 예시
    int displayRow = 0; // 표시할 행의 위치
    int displayCol = COL / 2 - 1; // 표시할 열의 중앙 위치

    board[displayRow][displayCol] = puyo_shape[0]; // 첫 번째 뿌요
    board[displayRow + 1][displayCol] = puyo_shape[1]; // 두 번째 뿌요
}

// 새로운 뿌요 생성
void PuyopuyoGame::createNewPuyo() {
    applyGravity();


    // 새로운 뿌요 생성
    string newPuyo = createRandomPuyo();

    // 큐에 새로운 뿌요 색상 추가
    string* newPuyoPair = new string[2];
    newPuyoPair[0] = newPuyo[0];
    newPuyoPair[1] = newPuyo[1];
    nextQueue.push(newPuyoPair);

    // 기존의 뿌요 색상 업데이트
    string* currentPuyo1 = nextQueue.front();
    nextQueue.pop();
    delete[] currentPuyo1;

    // next[0]의 뿌요을 next[1]로 할당
    next[0] = next[1];
    // next[1]은 새로운 뿌요로 할당
    next[1] = newPuyo;

    // 축 및 모양 고정
    fixPuyoShapeAndAxis();


    // 뿌요 표시
    displayNextPuyo();
}

// 색 설정
void PuyopuyoGame::setColorNum(int row, int col, string color) {
    // 빈 칸과 회색이 아니라면
    if (board[row][col] == "0" && board[row][col] != "Gray") {
        int colorNumber = 0;
        // 각 색별로 숫자 할당
        switch (color[0]) {
        case 'R':
            colorNumber = -1;
            break;
        case 'Y':
            colorNumber = -2;
            break;
        case 'G':
            colorNumber = -3;
            break;
        case 'B':
            colorNumber = -4;
            break;
        case 'P':
            colorNumber = -5;
            break;
        }

        // 숫자 할당
        if (board[row][col] != "Gray") {
            colorNumber = abs(colorNumber);
            board[row][col] = to_string(colorNumber);
        }
    }
}



// 연결된 뿌요를 제거하는 함수 (BFS)
void PuyopuyoGame::removeConnectedPuyos() {
    vector<vector<bool>> visited(ROW, vector<bool>(COL, false));
    bool isPuyoRemoved = false;

    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j) {
            // 빈칸이 아니거나 / 방문하지 않았거나 / 회색이 아니면
            if (board[i][j] != "0" && !visited[i][j] && board[i][j]!="Gray") {
                queue<pair<int, int>> puyoQueue; // 연결된 뿌요를 저장할 큐

                // 현재 위치에서 연결된 뿌요가 있는지 확인
                checkPuyo(i, j, board[i][j], visited, puyoQueue);

                // 연결된 뿌요 개수가 4 이상이면
                if (puyoQueue.size() >= 4) {
                    isPuyoRemoved = true; // 뿌요 제거됨

                    // 큐에 저장된 뿌요 제거
                    while (!puyoQueue.empty()) {
                        int r = puyoQueue.front().first; // 큐 맨 앞에 있는 뿌요의 행 위치
                        int c = puyoQueue.front().second; // 큐의 맨 앞에 있는 뿌요의 열 위치

                        // 뿌요 큐에서 제거 및 빈칸으로 설정
                        puyoQueue.pop();
                        board[r][c] = "0";

                        // 뿌요가 양수인경우 블럭으로 설정 및 점수 증가
                        // if (stoi(board[r][c]) > 0) {
                        //     board[r][c] = "0";
                        //     score++;
                        // }
                        score++;

                    }
                }
            }

        }
    }
    // 뿌요가 제거되면 새로운 뿌요 생성
    if (isPuyoRemoved) {
        createNewPuyo();
    }
}


// 뿌요가 연결되어 있는지 확인하는 함수
void PuyopuyoGame::checkPuyo(int row, int col, string color, vector<vector<bool>>& visited, queue<pair<int, int>>& puyoQueue) {
    static const int dx[] = {1, 0, -1, 0}; // 행의 방향 벡터
    static const int dy[] = {0, 1, 0, -1}; // 열의 방향 벡터

    // BFS를 위한 큐
    queue<pair<int, int>> q;
    q.push({row, col}); // 시작 위치 삽입
    puyoQueue.push({row, col}); // 연결된 뿌요 큐에도 삽입
    visited[row][col] = true; // 방문 표시

    // BFS를 활용하여 연결된 모든 뿌요를 탐색
    while (!q.empty()) {
        int r = q.front().first; // 현재 위치의 행
        int c = q.front().second; // 현재 위치의 열
        q.pop(); // 큐에서 제거

        // 상하 죄우로 탐색
        for (int k = 0; k < 4; ++k) {
            int nr = r + dx[k]; // 다음 위치의 행
            int nc = c + dy[k]; // 다음 위치의 열

            // 다음 위치가 보드 범위를 벗어나면 / 방문하지 않았으면 / 같은 색의 뿌요라면
            if (nr >= 0 && nr < ROW && nc >= 0 && nc < COL && !visited[nr][nc] && board[nr][nc] == color) {
                q.push({nr, nc}); // 다음 위치를 큐에 삽입
                puyoQueue.push({nr, nc}); // 연결된 뿌요 큐에도 삽입
                visited[nr][nc] = true; // 방문 표시
            }
        }
    }
}

// 중력을 작용하게 하는 함수
void PuyopuyoGame::applyGravity() {
    for (int col = 0; col < COL; ++col) {
        for (int row = ROW - 1; row > 0; --row) {
            // 현재 위치에 빈 칸이 있는 경우
            if (board[row][col] == "0"||board[row][col]==puyo_color) {
                // 빈 칸 위에 있는 뿌요들을 찾아서 아래로 이동시킴
                for (int i = row - 1; i >= 0; --i) {
                    if (board[i][col] != "0" && board[i][col]!="Gray") {
                        // 뿌요를 아래로 이동시킴
                        board[row][col] = board[i][col];
                        board[i][col] = "0";
                        break;
                    }
                }
            }
        }
    }
}

// PuyopuyoGame class 생산자
PuyopuyoGame::PuyopuyoGame() : Game(14, 6, 2)
{
    srand((time(nullptr)));

    next[0]="RR";
    next[1]="YY";

    axis_row = 1;
    axis_col = 2;

    score = 0;


    // 보드 흰 색으로 설정
    for(int i = 0; i < ROW; i++)
        for(int j =0; j < COL; j++)
            board[i][j] = "White";

    // 보드의 빈 칸의 값을 0으로 설정
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            board[i][j] = "0";

    // 뿌요가 처음에 위치하는 곳을 회색으로 설정 / 게임 필드 아님
    for(int i=0; i<2; i++)
        for(int j=0; j<COL; j++)
            board[i][j]="Gray";

    // 뿌요 색 설정
    for (int i = 0; i < LENGTH; i++) {
        // 두 색상 생성
        string randomPuyo = createRandomPuyo();
        string randomPuyo2 = createRandomPuyo();

        next[i][0] = randomPuyo[0];

        // 다른 색이 나올 때까지 반복
        while (randomPuyo2[0] == next[i][0]) {
            randomPuyo2 = createRandomPuyo();
        }
        next[i][1] = randomPuyo2[1];

        // 큐에 뿌요 쌍 삽입
        std::string* puyoPair = new std::string[2];
        puyoPair[i] = next[i];

        nextQueue.push(puyoPair);

        if(nextQueue.empty()){
            nextQueue.push(puyoPair);
        }
    }


    // 축 및 모양을 고정하고 표시
    fixPuyoShapeAndAxis();
    displayNextPuyo();

}

// PuyopuyoGame class 소멸자
PuyopuyoGame::~PuyopuyoGame()
{
    while (!nextQueue.empty()) {
        delete[] nextQueue.front();
        nextQueue.pop();
    }
}



// TetrisGame //
// 램덤한 테트로미노 모양 생성
string TetrisGame::createRandomTetris(){
    const string Tetris_color[7]={"T", "I", "S", "Z", "O", "L", "J"}; // 테트로미노 모양 배열

    srand((time(nullptr)));

    // 램덤한 생성
    int randomIndex = rand()%7;
    string selectTetris = Tetris_color[randomIndex];

    return selectTetris; // "T"와 같이 반환
}

// 테트로미노의 모양 및 축 고정
void TetrisGame::fixTetrisShapeAndAxis() {
    tetris_shape = next[0];

    // 테트로미노 모양에 따라 첫 시작 보드 위치 및 축 정의
    if(tetris_shape=="T"){
        board[1][3]="Gray";
        board[1][4]="Gray";
        board[1][5]="Gray";
        board[1][6]="Gray";

        axis_row=1;
        axis_col=4;
    }

    else if(tetris_shape=="I"){
        board[1][2]="Gray";
        board[1][3]="Gray";
        board[1][4]="Gray";
        board[1][5]="Gray";

        axis_row=1.5;
        axis_col=3.5;
    }
    else if(tetris_shape=="S"){
        board[0][4]="Gray";
        board[0][5]="Gray";
        board[1][3]="Gray";
        board[1][4]="Gray";

        axis_row=1;
        axis_col=4;
    }
    else if(tetris_shape=="Z"){
        board[0][2]="Gray";
        board[0][3]="Gray";
        board[1][3]="Gray";
        board[1][4]="Gray";

        axis_row=1;
        axis_col=3;
    }
    else if(tetris_shape=="O"){
        board[0][4]="Gray";
        board[0][5]="Gray";
        board[1][4]="Gray";
        board[1][5]="Gray";
    }
    else if(tetris_shape=="L"){
        board[0][5]="Gray";
        board[1][3]="Gray";
        board[1][4]="Gray";
        board[1][5]="Gray";

        axis_row=1;
        axis_col=4;
    }
    else if(tetris_shape=="J"){
        board[0][3]="Gray";
        board[1][3]="Gray";
        board[1][4]="Gray";
        board[1][5]="Gray";

        axis_row=1;
        axis_col=4;
    }
}

// 다음 테트로미노 표시
void TetrisGame::displayNextTetris() {
    int displayRow = axis_row; // 표시할 행의 위치
    int displayCol = axis_col; // 표시할 열의 중앙 위치

    board[displayRow][displayCol] = tetris_shape; // 첫 번째 테트로미노
}

// 새로운 테트로미노 생성
void TetrisGame::createNewTetris() {
    // 큐가 비어있다면 넣기
    if(nextQueue.empty()){
        fillQueue();
    }

    // 램덤한 테트로미노 모양 생성
    string newTetromino = createRandomTetris();
    // 큐에 새로운 뿌요 색상 추가
    nextQueue.push(newTetromino);

    // 기존의 테트로미노 색상 업데이트
    string currentTetromino = nextQueue.front();
    nextQueue.pop();

    next[0]=currentTetromino;
    next[0]=next[1];
    next[1]=next[2];
    next[2]=next[3];
    next[3]=next[4];
    next[4]=createRandomTetris();

    // 축 및 모양 고정 및 표시
    fixTetrisShapeAndAxis();
    displayNextTetris();

}


// 큐 채우기
void TetrisGame::fillQueue(){
    // 큐의 크기가 5보다 작다면 새로운 램덤한 테트로미노 채우기
    while(nextQueue.size()<5){
        nextQueue.push(createRandomTetris());
    }
}


TetrisGame::TetrisGame() : Game(22, 10, 5)
{
    next[0] = "T";
    next[1] = "I";
    next[2] = "S";
    next[3] = "Z";
    next[4] = "O";


    score = 0;

    // 보드를 흰 색으로 설정
    for(int i = 0; i < ROW; i++)
        for(int j =0; j < COL; j++)
            board[i][j] = "White";

    // 보드의 값을 0으로 설정
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            board[i][j] = "0";

    // 처음 테트로미노 나오는 칸 설정 / 게임 필드 아님
    for(int i=0; i<2; i++){
        for(int j=0; j<10; j++){
            board[i][j]="silver";
        }
    }

    // 램덤한 테트로미노 큐에 삽입
    for (int i = 0; i < LENGTH; i++) {
        next[i] = createRandomTetris();
        nextQueue.push(next[i]);
    }

    next[0]=nextQueue.front();
    nextQueue.pop();
\

    for (int i = 0; i < LENGTH; i++) {
        string tetromino = createRandomTetris();
        next[i] = "Gray";
        next[i] = tetromino;

        if (nextQueue.empty()) {
            nextQueue.push(tetromino);
        }
    }

    // 테트로미노의 축과 모양 고정 및 표시
    fixTetrisShapeAndAxis();
    displayNextTetris();


}

TetrisGame::~TetrisGame()
{
    while (!nextQueue.empty()) {
        nextQueue.pop();
    }
}

// PuyopuyoTetrisGame //
PuyopuyoTetrisGame::PuyopuyoTetrisGame() : Game(16, 8, 5)
{
    next[0] = "RY"; //Red Yellow
    next[1] = "GB"; //Green Blue
    next[2] = "PP"; //Purple Purple
    next[3] = "L";
    next[4] = "J";

    for(int i = 0; i < ROW; i++)
        for(int j =0; j < COL; j++)
            board[i][j] = "White";



    axis_row = 2;
    axis_col = 6;

    score = 2;
}

PuyopuyoTetrisGame::~PuyopuyoTetrisGame()
{
    ;
}

