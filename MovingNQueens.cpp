#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

class MovingNQueens {
  public:

  vector<string> rearrange(vector<int> a, vector<int> b){

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
  for(int i = 0; i < len; ++i){
    cout << ret[i];
  }
}