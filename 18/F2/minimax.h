#include <iostream>
#include <vector>
#include "MancalaSate.h"

using namespace std;

int minimaxInit (MancalaState* state, int depth, int alpha, int beta, int initDepth, MancalaState** initStateRef) {
    //cout << "depth = " << depth << endl;
    if (depth==0 || state->isGameOver()) return state->h4();

    if (state->isPlayingA()) {
        int maxEval = INT32_MIN;
        vector<MancalaState*>* children = state->getChildren();
        vector<MancalaState*>::iterator it;
        for (it = children->begin(); it!=children->end(); it++) {
            int eval = minimaxInit (*it, depth-1, alpha, beta, initDepth, initStateRef);
            maxEval = max (maxEval, eval);
            if (maxEval == eval && depth==initDepth) *initStateRef = (*it);
            alpha = max (alpha, eval);
            if (beta <= alpha) break;
        };
        return maxEval;
    } else {
        int minEval = INT32_MAX;
        vector<MancalaState*>* children = state->getChildren();
        vector<MancalaState*>::iterator it;
        for (it = children->begin(); it!=children->end(); it++) {
            int eval = minimaxInit (*it, depth-1, alpha, beta, initDepth, initStateRef);
            minEval = min (minEval, eval);
            if (minEval == eval && depth==initDepth) *initStateRef = (*it);
            beta = min (beta, eval);
            if (beta <= alpha) break;
        };
        return minEval;
    };
    return 0;
};

void minimax (MancalaState** initState, int depth) {
    minimaxInit (*initState, depth, INT32_MIN, INT32_MAX, depth, initState);
};
