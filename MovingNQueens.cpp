#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cassert>

using namespace std;

const int MAX_QUEEN = 200;
const int UNDEFINED = -1;

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

class MovingNQueens {
  public:

  vector<string> rearrange(vector<int> queenRows, vector<int> queenCols){
    vector<string> ret;
    N = queenRows.size();
    int index, row, col;

    for(int i = 0; i < N; i++){
      index = i;
      row = queenRows[0];
      col = queenCols[0];
      ret.push_back(int2str(0) + " " + int2str(row+i+1) + " " + int2str(col+i+1));
      fprintf(stderr,"index = %d, row = %d, col = %d\n", index, row, col);
    }

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
