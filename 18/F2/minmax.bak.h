#include <iostream>
#include <vector>
#include "MancalaSate.h"

using namespace std;

int minimax (MancalaState* state, int depth, int alpha, int beta, int initDepth) {
    cout << "depth = " << depth << endl;
    if (depth==0 || state->isGameOver()) return state->h2();

    if (state->isPlayingA()) {
        int maxEval = INT32_MIN;
        vector<MancalaState*>* children = state->getChildren();
        vector<MancalaState*>::iterator it;
        for (it = children->begin(); it!=children->end(); it++) {
            int eval = minimax (*it, depth-1, alpha, beta, initDepth);
            maxEval = max (maxEval, eval);
            if (depth==initDepth) cout << "==================" << endl;
            if (maxEval == eval) cout << "_______________" << endl;
            cout << "maxEval = " << maxEval << endl;
            cout << "depth = " << depth << endl;
            if (maxEval == eval || depth==initDepth) (*it)->printState();
            if (depth==initDepth) cout << "==================" << endl;
            alpha = max (alpha, eval);
            if (beta <= alpha) break;
        };
        return maxEval;
    } else {
        int minEval = INT32_MAX;
        vector<MancalaState*>* children = state->getChildren();
        vector<MancalaState*>::iterator it;
        for (it = children->begin(); it!=children->end(); it++) {
            int eval = minimax (*it, depth-1, alpha, beta, initDepth);
            minEval = min (minEval, eval);
            if (depth==initDepth) cout << "==================" << endl;
            if (minEval == eval) cout << "_______________" << endl;
            cout << "minEval = " << minEval << endl;
            cout << "depth = " << depth << endl;
            if (minEval == eval || depth==initDepth) (*it)->printState();
            if (depth==initDepth) cout << "==================" << endl;
            beta = min (beta, eval);
            if (beta <= alpha) break;
        };
        return minEval;
    };
    return 0;
};