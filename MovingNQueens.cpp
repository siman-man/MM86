#include <iostream>
#include <vector>
#include <cstdio>
#include <map>
#include <algorithm>
#include <boost/multiprecision/cpp_int.hpp>
#include <string.h>
#include <sstream>
#include <cassert>

using namespace std;
using namespace boost::multiprecision;

const int MAX_QUEEN = 200;
const int UNDEFINED = -1;

class C128 {
  private:
    unsigned long long lo;
    unsigned long long hi;

  public:
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
  C128 operator~() const { return C128(~lo, ~hi); };
  C128 operator+(unsigned long long u) const { return C128(*this) += u; }
  C128 operator+(C128 c) const { return C128(*this) += c; }
  C128 operator-(unsigned long long u) const { return C128(*this) -= u; }
  C128 operator-(C128 c) const { return C128(*this) -= c; }
  C128 operator-() const { return C128() -= *this; }

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

class MovingNQueens {
  public:
  void init(){
    memset(board, 0, sizeof(board)); 
  }

  vector<string> rearrange(vector<int> queenRows, vector<int> queenCols){
    vector<string> ret;
    N = queenRows.size();
    int index, row, col;
    index = 0;
    C128 c1(0);
    C128 c2(2);

    for(int id = 0; id < N; ++id){
      row = queenRows[id];
      col = queenCols[id];

      queenList[id] = Queen(id, row, col);
      fprintf(stderr,"index = %d, row = %d, col = %d\n", id, row, col);
      board[row][col] = 1;
    }

    ret.push_back(int2str(0) + " " + int2str(3) + " " + int2str(1));

    showBoard();

    return ret;
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
  int128_t b;

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
