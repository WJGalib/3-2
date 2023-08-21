#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <climits>

using namespace std;

int localIter = 0;

std::ofstream outputFile("output.txt");


struct Edge {
    int u, v, weight;
};


bool contains(const unordered_set<int>& set, int vertex) {
    return set.find(vertex) != set.end();
}

int calcCutWeight(const vector<Edge>& edges, const unordered_set<int>& partX, const unordered_set<int>& partY) {
    int cutWeight = 0;
    for (const Edge& edge : edges) {
        if ((contains(partX, edge.u) && contains(partY, edge.v)) ||
            (contains(partX, edge.v) && contains(partY, edge.u))) {
            cutWeight += edge.weight;
        }
    }
    return cutWeight;
}

int calcGreedyValue(int vertex, const vector<Edge>& edges, const unordered_set<int>& partX, const unordered_set<int>& partY) {
    int sigmaX = 0;
    for (const Edge& edge : edges) {
        if (edge.u == vertex) {
            if (contains(partY, edge.v)) {
                sigmaX += edge.weight;
            }
        } else if (edge.v == vertex) {
            if (contains(partY, edge.u)) {  
                sigmaX += edge.weight;      
            }
        }
    }
    return sigmaX;
}

pair<unordered_set<int>, unordered_set<int>> pureGreedyMaxCut(const vector<Edge>& edges, int numVerts, int& maxCutWeight) {
    unordered_set<int> partX, partY;

    for (int vertex = 1; vertex <= numVerts; ++vertex) {
        if (vertex % 2 == 0) {
            partX.insert(vertex);
        } else {
            partY.insert(vertex);
        }
    }

    maxCutWeight = calcCutWeight(edges, partX, partY);

    bool improve = true;
    while (improve) {
        improve = false;

        for (int vertex = 1; vertex <= numVerts; ++vertex) {
            //initialization lage
            unordered_set<int>& sourcePart = contains(partX, vertex) ? partX : partY;
            unordered_set<int>& targetPart = contains(partX, vertex) ? partY : partX;

            int currCutWeight = calcCutWeight(edges, partX, partY);

            if (contains(sourcePart, vertex)) {
                sourcePart.erase(vertex);
                targetPart.insert(vertex);

                int newCutWeight = calcCutWeight(edges, partX, partY);

                if (newCutWeight > currCutWeight) {
                    improve = true;
                    maxCutWeight = newCutWeight;
                } else {
                    targetPart.erase(vertex);
                    sourcePart.insert(vertex);
                }
            }
        }
    }
    return { partX, partY };
}

pair<unordered_set<int>, unordered_set<int>> semiGreedyMaxCut(const vector<Edge>& edges, int numVerts, double alpha) {
    unordered_set<int> partX, partY;

    int wmin = INT_MAX, wmax = INT_MIN;
    for (const Edge& edge : edges) {
        wmin = min(wmin, edge.weight);
        wmax = max(wmax, edge.weight);
    }

    int mu = wmin + static_cast<int>(alpha * (wmax - wmin));

    vector<Edge> RCLe;
    for (const Edge& edge : edges) {
        if (edge.weight >= mu) {
            RCLe.push_back(edge);
        }
    }

    int randomIndex = rand() % RCLe.size();
    int iStar = RCLe[randomIndex].u;
    int jStar = RCLe[randomIndex].v;

    partX.insert(iStar);
    partY.insert(jStar);

    while (partX.size() + partY.size() < numVerts) {
        unordered_set<int> V_prime;
        for (int vertex = 1; vertex <= numVerts; ++vertex) {
            if (!contains(partX, vertex) && !contains(partY, vertex)) {
                V_prime.insert(vertex);
            }
        }

        int wmin_v = INT_MAX, wmax_v = INT_MIN;
        for (int vertex : V_prime) {
            int sigmaX_v = calcGreedyValue(vertex, edges, partX, partY);
            int sigmaY_v = calcGreedyValue(vertex, edges, partY, partX);
            wmin_v = min(wmin_v, min(sigmaX_v, sigmaY_v));
            wmax_v = max(wmax_v, max(sigmaX_v, sigmaY_v));
        }

        int mu_v = wmin_v + static_cast<int>(alpha * (wmax_v - wmin_v));

        vector<int> RCL_v;
        for (int vertex : V_prime) {
            int sigmaX_v = calcGreedyValue(vertex, edges, partX, partY);
            int sigmaY_v = calcGreedyValue(vertex, edges, partY, partX);
            if (max(sigmaX_v, sigmaY_v) >= mu_v) {
                RCL_v.push_back(vertex);
            }
        }

        if (RCL_v.empty()) {
            break; // No more candidates to choose from
        }

        int selectedVertex = RCL_v[rand() % RCL_v.size()];

        int sigmaX_star = calcGreedyValue(selectedVertex, edges, partX, partY);
        int sigmaY_star = calcGreedyValue(selectedVertex, edges, partY, partX);

        if (sigmaX_star > sigmaY_star) {
            partX.insert(selectedVertex);
        } else {
            partY.insert(selectedVertex);
        }
    }

    return { partX, partY };
}

pair<unordered_set<int>, unordered_set<int>> completelyRandomMaxCut(const vector<Edge>& edges, int numVerts) {
    unordered_set<int> partX, partY;

    // Randomly assign vertices to partitions
    for (int vertex = 1; vertex <= numVerts; ++vertex) {
        if (rand() % 2 == 0) {
            partX.insert(vertex);
        } else {
            partY.insert(vertex);
        }
    }

    int cutWeight = calcCutWeight(edges, partX, partY);
    return { partX, partY };
}


int localSearch(const vector<Edge>& edges, const unordered_set<int>& partX, const unordered_set<int>& partY, int& iter) {
    unordered_set<int> newX = partX;
    unordered_set<int> newY = partY;
    int maxVerts = newX.size()+ newY.size();

    bool change = true;
    while (change) {
        iter++;
        change = false;

        for (int vertex = 1; vertex <= maxVerts; ++vertex) {
            bool sourceChanged = false;
            bool targetChanged = false;

            if (contains(newX, vertex)) {
                int delta = calcGreedyValue(vertex, edges, newX, newY) - calcGreedyValue(vertex, edges, newY, newX);
                if (delta < 0) {
                    newX.erase(vertex);
                    newY.insert(vertex);
                    change = true;
                    break;
                }
            }
            if (contains(newY, vertex)) {
                int delta = calcGreedyValue(vertex, edges, newY, newX) - calcGreedyValue(vertex, edges, newX, newY);
                if (delta < 0) {
                    newY.erase(vertex);
                    newX.insert(vertex);
                    change = true;
                    break;
                }
            }

        }
    }
    localIter = localIter + iter;

    return calcCutWeight(edges, newX, newY);
}

pair<unordered_set<int>, unordered_set<int>> graspMaxCut(const vector<Edge>& edges, int numVerts, int maxIter) {
    unordered_set<int> bestPartX, bestPartY;
    int bestCutWeight = 0;
    int localavg = 0;
    int semiGreedyCutWeight = 0;

    double alpha = 0.8;

    for (int i = 1; i <= maxIter; ++i) {


        // Perform Semi-Greedy Max Cut
        auto semiGreedyParts = semiGreedyMaxCut(edges, numVerts, alpha);
        unordered_set<int>& semiGreedyPartX = semiGreedyParts.first;
        unordered_set<int>& semiGreedyPartY = semiGreedyParts.second;

        //Local Search to Semi-Greedy Result
        int localSearchIter = 0;
        semiGreedyCutWeight = localSearch(edges, semiGreedyPartX, semiGreedyPartY, localSearchIter);
        localavg += semiGreedyCutWeight;

        if (semiGreedyCutWeight > bestCutWeight) {
            bestCutWeight = semiGreedyCutWeight;
        }

        if (i == maxIter) {
            localavg= localavg/i;
            cout<<"Average local iterations: "<<localIter/i<<endl;
            outputFile << "Average local iterations: " << localIter/i << endl;
            cout<<"Average cut weight for local search: "<<localavg<<endl;
            outputFile << "Average cut weight for local search: " << localavg << endl;
            cout << "best cut weight for GRASP: " << bestCutWeight << endl;
            outputFile << "best cut weight for GRASP: " << bestCutWeight << endl;
            cout<<"Number of iterations for GRASP: "<<i<<endl<<endl;
            outputFile << "Number of iterations for GRASP: " << i << endl << endl;
        }
    }
    localIter = 0;

    return { bestPartX, bestPartY };
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    int numCases = 50;
    for (int caseNum = 41; caseNum <= numCases; ++caseNum) {
        std::string inputFileName = "set1/g" + std::to_string(caseNum) + ".rud";
        std::ifstream inputFile(inputFileName);
        if (!inputFile) {
            std::cerr << "Error opening input file: " << inputFileName << std::endl;
            return 1;
        }
    cout<<inputFileName<<endl;
    outputFile << inputFileName << endl;

    int numVerts, numEdges;
    inputFile >> numVerts >> numEdges;
    outputFile<<"Vertices "<<numVerts<<" Edges "<<numEdges<<endl;

    vector<Edge> edges(numEdges);
    for (int i = 0; i < numEdges; ++i) {
        inputFile >> edges[i].u >> edges[i].v >> edges[i].weight;
    }

    int maxIter = 20; // beshi kaj kore na

    int maxCutWeight, semiGreedyCutWeight, randomCutWeight;
    double alpha = 0.8;

    // Perform Greedy Max Cut
    auto greedyParts = pureGreedyMaxCut(edges, numVerts, maxCutWeight);
    unordered_set<int>& greedyPartX = greedyParts.first;
    unordered_set<int>& greedyPartY = greedyParts.second;

    maxCutWeight = calcCutWeight(edges, greedyPartX, greedyPartY);

    cout << "Max Cut Weight (Greedy): " << maxCutWeight << endl;
    outputFile << "Max Cut Weight (Greedy): " << maxCutWeight << endl;

    auto semiGreedyParts = semiGreedyMaxCut(edges, numVerts, alpha);
    unordered_set<int>& semiGreedyPartX = semiGreedyParts.first;
    unordered_set<int>& semiGreedyPartY = semiGreedyParts.second;

    semiGreedyCutWeight = calcCutWeight(edges, semiGreedyPartX, semiGreedyPartY);

    cout << "Max Cut Weight (Semi-Greedy): " << semiGreedyCutWeight << endl;
    outputFile << "Max Cut Weight (Semi-Greedy): " << semiGreedyCutWeight << endl;

    auto randomParts = completelyRandomMaxCut(edges, numVerts);
    unordered_set<int>& randomPartX = randomParts.first;
    unordered_set<int>& randomPartY = randomParts.second;

    randomCutWeight = calcCutWeight(edges, randomPartX, randomPartY);

    cout << "Max Cut Weight (Random): " << randomCutWeight << endl;
    outputFile << "Max Cut Weight (Random): " << randomCutWeight << endl;

    // GRASP Max Cut
    graspMaxCut(edges, numVerts, maxIter);

    outputFile << "====================" << std::endl;
    }

    outputFile.close();

    return 0;
}
