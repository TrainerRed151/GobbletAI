// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include <vector>
#include <string>
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
    bool piece_color = false;

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
    //legal = false;
    //for (Move move : legal_moves()) {
    //    if 
    //    return False

    int r1 = coords.from.r;
    int c1 = coords.from.c;
    int r2 = coords.to.r;
    int c2 = coords.to.c;

    if (r1 == -1) {
        int piece = stage[(int) white][c1].back();
        board[r2][c2].push_back(piece);
        stage[(int) white][c1].pop_back();
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

/*

    def undo_move(self, coords):
        self.white = not self.white
        self.ply -= 1
        r2, c2, r1, c1 = coords

        if r2 == -1:
           self.stage[int(not self.white)][c2].append(self.board[r1][c1].pop())

        else:
            self.board[r2][c2].append(self.board[r1][c1].pop())

    def legal_moves(self):
        opponent = not self.white
        moves = []
        for i, stack in enumerate(self.stage[int(not self.white)]):
            if not stack:
                continue

            piece = stack[-1]
            for r in range(4):
                for c in range(4):
                    if not self.board[r][c]:
                        moves.append((-1, i, r, c))

                    elif abs(piece) > abs(self.board[r][c][-1]) and self.is_part_of_3_in_a_row(opponent, (r, c)):
                        moves.append((-1, i, r, c))

        for r in range(4):
            for c in range(4):
                if self.board[r][c]:
                    piece = self.board[r][c][-1]
                    color = piece > 0
                    if color == self.white:
                        for r2 in range(4):
                            for c2 in range(4):
                                if not self.board[r2][c2]:
                                    moves.append((r, c, r2, c2))

                                elif abs(piece) > abs(self.board[r2][c2][-1]):
                                    moves.append((r, c, r2, c2))

        return moves

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

    def display(self):
        cs = 'abcd'

        print(Fore.WHITE + '  a b c d')

        for r, row in enumerate(self.board[::-1]):
            print(Fore.WHITE + str(4 - r), end=' ')
            for stack in row:
                if not stack:
                    print(Fore.WHITE + '.', end=' ')
                elif stack[-1] > 0:
                    print(Fore.GREEN + str(stack[-1]), end=' ')
                else:
                    print(Fore.RED + str(-stack[-1]), end=' ')

            print(Fore.WHITE + f'{4 - r}')

        print(Fore.WHITE + '  a b c d')

        for side in self.stage:
            print(Fore.WHITE + '(', end='')
            for stack in side:
                if not stack:
                    print(Fore.WHITE + '.', end=' ')
                else:
                    if stack[-1] > 0:
                        print(Fore.GREEN + str(stack[-1]), end=' ')
                    else:
                        print(Fore.RED + str(-stack[-1]), end=' ')

            print(Fore.WHITE + '\b)')

    def get_turn(self):
        return self.white

    def alg_to_coord(self, alg):
        letter_to_coord_map = {'a': 0, 'b': 1, 'c': 2, 'd': 3}

        if alg[0] == 'x':
            r1 = -1
            c1 = int(alg[1]) - 1
        else:
            r1 = int(alg[1]) - 1
            c1 = letter_to_coord_map[alg[0]]

        r2 = int(alg[3]) - 1
        c2 = letter_to_coord_map[alg[2]]

        return (r1, c1, r2, c2)

    def coord_to_alg(self, coords):
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