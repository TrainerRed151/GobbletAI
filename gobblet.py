# Copyright © 2023 Brian Pomerantz. All Rights Reserved.

import numpy as np

class Gobblet:
    def __init__(self):
        self.board = [[[], [], [], []], [[], [], [], []], [[], [], [], []], [[], [], [], []]]
        self.stage = [
            [[1, 2, 3, 4], [1, 2, 3, 4], [1, 2, 3, 4]],
            [[-1, -2, -3, -4], [-1, -2, -3, -4], [-1, -2, -3, -4]]
        ]
        self.turn = 0

    def to_array(self):
        b = []
        for row in self.board:
            b.append([])
            for stack in row:
                if len(stack) == 0:
                    b[-1].append(0)
                else:
                    b[-1].append(stack[-1])

        return np.array(b)

    def is_mate(self):
        b = self.to_array()

        for a in [-1, 1]:
            for i in range(4):
                if sum(a*b[i] > 0) == 4:
                    return a

                if sum(a*b[:,i] > 0) == 4:
                    return a

            if sum(a*b.diagonal() > 0) == 4:
                return a

            if a*b[0, 3] > 0 and a*b[1, 2] > 0 and a*b[2, 1] > 0 and a*b[3, 0] > 0:
                return a

        return 0

    def move(self, coords):
        if coords not in self.legal_moves():
            return False

        r1, c1, r2, c2 = coords

        if r1 == -1:
           self.board[r2][c2].append(self.stage[self.turn][c1].pop())

        else:
            self.board[r2][c2].append(self.board[r1][c1].pop())

        self.turn = (self.turn + 1) % 2
        return True

    def undo_move(self, coords):
        self.turn = (self.turn + 1) % 2
        r2, c2, r1, c1 = coords

        if r2 == -1:
           self.stage[self.turn][c2].append(self.board[r1][c1].pop())

        else:
            self.board[r2][c2].append(self.board[r1][c1].pop())

    def legal_moves(self):
        moves = []
        for i, stack in enumerate(self.stage[self.turn]):
            if len(stack) == 0:
                continue

            piece = stack[-1]
            for r in range(4):
                for c in range(4):
                    if len(self.board[r][c]) == 0:
                        moves.append((-1, i, r, c))

                    elif abs(piece) > abs(self.board[r][c][-1]):
                        moves.append((-1, i, r, c))

        ref = (1, 0, 0)
        for r in range(4):
            for c in range(4):
                if len(self.board[r][c]) != 0:
                    piece = self.board[r][c][-1]
                    color = ref[(abs(piece) // piece) + 1]
                    if color == self.turn:
                        for r2 in range(4):
                            for c2 in range(4):
                                if len(self.board[r2][c2]) == 0:
                                    moves.append((r, c, r2, c2))

                                elif abs(piece) > abs(self.board[r2][c2][-1]):
                                    moves.append((r, c, r2, c2))

        return moves

    def minmax(self, depth, alpha, beta):
        if self.is_mate():
            return -1 if self.turn else 1, None

        if depth == 0:
            return 0, None

        best_score = -2 if self.turn else 2
        best_move = None

        for move in self.legal_moves():
            self.move(move)
            value, _ = self.minmax(depth - 1, alpha, beta)
            self.undo_move(move)

            if self.turn:
                if value > best_score:
                    best_score, best_move = value, move

                if value >= beta:
                    break
                alpha = max(value, alpha)

            else:
                if value < best_score:
                    best_score, best_move = value, move

                if value <= alpha:
                    break
                beta = min(value, beta)

        return best_score, best_move

    def ai(self, depth=4):
        return self.minmax(depth, -1, 1)

    def display(self):
        print(self.to_array())
        print(self.stage)

    def get_turn(self):
        return self.turn


if __name__ == '__main__':
    game = Gobblet()
    coords = None

    while True:
        game.display()
        print(f'Turn: {game.get_turn()}')
        move = input('Move: ')
        if move == 'end':
            break

        elif move == 'undo':
            game.undo_move(coords)
            continue

        elif move == 'ai':
            score, coords = game.ai()
            print(f'AI: {coords} [{score}]')

        else:
            coords = tuple([int(x) for x in move.split(',')])

        if not game.move(coords):
            print('Illegal move')
            continue

        if game.is_mate():
            side = game.get_turn() + 1 % 2
            print(f'{side} wins!')
            break
