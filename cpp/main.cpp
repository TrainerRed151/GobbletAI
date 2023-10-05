// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include <ctime>
//#include <string>
#include "gobblet.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
    Gobblet game = Gobblet();
    string alg, move_in, turn_str;

    int move_time = 10;
    if (argc == 2) {
        move_time = atoi(argv[1]);
    }
        
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
            game.undo_move(alg);
            continue;
        }

        else if (move_in == "ai") {
            int t1 = time(nullptr);
            AIMove ai_move = game.ai(move_time);
            int t2 = time(nullptr);
            alg = game.coord_to_alg(ai_move.move);
            cout << "AI: " << alg << " [" << ai_move.score << ", " << ai_move.depth << ", " << to_string(t2-t1) << "]" << endl;
        }

        else {
            alg = move_in;
        }

        if (!game.move(alg)) {
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
