#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <cassert>
#include <climits>
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::array<std::array<int, SIZE>, SIZE> Map = {
     30, 15, 15, 15, 15, 15, 15, 30,
     15, 1, 1, 1, 1, 1, 1, 15,
     15, 1, 1, 1, 1, 1, 1, 15,
     15, 1, 1, 1, 1, 1, 1, 15,
     15, 1, 1, 1, 1, 1, 1, 15,
     15, 1, 1, 1, 1, 1, 1, 15,
     15, 1, 1, 1, 1, 1, 1, 15,
     30, 15, 15, 15, 15, 15, 15, 30
}; //設定的map（用來設heuristic）邊界（尤其是四角）佔優勢，分數高

std::array<int, 3> disc_count = {0,0,0};
struct Point { //把main.cpp的struct抓來用
    int x, y;
	Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};
std::vector<Point> next_valid_spots;
int player;
class OthelloBoard { //把main.cpp的class抓出來用，因為裡面有function用得到
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
    std::string encode_player(int state) {
        if (state == BLACK) return "O";
        if (state == WHITE) return "X";
        return "Draw";
    }
    std::string encode_spot(int x, int y) {
        if (is_spot_valid(Point(x, y))) return ".";
        if (board[x][y] == BLACK) return "O";
        if (board[x][y] == WHITE) return "X";
        return " ";
    }
    std::string encode_output(bool fail=false) {
        int i, j;
        std::stringstream ss;
        ss << "Timestep #" << (8*8-4-disc_count[EMPTY]+1) << "\n";
        ss << "O: " << disc_count[BLACK] << "; X: " << disc_count[WHITE] << "\n";
        if (fail) {
            ss << "Winner is " << encode_player(winner) << " (Opponent performed invalid move)\n";
        } else if (next_valid_spots.size() > 0) {
            ss << encode_player(cur_player) << "'s turn\n";
        } else {
            ss << "Winner is " << encode_player(winner) << "\n";
        }
        ss << "+---------------+\n";
        for (i = 0; i < SIZE; i++) {
            ss << "|";
            for (j = 0; j < SIZE-1; j++) {
                ss << encode_spot(i, j) << " ";
            }
            ss << encode_spot(i, j) << "|\n";
        }
        ss << "+---------------+\n";
        ss << next_valid_spots.size() << " valid moves: {";
        if (next_valid_spots.size() > 0) {
            Point p = next_valid_spots[0];
            ss << "(" << p.x << "," << p.y << ")";
        }
        for (size_t i = 1; i < next_valid_spots.size(); i++) {
            Point p = next_valid_spots[i];
            ss << ", (" << p.x << "," << p.y << ")";
        }
        ss << "}\n";
        ss << "=================\n";
        return ss.str();
    }
    std::string encode_state() {
        int i, j;
        std::stringstream ss;
        ss << cur_player << "\n";
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE-1; j++) {
                ss << board[i][j] << " ";
            }
            ss << board[i][j] << "\n";
        }
        ss << next_valid_spots.size() << "\n";
        for (size_t i = 0; i < next_valid_spots.size(); i++) {
            Point p = next_valid_spots[i];
            ss << p.x << " " << p.y << "\n";
        }
        return ss.str();
    }
    int get_score() const{ // 設定heuristic的function，寫在class裡面
        int score = 0;
        if(winner == player) // 當跑到設定層數的時候如果已經有winner出現那就確認winner是誰，
            score += 300; //如果是自己就加分，別人就扣分
        if(winner == 3-player)
            score -= 300;
        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){     //看黑棋白棋對應位置的Map，依照Map給分
                if(board[i][j] == player)
                    score += Map[i][j];     //如果是自己就加上分數，敵方就減上分數
                else if(board[i][j] == 3 - player)
                    score -= Map[i][j];
            }
        }
        
        if(cur_player==player) score += get_valid_spots().size();   //最後算這樣的棋盤我的下一步有幾個選擇，越多選擇越好
        return score;   //return最後的分數
    }
};

struct Node{ //用一個struct node用來當作參數傳遞（因為這樣可以同時傳遞point跟分數）
    Point p;
    int score;
    Node(){};
    Node(Point _p, int _score) : p(_p), score(_score) {}
};



//Node minimax(const OthelloBoard, int, int, int, int);
Node max(const OthelloBoard, int, int, int); //這個function用來算minimax中max的那一層
Node min(const OthelloBoard, int, int, int); //用來算minimax中min的那一層


void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
            disc_count[board[i][j]]++;
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    
    OthelloBoard cur; //先宣告一個OthelloBoard來記錄當下的棋盤狀態
    cur.board = board;
    cur.cur_player = player;
    cur.disc_count = disc_count;
    int alpha = INT_MIN; //alpha初始值為負數的int max
    int beta = INT_MAX; //beta初始值為int max
    Node ans = max(cur, alpha, beta, 6); //用DFS找對我們最有利的情況
    //Node ans = minimax(cur, alpha, beta, 6, 1);
    
    Point p = ans.p; //最後把回傳的點設定為下一步
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}
Node max(const OthelloBoard cur, int alpha, int beta, int depth){
    if(!depth || cur.done) //當設定層數達到或是已經有結果了，就回傳當下棋盤狀況的分數
        return Node(Point(-2,-2), cur.get_score()); //這邊主要是要回傳score，point的值不太重要
    
    Point maxp = Point(-2, -2);
    int maxscore = INT_MIN; //用來記哪一個valid point的分數最高，下一步就走那邊
    std::vector<Point> valid_spots = cur.get_valid_spots();
    int valid_num = valid_spots.size();
    Node tmp;
    
    for(int i=0; i<valid_num; i++){ //用迴圈一個一個帶valid的點進去看結果
        OthelloBoard now = cur;
        now.put_disc(valid_spots[i]); //假設走了那一步
        
        tmp = min(now, alpha, beta, depth - 1); //用新的棋盤狀態去算下一輪，跳到min
        
        if(tmp.score > maxscore){ //如果這次算出來的分數比之前的最大還大，那就改maxscore，跟對應的下一步
            maxp = valid_spots[i];
            maxscore = tmp.score;
        }
        if(maxscore >= beta) //已知可能的score介於alpha~beta之間，如果大於上限，那就直接跳出不用算
            break;          //如果找最大的這一層有beta就代表是別層下來的限制
        if(maxscore > alpha) //因為這一輪是找最大，alpha代表是目前最小能夠拿到的分數，如果可以拿到比他大的分數
            alpha = maxscore; //，就更新alpha
    }
    
    return Node(maxp, maxscore); //回傳分數跟位置（point）
}
Node min(const OthelloBoard cur, int alpha, int beta, int depth){
    if(!depth || cur.done)      //當設定層數達到或是已經有結果了，就回傳當下棋盤狀況的分數
        return Node(Point(-2,-2), cur.get_score());
    
    Point minp = Point(-2, -2);
    int minscore = INT_MAX;
    std::vector<Point> valid_spots = cur.get_valid_spots();
    int valid_num = valid_spots.size();
    Node tmp;
    
    for(int i=0; i<valid_num; i++){ //用迴圈一個一個帶valid的點進去看結果
        OthelloBoard now = cur;
        now.put_disc(valid_spots[i]);   //假設走了那一步
        tmp = max(now, alpha, beta, depth - 1);     //用新的棋盤狀態去算下一輪，跳到max
        
        if(tmp.score < minscore){   //如果這次算出來的結果比min還小，那就更新min，跟對應的下一步
            minp = valid_spots[i];
            minscore = tmp.score;
        }
        if(minscore <= alpha)   //已知可能的score介於alpha~beta之間，如果小於下限，那就直接跳出不用算
           break;               //如果找最小的這一層有alpha就代表是別層下來的限制
        if(minscore < beta)   //因為這一輪是找最小，beta代表是目前最大能夠拿到的分數，如果可以拿到比他小的分數
            beta = minscore;    //，就更新beta
        
    }
    return Node(minp, minscore); //回傳最後最小的結果
}
/*Node minimax(const OthelloBoard cur, int alpha, int beta, int depth, int type){
    if(type){
        if(!depth || cur.done)
            return Node(Point(-2,-2), cur.get_score());
        
        Point maxp = Point(-100,-100);
        int maxscore = INT_MIN;
        std::vector<Point> valid_spots = cur.get_valid_spots();
        int valid_num = valid_spots.size();
        Node tmp;
        
        for(int i=0; i<valid_num; i++){
            OthelloBoard now = cur;
            now.put_disc(valid_spots[i]);//動下一步
            
            tmp = minimax(now, alpha, beta, depth - 1, 0);
            
            if(tmp.score > maxscore){
                maxp = valid_spots[i];
                maxscore = tmp.score;
            }
            if(maxscore >= beta)
                break;
            if(maxscore > alpha)
                alpha = maxscore;
        }
        
        return Node(maxp, maxscore);
    }else{
        
        if(!depth || cur.done)
            return Node(Point(-2,-2), cur.get_score());
        
        Point minp = Point(-100, -100);
        int minscore = INT_MAX;
        std::vector<Point> valid_spots = cur.get_valid_spots();
        int valid_num = valid_spots.size();
        Node tmp;
        
        for(int i=0; i<valid_num; i++){
            OthelloBoard now = cur;
            now.put_disc(valid_spots[i]);//動下一步
            tmp = minimax(now, alpha, beta, depth - 1, 1);
            
            if(tmp.score < minscore){
                minp = valid_spots[i];
                minscore = tmp.score;
            }
            if(minscore <= alpha)
               break;
            if(minscore < beta)
                beta = minscore;
            
        }
        return Node(minp, minscore);
    }
}*/
int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
