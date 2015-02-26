#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <map>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <cassert>

using namespace std;

const int MAX_QUEEN = 200;
const int MAX_SIZE  = 100000;
const int OFFSET    = 50000;
const int UNDEFINED = -1;
const double PI = 3.141592653589793;

struct Queen {
  int id;
  int y;
  int originalY;
  int x;
  int originalX;
  int degree;

  Queen(int id = UNDEFINED, int y = UNDEFINED, int x = UNDEFINED){
    this->id  = id;
    this->y   = y;
    this->x   = x;
  }
};

struct Coord {
  int y;
  int x;

  Coord(int y = UNDEFINED, int x = UNDEFINED){
    this->y = y;
    this->x = x;
  }
};

Queen queenList[MAX_QUEEN];
int N;
int board[MAX_QUEEN][MAX_QUEEN];
int cnt = 0;
int DY[13] = { 0, -1, -1, -1, -1,  0,  0,  1,  1,  1,  1,  0,  0};
int DX[13] = { 1,  1,  0,  0, -1, -1, -1, -1,  0,  0,  1,  1,  0};

int horizontalCnt[MAX_SIZE];
int verticalCnt[MAX_SIZE];
int diagonalUpCnt[MAX_SIZE];
int diagonalDownCnt[MAX_SIZE];

Coord center;

unsigned long long xor128() {
  static unsigned long long rx=123456789, ry=362436069, rz=521288629, rw=88675123;
  unsigned long long rt = (rx ^ (rx<<11));
  rx=ry; ry=rz; rz=rw;
  return (rw=(rw^(rw>>19))^(rt^(rt>>8)));
}

class MovingNQueens {
  public:
  void init(vector<int> queenRows, vector<int> queenCols){
    int row;
    int col;

    // サイズの取得
    N = queenRows.size();
    // ボードの初期化
    memset(board, 0, sizeof(board)); 
    // 重複個数リストの初期化
    memset(horizontalCnt, 0, sizeof(horizontalCnt));
    memset(verticalCnt, 0, sizeof(verticalCnt));
    memset(diagonalUpCnt, 0, sizeof(diagonalUpCnt));
    memset(diagonalDownCnt, 0, sizeof(diagonalDownCnt));

    // クイーンの情報の初期化を行う
    for(int id = 0; id < N; ++id){
      row = queenRows[id];
      col = queenCols[id];

      queenList[id] = Queen(id, row, col);
      queenList[id].originalY = row;
      queenList[id].originalX = col;
      fprintf(stderr,"id = %d, row = %d, col = %d\n", id, row, col);
      board[row][col] = 1;
    }

    // 各クイーンの重心を求める
    center = calcCenterCoord();

    fprintf(stderr,"N = %d\n", N);
  }

  /*
   * Queenの移動を行う
   */
  void moveQueen(int id, int y, int x){
    Queen *queen = getQueen(id);
    removeQueen(id, queen->y, queen->x);
    setQueen(id, y, x);
  }

  // クイーンを設置
  void setQueen(int id, int y, int x){
    Queen *queen = getQueen(id);
    queen->y = y;
    queen->x = x;

    horizontalCnt[OFFSET + y] += 1;
    verticalCnt[OFFSET + x] += 1;
    diagonalUpCnt[OFFSET + y - x] += 1;
    diagonalDownCnt[OFFSET + y + x] += 1;
  }

  // クイーンの削除
  void removeQueen(int id, int y, int x){ 
    Queen *queen = getQueen(id);

    horizontalCnt[OFFSET + y] -= 1;
    verticalCnt[OFFSET + x] -= 1;
    diagonalUpCnt[OFFSET + y - x] -= 1;
    diagonalDownCnt[OFFSET + y + x] -= 1;
  }

  // ボード全体のスコア計算
  int calcScoreALl(){
    int score = 0;
    bool success = true;  // 各クイーンが重複していないかどうかのフラグ

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);  
      score -= horizontalCnt[queen->y];
      score -= verticalCnt[queen->x];
      score -= diagonalUpCnt[queen->y - queen->x];
      score -= diagonalDownCnt[queen->y + queen->x];
      score -= max(abs(queen->y - center.y), abs(queen->x - center.x));
    }

    return score + success * 1000;
  }

  // 最初に重心から離れる方向を決める
  int checkDirection(Queen *queen){
    if(queen->degree != 180){
      return queen->degree / 30;
    }else{
      if(queen->x > center.x){
        return 0;
      }else if(queen->x < center.x){
        return 5;
      }else if(queen->y > center.y){
        return 8;
      }else if(queen->y < center.y){
        return 2;
      }else{
        return 12;
      }
    }
  }

  vector<string> rearrange(vector<int> queenRows, vector<int> queenCols){
    vector<string> ret;

    init(queenRows, queenCols);

    ret.push_back(int2str(0) + " " + int2str(3) + " " + int2str(1));
    fprintf(stderr,"centerY = %d, centerX = %d\n", center.y, center.x);

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      int dy = center.y - queen->y;
      int dx = center.x - queen->x;
      int degree = 180 - (int)(atan2(dy,dx) * 180.0 / PI);
      queen->degree = degree;
      int direct = checkDirection(queen);
      double dist = sqrt(dy*dy + dx*dx);
      fprintf(stderr,"id = %d, dist = %4.1f, degree = %d, direct = %d\n", id, dist, queen->degree, direct);
    }

    int score = calcScoreALl();
    fprintf(stderr,"Score = %d\n", score);

    showBoard();

    return ret;
  }

  /*
   * 重心の座標を調べる
   */
  Coord calcCenterCoord(){
    int y = 0;
    int x = 0;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      y += queen->y;
      x += queen->x;
    }

    return Coord(y/N, x/N);
  }

  /*
   * 重なっているクイーンがいるかどうかを調べる
   */
  bool isOver(){
    map<int, bool> horizontal;
    map<int, bool> vertical;
    map<int, bool> diagonal1;
    map<int, bool> diagonal2;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      if(horizontal[queen->y]) return false;
      if(vertical[queen->x]) return false;
      if(diagonal1[queen->x + queen->y]) return false;
      if(diagonal2[queen->x - queen->y]) return false;

      horizontal[queen->y] = true;
      vertical[queen->x] = true;
      diagonal1[queen->x + queen->y] = true;
      diagonal2[queen->x - queen->y] = true;
    }

    return true;
  }

  Queen* getQueen(int id){
    return &queenList[id];
  }

  string int2str(int i){
    stringstream ss;
    ss << i;
    return ss.str();
  }

  void showBoard(){
    fprintf(stderr,"\n");

    for(int y = 0; y < N; ++y){
      for(int x = 0; x < N; x++){
        fprintf(stderr,"+-"); 
      }
      fprintf(stderr,"+\n");

      for(int x = 0; x < N; ++x){
        fprintf(stderr,"|");

        fprintf(stderr,"%c", (board[y][x] == 0)? ' ' : 'o');
      }
      fprintf(stderr,"|\n");
    }

    for(int x = 0; x < N; x++){
      fprintf(stderr,"+-"); 
    }
    fprintf(stderr,"+\n");
  }
};

int main(){
  int len, row, col;
  vector<int> queenRows;
  vector<int> queenCols;
  vector<string> ret;
  MovingNQueens mq;

  cin >> len;
  for(int i = 0; i < len; ++i){
    cin >> row;
    queenRows.push_back(row);
  }

  cin >> len;
  for(int i = 0; i < len; ++i){
    cin >> col;
    queenCols.push_back(col);
  }

  ret = mq.rearrange(queenRows, queenCols);

  len = ret.size();
  cout << len << endl;
  for(int i = 0; i < len; ++i){
    cout << ret[i] << endl;
    fprintf(stderr,"%s\n",ret[i].c_str());
  }
}
