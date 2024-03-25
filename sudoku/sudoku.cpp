#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define INIT_FILE "init.txt"
#define BORDER_COL GREEN
#define CONF_COL YELLOW
#define USER_COL RED
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
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
        char type = '\0';
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
        Square **board; // board[SIZE][SIZE] (Board()参照)
    public:
        Board();
        Board(string init_file);
        Board(const Board& b); // コピーコンストラクタ
        void printBoard();
        int put(int i, int j, int n);
        int put(int i, int j, int n, char type);
        int auto_fill();
        void test();
        void test_copy();
};
/* コンストラクタ */
Board::Board(){
    board = new Square* [SIZE];
    for (int i = 0; i < SIZE; i++) {
        board[i] = new Square[SIZE];
    }
}
/* コンストラクタ(ファイルによる初期化) */
Board::Board(string init_file): Board(){
    ifstream ifs(init_file);
    if (ifs.fail()) {
        cerr << "file open error (" << init_file << ")" << endl;
        exit(1);
    }
    string str;
    while (getline(ifs, str)) {
        stringstream ss(str);
        int r, c, x;
        ss >> r >> c >> x;
        r--; c--;
        put(r, c, x, 'i');
    }
    ifs.close();
}
/* コピーコンストラクタ */
Board::Board(const Board& b): Board(){
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            board[r][c] = b.board[r][c];
        }
    }
}
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
                        int col;
                        switch (board[i][j].type) {
                            case 'i':
                                col = BORDER_COL;
                                break;
                            case 'a':
                                col = CONF_COL;
                                break;
                            case 'u':
                                col = USER_COL;
                                break;
                            default:
                                col = CONF_COL;
                        }
                        cout << (k * SQRT_SIZE + l == (SIZE - 1) / 2 ? colored_string(to_string(board[i][j].value), col) : " ");
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
int Board::put(int i, int j, int n, char type){
    int res = put(i, j, n);
    if (!res) {
        board[i][j].type = type;
    }
    return res;
}
int Board::auto_fill(){
    while (true) {
        bool unq_found = false;
        /* 同マス内 */
        for (int r = 0; r < SIZE; r++) {
            if (unq_found) {
                break;
            }
            for (int c = 0; c < SIZE; c++) {
                if (board[r][c].value) {
                    continue;
                }
                int unq_p = 0;
                for (int p = 1; p <= 9; p++) {
                    if (board[r][c].possible[p]) {
                        if (unq_p) {
                            unq_p = -1;
                            break;
                        }
                        unq_p = p;
                    }
                }
                if (unq_p && unq_p != -1) {
                    put(r, c, unq_p);
                    unq_found = true;
                    break;
                }
            }
        }
        /* 同行内 */
        for (int r = 0; r < SIZE; r++ ) {
            if (unq_found) {
                break;
            }
            for (int p = 1; p <= 9; p++) {
                int unq_c = SIZE; // pが唯一存在する列
                for (int c = 0; c < SIZE; c++) {
                    if (board[r][c].value) {
                        continue;
                    }
                    if (board[r][c].possible[p]) {
                        if (unq_c != SIZE) {
                            unq_c = -1;
                            break;
                        }
                        unq_c = c;
                    }
                }
                if (unq_c != SIZE && unq_c != -1) {
                    put(r, unq_c, p);
                    unq_found = true;
                    break;
                }
            }
        }
        /* 同列内 */
        for (int c = 0; c < SIZE; c++) {
            if (unq_found) {
                break;
            }
            for (int p = 1; p <= 9; p++) {
                int unq_r = SIZE;
                for (int r = 0; r < SIZE; r++) {
                    if (board[r][c].value) {
                        continue;
                    }
                    if (board[r][c].possible[p]) {
                        if (unq_r != SIZE) {
                            unq_r = -1;
                            break;
                        }
                        unq_r = r;
                    }
                }
                if (unq_r != SIZE && unq_r != -1) {
                    put(unq_r, c, p);
                    unq_found = true;
                    break;
                }
            }
        }
        /* 同セグメント内 */
        for (int seg = 0; seg < SIZE; seg++) {
            if (unq_found) {
                break;
            }
            for (int p = 1; p <= 9; p++) {
                int unq_i = SIZE, unq_j = SIZE;
                for (auto pos : members(seg)) {
                    if (board[pos.i][pos.j].value) {
                        continue;
                    }
                    if (board[pos.i][pos.j].possible[p]) {
                        if (unq_i) {
                            unq_i = -1;
                            break;
                        }
                        unq_i = pos.i;
                        unq_j = pos.j;
                    }
                }
                if (unq_i != SIZE && unq_i != -1) {
                    put(unq_i, unq_j, p);
                    unq_found = true;
                    break;
                }
            }
        }
        if (!unq_found) {
            return 0; // fillした数やfillした座標のリストを返すべき #todo
        }
    }
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
/* コピーコンストラクタが正しく実装できているかどうかのテスト用 */
void Board::test_copy(){
    Board game(*this);
    game.put(8,8,5);
    game.printBoard();
}

int main(){
    Board game(INIT_FILE);
    game.auto_fill();
    game.printBoard();
    cout << "test_copy()メソッド呼び出し" << endl;
    game.test_copy();
    cout << "コピー元のprintBoard()メソッドの再呼び出し" << endl;
    game.printBoard();
    // while (true) {
    //     int i, j, n;
    //     cin >> i;
    //     i--;
    //     if (i == -1) {
    //         break;
    //     }
    //     cin >> j >> n;
    //     j--;
    //     game.put(i, j, n, 'u');
    //     game.auto_fill();
    //     game.printBoard();
    // }
    return 0;
}
