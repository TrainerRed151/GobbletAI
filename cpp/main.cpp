// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include "gobblet.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
    Gobblet game = Gobblet();
    game.display();

    game.move("x1a1");
    game.display();
    cout << game.is_mate() << endl;
    game.move("x1b1");
    game.display();
    cout << game.is_mate() << endl;

    game.move("x1a2");
    game.display();
    cout << game.is_mate() << endl;
    game.move("x1b2");
    game.display();
    cout << game.is_mate() << endl;

    game.move("x1a3");
    game.display();
    cout << game.is_mate() << endl;
    game.move("x1b3");
    game.display();
    cout << game.is_mate() << endl;

    game.move("x1a4");
    game.display();
    cout << game.is_mate() << endl;
    game.move("x1c4");
    game.display();
    cout << game.is_mate() << endl;
    
    return 0;
}
