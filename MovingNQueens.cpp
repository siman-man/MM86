#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <cassert>

using namespace std;
typedef long long ll;

const int MAX_QUEEN = 200;
const int MAX_SIZE  = 100000;
const int OFFSET    = 30000;
const int UNDEFINED = -1;
const double PI = 3.141592653589793;

struct Queen {
  int id;
  int y;
  int originalY;
  int beforeY;
  int x;
  int originalX;
  int beforeX;
  int degree;
  int value;

  Queen(int id = UNDEFINED, int y = UNDEFINED, int x = UNDEFINED){
    this->id  = id;
    this->y   = y;
    this->x   = x;
  }

  bool operator >(const Queen &e) const{
    return value > e.value;
  }    
};

struct EvalResult {
  int value;
  int y;
  int x;

  EvalResult(int value = UNDEFINED, int y = UNDEFINED, int x = UNDEFINED){
    this->value = value;
    this->y = y;
    this->x = x;
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

struct Cell {
  int y;
  int x;
  int dist;
  int value;
  int pd;
  int py;
  int px;

  Cell(int y = UNDEFINED, int x = UNDEFINED, int dist = 0, int value = UNDEFINED, int parentDirect = UNDEFINED){
    this->y     = y;
    this->x     = x;
    this->dist  = dist;
    this->value = value;
    this->pd    = parentDirect;
  }

  bool operator >(const Cell &e) const{
    return value + dist > e.value + e.dist;
  }    
};


struct Edge {
  int from;
  int to;
  int cost;

  Edge(int from = UNDEFINED, int to = UNDEFINED, int cost = UNDEFINED){
    this->from  = from;
    this->to    = to;
    this->cost  = cost;
  }
};

Queen queenList[MAX_QUEEN];
int N;
int board[MAX_QUEEN][MAX_QUEEN];
int cnt = 0;
int DY[13] = { 0, -1, -1, -1, -1,  0,  0,  1,  1,  1,  1,  0,  0};
int DX[13] = { 1,  1,  0,  0, -1, -1, -1, -1,  0,  0,  1,  1,  0};
int MY[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
int MX[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

int horizontalCnt[MAX_SIZE];
int verticalCnt[MAX_SIZE];
int diagonalUpCnt[MAX_SIZE];
int diagonalDownCnt[MAX_SIZE];

int bestRows[MAX_QUEEN];
int bestCols[MAX_QUEEN];

map<ll, bool> queenCheck;

Coord center;

unsigned long long xor128() {
  static unsigned long long rx=123456789, ry=362436069, rz=521288629, rw=88675123;
  unsigned long long rt = (rx ^ (rx<<11));
  rx=ry; ry=rz; rz=rw;
  return (rw=(rw^(rw>>19))^(rt^(rt>>8)));
}

int matrix[MAX_QUEEN][MAX_QUEEN];

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

    // 行列の初期化
    memset(matrix, 0, sizeof(matrix));

    // クイーンの情報の初期化を行う
    for(int id = 0; id < N; ++id){
      row = queenRows[id];
      col = queenCols[id];

      queenList[id] = Queen(id, row, col);
      queenList[id].originalY = row;
      queenList[id].originalX = col;
      setQueen(id, row, col);
      fprintf(stderr,"id = %d, row = %d, col = %d\n", id, row, col);

      ll hash = calcHash(row, col);
      queenCheck[hash] = true;

      board[row][col] = 1;
    }

    // 各クイーンの重心を求める
    center = calcCenterCoord();

    fprintf(stderr,"N = %d\n", N);
  }

  void updateBestPositions(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      bestRows[id] = queen->y;
      bestCols[id] = queen->x;
    }
  }

  /*
   * Queenの移動を行う
   */
  void moveQueen(int id, int y, int x){
    Queen *queen = getQueen(id);
    removeQueen(queen->y, queen->x);
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
  void removeQueen(int y, int x){ 
    horizontalCnt[OFFSET + y] -= 1;
    verticalCnt[OFFSET + x] -= 1;
    diagonalUpCnt[OFFSET + y - x] -= 1;
    diagonalDownCnt[OFFSET + y + x] -= 1;
  }

  ll calcHash(int y, int x){
    ll hash = (OFFSET + y + 1) * OFFSET + x;
    assert(hash >= 0);
    return hash;
  }

  // ボード全体のスコア計算
  int calcScoreAll(){
    int score = 0;
    int cnt;
    bool success = true;  // 各クイーンが重複していないかどうかのフラグ

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);  

      cnt = horizontalCnt[OFFSET + queen->y] + verticalCnt[OFFSET + queen->x];
      cnt += diagonalUpCnt[OFFSET + queen->y - queen->x] + diagonalDownCnt[OFFSET + queen->y + queen->x];

      if(cnt > 4) success = false;

      score -= cnt;
      score -= max(abs(queen->y - center.y), abs(queen->x - center.x));
    }

    return score + success * 100000;
  }

  // Queen単体のスコア計算
  int calcScoreSingle(int id){
    int score = 0;
    int cnt;
    bool success = true;
    Queen *queen = getQueen(id);  

    cnt = horizontalCnt[OFFSET + queen->y] + verticalCnt[OFFSET + queen->x];
    cnt += diagonalUpCnt[OFFSET + queen->y - queen->x] + diagonalDownCnt[OFFSET + queen->y + queen->x];
    score -= 4 * cnt;
    //score -= max(abs(queen->y - center.y), abs(queen->x - center.x));

    if(cnt > 16) success = false;

    return score + 10 * success;
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

  // 一番最初の行動
  void firstMove(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      int direct = checkDirection(queen);
      int dist = max(abs(queen->y - center.y), abs(queen->x - center.x));

      int ny = queen->y + N * dist * DY[direct];
      int nx = queen->x + N * dist * DX[direct];
      moveQueen(id, ny, nx);
    }
  }

  // 評価を行う前に行う処理
  void eachTurnProc(int id){
    Queen *queen = getQueen(id);
    queen->beforeY = queen->y;
    queen->beforeX = queen->x;
  }

  // 行動のロールバック
  void rollback(int id){
    Queen *queen = getQueen(id);
    moveQueen(id, queen->beforeY, queen->beforeX);
  }

  vector<string> rearrange(vector<int> queenRows, vector<int> queenCols){
    vector<string> ret;

    init(queenRows, queenCols);

    fprintf(stderr,"centerY = %d, centerX = %d\n", center.y, center.x);

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      int dy = center.y - queen->y;
      int dx = center.x - queen->x;
      int degree = 180 - (int)(atan2(dy,dx) * 180.0 / PI);
      queen->degree = degree;
      //fprintf(stderr,"id = %d, dist = %4.1f, degree = %d, direct = %d\n", id, dist, queen->degree, direct);
    }

    // 初期移動
    firstMove();

    int maxScore = INT_MIN;

    for(int i = 0; i < 100000; ++i){
      // ランダムにIDを選択
      int id = xor128() % N;

      assert(id >= 0);
      eachTurnProc(id);

      int r = xor128() % N;
      assert(r >= 0);

      EvalResult result = updateQueen(id);
      moveQueen(id, result.y, result.x);
      //swapQueen(id, (r % N));

      int score = calcScoreAll();

      if(maxScore < score){
        maxScore = score;
        updateBestPositions();
      }
    }

    for(int id = 0; id < N; ++id){
      fprintf(stderr,"nodeId = %d, y = %d, x = %d\n", id, bestRows[id], bestCols[id]);
    }


    resetAllPosition();
    vector<int> positionList = checkPosition();

    priority_queue< Queen, vector<Queen>, greater<Queen>  > que;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      Queen q(id, queen->y, queen->x);
      q.value = -1 * (abs(queen->y - center.y) + abs(queen->x - center.x));

      que.push(q);
    }

    while(!que.empty()){
      Queen q = que.top(); que.pop();

      Queen *queen = getQueen(q.id);
      int nodeId = positionList[queen->id];

      fprintf(stderr,"id = %d, queen->y = %d, queen->x = %d, bestRow = %d, bestCol = %d\n", 
          queen->id, queen->y, queen->x, bestRows[nodeId], bestCols[nodeId]);

      /*
      queen->y = bestRows[nodeId];
      queen->x = bestCols[nodeId];
      */

      searchPath(queen->id, bestRows[nodeId], bestCols[nodeId], ret);
      ll hash = calcHash(queen->y, queen->x);
      queenCheck[hash] = false;
      hash = calcHash(bestRows[nodeId], bestCols[nodeId]);
      queenCheck[hash] = true;

      fprintf(stderr,"id = %d, position = %d\n", queen->id, positionList[queen->id]);
    }

    int score = calcScoreAll();
    fprintf(stderr,"Current score = %d\n", score);

    //createAnswer();

    //showBoard();

    return ret;
  }

  vector<int> checkPosition(){
    vector<int> position(N, UNDEFINED);
    priority_queue< Queen, vector<Queen>, greater<Queen>  > que;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      Queen q(id, queen->y, queen->x);
      q.value = abs(queen->y - center.y) + abs(queen->x - center.x);

      que.push(q);
    }

    map<int, bool> checkList;

    while(!que.empty()){
      Queen q = que.top(); que.pop();
      int minDist = INT_MAX;
      int bestPosition = UNDEFINED;

      for(int nodeId = 0; nodeId < N; ++nodeId){
        if(checkList[nodeId]) continue;

        int dist = max(abs(q.y - bestRows[nodeId]), abs(q.x - bestCols[nodeId]));

        if(minDist > dist){
          minDist = dist;
          bestPosition = nodeId;
        }
      }

      position[q.id] = bestPosition;
      checkList[bestPosition] = true;
    }

    return position;
  }

  void searchPath(int id, int destY, int destX, vector<string> &ret){
    Queen *queen = getQueen(id);
    ll hash;
    priority_queue< Cell, vector<Cell>, greater<Cell>  > que;
    que.push(Cell(queen->y, queen->x, 0, 0));
    map<ll, Cell> history;
    map<int, bool> checkList;

    while(!que.empty()){
      Cell c = que.top(); que.pop();
      hash = calcHash(c.y, c.x);

      if(checkList[hash]) continue;
      checkList[hash] = true;

      if(c.y == destY && c.x == destX){
        stack<string> list;
        list.push(int2str(id) + " " + int2str(c.y) + " " + int2str(c.x));

        while(c.pd != UNDEFINED){
          hash = calcHash(c.y, c.x);
          Cell pc = history[hash];
          //fprintf(stderr,"c.y = %d, c.x = %d, c.pd = %d, pc.pd = %d, hash = %lld\n", c.y, c.x, c.pd, pc.pd, hash);

          if(c.pd != pc.pd && pc.pd != UNDEFINED){
            list.push(int2str(id) + " " + int2str(pc.y) + " " + int2str(pc.x));
          }

          c = pc;
        }

        while(!list.empty()){
          string str = list.top(); list.pop();
          ret.push_back(str);
        }

        return;
      }

      for(int i = 0; i < 8; ++i){
        int ny = c.y + MY[i];
        int nx = c.x + MX[i];
        hash = calcHash(ny, nx);

        if(!queenCheck[hash]){
          int cost = abs(ny - destY) + abs(nx - destX);
          Cell cell(ny, nx, c.dist+1, cost, i);

          if(!checkList[hash]){
            history[hash] = c;
          }
          que.push(cell);
        }
      }
    }
  }

  void initMatrix(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      for(int nodeId = 0; nodeId < N; ++nodeId){
        int cost = max(abs(queen->y - bestRows[nodeId]), abs(queen->x - bestCols[nodeId]));

        matrix[id][nodeId] = cost;
      }
    }
  }

  vector<int> createAnswer(){
    int p, q;

    initMatrix();

    vector<int> fx(N, INT_MIN);
    vector<int> fy(N, 0);
    vector<int> x(N, UNDEFINED);  // 左側頂点に対応する右側頂点
    vector<int> y(N, UNDEFINED);  // 右側頂点に対応する左側頂点

    // 各クイーンの最小値を記録
    for(int id = 0; id < N; ++id){
      for(int j = 0; j < N; ++j){
        fx[id] = min(fx[id], matrix[id][j]);
      }
    }

    for(int id = 0; id < N;){
      vector<int> t(N, UNDEFINED);
      vector<int> s(N+1, id);

      for(p = q = 0; p <= q && x[id] < 0; ++p){
        for(int k = s[p], j = 0; j < N && x[id] < 0; ++j){
          if(fx[k] + fy[j] == matrix[k][j] && t[j] < 0){
            s[++q] = y[j];
            t[j] = k;

            if(s[q] < 0){
              for(p = j; p >= 0; j = p){
                y[j] = k = t[j];
                p = x[k];
                x[k] = j;
              }
            }
          }
        }
      }

      // 最適なマッチングがまだ決まっていない場合
      if(x[id] < 0){
        int d = INT_MIN;

        for(int k = 0; k <= q; ++k){
          for(int j = 0; j < N; ++j){
            if(t[j] < 0){
              d = min(d, fx[s[k]] + fy[j] - matrix[s[k]][j]);
            }
          }

          for(int j = 0; j < N; ++j){
            fy[j] += (t[j] < 0 ? 0 : d);
          }
          for(int k = 0; k <= q; ++k){
            fx[s[k]] -= d;
          }
        }
      }else{
        ++id;
      }
    }

    return x;
  }

  // 全てのクイーンの位置を元に戻す
  void resetAllPosition(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      queen->y = queen->originalY;
      queen->x = queen->originalX;
    }
  }

  EvalResult updateQueen(int id){
    Queen *queen = getQueen(id);
    int maxScore = INT_MIN;
    int bestY;
    int bestX;
    int score;

    for(int i = 0; i < 8; ++i){
      int ny = queen->y + MY[i];
      int nx = queen->x + MX[i];
      moveQueen(id, ny, nx);

      score = calcScoreSingle(id);

      if(maxScore < score){
        maxScore = score;
        bestY = ny;
        bestX = nx;
      }

      rollback(id);
    }

    return EvalResult(maxScore, bestY, bestX);
  }

  // クイーンの一部座標の交換を行う
  int swapQueen(int idA, int idB){
    if(idA == idB) return 0;

    Queen *queenA = getQueen(idA);
    Queen *queenB = getQueen(idB);

    int r = xor128() % 100;
    int tmp;

    if(r > 50){
      tmp = queenA->x;
      queenA->x = queenB->x;
      queenB->x = tmp;
    }else{
      tmp = queenA->y;
      queenA->y = queenB->y;
      queenB->y = tmp;
    }
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
