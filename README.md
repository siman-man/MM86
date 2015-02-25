# MM86
Problem: MovingNQueens

# 問題文

賞金
You are given the coordinates of N chess queens placed on an infinite chess board. 
あなたはN個のクイーンを持ったと無限の広さがあるチェスボードを持っています。
Your goal is to make some valid moves to rearrange the queens so that in the end no 
あなたの目的は２つのクイーンが互いに干渉しないように効率よく動かすことです。
two queens threaten each other (i.e., no two queens share the same row, column or diagonal).
（縦、横、斜めに他のクイーンが被らないようにする）

The coordinates of the queens are given as int[]s queenRows and queenCols of the same length N. 
クイーンの座標はsで与えられているNの数に等しい
You have to return the list of moves as a String[]. You can use at most 8*N moves. Each element of 
あなたは行動のリストをStringの配列で返す. 最大で8*Nまでの長さを返すことができる。
your return should be formatted as "INDEX ROW COL"; the corresponding move will move queen INDEX (0-based) 
各要素は以下のフォーマットで定められています（インデックス　行　列） この処理を元にクイーンが移動を行います。
from its current position to a cell with coordinates (ROW, COL). Each move must be a valid chess 
現在いる座標から。
queen move: the queen can be moved any number of unoccupied squares in a straight line vertically, horizontally or diagonally.
それぞれの行動は有効である必要があります。 各クイーンはその正方形の縦、横、斜めを占領します。

点数計算
Your raw score for a test case will be the sum of Chebyshev distances between starting cell and end 
あなたのスコアはクイーンの初期位置と最終位置のChebyshev distancesによって定まります。あなたの目的はこの距離の合計をできるだけ小さくすることです。
cell of each move, and your goal is to minimize it. If your return was invalid (had too many moves, 
もし不正な答えを行った場合は、そのテストケースでの点数は0になります。そしてスコアは、「このテストケースでの最小点/あなたのスコア」となります。
used invalid moves or resulted in a pair of queens threatening each other), your raw score for a test 
case will be 0. Your overall score will be the sum of MIN/YOUR over all test cases, where YOUR is your 
raw score on a test case, and MIN is the smallest score achieved by anyone on that test case (test cases 
on which you scored 0 don't contribute to the sum).


# 方針
- [ ] バックトラック法によりNクイーンの問題の解を生成
- [ ] 生成された解と現在のクイーンの位置から、どれだけ移動すれば解の位置に移動できるかを計算
- [ ] 移動が定まったら、それを回答形式にして返却する