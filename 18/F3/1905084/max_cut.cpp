#include<iostream>
#include<fstream>
#include<vector>
#include<set>
#include<tuple>
#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<cmath>

using namespace std;

enum constr_algo {
    greedy,
    semi_greedy,
    randomised
};

class Graph {

    int nV, nE;
    vector<tuple<double, int, int>> edge_list;
    double** adj_mat;
    set<int> S, _S;
    double w (set<int>& S, set<int>& _S);
    double greedy_random_maxcut (double alpha, set<int>& S, set<int>& _S);
    pair<double, int>  local_search (set<int>& S, set<int>& _S);
    void print_partition (set<int>& S, set<int>& _S);

    inline double semi_greedy_maxcut (set<int>& S, set<int>& _S);
    inline double simple_greedy_maxcut (set<int>& S, set<int>& _S);
    inline double random_maxcut (set<int>& S, set<int>& _S);

    public:
        Graph (int nV);
        void add_edge (int v, int w, double weight);
        void print_matrix();
        void print_edges();
        double maxcut_GRASP (int max_itr, constr_algo algo, bool print_steps, bool print_part);
        void print_maxcut_GRASP_steps (int max_itr, string input_file, int rand_itr);
};


Graph::Graph (int nV) {
    this->nV = nV, this->nE = 0;
    adj_mat = new double* [nV];
    for (int i=0; i<nV; i++) {
        adj_mat[i] = new double[nV];
        for (int j=0; j<nV; j++) adj_mat[i][j] = 0;
    };
};

void Graph::add_edge (int u, int v, double weight) {
    nE++;
    edge_list.push_back (make_tuple(weight, u, v));
    adj_mat[u][v] = adj_mat[v][u] = weight;
};

void Graph::print_matrix() {
    for (int i=0; i<nV; i++) {
        for (int j=0; j<nV; j++)
            cout << adj_mat[i][j] << " ";
        cout << endl;
    };
    cout << endl;
};

void Graph::print_edges() {
    int i;
    cout << "{" << endl;
    for (i=0; i<nE; i++) 
        cout << "  " << get<0>(edge_list[i]) << "(" 
             << get<1>(edge_list[i])+1 << ", " << get<2>(edge_list[i])+1 << ")" << endl;
    cout << "}" << endl;
};

double Graph::w (set<int>& S, set<int>& _S) {
    double w = 0;
    for (set<int>::iterator i = S.begin(); i!=S.end(); i++) 
        for (set<int>::iterator j = _S.begin(); j!=_S.end(); j++)
            w += adj_mat[*i][*j]; 
    return w;
};

double Graph::semi_greedy_maxcut (set<int>& S, set<int>& _S) {
    double alpha = 0.01 * (10 + rand()%80);
    return greedy_random_maxcut (alpha, S, _S);
};

double Graph::simple_greedy_maxcut (set<int>& S, set<int>& _S) {
    return greedy_random_maxcut (1.0, S, _S);
};

double Graph::random_maxcut (set<int>& S, set<int>& _S) {
    return greedy_random_maxcut (0.0, S, _S);
};

double Graph::greedy_random_maxcut (double alpha, set<int>& S, set<int>& _S) {
    double w_min = get<0>(*min_element(edge_list.begin(), edge_list.end()));
    double w_max = get<0>(*max_element(edge_list.begin(), edge_list.end()));
    double mu = w_min + alpha*(w_max - w_min);
    vector<tuple<double, int, int>> RCL_e;
    for (int i=0; i<nE; i++) if (get<0>(edge_list[i]) >= mu) RCL_e.push_back(edge_list[i]);
    tuple<double, int, int> e = RCL_e[rand()%RCL_e.size()];
    set<int> X, Y, XUY, V;
    for (int i=0; i<nV; i++) V.insert(i);
    X.insert (get<1>(e)), Y.insert (get<2>(e));
    set_union (X.begin(), X.end(), Y.begin(), Y.end(), inserter(XUY, XUY.end()));
    while (XUY != V) {
        set<int> V_;
        set_difference (V.begin(), V.end(), XUY.begin(), XUY.end(), inserter(V_, V_.end()));
        double *sig_X = new double[nV], *sig_Y = new double[nV];
        double sig_X_min = INFINITY, sig_Y_min = INFINITY, sig_X_max = -INFINITY, sig_Y_max = -INFINITY;
        for (int i=0; i<nV; i++) sig_X[i] = sig_Y[i] = 0;
        for (set<int>::iterator i = V_.begin(); i!=V_.end(); i++) {
            for (set<int>::iterator k = Y.begin(); k!=Y.end(); k++) {
                sig_X[*i] += adj_mat[*i][*k];
            };
            if (sig_X[*i] < sig_X_min) sig_X_min = sig_X[*i];
            if (sig_X[*i] > sig_X_max) sig_X_max = sig_X[*i];
            for (set<int>::iterator k = X.begin(); k!=X.end(); k++) {
                sig_Y[*i] += adj_mat[*i][*k];
            };
            if (sig_Y[*i] < sig_Y_min) sig_Y_min = sig_Y[*i];
            if (sig_Y[*i] > sig_Y_max) sig_Y_max = sig_Y[*i];
        };
        w_min = min (sig_X_min, sig_Y_min);
        w_max = max (sig_Y_max, sig_Y_max);
        mu = w_min + alpha*(w_max - w_min);
        vector<int> RCL_v;
        for (set<int>::iterator i = V_.begin(); i!=V_.end(); i++) {
            if (max(sig_X[*i], sig_Y[*i]) >= mu) RCL_v.push_back(*i);
        };
        int v_ = RCL_v[rand()%RCL_v.size()];
        if (sig_X[v_] > sig_Y[v_]) X.insert(v_);
        else Y.insert(v_);
        set_union (X.begin(), X.end(), Y.begin(), Y.end(), inserter(XUY, XUY.end()));
        delete[] sig_Y, delete[] sig_X;
    };
    S = X, _S = Y;
    return w(S, _S);
};

pair<double, int> Graph::local_search (set<int>& S, set<int>& _S) {
    bool change = true;
    int itr = 0;
    while (change) {
        itr++;
        change = false;
        double *sig_X = new double[nV], *sig_Y = new double[nV];
        for (int i=0; i<nV; i++) {
            sig_X[i] = sig_Y[i] = 0;
            for (set<int>::iterator k = _S.begin(); k!=_S.end(); k++) 
                sig_X[i] += adj_mat[i][*k];
            for (set<int>::iterator k = S.begin(); k!=S.end(); k++)
                sig_Y[i] += adj_mat[i][*k];
        };
        for (int v=0; v<nV && !change; v++) {
            if (S.count(v)>0 && sig_Y[v]-sig_X[v]>0) {
                S.erase(v), _S.insert(v), change = true;
            } else if (_S.count(v)>0 && sig_X[v]-sig_Y[v]>0) {
                _S.erase(v), S.insert(v), change = true;
            };
        };
    };
    return {w(S, _S), itr};
};

double Graph::maxcut_GRASP (int max_itr, constr_algo algo, bool print_steps=false, bool print_part=false) {
    double w_ = -INFINITY, w_ls = 0;
    set<int> S, _S;
    int ls_itr = 0;
    double (Graph::*construct_maxcut) (set<int>&, set<int>&);
    if (algo == greedy) construct_maxcut = &Graph::simple_greedy_maxcut;
    else if (algo == semi_greedy) construct_maxcut = &Graph::semi_greedy_maxcut;
    else if (algo == randomised) construct_maxcut = &Graph::random_maxcut;
    for (int i=0; i<max_itr; i++) {
        double w = (this->*construct_maxcut)(S, _S);
        pair<double, int> ls = local_search (S, _S);
        w = ls.first, w_ls += ls.first, ls_itr += ls.second;
        if (w > w_) {
            w_ = w;  
            if (print_part) print_partition (S, _S);
        };  
    };
    ls_itr /= max_itr, w_ls /= max_itr;
    if (print_steps) cout << ls_itr << "," << (int)w_ls << "," << max_itr << ",";
    return w_;
};

void Graph::print_partition (set<int>& S, set<int>& _S) {
    cout << endl; cout << endl;  
    for (set<int>::iterator k = S.begin(); k!=S.end(); k++) cout << *k + 1<< " ";
    cout << endl;
    for (set<int>::iterator k = _S.begin(); k!=_S.end(); k++) cout << *k + 1 << " ";
    cout << endl; cout << endl;  
};

void Graph::print_maxcut_GRASP_steps (int max_itr, string input_file, int rand_itr = 10) {
    double w_ = -INFINITY;
    set<int> S, _S;
    double w_semi_greedy=0, w_random=0;
    double w_greedy = simple_greedy_maxcut (S, _S); S.clear(), _S.clear();
    for (int i=0; i<rand_itr; i++) {
        w_semi_greedy += semi_greedy_maxcut (S, _S); S.clear(), _S.clear();
        w_random += random_maxcut (S, _S); S.clear(), _S.clear();
    };
    w_semi_greedy /= rand_itr, w_random /= rand_itr;
    cout << input_file << "," << nV << "," << nE << "," 
         << (int)w_random << "," << w_greedy << "," << (int)w_semi_greedy << ",";
    cout <<  maxcut_GRASP (max_itr, randomised, true) << ",randomised" << endl;
    cout << input_file << "," << nV << "," << nE << "," 
         << (int)w_random << "," << w_greedy << "," << (int)w_semi_greedy << ",";    
    cout << maxcut_GRASP (max_itr, greedy, true) << ",simple_greedy" << endl;
    cout << input_file << "," << nV << "," << nE << "," 
         << (int)w_random << "," << w_greedy << "," << (int)w_semi_greedy << ",";
    cout << maxcut_GRASP (max_itr, semi_greedy, true) << ",semi-greedy" << endl;
};

int main (int argc, char** argv) {
    srand(0);
    int V, E, n_itr;
    ifstream fin;
    fin.open(argv[1]);
    n_itr = argc>2? atoi(argv[2]) : 50;
    fin >> V >> E;
    Graph* g = new Graph(V);
    for (int i=0; i<E; i++) {
        int u, v, W;
        fin >> u >> v >> W;
        g->add_edge(u-1, v-1, W);
    };
    g->print_maxcut_GRASP_steps (n_itr, argv[1]);
    fin.close();
    return 0;
};