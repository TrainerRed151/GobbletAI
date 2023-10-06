// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include <ctime>
#include "gobblet.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
    Gobblet game = Gobblet();
    string alg, move_in, turn_str;
    bool once = false;

    int move_time = 10;
    if (argc == 2) {
        move_time = atoi(argv[1]);
    }

    if (move_time < 0) {
        once = true;
    }
        
    while (true) {
        game.display();
        turn_str = (game.get_turn()) ? "White" : "Black";
        cout << "Turn: " << turn_str << endl;
        cout << "Move: ";
        if (!once) {
            cin >> move_in;
        }
        else {
            move_in = "ai";
        }

        if (move_in == "end") {
            break;
        }

        else if (move_in == "undo") {
            game.undo_move(alg);
            continue;
        }

        else if (move_in == "ai") {
            int t1 = clock();
            AIMove ai_move = game.ai(move_time);
            int t2 = clock();
            int secs = (t2 - t1)/CLOCKS_PER_SEC;
            alg = game.coord_to_alg(ai_move.move);
            cout << "AI: " << alg << " [" << ai_move.score << ", " << ai_move.depth << ", " << to_string(secs) << "]" << endl;
        }

        else {
            alg = move_in;
        }

        if (!game.move(alg)) {
            cout << "Illegal move" << endl;
            continue;
        }

        if (once) {
            break;
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
