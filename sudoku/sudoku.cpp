#include <iostream>
#include <vector>
#define CONF_COL RED
#define BORDER_COL GREEN
#define RED 31
#define GREEN 32
using namespace std;

/* 座標(i行j列) */
struct Pos{
    int i;
    int j;
};

/* 一つのマス */
class Square{
    public:
        int value = 0;
        bool possible[10];
        Square();
};
/* マスのコンストラクタ。最初はすべての数字の可能性がある。 */
Square::Square(){
    for (bool &p : possible) {
        p = true;
    }
}

/* 数独の盤 */
class Board{
    private:
        static const int SIZE = 9;
        static const int SQRT_SIZE = 3;
        static vector<Pos> members(int part);
        static int part_of(Pos p){return p.i / 3 * 3 + p.j / 3;}; // 座標pが属するセグメント番号を返す
        Square board[SIZE][SIZE];
    public:
        void printBoard();
        int put(int i, int j, int n);
        void test();
};
/* 指定したセグメント番号に属する座標のリストを返す */
vector<Pos> Board::members(int part){
    int i = part / SQRT_SIZE, j = part % SQRT_SIZE;
    vector<Pos> res;
    for (int k = 0; k < SQRT_SIZE; k++) {
        for (int l = 0; l < SQRT_SIZE; l++) {
            res.push_back({i * 3 + k, j * 3 + l});
        }
    }
    return res;
}
/* 指定した色での着色用の文字列を付加した文字列を返す */
string colored_string(string S, int color){
    // cout << "\033[" + to_string(color) + "m";
    // cout << S;
    // cout << "\033[m";
    return "\033[" + to_string(color) + "m" + S + "\033[m";
}
/* boardの内容を出力する */
void Board::printBoard(){
    for (int i = 0; i < SIZE; i++) {
        for (int k = 0; k < SQRT_SIZE; k++) {
            cout << colored_string("|", BORDER_COL);
            for (int j = 0; j < SIZE; j++) {
                cout << ' ';
                for (int l = 0; l < SQRT_SIZE; l++) {
                    if (board[i][j].value) {
                        cout << (k * SQRT_SIZE + l == (SIZE - 1) / 2 ? colored_string(to_string(board[i][j].value), CONF_COL) : " ");
                    } else {
                        // cout << k << ',' << l << ' ' << k * SQRT_SIZE + l + 1 << endl;
                        cout << (board[i][j].possible[k * SQRT_SIZE + l + 1] ? to_string(k * SQRT_SIZE + l + 1) : " ");
                    }
                }
                cout << ((j + 1) % 3 ? " |" : colored_string(" |", BORDER_COL));
            }
            cout << endl;
        }
        for (int h = 0; h < 1 + (1 + SQRT_SIZE + 2) * SIZE; h++) {
            cout << ((i + 1) % 3 ? "-" : colored_string("-", BORDER_COL));
        }
        cout << endl;
    }
    return;
}
/* i行j列をnで確定する */
int Board::put(int i, int j, int n){
    if (!board[i][j].possible[n]) {
        return 1;
    }
    board[i][j].value = n;
    /* 同じグループのnの可能性を排除 */
    vector<Pos> part_member(members(part_of({i, j})));
    for (int k = 0; k < SIZE; k++) {
        board[i][k].possible[n] = false; // 同じ行
        board[k][j].possible[n] = false; // 同じ列
        board[part_member[k].i][part_member[k].j].possible[n] = false; // 同じセグメント
    }
    return 0;
}
/* members()メソッドとpart_of()メソッドが正しく実装できているかどうかのテスト用 */
void Board::test(){
    for (int i = 0; i < 9; i++) {
        for (Pos p : members(i)) {
            cout << '[' << part_of(p) << ']';
            cout << '(' << p.i << ',' << p.j << ')';
        }
        cout << endl;
    }
}

int main(){
    Board game;
    // game.put(0, 3, 4);
    game.printBoard();
    // game.test();
    while (true) {
        int i, j, n;
        cin >> i;
        i--;
        if (i == -1) {
            break;
        }
        cin >> j >> n;
        j--;
        game.put(i, j, n);
        game.printBoard();
    }
    return 0;
}
