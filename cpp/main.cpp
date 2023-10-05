// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include "gobblet.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
    Gobblet game = Gobblet();
    string move, move_in, turn_str;

    while (true) {
        game.display();
        turn_str = (game.get_turn()) ? "White" : "Black";
        cout << "Turn: " << turn_str << endl;
        cout << "Move: ";
        cin >> move_in;

        if (move_in == "end") {
            break;
        }

        else if (move_in == "undo") {
            game.undo_move(move);
        }

        else if (move_in == "ai") {
            
        }

        else {
            move = move_in;
        }

        if (!game.move(move)) {
            cout << "Illegal move" << endl;
            continue;
        }

        if (game.is_mate()) {
            game.display();
            turn_str = (game.get_turn()) ? "Black" : "White";
            cout << turn_str << " wins!" << endl;
            break;
        }
    }

    return 0;
}
