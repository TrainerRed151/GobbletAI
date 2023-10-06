// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#ifndef GOBBLET_HPP
#define GOBBLET_HPP

#define MAX_SCORE 20
#define TT_EXACT 0
#define TT_LOWER 1
#define TT_UPPER 2

#include <vector>
#include <string>
#include <unordered_map>

struct Coord {
    int r;
    int c;

    bool operator==(const Coord& rhs) const;
};

struct Move {
    Coord from;
    Coord to;

    bool operator==(const Move& rhs) const;
};

struct AIMove {
    Move move;
    int score;
    int depth;
};

struct TTEntry {
    int ttDEPTH;
    int ttFLAG;
    int ttVALUE;
};

class Gobblet {
private:
    bool white;
    int ply;
    std::vector<std::vector<std::vector<int>>> board, stage;
    std::unordered_map<std::string, TTEntry> transposition_table;
    std::unordered_map<std::string, Move> killer_heuristic_table;

public:
    Gobblet();
    
    bool get_turn();

    bool is_part_of_3_in_a_row(bool color, Coord coord);
    bool is_mate();

    bool move(Move coords);
    bool move(std::string alg);
    void undo_move(Move coords);
    void undo_move(std::string alg);
    std::vector<Move> legal_moves();

    std::string board_hasher();
    int board_evaluation();
    AIMove negamax(int depth, int alpha, int beta, int time_limit);
    AIMove ai(int move_time);

    Move alg_to_coord(std::string alg);
    std::string coord_to_alg(Move coords);

    void display();

    ~Gobblet();
};

#endif /* end of include guard: GOBBLET_HPP */
