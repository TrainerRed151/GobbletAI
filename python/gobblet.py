# Copyright © 2023 Brian Pomerantz. All Rights Reserved.

import sys
import time
from colorama import Fore

TIME_LIMIT = 10
MAX_SCORE = 20

ttDEPTH, ttFLAG, ttVALUE = 0, 1, 2
ttEXACT, ttLOWERBOUND, ttUPPERBOUND = 0, 1, 2


class Gobblet:
    def __init__(self):
        self.board = [[[], [], [], []], [[], [], [], []], [[], [], [], []], [[], [], [], []]]
        self.stage = [
            [[1, 2, 3, 4], [1, 2, 3, 4], [1, 2, 3, 4]],
            [[-1, -2, -3, -4], [-1, -2, -3, -4], [-1, -2, -3, -4]]
        ]
        self.white = True
        self.ply = 0

        self.transposition_table = {}
        self.killer_heuristic_table = {}

    def is_part_of_3_in_a_row(self, color, coord):
        r = coord[0]
        c = coord[1]

        count = 0
        for i in range(4):
            if self.board[r][i]:
                piece = self.board[r][i][-1]
                piece_color = piece > 0
                if color == piece_color:
                    count += 1

        if count >= 3:
            return True

        count = 0
        for i in range(4):
            if self.board[i][c]:
                piece = self.board[i][c][-1]
                piece_color = piece > 0
                if color == piece_color:
                    count += 1

        if count >= 3:
            return True

        if r == c:
            count = 0
            for i in range(4):
                if self.board[i][i]:
                    piece = self.board[i][i][-1]
                    piece_color = piece > 0
                    if color == piece_color:
                        count += 1

            if count >= 3:
                return True

        if r == 3 - c:
            count = 0
            for i in range(4):
                if self.board[i][3-i]:
                    piece = self.board[i][3-i][-1]
                    piece_color = piece > 0
                    if color == piece_color:
                        count += 1

            if count >= 3:
                return True

        return False

    def is_mate(self):
        # flipped because turn changed after move
        val = 1 if not self.white else -1

        for i in range(4):
            if (self.board[i][0] and val*self.board[i][0][-1] > 0
                    and self.board[i][1] and val*self.board[i][1][-1] > 0
                    and self.board[i][2] and val*self.board[i][2][-1] > 0
                    and self.board[i][3] and val*self.board[i][3][-1] > 0):
                return True

            if (self.board[0][i] and val*self.board[0][i][-1] > 0
                    and self.board[1][i] and val*self.board[1][i][-1] > 0
                    and self.board[2][i] and val*self.board[2][i][-1] > 0
                    and self.board[3][i] and val*self.board[3][i][-1] > 0):
                return True

        if (self.board[0][0] and val*self.board[0][0][-1] > 0
                and self.board[1][1] and val*self.board[1][1][-1] > 0
                and self.board[2][2] and val*self.board[2][2][-1] > 0
                and self.board[3][3] and val*self.board[3][3][-1] > 0):
            return True

        if (self.board[0][3] and val*self.board[0][3][-1] > 0
                and self.board[1][2] and val*self.board[1][2][-1] > 0
                and self.board[2][1] and val*self.board[2][1][-1] > 0
                and self.board[3][0] and val*self.board[3][0][-1] > 0):
            return True

        return False

    def move(self, coords):
        if coords not in self.legal_moves():
            return False

        r1, c1, r2, c2 = coords

        if r1 == -1:
           self.board[r2][c2].append(self.stage[int(not self.white)][c1].pop())

        else:
            self.board[r2][c2].append(self.board[r1][c1].pop())

        self.white = not self.white
        self.ply += 1
        return True

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

        alphaOrig = alpha

        zhash = str(self.board)
        ttEntry = self.transposition_table.get(zhash)
        if ttEntry and ttEntry[ttDEPTH] >= depth:
            if ttEntry[ttFLAG] == ttEXACT:
                return ttEntry[ttVALUE], None
            elif ttEntry[ttFLAG] == ttLOWERBOUND:
                alpha = max(alpha, ttEntry[ttVALUE])
            elif ttEntry[ttFLAG] == ttUPPERBOUND:
                beta = min(beta, ttEntry[ttVALUE])

            if alpha >= beta:
                return ttEntry[ttVALUE], None

        if self.is_mate():
            return -MAX_SCORE, None

        if depth == 0:
            return self.get_board_score(), None

        best_score = -MAX_SCORE - 1

        killer = self.killer_heuristic_table.get(zhash)
        move_list = self.legal_moves()
        if killer:
            try:
                move_list.remove(killer)
            except:
                pass

            move_list.insert(0, killer)

        for move in move_list:
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

        new_ttEntry = [0, 0, 0]
        new_ttEntry[ttVALUE] = best_score
        if best_score <= alphaOrig:
            new_ttEntry[ttFLAG] = ttUPPERBOUND
        elif best_score >= beta:
            new_ttEntry[ttFLAG] = ttLOWERBOUND
        else:
            new_ttEntry[ttFLAG] = ttEXACT
        new_ttEntry[ttDEPTH] = depth
        self.transposition_table[zhash] = new_ttEntry

        return best_score, best_move

    def MTDf(self, depth, first_guess, time_limit):
        g = first_guess
        upperbound = MAX_SCORE
        lowerbound = -MAX_SCORE
        while lowerbound < upperbound:
            beta = max(g, lowerbound + 1)
            g, move = self.negamax(depth, beta - 1, beta, time_limit)

            if g is None:
                return None, None

            if g < beta:
                upperbound = g
            else:
                lowerbound = g

        return g, move

    def ai(self, move_time=TIME_LIMIT):
        max_depth = 30
        if move_time < 0:
            max_depth = -move_time
            move_time = 300

        self.transposition_table.clear()
        self.killer_heuristic_table.clear()

        time_limit = time.time() + move_time
        color = [-1, 1][self.white]

        best_score = 0
        best_move = self.legal_moves()[0]

        depth = 1
        first_guess = 0
        while depth < max_depth:
            first_guess, move = self.MTDf(depth, first_guess, time_limit)
            if first_guess is None:
                depth -= 1
                break

            best_move = move
            best_score = color*first_guess

            if abs(best_score) == MAX_SCORE:
                return depth, best_score, best_move

            depth += 1

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
    once = False
    move_time = int(sys.argv[1]) if len(sys.argv) == 2 else TIME_LIMIT

    if move_time < 0:
        once = True

    while True:
        game.display()
        turn_str = 'White' if game.get_turn() else 'Black'
        print(f'Turn: {turn_str}')
        if not once:
            move = input('Move: ')
        else:
            move = 'ai'

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

        if once:
            break

        if game.is_mate():
            game.display()
            turn_str = 'Black' if game.get_turn() else 'White'
            print(f'{turn_str} wins!')
            break
