// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#ifndef GOBBLET_HPP
#define GOBBLET_HPP

#include <vector>
#include <string>

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
};

class Gobblet {
private:
    bool white;
    int ply, max_score;
    std::vector<std::vector<std::vector<int>>> board, stage;

public:
    Gobblet();
    
    bool get_turn();

    bool is_part_of_3_in_a_row(bool color, Coord coord);
    bool is_mate();

    bool move(Move coords);
    void undo_move(Move coords);
    std::vector<Move> legal_moves();

    int board_evaluation();
    AIMove negamax(int depth, int alpha, int beta);
    AIMove ai();

    Move alg_to_coord(std::string alg);
    std::string coord_to_alg(Move coords);

    void display();

    ~Gobblet();
};

#endif /* end of include guard: GOBBLET_HPP */
