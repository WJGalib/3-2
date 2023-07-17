#include <iostream>
#include <cstdlib>
#include <ctime>
#include "minimax.h"

using namespace std;

int main() {
    srand(time(0));
    int input, depth = 6 + rand()%5;
    MancalaState* game = new MancalaState();

    cout << "1) User (A) vs User (B)" << endl;
    cout << "2) User (A) vs Computer (B)" << endl;
    cout << "3) Computer (A) vs User (B)" << endl;
    cout << "4) Computer (A) vs Computer (B)" << endl;
    cout << "Select game mode: ";
    cin >> input;
    cout << endl;
    game->printState();
    if (input == 1) {
        while (!game->isGameOver()) {
            if (game->isPlayingA()) cout << "Enter move (cell) for A: ";
            else cout << "Enter move (cell) for B: ";
            cin >> input;
            game->play(input);
            game->printState();
        };
    } else if (input == 2) {
        while (!game->isGameOver()) {
            if (game->isPlayingA()) {
                cout << "Enter move (cell) for A: ";
                cin >> input;
                game->play(input);
            } else {
                cout << "Computer (B) is making a move: " << endl;
                minimax(&game, depth);
            };
            game->printState();
        };
    } else if (input == 3) {
        while (!game->isGameOver() && game->isPlayingA()) {
            cout << "Computer (A) is making a move: " << endl;
            minimax(&game, depth);
            game->printState();       
        };
        while (!game->isGameOver()) {
            if (!game->isPlayingA()) {
                cout << "Enter move (cell) for B: ";
                cin >> input;
                game->play(input);
            } else {
                cout << "Computer (A) is making a move: " << endl;
                minimax(&game, depth);
            };
            game->printState();
        };
    } else if (input == 4) {
        while (!game->isGameOver()) {
            if (game->isPlayingA()) cout << "Computer (A) is making a move: " << endl;
            else cout << "Computer (B) is making a move: " << endl;
            minimax (&game, depth);
            game->printState();
        };
    };

    cout << "GAME OVER!" << endl;
    int scoreA = game->getScoreA(), scoreB = game->getScoreB();
    cout << "Player A scored " << scoreA << " points" << endl;
    cout << "Player B scored " << scoreB << " points" << endl;
    game->printState();
    if (scoreA > scoreB) cout << "Player A wins!!" << endl;
    else if (scoreA < scoreB) cout << "Player B wins!!" << endl;
    else cout << "Match drawn!!" << endl;

    return 0;
};