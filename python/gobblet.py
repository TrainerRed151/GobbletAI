# Copyright © 2023 Brian Pomerantz. All Rights Reserved.

import sys
import time
from colorama import Fore

TIME_LIMIT = 10
MAX_SCORE = 20


class Gobblet:
    def __init__(self):
        self.board = [[[], [], [], []], [[], [], [], []], [[], [], [], []], [[], [], [], []]]
        self.stage = [
            [[1, 2, 3, 4], [1, 2, 3, 4], [1, 2, 3, 4]],
            [[-1, -2, -3, -4], [-1, -2, -3, -4], [-1, -2, -3, -4]]
        ]
        self.white = True
        self.ply = 0

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
        max_depth = 30
        if move_time < 0:
            max_depth = -move_time
            move_time = 300

        time_limit = time.time() + move_time
        depth = 1
        color = [-1, 1][self.white]
        best_score, best_move = self.negamax(depth, -MAX_SCORE, MAX_SCORE, time_limit)
        best_score *= color

        while depth < max_depth:
            if self.white and best_score == MAX_SCORE:
                return depth, best_score, best_move

            if not self.white and best_score == -MAX_SCORE:
                return depth, best_score, best_move

            depth += 1
            new_score, new_move = self.negamax(depth, -MAX_SCORE, MAX_SCORE, time_limit)

            if new_score is None:
                depth -= 1
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
