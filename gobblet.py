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
                if sum(b[i] > 0) == 4:
                    return a

                if sum(a*b[:,i] > 0) == 4:
                    return a

                if sum(a*b.diagonal() > 0) == 4:
                    return a

        return 0

    def move(self, coords):
        if coords in self.legal_moves():
            return False

        r1, c1, r2, c2 = coords

        if r1 == -1:
           self.board[r2][c2].append(self.stage[self.turn][c1].pop())

        else:
            self.board[r2][c2].append(self.board[r1][c1].pop())

        self.turn = (self.turn + 1) % 2
        return True

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

    def display(self):
        print(self.to_array())
        print(self.stage)

    def get_turn(self):
        return self.turn


if __name__ == '__main__':
    game = Gobblet()
    game.display()

    while True:
        print(f'Turn: {game.get_turn()}')
        #print(f'Legal Moves: {game.legal_moves()}')
        move = input('Move: ')
        if move == 'end':
            break

        coords = [int(x) for x in move.split(',')]

        if not game.move(coords):
            print('Illegal move')
            continue

        game.display()

        if game.is_mate():
            side = game.get_turn() + 1 % 2
            print(f'{side} wins!')
            break
