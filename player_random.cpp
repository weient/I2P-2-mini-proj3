#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Point {
    int x, y;
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
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
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

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

/*int minimax(OthelloBoard cur,int alpha,int beta,int depth, int type){
    if(type){
        if(!depth || cur.done)
            return cur.get_score();
        
            
        //Point maxChild = Point(-100,-100);
        int maxscore = INT_MIN;
        std::vector<Point> valid_spots = cur.get_valid_spots();
        int valid_num = valid_spots.size();
        //Node ret_Node;
        if(valid_num==0){ //直接換下一個人
            OthelloBoard now = cur;
            now.cur_player = 3 - now.cur_player;
            int tmp = minimax(now, alpha, beta, depth - 1, 1);
            //ret_Node = min(new_game, _alpha, _beta, _step_left-1);
            if(tmp > maxscore)
                maxscore = tmp;
            return maxscore;
        }
        
        for(int i=0; i<valid_num; i++){
            OthelloBoard now = cur;
            now.put_disc(valid_spots[i]);//動下一步
            int tmp = minimax(now, alpha, beta, depth - 1, 0);
            //ret_Node = min(new_game, _alpha, _beta, _step_left-1);
            if(tmp > maxscore){
                finall = valid_spots[i];
                //maxChild = valid_spots[i];
                maxscore = tmp;
            }
            if(maxscore >= beta)
                break;
            if(maxscore > alpha)
                alpha = maxscore;
            
                
        }
        return maxscore;
    }else{
        if(!depth || cur.done)
            return cur.get_score();
        
        //Point minChild = Point(-100,-100);
        int minscore = INT_MAX;
        std::vector<Point> valid_spots = cur.get_valid_spots();
        int valid_num = valid_spots.size();
        //Node ret_Node;
        if(valid_num==0)//不能動，通常就代表要輸了
            return Node(Point(-1, -1), minUtility);
        if(valid_num==0){ //直接換下一個人
            OthelloBoard now = cur;
            now.cur_player = 3 - now.cur_player;
            int tmp = minimax(now, alpha, beta, depth - 1, 0);
            //ret_Node = Max(new_game, _alpha, _beta, _step_left-1);
            if(tmp > minscore)
                minscore = tmp;
            
            return minscore;
        }
        for(int i=0; i < valid_num; i++){
            OthelloBoard now = cur;
            now.put_disc(valid_spots[i]);//動下一步
            int tmp = minimax(now, alpha, beta, depth - 1, 1);
            //ret_Node = Max(new_game, _alpha, _beta, _step_left-1);
            if(tmp < minscore){
                finall = valid_spots[i];
                minscore = tmp;
            }
            if(minscore <= alpha)
               break;
            if(minscore < beta)
                beta = minscore;
            
                
        }
        return minscore;
    }
}*/
