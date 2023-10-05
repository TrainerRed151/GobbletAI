// Copyright © 2023 Brian Pomerantz. All Rights Reserved.

#include <iostream>
#include "gobblet.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
    Gobblet game = Gobblet();

    Coord f, t;
    f.r = -1;
    f.c = 0;
    t.r = 0;
    t.c = 0;

    Move m;
    m.from = f;
    m.to = t;

    game.move(m);

    cout << game.is_mate() << endl;
    
    return 0;
}
