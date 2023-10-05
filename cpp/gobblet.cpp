// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "gobblet.hpp"

//using namespace std;

bool Coord::operator==(const Coord& rhs) const {
    return this->r == rhs.r && this->c == rhs.c;
}

bool Move::operator==(const Move& rhs) const {
    return this->from == rhs.from && this->to == rhs.to;
}

Gobblet::Gobblet() {
    white = true;
    ply = 0;
    max_score = 20;
    board = {{{}, {}, {}, {}}, {{}, {}, {}, {}}, {{}, {}, {}, {}}, {{}, {}, {}, {}}};
    stage = {
        {{1, 2, 3, 4}, {1, 2, 3, 4}, {1, 2, 3, 4}},
        {{-1, -2, -3, -4}, {-1, -2, -3, -4}, {-1, -2, -3, -4}}
    };
}

bool Gobblet::get_turn() {
    return white;
}

bool Gobblet::is_part_of_3_in_a_row(bool color, Coord coord) {
    int r = coord.r;
    int c = coord.c;

    int count = 0;
    int piece = 0;
    bool piece_color;

    for (int i = 0; i < 4; i++) {
        if (!board[r][i].empty()) {
            piece = board[r][i].back();
            piece_color = piece > 0;
            if (color == piece_color) {
                count += 1;
            }
        }
    }

    if (count >= 3) {
        return true;
    }

    count = 0;
    for (int i = 0; i < 4; i++) {
        if (!board[i][c].empty()) {
            piece = board[i][c].back();
            piece_color = piece > 0;
            if (color == piece_color) {
                count += 1;
            }
        }
    }

    if (count >= 3) {
        return true;
    }

    if (r == c) {
        count = 0;
        for (int i = 0; i < 4; i++) {
            if (!board[i][i].empty()) {
                piece = board[i][i].back();
                piece_color = piece > 0;
                if (color == piece_color) {
                    count += 1;
                }
            }
        }

        if (count >= 3) {
            return true;
        }
    }

    if (r == 3 - c) {
        count = 0;
        for (int i = 0; i < 4; i++) {
            if (!board[i][3-i].empty()) {
                piece = board[i][3-i].back();
                piece_color = piece > 0;
                if (color == piece_color) {
                    count += 1;
                }
            }
        }

        if (count >= 3) {
            return true;
        }
    }

    return false;
}


bool Gobblet::is_mate() {
        // flipped because turn changed after move
    int val = (white) ? -1 : 1;

    for (int i = 0; i < 4; i++) {
        if (!board[i][0].empty() && val*board[i][0].back() > 0
                and !board[i][1].empty() && val*board[i][1].back() > 0
                and !board[i][2].empty() && val*board[i][2].back() > 0
                and !board[i][3].empty() && val*board[i][3].back() > 0) {
            return true;
        }

        if (!board[0][i].empty() && val*board[0][i].back() > 0
                and !board[1][i].empty() && val*board[1][i].back() > 0
                and !board[2][i].empty() && val*board[2][i].back() > 0
                and !board[3][i].empty() && val*board[3][i].back() > 0) {
            return true;
        }
    }

    if (!board[0][0].empty() && val*board[0][0].back() > 0
            and !board[1][1].empty() && val*board[1][1].back() > 0
            and !board[2][2].empty() && val*board[2][2].back() > 0
            and !board[3][3].empty() && val*board[3][3].back() > 0) {
        return true;
    }

    if (!board[0][3].empty() && val*board[0][3].back() > 0
            and !board[1][2].empty() && val*board[1][2].back() > 0
            and !board[2][1].empty() && val*board[2][1].back() > 0
            and !board[3][0].empty() && val*board[3][0].back() > 0) {
        return true;
    }

    return false;
}

bool Gobblet::move(Move coords) {
    bool legal = false;
    for (Move move : legal_moves()) {
        if (coords == move) {
            legal = true;
            break;
        }
    }

    if (!legal) {
        return false;
    }

    int r1 = coords.from.r;
    int c1 = coords.from.c;
    int r2 = coords.to.r;
    int c2 = coords.to.c;

    if (r1 == -1) {
        int piece = stage[not white][c1].back();
        board[r2][c2].push_back(piece);
        stage[not white][c1].pop_back();
    }

    else {
        int piece = board[r1][c1].back();
        board[r2][c2].push_back(piece);
        board[r1][c1].pop_back();
    }

    white = !white;
    ply += 1;
    return true;
}

bool Gobblet::move(std::string alg) {
    Move m = alg_to_coord(alg);
    return move(m);
}

void Gobblet::undo_move(Move coords) {
    white = !white;
    ply -= 1;
    int r2 = coords.from.r;
    int c2 = coords.from.c;
    int r1 = coords.to.r;
    int c1 = coords.to.c;

    if (r2 == -1) {
        int piece = board[r1][c1].back();
        stage[!white][c2].push_back(piece);
        board[r1][c1].pop_back();
    }

    else {
        int piece = board[r1][c1].back();
        board[r2][c2].push_back(piece);
        board[r1][c1].pop_back();
    }
}

void Gobblet::undo_move(std::string alg) {
    Move m = alg_to_coord(alg);
    undo_move(m);
}

std::vector<Move> Gobblet::legal_moves() {
    bool opponent = !white;
    int piece;
    bool color;
    std::vector<Move> moves = {};

    for (int i = 0; i < 2; i++) {
        if (stage[!white][i].empty()) {
            continue;
        }

        piece = stage[!white][i].back();
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                Coord t; t.r = r; t.c = c;

                if (board[r][c].empty()) {
                    Coord f; f.r = -1; f.c = i;
                    Move m; m.from = f; m.to = t;
                    moves.push_back(m);
                }

                else if (std::abs(piece) > std::abs(board[r][c].back()) && is_part_of_3_in_a_row(opponent, t)) {
                    Coord f; f.r = -1; f.c = i;
                    Move m; m.from = f; m.to = t;
                    moves.push_back(m);
                }
            }
        }
    }

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (!board[r][c].empty()) {
                piece = board[r][c].back();
                color = piece > 0;
                if (color == white) {
                    for (int r2 = 0; r2 < 4; r2++) {
                        for (int c2 = 0; c2 < 4; c2++) {
                            if (board[r2][c2].empty()) {
                                Coord f; f.r = r; f.c = c;
                                Coord t; t.r = r2; t.c = c2;
                                Move m; m.from = f; m.to = t;
                                moves.push_back(m);
                            }

                            else if (std::abs(piece) > std::abs(board[r2][c2].back())) {
                                Coord f; f.r = r; f.c = c;
                                Coord t; t.r = r2; t.c = c2;
                                Move m; m.from = f; m.to = t;
                                moves.push_back(m);
                            }
                        }
                    }
                }
            }
        }
    }

    return moves;
}

/*
    def get_board_score(self):
        count_3 = 0
        for r in range(4):
            for c in range(4):
                if self.board[r][c]:
                    if self.is_part_of_3_in_a_row(True, (r, c)):
                        count_3 += 1

                    if self.is_part_of_3_in_a_row(False, (r, c)):
                        count_3 -= 1

        return count_3 if self.white else -count_3

    def negamax(self, depth, alpha, beta, time_limit):
        if time.time() > time_limit:
            return None, None

        if self.is_mate():
            return -MAX_SCORE, None

        if depth == 0:
            return self.get_board_score(), None

        best_score = -MAX_SCORE - 1
        best_move = None

        for move in self.legal_moves():
            self.move(move)
            value, _ = self.negamax(depth - 1, -beta, -alpha, time_limit)
            self.undo_move(move)
            if value is None:
                return None, None

            value = -value
            if value > best_score:
                best_score, best_move = value, move

            alpha = max(value, alpha)
            if alpha >= beta:
                break

        return best_score, best_move

    def ai(self, move_time=TIME_LIMIT):
        time_limit = time.time() + move_time
        depth = 1
        color = [-1, 1][self.white]
        best_score, best_move = self.negamax(depth, -MAX_SCORE, MAX_SCORE, time_limit)
        best_score *= color

        while True:
            if self.white and best_score == MAX_SCORE:
                return depth, best_score, best_move

            if not self.white and best_score == -MAX_SCORE:
                return depth, best_score, best_move

            depth += 1
            new_score, new_move = self.negamax(depth, -MAX_SCORE, MAX_SCORE, time_limit)

            if new_score is None:
                break

            best_score = color*new_score
            best_move = new_move

        return depth, best_score, best_move

*/

void Gobblet::display() {
    std::cout << "  a b c d" << std::endl;

    for (int r = 3; r >= 0; r--) {
        std::cout << r + 1 << " ";
        for (int c = 0; c < 4; c++) {
            if (board[r][c].empty()) {
                std::cout << ". ";
            }

            else if (board[r][c].back() > 0) {
                std::cout << "\033[1;32m" << board[r][c].back() << "\033[0m ";
            }

            else {
                std::cout << "\033[1;31m" << -board[r][c].back() << "\033[0m ";
            }
        }

        std::cout << r + 1 << std::endl;
    }

    std::cout << "  a b c d" << std::endl;

    for (int s = 0; s < 2; s++) {
        std::cout << "(";
        for (int i = 0; i < 3; i++) {
            if (stage[s][i].empty()) {
                std::cout << ". ";
            }

            else {
                if (stage[s][i].back() > 0) {
                    std::cout << "\033[1;32m" << stage[s][i].back() << "\033[0m ";
                }

                else {
                    std::cout << "\033[1;31m" << -stage[s][i].back() << "\033[0m ";
                }
            }
        }

        std::cout << "\b)" << std::endl;
    }
}

Move Gobblet::alg_to_coord(std::string alg) {
    Coord f, t;
    Move m;

    if (alg[0] == 'x') {
        f.r = -1;
        f.c = (alg[1] - '0') - 1;
    }

    else {
        f.r = (alg[1] - '0') - 1;
        switch (alg[0]) {
            case 'a':
                f.c = 0;
                break;
            case 'b':
                f.c = 1;
                break;
            case 'c':
                f.c = 2;
                break;
            case 'd':
                f.c = 3;
                break;
        }
    }

    t.r = (alg[3] - '0') - 1;
    switch (alg[2]) {
        case 'a':
            t.c = 0;
            break;
        case 'b':
            t.c = 1;
            break;
        case 'c':
            t.c = 2;
            break;
        case 'd':
            t.c = 3;
            break;
        }

    m.from = f;
    m.to = t;

    return m;
}

/*
std::string Gobblet::coord_to_alg(Move coords):
        coord_to_letter_map = 'abcd'

        if coords[0] == -1:
            alg = f'x{coords[1]+1}'
        else:
            alg = f'{coord_to_letter_map[coords[1]]}{coords[0]+1}'

        alg += f'{coord_to_letter_map[coords[3]]}{coords[2]+1}'

        return alg


if __name__ == '__main__':
    game = Gobblet()
    coords = None
    move_time = int(sys.argv[1]) if len(sys.argv) == 2 else TIME_LIMIT

    while True:
        game.display()
        turn_str = 'White' if game.get_turn() else 'Black'
        print(f'Turn: {turn_str}')
        move = input('Move: ')
        if move == 'end':
            print(Fore.RESET)
            break

        elif move == 'undo':
            game.undo_move(coords)
            continue

        elif move == 'ai':
            t1 = time.time()
            depth, score, coords = game.ai(move_time=move_time)
            t2 = time.time()
            print(f'AI: {game.coord_to_alg(coords)} [{score}, {depth}, {int(t2-t1)}]')

        else:
            coords = game.alg_to_coord(move)

        if not game.move(coords):
            print('Illegal move')
            continue

        if game.is_mate():
            game.display()
            turn_str = 'Black' if game.get_turn() else 'White'
            print(f'{turn_str} wins!')
            break

*/

Gobblet::~Gobblet() {}
