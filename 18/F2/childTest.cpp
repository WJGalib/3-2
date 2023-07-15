#include <iostream>
#include "minimax.h"

using namespace std;

int main() {
    int input;
    MancalaState* game = new MancalaState();
    // game->play(5);
    game->printState();
    while (!game->isGameOver()) {
        minimax (&game, 5);
        game->printState();
    };
};