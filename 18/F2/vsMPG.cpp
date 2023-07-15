#include <iostream>
#include "minimax.h"

using namespace std;

int main() {
    int input;
    MancalaState* game = new MancalaState();
    game->printState();
    while (game->isPlayingA()) {
        cout << "Computer (A) is making a move" << endl;
        minimax(&game, 7);
        game->printState();       
    };
    while (!game->isPlayingA()) {
        cout << "Enter move (cell) for B: ";
        cin >> input;
        game->play(input);
        game->printState();
    };
    while(game->isPlayingA()) {
        cout << "Computer (A) is making a move" << endl;
        minimax(&game, 7);
        game->printState();    
    };
    while (!game->isGameOver()) {
        if (!game->isPlayingA()) {
            cout << "Enter move (cell) for B: ";
            cin >> input;
            game->play(input);
        } else {
            cout << "Computer (A) is making a move" << endl;
            minimax(&game, 7);
        };
        game->printState();
    };
    game->printState();
    cout << "GAME OVER!" << endl;
    cout << "Player A scored " << game->getScoreA() << " points" << endl;
    cout << "Player B scored " << game->getScoreB() << " points" << endl;
    return 0;
};