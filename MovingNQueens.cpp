#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include <limits.h>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <cassert>

using namespace std;
typedef long long ll;

const int MAX_QUEEN = 250;
const int MAX_SIZE  = 50000;
const int INF       = INT_MAX;
const ll  OFFSET    = 30000;
const int UNDEFINED = -1;
const double PI = 3.141592653589793;

ll timeLimit = 9500;
ll currentTime;

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

ll getTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  ll result =  tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
  return result;
}

struct Edge {
  int to;
  int cap;
  int cost;
  int rev;

  Edge(int to = UNDEFINED, int cap = UNDEFINED, int cost = UNDEFINED, int rev = UNDEFINED){
    this->to    = to;
    this->cap   = cap;
    this->cost  = cost;
    this->rev   = rev;
  }
};

vector<Edge> G[MAX_QUEEN];

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

vector<int> selectedNodeId(MAX_QUEEN, 0);

map<ll, int> queenCheck;

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
      setQueen(id, row, col);
      //fprintf(stderr,"id = %d, row = %d, col = %d\n", id, row, col);

      ll hash = calcHash(row, col);
      queenCheck[hash] += 1;

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

    ll hash = calcHash(y, x);
    queenCheck[hash] += 1;

    horizontalCnt[OFFSET + y] += 1;
    verticalCnt[OFFSET + x] += 1;
    diagonalUpCnt[OFFSET + y - x] += 1;
    diagonalDownCnt[OFFSET + y + x] += 1;
  }

  // クイーンの削除
  inline void removeQueen(int y, int x){ 
    ll hash = calcHash(y, x);
    queenCheck[hash] -= 1;

    --horizontalCnt[OFFSET + y];
    --verticalCnt[OFFSET + x];
    --diagonalUpCnt[OFFSET + y - x];
    --diagonalDownCnt[OFFSET + y + x];
  }

  ll calcHash(int y, int x){
    ll hash = ((OFFSET + y + 1) * OFFSET) + x;
    //assert(hash >= 0);
    return hash;
  }

  // ボード全体のスコア計算
  int calcScoreAll(){
    //fprintf(stderr,"calcScoreAll =>\n");
    int score = 0;
    int cnt = 0;
    bool success = true;  // 各クイーンが重複していないかどうかのフラグ

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);  

      cnt = horizontalCnt[OFFSET + queen->y] + verticalCnt[OFFSET + queen->x];
      cnt += diagonalUpCnt[OFFSET + queen->y - queen->x] + diagonalDownCnt[OFFSET + queen->y + queen->x];

      if(cnt > 4){
        success = false;
      }

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

    if(cnt > 4) success = false;

    score -= 2 * cnt;
    //score -= max(abs(queen->y - center.y), abs(queen->x - center.x));

    return score + 100 * success;
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

      int ny = queen->y + (N/8) * dist * DY[direct];
      int nx = queen->x + (N/8) * dist * DX[direct];
      moveQueen(id, ny, nx);
    }
  }

	void randomMove(){
    for(int id = 0; id < N; ++id){
			int dy = (xor128() % (2*N)) - N;
			int dx = (xor128() % (2*N)) - N;

      int ny = center.y + dy;
      int nx = center.x + dx;
      moveQueen(id, ny, nx);
    }
	}

  // 少しだけ全体を揺らす
  void littleMoveAll(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      int direct = xor128() % 8;

      int ny = queen->y + (N/8) * MY[direct];
      int nx = queen->x + (N/8) * MX[direct];

      moveQueen(id, ny, nx);
    }
  }

  // 評価を行う前に行う処理
  void eachTurnProc(int id){
    //fprintf(stderr,"eachTurnProc =>\n");
    Queen *queen = getQueen(id);
    queen->beforeY = queen->y;
    queen->beforeX = queen->x;
  }

  // 行動のロールバック
  void rollback(int id){
    //fprintf(stderr,"rollback =>\n");
    Queen *queen = getQueen(id);
    moveQueen(id, queen->beforeY, queen->beforeX);
  }

  vector<string> rearrange(vector<int> queenRows, vector<int> queenCols){
    vector<string> ret;

    init(queenRows, queenCols);

    //fprintf(stderr,"centerY = %d, centerX = %d\n", center.y, center.x);

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

    int bestScore = INT_MIN;
    int goodScore = INT_MIN;
    const ll startTime = getTime();
    const ll endTime = startTime + timeLimit;
    currentTime = startTime;
    int turn = 0;
    double T = 10000.0; 
    double k = 10.0;
    double alpha = 0.998;
    int notChangeCnt = 0;
    int idA, idB;

    //for(int i = 0; i < 400000; ++i){
    while(currentTime < endTime){
      ++turn;
      //fprintf(stderr,"turn = %d start =>\n", turn);
      idB = UNDEFINED;

      // ランダムにIDを選択
      int id = xor128() % N;

      //assert(id >= 0);
      eachTurnProc(id);

      int r = xor128() % 100;
      //assert(r >= 0);

      if(r > 3){
        EvalResult result = updateQueen(id);
        moveQueen(id, result.y, result.x);
      }else if(goodScore > 0){
			//}else{
      //}else if(notChangeCnt > 10){
        idA = id;
        idB = (xor128() % N);
      	eachTurnProc(idB);
        swapQueen(idA, idB);
        /*
				resetPosition(id);
        EvalResult result = updateQueen(id);
        moveQueen(id, result.y, result.x);
				*/
      }else{
        /*
				resetPosition(id);
        EvalResult result = updateQueen(id);
        moveQueen(id, result.y, result.x);
        */
      }

      int score = calcScoreAll();

      if(bestScore < score){
        bestScore = score;
        updateBestPositions();
      }

      double rate = exp(-(goodScore-score)/(k*T));

      if(goodScore < score){
        goodScore = score;
        notChangeCnt = 0;
      }else if(T > 0.001 && xor128() % 100 < 100.0 * rate){
        //fprintf(stderr,"turn = %d, rate = %f\n", turn, rate);
        goodScore = score;
        notChangeCnt = 0;
      }else{
        ++notChangeCnt;
        rollback(id);

        if(idB != UNDEFINED){
          rollback(idB);
        }

        if(notChangeCnt > 200){
          //fprintf(stderr,"ReBrun =>\n");
    			resetAllPosition();
    			firstMove();
          //littleMoveAll();
					//randomMove();
         	T = 10000.0;
					notChangeCnt = 0;
					
					if(goodScore < 0){
						//alpha += 0.001;
					}else{
						alpha -= 0.001;
					}
        }
      }

			if(turn % 10000 == 0){
      	currentTime = getTime();
			}

      T *= alpha;
      //fprintf(stderr,"turn = %d end =>\n", turn);
    }

    fprintf(stderr,"turn = %d, T = %4.2f\n", turn, T);

    for(int id = 0; id < N; ++id){
      //fprintf(stderr,"nodeId = %d, y = %d, x = %d\n", id, bestRows[id], bestCols[id]);
    }

    resetAllPosition();
    solve();
    vector<int> positionList = checkPosition();

    for(int id = 0; id < N; ++id){
      //fprintf(stderr,"Queen %d -> Node %d\n", id, positionList[id]);
      //fprintf(stderr,"Queen %d -> Node %d\n", id, selectedNodeId[id]);
    }
    positionList = selectedNodeId;

    priority_queue< Queen, vector<Queen>, greater<Queen>  > que;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      Queen q(id, queen->y, queen->x);
      q.value = -1 * (abs(queen->y - center.y) + abs(queen->x - center.x));

      for(int i = 0; i < 8; ++i){
        int ny = queen->y + MY[i];
        int nx = queen->x + MX[i];

        ll hash = calcHash(ny, nx);

        if(queenCheck[hash] > 0){
          q.value += 1;
        }
      }

      que.push(q);
    }

    while(!que.empty()){
      Queen q = que.top(); que.pop();

      Queen *queen = getQueen(q.id);
      int nodeId = positionList[queen->id];

      //fprintf(stderr,"id = %d, queen->y = %d, queen->x = %d, bestRow = %d, bestCol = %d\n", queen->id, queen->y, queen->x, bestRows[nodeId], bestCols[nodeId]);

      /*
      queen->y = bestRows[nodeId];
      queen->x = bestCols[nodeId];
      */

      if(searchPath(queen->id, bestRows[nodeId], bestCols[nodeId], ret)){
        ll hash = calcHash(queen->y, queen->x);
        queenCheck[hash] -= 1;
        hash = calcHash(bestRows[nodeId], bestCols[nodeId]);
        queenCheck[hash] += 1;
        moveQueen(queen->id, bestRows[nodeId], bestCols[nodeId]);
      }else{
        int size = que.size();
        fprintf(stderr,"queue size = %d\n", size);
        Queen q(queen->id, queen->y, queen->x);
        q.value = OFFSET + 2 * (abs(queen->y - center.y) + abs(queen->x - center.x));
        //que.push(q);
      }

      //fprintf(stderr,"id = %d, position = %d\n", queen->id, positionList[queen->id]);
    }

    int score = calcScoreAll();
    fprintf(stderr,"Current score = %d\n", score);

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

      for(int i = 0; i < 8; ++i){
        int ny = queen->y + MY[i];
        int nx = queen->x + MX[i];

        ll hash = calcHash(ny, nx);

        if(queenCheck[hash] > 0){
          q.value -= 1;
        }
      }

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

  bool searchPath(int id, int destY, int destX, vector<string> &ret){
    Queen *queen = getQueen(id);

    if(queen->y == destY && queen->x == destX) return true;

    ll hash;
    priority_queue< Cell, vector<Cell>, greater<Cell>  > que;
    que.push(Cell(queen->y, queen->x, 0, 0));
    map<ll, int> history;
    map<int, bool> checkList;
    int limitDist = 2 * (abs(queen->y - destY) + abs(queen->x - destX));

    while(!que.empty()){
      Cell c = que.top(); que.pop();
      hash = calcHash(c.y, c.x);

      if(checkList[hash] || limitDist < c.dist) continue;
      checkList[hash] = true;

      if(c.y == destY && c.x == destX){
        stack<string> list;
        int direct = UNDEFINED;

        while(queen->y != c.y || queen->x != c.x){
          hash = calcHash(c.y, c.x);
          //fprintf(stderr,"id = %d, c.y = %d, c.x = %d\n", id, c.y, c.x);

          if(direct != history[hash]){
            list.push(int2str(id) + " " + int2str(c.y) + " " + int2str(c.x));
          }

          direct = history[hash];
          c.y -= MY[direct];
          c.x -= MX[direct];
        }

        while(!list.empty()){
          string str = list.top(); list.pop();
          ret.push_back(str);
        }

        return true;
      }

      for(int i = 0; i < 8; ++i){
        int ny = c.y + MY[i];
        int nx = c.x + MX[i];
        hash = calcHash(ny, nx);

        if(queenCheck[hash] == 0 && !checkList[hash]){
          int cost = abs(ny - destY) + abs(nx - destX);
          Cell cell(ny, nx, c.dist+1, cost, i);
          history[hash] = i;
          que.push(cell);
        }
      }
    }

    return false;
  }

  void addEdge(int from, int to, int cap, int cost){
    //fprintf(stderr,"from = %d, to = %d\n", from, to);
    Edge edgeA(to, cap, cost, G[to].size());
    Edge edgeB(from, 0, -cost, G[from].size());
    G[from].push_back(edgeA);
    G[to].push_back(edgeB);
  }

  void solve(){
    //fprintf(stderr,"solve =>\n");
    // マッチンググラフを作成
    int s = N + N + 1;
    int t = s + 1;

    int cost = 0;
    int F = 0;

    for(int i = 0; i < N; i++){
      Queen *queen = getQueen(i);

      for(int j = 0; j < N; ++j){
        int c = max(abs(queen->y - bestRows[j]), abs(queen->x - bestCols[j]));
        addEdge(i, N + j, INF, c);
        cost += c;
      }
    }

    //fprintf(stderr,"Add Queen Edge =>\n");
    // 始点sに対してQueenに辺を張る
    for(int i = 0; i < N; i++){
      addEdge(s, i, 1, 0);
      F += 1;
    }

    //fprintf(stderr,"Add Node Edge =>\n");
    // 終点tに対してNodeに辺を張る
    for(int i = 0; i < N; i++){
      addEdge(N + i, t, 1, 0);
    }
    //fprintf(stderr,"Add Node Edge end =>\n");

    int minCost = minCostFlow(s, t, F);

    fprintf(stderr,"minCost = %d\n", minCost);
  }

  int minCostFlow(int s, int t, int f){
    //fprintf(stderr,"minCostFlow =>\n");
    int res = 0;
    int V = 2 * N + 3;
    int dist[V];  // 最短距離
    int prevv[V]; // 直前の頂点
    int preve[V]; // 直前の辺

    while(f > 0){
      for(int i = 0; i < V; i++){
        dist[i] = INF;
      }
      dist[s] = 0;
      bool update = true;

      while(update){
        update = false;

        for(int v = 0; v < V; v++){
          //fprintf(stderr,"dist[%d] = %d\n", v, dist[v]);
          if(dist[v] == INF) continue;

          for(int i = 0; i < G[v].size(); i++){
            Edge &e = G[v][i];

            //fprintf(stderr,"cap = %d\n", e.cap);

            if(e.cap > 0 && dist[e.to] > dist[v] + e.cost){
              dist[e.to] = dist[v] + e.cost;
              prevv[e.to] = v;
              preve[e.to] = i;
              update = true;
            }
          }
        }
      }

      if(dist[t] == INF){
        // これ以上流せない
        return -1;
      }

      // s-t間最短路に沿って目一杯流す
      int d = f;

      for(int v = t; v != s; v = prevv[v]){
        d = min(d, G[prevv[v]][preve[v]].cap);

        if(0 <= prevv[v] && prevv[v] < N && 0 <= (v - N) && (v - N) < N){
          //fprintf(stderr,"%d -> %d\n", prevv[v], v - N);
          selectedNodeId[prevv[v]] = v - N;
        }
      }
      f -= d;
      res += d * dist[t];
      //fprintf(stderr,"res = %d + %d * %d\n", res, d, dist[t]);
      for(int v = t; v != s; v = prevv[v]){
        Edge &e = G[prevv[v]][preve[v]];
        e.cap -= d;
        G[v][e.rev].cap += d;
      }
    }

    return res;
  }

	void resetPosition(int id){
   	Queen *queen = getQueen(id);
   	moveQueen(id, queen->originalY, queen->originalX);
	}

  // 全てのクイーンの位置を元に戻す
  void resetAllPosition(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      moveQueen(id, queen->originalY, queen->originalX);
    }
  }

  EvalResult updateQueen(int id){
    Queen *queen = getQueen(id);
    int maxScore = INT_MIN;
    int bestY;
    int bestX;
    int score;

    for(int i = 0; i < 8; ++i){
      //int diff = 2;
      int diff = (xor128() % (N/8)) + 1;
      int ny = queen->y + diff * MY[i];
      int nx = queen->x + diff * MX[i];
      moveQueen(id, ny, nx);

      score = calcScoreSingle(id);
      //score = calcScoreAll();

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
  void swapQueen(int idA, int idB){
    if(idA == idB) return;

    Queen *queenA = getQueen(idA);
    Queen *queenB = getQueen(idB);

    int r = xor128() % 100;
    int ay = queenB->y;
    int ax = queenB->x;
    int by = queenA->y;
    int bx = queenA->x;

    if(r > 50){
      moveQueen(idA, queenA->y, ax);
      moveQueen(idB, queenB->y, bx);
    }else{
      moveQueen(idA, ay, queenA->x);
      moveQueen(idB, by, queenB->x);
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
    int fromY = INT_MAX;
    int fromX = INT_MAX;
    int destY = INT_MIN;
    int destX = INT_MIN;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      fromY = min(fromY, queen->y);
      fromX = min(fromX, queen->x);
      destY = max(destY, queen->y);
      destX = max(destX, queen->x);
    }

    for(int y = fromY; y <= destY; ++y){
      for(int x = fromX; x <= destX; x++){
        fprintf(stderr,"+-"); 
      }
      fprintf(stderr,"+\n");


      for(int x = fromX; x <= destX; ++x){
        ll hash = calcHash(y, x);
        fprintf(stderr,"|");

        fprintf(stderr,"%c", (queenCheck[hash] > 0)? 'o' : ' ');
      }
      fprintf(stderr,"|\n");
    }

    for(int x = fromX; x <= destX; x++){
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
  timeLimit = 1000;

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
    //fprintf(stderr,"%s\n",ret[i].c_str());
  }
}
