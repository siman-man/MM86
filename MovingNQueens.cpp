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

ll timeLimit = 9700;
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
int EY[17] = {0, -1, -1, -2, -1, -2, -1, -1,  0,  1,  1,  2, 1, 2, 1, 1, 0};
int EX[17] = {1,  2,  1,  1,  0, -1, -1, -2, -1, -2, -1, -1, 0, 1, 1, 2, 0};

int horizontalCnt[MAX_SIZE];
int verticalCnt[MAX_SIZE];
int diagonalUpCnt[MAX_SIZE];
int diagonalDownCnt[MAX_SIZE];

int bestRows[MAX_QUEEN];
int bestCols[MAX_QUEEN];
int bestRowsBK[MAX_QUEEN];
int bestColsBK[MAX_QUEEN];
int bestScore;
int goodScore;
int burnCnt = 0;
int tryLimit = 200;

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

    N = queenRows.size();
    memset(board, 0, sizeof(board)); 
    memset(horizontalCnt, 0, sizeof(horizontalCnt));
    memset(verticalCnt, 0, sizeof(verticalCnt));
    memset(diagonalUpCnt, 0, sizeof(diagonalUpCnt));
    memset(diagonalDownCnt, 0, sizeof(diagonalDownCnt));

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

    center = calcCenterCoord();

    fprintf(stderr,"N = %d\n", N);
  }

  void directTryLimit(){
    if(N <= 12){
      tryLimit = 50;
    }else if(N <= 20){
      tryLimit = 100;
    }else if(N <= 30){
      tryLimit = 700;
    }else if(N <= 40){
      tryLimit = 800;
    }else if(N <= 60){
      tryLimit = 1000;
    }else if(N <= 80){
      tryLimit = 1500;
    }else if(N <= 90){
      tryLimit = 3000;
    }else{
      tryLimit = 4000;
    }
  }

  void updateBestPositions(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      bestRows[id] = queen->y;
      bestCols[id] = queen->x;
    }
  }

  Coord rotateCoord(int y, int x, int angle){
    double theta = (angle / 180.0) * PI; 
        
    int ny = (x - center.x) * (int)sin(theta) + (y - center.y) * (int)cos(theta) + center.y;
    int nx = (x - center.x) * (int)cos(theta) - (y - center.y) * (int)sin(theta) + center.x;

    return Coord(ny, nx);
  }

  void rotateBestPosition(int angle){
    for(int nodeId = 0; nodeId < N; ++nodeId){
      Coord coord = rotateCoord(bestRows[nodeId], bestCols[nodeId], angle);

      bestRows[nodeId] = coord.y;
      bestCols[nodeId] = coord.x;
    }
  }

  void rotateQueen(int id, int angle){
    //fprintf(stderr,"rotate queen =>\n");

    Queen *queen = getQueen(id);
    Coord coord = rotateCoord(queen->y, queen->x, angle);

    moveQueen(id, coord.y, coord.x);
  }

  void rotateQueenAll(){
    int angle = ((xor128()%3)+1) * 90;
    //fprintf(stderr,"rotate queen =>\n");

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      Coord coord = rotateCoord(queen->y, queen->x, angle);

      moveQueen(id, coord.y, coord.x);
    }
  }

  void moveQueen(int id, int y, int x){
    Queen *queen = getQueen(id);
    removeQueen(queen->y, queen->x);
    setQueen(id, y, x);
  }

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

  inline void removeQueen(int y, int x){ 
    ll hash = calcHash(y, x);
    queenCheck[hash] -= 1;

    horizontalCnt[OFFSET + y] -= 1;
    verticalCnt[OFFSET + x] -= 1;
    diagonalUpCnt[OFFSET + y - x] -= 1;
    diagonalDownCnt[OFFSET + y + x] -= 1;
  }

  ll calcHash(int y, int x){
    ll hash = ((OFFSET + y + 1) * OFFSET) + x;
    //assert(hash >= 0);
    return hash;
  }

  int calcScoreAll(){
    int score = 0;
    int cnt = 0;
    bool success = true;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);  

      cnt = horizontalCnt[OFFSET + queen->y] + verticalCnt[OFFSET + queen->x];
      cnt += diagonalUpCnt[OFFSET + queen->y - queen->x] + diagonalDownCnt[OFFSET + queen->y + queen->x];

      if(cnt > 4){
        success = false;
      }

      score -= 100 * cnt;
      score -= max(abs(queen->y - center.y), abs(queen->x - center.x));
    }

    return score + success * 100000;
  }

  int calcScoreCoord(int y, int x){
    int score = 0;
    int cnt;
    bool success = true;

    int l = horizontalCnt[OFFSET + y] + verticalCnt[OFFSET + x];
    int d = diagonalUpCnt[OFFSET + y - x] + diagonalDownCnt[OFFSET + y + x];
    cnt = l + d;

    if(cnt > 4) success = false;

    score -= cnt;

    return score + 100 * success;
  }

  int calcScoreSingle(int id){
    int score = 0;
    int cnt;
    bool success = true;
    Queen *queen = getQueen(id);  

    int l = horizontalCnt[OFFSET + queen->y] + verticalCnt[OFFSET + queen->x];
    int d = diagonalUpCnt[OFFSET + queen->y - queen->x] + diagonalDownCnt[OFFSET + queen->y + queen->x];
    cnt = l + d;

    if(cnt > 4) success = false;

    score -= cnt;

    if(N >= 70 && goodScore > 0){
      //score -= max(abs(queen->y - center.y), abs(queen->x - center.x));
      score -= max(abs(queen->y - queen->originalY), abs(queen->x - queen->originalX));
      //score -= abs(queen->y - queen->originalY) + abs(queen->x - queen->originalX);
    }

    return score + 100 * success;
  }

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

  int checkDirection16(Queen *queen){
    if(queen->degree != 180){
      return (int)(queen->degree / 22.5);
    }else{
      if(queen->x > center.x){
        return 0;
      }else if(queen->x < center.x){
        return 8;
      }else if(queen->y > center.y){
        return 12;
      }else if(queen->y < center.y){
        return 4;
      }else{
        return 16;
      }
    }
  }

  void firstMove(){
    int ny, nx;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      int direct = checkDirection(queen);
      int dist = max(abs(queen->y - center.y), abs(queen->x - center.x));

      ny = queen->y + (N/8) * dist * DY[direct];
      nx = queen->x + (N/8) * dist * DX[direct];

      moveQueen(id, ny, nx);
    }
  }

  void newSetQueen(){
    map<int, bool> checkListVertical;

    for(int id = 0; id < N; ++id){
      int x = xor128() % N;

      while(checkListVertical[x]){
        x = xor128() % N;
      }

      moveQueen(id, center.y - N + id, x);
      checkListVertical[x] = true;
    }
  }

  void randomSet(){
    for(int id = 0; id < N; ++id){
      int dy = (xor128() % (2*N)) - N;
      int dx = (xor128() % (2*N)) - N;

      int ny = center.y + dy;
      int nx = center.x + dx;
      moveQueen(id, ny, nx);
    }
  }

  void moveAllPosition(int direct){
    for(int nodeId = 0; nodeId < N; ++nodeId){
      bestRows[nodeId] += MY[direct];
      bestCols[nodeId] += MX[direct];
    }
  }

  void moveAllQueen(int direct){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      int ny = queen->y + MY[direct];
      int nx = queen->x + MX[direct];

      moveQueen(id, ny, nx);
    }
  }

  void littleMoveAll(){
    int direct = xor128() % 8;

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      int ny = queen->y + (N/8) * MY[direct];
      int nx = queen->x + (N/8) * MX[direct];

      moveQueen(id, ny, nx);
    }
  }

  void eachTurnProc(int id){
    //fprintf(stderr,"eachTurnProc =>\n");
    Queen *queen = getQueen(id);
    queen->beforeY = queen->y;
    queen->beforeX = queen->x;
  }

  void rollback(int id){
    //fprintf(stderr,"rollback =>\n");
    Queen *queen = getQueen(id);
    moveQueen(id, queen->beforeY, queen->beforeX);
  }

  void rollbackAll(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      moveQueen(id, queen->beforeY, queen->beforeX);
    }
  }

  vector<string> rearrange(vector<int> queenRows, vector<int> queenCols){
    vector<string> ret;

    init(queenRows, queenCols);
    directTryLimit();

    //fprintf(stderr,"centerY = %d, centerX = %d\n", center.y, center.x);

    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      int dy = center.y - queen->y;
      int dx = center.x - queen->x;
      int degree = 180 - (int)(atan2(dy,dx) * 180.0 / PI);
      queen->degree = degree;
      //fprintf(stderr,"id = %d, dist = %4.1f, degree = %d, direct = %d\n", id, dist, queen->degree, direct);
    }

    firstMove();

    bestScore = INT_MIN;
    goodScore = INT_MIN;
    const ll startTime = getTime();
    const ll endTime = startTime + timeLimit;
    currentTime = startTime;
    int turn = 0;
    double T = 10000.0; 
    double k = 10.0;
    double alpha = 0.998;
    int notChangeCnt = 0;
    int idA, idB, idC;
    int swapRate = 3;

    //for(int i = 0; i < 400000; ++i){
    while(currentTime < endTime){
      ++turn;
      //fprintf(stderr,"turn = %d start =>\n", turn);
      idB = UNDEFINED;
      idC = UNDEFINED;

      int id = xor128() % N;

      //assert(id >= 0);
      eachTurnProc(id);

      int r = xor128() % 100;
      //assert(r >= 0);

      if(r > swapRate){
        EvalResult result = updateQueen(id);
        moveQueen(id, result.y, result.x);
      }else if(goodScore > 0){
      //}else{
      //}else if(notChangeCnt > 10){
        idA = id;
        idB = (xor128() % N);
        eachTurnProc(idB);
        /*
        idC = (xor128() % N);
        eachTurnProc(idC);
        swapQueen3(idA, idB, idC);
        */
        swapQueen(idA, idB);
        /*
        resetPosition(id);
        EvalResult result = updateQueen(id);
        moveQueen(id, result.y, result.x);
        */
      }else{
        //continue;
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
      }else if(T > 0 && (xor128() % 100) < 100.0 * rate){
        //fprintf(stderr,"turn = %d, rate = %f\n", turn, rate);
        goodScore = score;
        notChangeCnt = 0;
      }else{
        ++notChangeCnt;
        rollback(id);

        if(idB != UNDEFINED){
          rollback(idB);
        }
        if(idC != UNDEFINED){
          rollback(idC);
        }

        if(notChangeCnt > tryLimit){
          //fprintf(stderr,"ReBrun =>\n");
          resetAllPosition();
          firstMove();
          //rotateQueenAll();
          //newSetQueen();
          //moveAllPosition(xor128()%8);
          //littleMoveAll();
          //randomSet();
          T = 10000.0;
          burnCnt++;
          notChangeCnt = 0;
          
          if(goodScore < 0){
            //alpha += 0.001;
          }else{
            alpha = max(0.9, alpha-0.001);
          }
        }
      }

      if(turn % 20000 == 0){
        currentTime = getTime();
      }

      T *= alpha;
      //fprintf(stderr,"turn = %d end =>\n", turn);
    }

    fprintf(stderr,"turn = %d, alpha = %4.2f, T = %4.2f\n", turn, alpha, T);

    for(int id = 0; id < N; ++id){
      //fprintf(stderr,"nodeId = %d, y = %d, x = %d\n", id, bestRows[id], bestCols[id]);
    }

    //lastMove();
    lastCheck();
    //moveAllPosition(2);
    //rotateBestPosition(0);

    resetAllPosition();
    solve();
    vector<int> positionList;

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
          //q.value += 1;
        }
      }

      que.push(q);
    }

    int tryCnt = 0;

    while(!que.empty()){
      Queen q = que.top(); que.pop();

      Queen *queen = getQueen(q.id);
      int nodeId = positionList[queen->id];

      //fprintf(stderr,"id = %d, queen->y = %d, queen->x = %d, bestRow = %d, bestCol = %d\n", queen->id, queen->y, queen->x, bestRows[nodeId], bestCols[nodeId]);

      /*
      queen->y = bestRows[nodeId];
      queen->x = bestCols[nodeId];
      */

      if(searchPath(queen->id, bestRows[nodeId], bestCols[nodeId], ret, tryCnt)){
        ll hash = calcHash(queen->y, queen->x);
        queenCheck[hash] -= 1;
        hash = calcHash(bestRows[nodeId], bestCols[nodeId]);
        queenCheck[hash] += 1;
        moveQueen(queen->id, bestRows[nodeId], bestCols[nodeId]);
        tryCnt = 0;
      }else{
        //fprintf(stderr,"id = %d, queue size = %lu\n", queen->id, que.size());
        Queen q(queen->id, queen->y, queen->x);
        q.value = OFFSET - 2 * max(abs(queen->y - center.y), abs(queen->x - center.x));
        tryCnt++;
        que.push(q);
      }

      //fprintf(stderr,"id = %d, position = %d\n", queen->id, positionList[queen->id]);
    }

    //showBoard();

    return ret;
  }

  void lastCheck(){
    //fprintf(stderr,"best score = %d\n", bestScore);

    for(int id = 0; id < N; ++id){
      moveQueen(id, bestRows[id], bestCols[id]);
      eachTurnProc(id);
    }

    for(int id = 0; id < N; ++id){
      //Queen *queen = getQueen(id);
      //fprintf(stderr,"id = %d, y = %d, x = %d\n", id, queen->y, queen->x);
    }

    //fprintf(stderr,"before last score = %d\n", calcScoreAll());

    for(int direct = 0; direct < 8; ++direct){
      moveAllQueen(direct);
      int score = calcScoreAll();

      for(int id = 0; id < N; ++id){
        //Queen *queen = getQueen(id);
        //fprintf(stderr,"id = %d, y = %d, x = %d\n", id, queen->y, queen->x);
      }

      //fprintf(stderr,"last score = %d\n", score);
      if(bestScore < score){
        //fprintf(stderr,"update!\n");
        bestScore = score;
        updateBestPositions();
      }

      rollbackAll();
    }
  }

  void lastMove(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);
      eachTurnProc(id);
      int score;

      for(int i = 0; i < 8; ++i){
        int ny = queen->y + MY[i];
        int nx = queen->x + MX[i];

        moveQueen(id, ny, nx);

        score = calcScoreAll();

        if(bestScore < score){
          bestScore = score;
          updateBestPositions();
        }else{
          rollback(id);
        }
      }
    }
  }

  bool searchPath(int id, int destY, int destX, vector<string> &ret, int tryCnt){
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
        if(tryCnt < N && c.dist > max(abs(queen->y - destY), abs(queen->x - destX))) return false;
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
    for(int i = 0; i < N; i++){
      addEdge(s, i, 1, 0);
      F += 1;
    }

    //fprintf(stderr,"Add Node Edge =>\n");
    for(int i = 0; i < N; i++){
      addEdge(N + i, t, 1, 0);
    }
    //fprintf(stderr,"Add Node Edge end =>\n");

    int minCost = minCostFlow(s, t, F);

    fprintf(stderr,"minCost = %d, center.y = %d, cneter.x = %d\n", minCost, center.y, center.x);
  }

  int minCostFlow(int s, int t, int f){
    //fprintf(stderr,"minCostFlow =>\n");
    int res = 0;
    int V = 2 * N + 3;
    int dist[V];
    int prevv[V];
    int preve[V];

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
        return -1;
      }

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

  void warpQueen(int id){
    int ny = center.y + xor128() % (N);
    int nx = center.x + xor128() % (N);

    moveQueen(id, ny, nx);
  }

  void resetPosition(int id){
    Queen *queen = getQueen(id);
    moveQueen(id, queen->originalY, queen->originalX);
  }

  void resetAllPosition(){
    for(int id = 0; id < N; ++id){
      Queen *queen = getQueen(id);

      moveQueen(id, queen->originalY, queen->originalX);
    }
  }

  Coord checkBestCoord(int y, int x, int direct){
    int maxScore = INT_MIN;
    int score;
    int by = y;
    int bx = x;

    for(int i = 1; i < N; i++){
      int ny = y + i * MY[direct];
      int nx = x + i * MX[direct];

      score = calcScoreCoord(ny, nx);

      if(maxScore < score){
        maxScore = score;
        by = ny;
        bx = nx;
      }
    }

    return Coord(by, bx);
  }

  Coord checkAroundBestCoord(int y, int x){
    int maxScore = INT_MIN;
    int score;
    int by;
    int bx;
    int diff = (N/8);

    for(int ny = y - diff; ny <= y + diff; ++ny){
      for(int nx = x - diff; nx <= x + diff; ++nx){
        score = calcScoreCoord(ny, nx);

        if(maxScore < score){
          maxScore = score;
          by = ny;
          bx = nx;
        }
      }
    }

    return Coord(by, bx);
  }

  EvalResult updateQueen(int id){
    Queen *queen = getQueen(id);
    //int maxScore = calcScoreSingle(id);
    int maxScore = INT_MIN;
    int bestY = queen->y;
    int bestX = queen->x;
    int score;
    int direct;
    int diff;
    int ny, nx;
    map<int, bool> checkList;

    int limit = 5;
    for(int i = 0; i < limit; ++i){
    //for(int i = 0; i < 8; ++i){
      //direct = i;
      direct = xor128() % 8;
      while(checkList[direct]){
        direct = xor128() % 8;
      }
      checkList[direct] = true;
      //int diff = (xor128() % 2) + 1;
      if(N <= 20){
        diff = (xor128() % (N/4)) + 1;
      }else{
        diff = (xor128() % (N/2)) + 1;
      }
      if(burnCnt % 2 == 0){
        ny = queen->y + diff * MY[direct];
        nx = queen->x + diff * MX[direct];
      }else{
        //Coord coord = checkAroundBestCoord(queen->y, queen->x);
        Coord coord = checkBestCoord(queen->y, queen->x, direct);
        ny = coord.y;
        nx = coord.x;
      }

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

  void swapQueen3(int idA, int idB, int idC){
    if(idA == idB) return;
    if(idA == idC) return;
    if(idB == idC) return;

    Queen *queenA = getQueen(idA);
    Queen *queenB = getQueen(idB);
    Queen *queenC = getQueen(idC);

    int r = xor128() % 100;
    int ay = queenB->y;
    int ax = queenB->x;
    int by = queenC->y;
    int bx = queenC->x;
    int cy = queenA->y;
    int cx = queenA->x;

    if(r > 50){
      moveQueen(idA, queenA->y, ax);
      moveQueen(idB, queenB->y, bx);
      moveQueen(idC, queenC->y, cx);
    }else{
      moveQueen(idA, ay, queenA->x);
      moveQueen(idB, by, queenB->x);
      moveQueen(idC, cy, queenC->x);
    }
  }

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

      ll hash = calcHash(queen->originalY, queen->originalX);
      queenCheck[hash] = MAX_QUEEN;
    }

    for(int y = fromY; y <= destY; ++y){
      for(int x = fromX; x <= destX; x++){
        fprintf(stderr,"+-"); 
      }
      fprintf(stderr,"+\n");


      for(int x = fromX; x <= destX; ++x){
        ll hash = calcHash(y, x);
        fprintf(stderr,"|");

        if(center.y == y && center.x == x){
          fprintf(stderr,"C");
        }else if(queenCheck[hash] == MAX_QUEEN){
          fprintf(stderr,"x");
        }else if(queenCheck[hash] > 0){
          fprintf(stderr,"o");
        }else{
          fprintf(stderr," ");
        }
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
  timeLimit = 2000;

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
