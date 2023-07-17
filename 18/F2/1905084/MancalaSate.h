#include <cstdio>
#include <vector>

class MancalaState {
    int* pits;
    bool playingA;
    int additionalMoves = 0, stonesCaptured = 0, prevMove = -1;

    public:

    MancalaState (bool playingA = true) {
        this->pits = new int[14];
        for (int i=0; i<6; i++) this->pits[i] = 4;
        for (int i=7; i<13; i++) this->pits[i] = 4;
        this->playingA = playingA;
    };

    MancalaState (int* pits, bool playingA = true) {
        this->pits = new int[14];
        for (int i=0; i<14; i++) this->pits[i] = pits[i];
        this->playingA = playingA;
    };

    MancalaState (int* pits, int additionalMoves, int stonesCaptured, bool playingA = true) {
        this->pits = new int[14];
        for (int i=0; i<14; i++) this->pits[i] = pits[i];
        this->playingA = playingA;
        this->additionalMoves = additionalMoves, this->stonesCaptured = stonesCaptured;
    };

    ~MancalaState() {
        delete pits;
    };

    int getScoreA() {
        return pits[6];
    };

    int getScoreB() {
        return pits[13];
    };

    int move (int n) {
        if (n==6 || n==13) return -1;
        prevMove = n;
        int stones = pits[n], i, k;
        if (stones==0) {
            //printf("Invalid move : Empty pit!\n");
            return n<6? -6 : -13;
        };
        pits[n] = 0;
        k = stones;
        for (i=n+1; k>0; i=(i+1)%14) {
            if (n<6 && i==13 || n>6 && i==6) {
                continue;
            };
            pits[i]++, k--;
        };
        i = (i==0)? 13 : i-1;
        if (i!=13 && i!=6 && pits[i]==1) {
            if (playingA && i<6 || !playingA && i>6) {
                if (pits[12-i] > 0) {
                    int manI, inc;
                    if (i<6) manI = 6;
                    else manI = 13;
                    inc = pits[i] + pits[12-i];
                    stonesCaptured += (inc * n<6? 1 : -1);
                    pits[manI] += inc;
                    pits[i] = pits[12-i] = 0;
                };
            };
        };
        //printf("i = %d\n", i);
        return i;
    };

    bool moveA (int n) {
        if  (!playingA || n<0 || n>5) return false;
        int mr = move(n);
        bool repeat = (mr==6 || mr==-6);
        if (mr==6) additionalMoves += repeat;
        return repeat;
    };

    bool moveB (int n) {
        if  (playingA || n<0 || n>5) return false;
        int mr = move(7 + n);
        bool repeat = (mr==13 || mr==-13);
        if (mr==13) additionalMoves -= repeat;
        return repeat;
    };

    bool isPlayingA() {
        return this->playingA;
    };

    void play (int n) {
        if  (n<0 || n>5) return;
        if (playingA) playingA = moveA(n);
        else playingA = !moveB(n);
    };

    bool isGameOver() {
        int sumA = 0, sumB = 0;
        for (int i=0; i<6; i++) sumA += pits[i];
        for (int i=7; i<13; i++) sumB += pits[i];
        if (sumA==0 && sumB>0) { 
            pits[13] += sumB;
            for (int i=7; i<13; i++) pits[i] = 0;
            return true;
        } else if (sumB==0 && sumA>0) {
            pits[6] += sumA;
            for (int i=0; i<6; i++) pits[i] = 0;
            return true;
        } else if (sumB==0 && sumA==0) return true;
        return false;
    };

    void printState() {
        if (prevMove>6 && prevMove<13) {
            printf("      ");
            int k = prevMove - 7; 
            k = 5 - k;
            for (int i=0; i<k; i++) printf("     ");
            printf("┌──┐\n");
        };
        printf("       5    4    3    2    1    0\n");
        printf("+————+————+————+————+————+————+————+————+\n");
        printf("│ PB │");
        for (int i=12; i>=7; i--) printf(" %2d │", pits[i]);
        printf(" PA │\n");
        printf("│    │————+————+————+————+————+————│    │\n"); 
        printf("│ %2d │", pits[13]);
        for (int i=0; i<6; i++) printf(" %2d │", pits[i]);
        printf(" %2d │\n", pits[6]);
        printf("+————+————+————+————+————+————+————+————+\n");
        printf("       0    1    2    3    4    5\n");
        if (prevMove<6 && prevMove>=0) {
            printf("      ");
            for (int i=0; i<prevMove; i++) printf("     ");
            printf("└──┘\n\n");
        };
    };

    MancalaState* getCopy() {
        int* pitsCopy = new int[14];
        for (int i=0; i<14; i++) pitsCopy[i] = pits[i];
        MancalaState* r = new MancalaState (pitsCopy, additionalMoves, stonesCaptured, playingA);
        return r;
    };

    bool operator == (MancalaState const& other) {
        for (int i=0; i<14; i++) 
            if (this->pits[i] != other.pits[i]) return false;
        if (this->additionalMoves != other.additionalMoves) return false;
        if (this->stonesCaptured != other.stonesCaptured) return false;
        if (this->playingA != other.playingA) return false;
        return true;
    };

    std::vector<MancalaState*>* getChildren() {
        std::vector<MancalaState*>* list = new std::vector<MancalaState*>();
        for (int i=0; i<6; i++) {
            MancalaState* child = this->getCopy();
            child->play(i);
            if (!(*this == *child)) list->push_back(child);
            else delete child; 
        };
        return list;
    };

    int h1() {
        return getScoreA() - getScoreB();
    };

    int h2() {
        int sumA = 0, sumB = 0, h2;
        for (int i=0; i<6; i++) sumA += pits[i];
        for (int i=7; i<13; i++) sumB += pits[i];
        h2 = sumA - sumB;
        return 10*h1() + 6*h2;
    };

    int h3() {
        return h2() + 6*additionalMoves;
    };

    int h4() {
        return h3() + 8*stonesCaptured;
    };

    int h5() {
        return 10*h1() + 6*additionalMoves; 
    };

    int h6() {
        return h5() + 8*stonesCaptured;
    };

    int h7() {
        return h2() + 50*h1() + 60*additionalMoves + 80*stonesCaptured;
    };
};
