// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include "gobblet.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
    Gobblet game = Gobblet();
    string alg, move_in, turn_str;

    while (true) {
        game.display();
        turn_str = (game.get_turn()) ? "White" : "Black";
        cout << "Turn: " << turn_str << endl;
        cout << "Move: ";
        //cin >> move_in;
        move_in = "ai";

        if (move_in == "end") {
            break;
        }

        else if (move_in == "undo") {
            game.undo_move(alg);
            continue;
        }

        else if (move_in == "ai") {
            AIMove ai_move = game.ai(6);
            alg = game.coord_to_alg(ai_move.move);
            cout << "AI: " << alg << " [" << ai_move.score << "]" << endl;
        }

        else {
            alg = move_in;
        }

        if (!game.move(alg)) {
            cout << "Illegal move" << endl;
            continue;
        }

        break;

        if (game.is_mate()) {
            game.display();
            turn_str = (game.get_turn()) ? "Black" : "White";
            cout << turn_str << " wins!" << endl;
            break;
        }
    }

    return 0;
}
