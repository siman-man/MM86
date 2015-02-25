#include <iostream>
#include <vector>
#include <cstdio>
#include <map>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <cassert>

using namespace std;

const int MAX_QUEEN = 200;
const int UNDEFINED = -1;

class C128 {
  public:
  unsigned long long lo;
  unsigned long long hi;

  C128(){
    lo = 0;
    hi = 0;
  }
  C128(unsigned long long m, unsigned long long n = 0){
    lo = m;
    hi = n; 
  }
  ~C128() {}

  C128& operator<<=(unsigned int n){
    n &= 0x07f;
    if(n >= 64){
      hi = lo << (n-64);
      lo = 0;
    }else if(n){
      hi <<= n;
      hi |= lo >> (64-n);
      lo <<= n;
    }
    return *this;
  }

  C128& operator>>=(unsigned int n){
    n &= 0x07f;
    if(n >= 64){
      lo = hi >> (n-64);
      hi = 0;
    }else if(n){
      lo >>= n;
      lo |= hi << (64-n);
      hi >>= n;
    }
    return *this;
  }

  C128 operator<<(unsigned int n) const { 
    return C128(*this) <<= n;
  }

  C128 operator>>(unsigned int n) const { 
    return C128(*this) >>= n; 
  }

  C128& operator+=(unsigned long long u){
    unsigned long long t = lo;
    if((lo += u) < t){
      ++hi;
    }
    return *this;
  }

  C128& operator+=(C128 c){
    unsigned long long t = lo;
    if((lo += c.lo) < t){
      ++hi;
    }
    hi += c.hi;
    return *this;
  }

  C128& operator-=(unsigned long long u){
    unsigned long long t = lo;
    if((lo -= u) > t){
      --hi;
    }
    return *this;
  }

  C128 operator-=(C128 c){
    unsigned long long t = lo;
    if((lo -= c.lo) > t){
      --hi;
    }
    hi -= c.hi;
    return *this;
  }

  C128 operator&(C128 c) const { 
    return C128(*this) &= c; 
  }
  C128 operator|(C128 c) const { return C128(*this) |= c; }
  C128 operator^(C128 c) const { return C128(*this) ^= c; }
  C128 operator~() const { 
    return C128(~lo, ~hi); 
  };
  C128 operator+(unsigned long long u) const { return C128(*this) += u; }
  C128 operator+(C128 c) const { return C128(*this) += c; }
  C128 operator-(unsigned long long u) const { return C128(*this) -= u; }
  C128 operator-(C128 c) const { return C128(*this) -= c; }
  C128 operator-() const { 
    return C128() -= *this; 
  }

  C128& operator&=(C128 c){
    lo &= c.lo;
    hi &= c.hi;
    return *this;
  }

  C128& operator|=(C128 c){
    lo |= c.lo;
    hi |= c.hi;
    return *this;
  }

  C128& operator^=(C128 c){
    lo ^= c.lo;
    hi ^= c.hi;
    return *this;
  }
};

struct Queen {
  int id;
  int y;
  int x;

  Queen(int id = UNDEFINED, int y = UNDEFINED, int x = UNDEFINED){
    this->id  = id;
    this->y   = y;
    this->x   = x;
  }
};

Queen queenList[MAX_QUEEN];
int N;
int board[MAX_QUEEN][MAX_QUEEN];
C128 MASK;
int cnt = 0;

unsigned long long MASK_LIST[65] = {
  0,
  1,
  3,
  7,
  15,
  31,
  63,
  127,
  255,
  511,
  1023,
  2047,
  4095,
  8191,
  16383,
  32767,
  65535,
  131071,
  262143,
  524287,
  1048575,
  2097151,
  4194303,
  8388607,
  16777215,
  33554431,
  67108863,
  134217727,
  268435455,
  536870911,
  1073741823,
  2147483647,
  4294967295,
  8589934591,
  17179869183,
  34359738367,
  68719476735,
  137438953471,
  274877906943,
  549755813887,
  1099511627775,
  2199023255551,
  4398046511103,
  8796093022207,
  17592186044415,
  35184372088831,
  70368744177663,
  140737488355327,
  281474976710655,
  562949953421311,
  1125899906842623,
  2251799813685247,
  4503599627370495,
  9007199254740991,
  18014398509481983,
  36028797018963967,
  72057594037927935,
  144115188075855871,
  288230376151711743,
  576460752303423487,
  1152921504606846975,
  2305843009213693951,
  4611686018427387903,
  9223372036854775807,
  18446744073709551615
};

unsigned long long xor128() {
  static unsigned long long rx=123456789, ry=362436069, rz=521288629, rw=88675123;
  unsigned long long rt = (rx ^ (rx<<11));
  rx=ry; ry=rz; rz=rw;
  return (rw=(rw^(rw>>19))^(rt^(rt>>8)));
}

class MovingNQueens {
  public:
  void init(){
    memset(board, 0, sizeof(board)); 

    fprintf(stderr,"N = %d\n", N);

    if(N >= 64){
      MASK.lo = 18446744073709551615;
      MASK.hi = MASK_LIST[N-64];
    }else{
      MASK.lo = MASK_LIST[N];
      MASK.hi = 0;
    }

    fprintf(stderr,"MASK.hi = %llu, MASK.lo = %llu\n", MASK.hi, MASK.lo);
  }



  vector<string> rearrange(vector<int> queenRows, vector<int> queenCols){
    vector<string> ret;
    N = queenRows.size();

    init();

    int index, row, col;
    index = 0;

    for(int id = 0; id < N; ++id){
      row = queenRows[id];
      col = queenCols[id];

      queenList[id] = Queen(id, row, col);
      fprintf(stderr,"index = %d, row = %d, col = %d\n", id, row, col);
      board[row][col] = 1;
    }

    backTrack(0, 0, 0, 0);
    ret.push_back(int2str(0) + " " + int2str(3) + " " + int2str(1));

    //showBoard();

    return ret;
  }

  void backTrack(int y, C128 left, C128 down, C128 right){
    C128 bitmap, bit;

    if(y > 0 && xor128()%100 > 15) return;

    if(y == 1){
      fprintf(stderr,"down.hi = %llu down.lo = %llu\n", down.hi, down.lo);
    }

    if(y == N){
      cnt++;
      fprintf(stderr,"N = %d, hello world! %d\n", N, cnt);
    }else{
      bitmap = MASK & ~(left | down | right);

      while(bitmap.hi | bitmap.lo){
        bit = -bitmap & bitmap;
        bitmap ^= bit;
        backTrack(y+1, ((left|bit)<<1), (down|bit), ((right|bit)>>1));
      }
    }
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
