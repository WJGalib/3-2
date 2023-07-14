#include <iostream>
#include "MancalaSate.h"

using namespace std;

int main() {
    int input;
    MancalaState* game = new MancalaState();
    game->printState();
    while (!game->isGameOver()) {
        if (game->isPlayingA()) cout << "Enter move (cell) for A: ";
        else cout << "Enter move (cell) for B: ";
        cin >> input;
        game->play(input);
        game->printState();
    };
    game->printState();
    cout << "GAME OVER!" << endl;
    cout << "Player A scored " << game->getScoreA() << " points" << endl;
    cout << "Player B scored " << game->getScoreB() << " points" << endl;
    return 0;
};