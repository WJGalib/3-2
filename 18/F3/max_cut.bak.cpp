#include<iostream>
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
    double local_search (set<int>& S, set<int>& _S);
    void print_partition (set<int>& S, set<int>& _S);

    inline double semi_greedy_maxcut (set<int>& S, set<int>& _S);
    inline double simple_greedy_maxcut (set<int>& S, set<int>& _S);
    inline double random_maxcut (set<int>& S, set<int>& _S);

    public:
        Graph (int nV);
        void add_edge (int v, int w, double weight);
        void print_matrix();
        void print_edges();
        double maxcut_GRASP (int max_itr, bool print_steps, bool print_part);
        void print_maxcut_GRASP_steps (int max_itr);
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
    //cout << endl << endl << "outer " << RCL_e.size() << endl << endl;
    tuple<double, int, int> e = RCL_e[rand()%RCL_e.size()];
    S.insert (get<1>(e)), _S.insert (get<2>(e));
    set<int> SU_S, V;
    for (int i=0; i<nV; i++) V.insert(i);
    set_union (S.begin(), S.end(), _S.begin(), _S.end(), inserter(SU_S, SU_S.end()));
    while (SU_S != V) {
        set<int> V_;
        set_difference (V.begin(), V.end(), SU_S.begin(), SU_S.end(), inserter(V_, V_.end()));
        double *sig_X = new double[nV], *sig_Y = new double[nV];
        double sig_X_min = INFINITY, sig_Y_min = INFINITY, sig_X_max = -INFINITY, sig_Y_max = -INFINITY;
        for (int i=0; i<nV; i++) sig_X[i] = sig_Y[i] = 0;
        for (set<int>::iterator i = V_.begin(); i!=V_.end(); i++) {
            for (set<int>::iterator k = _S.begin(); k!=_S.end(); k++) {
                sig_X[*i] += adj_mat[*i][*k];
                // if (adj_mat[*i][*k] != 0) {
                //     cout << *i << " " << *k << " " << adj_mat[*i][*k] << " " << sig_X[*i] << " " << sig_X_max << " " << sig_X_min << " X" << endl;
                // };
            };
            if (sig_X[*i] <= sig_X_min) sig_X_min = sig_X[*i];
            if (sig_X[*i] >= sig_X_max) sig_X_max = sig_X[*i];
            //cout << *i << " " << sig_X[*i] << " " << sig_X_max << " " << sig_X_min << " X" << endl;
            for (set<int>::iterator k = S.begin(); k!=S.end(); k++) {
                sig_Y[*i] += adj_mat[*i][*k];
                // if (adj_mat[*i][*k] != 0) {
                //     cout << *i << " " << *k << " " << adj_mat[*i][*k] << " " << sig_Y[*i] << " " << sig_Y_max << " " << sig_Y_min << " Y" << endl;
                // };
            };
            if (sig_Y[*i] <= sig_Y_min) sig_Y_min = sig_Y[*i];
            if (sig_Y[*i] >= sig_Y_max) sig_Y_max = sig_Y[*i];
            //cout << *i << " " << sig_Y[*i] << " " << sig_Y_max << " " << sig_Y_min << " Y" << endl;
        };
        w_min = min (sig_X_min, sig_Y_min);
        w_max = max (sig_Y_max, sig_Y_max);
        mu = w_min + alpha*(w_max - w_min);
        vector<int> RCL_v;
        //cout << "building an RCL_v" << endl;
        for (set<int>::iterator i = V_.begin(); i!=V_.end(); i++) {
            //cout << *i << " " << sig_X[*i] << " " << sig_Y[*i] << " " << endl;
            if (max(sig_X[*i], sig_Y[*i]) >= mu) RCL_v.push_back(*i);
        };
        //cout << "inner " << RCL_v.size() << " " << V_.size() << " " << w_max << " " << w_min << " " << mu << endl;
        int v_ = RCL_v[rand()%RCL_v.size()];
        if (sig_X[v_] >= sig_Y[v_]) S.insert(v_);
        else _S.insert(v_);
        SU_S.clear();
        set_union (S.begin(), S.end(), _S.begin(), _S.end(), inserter(SU_S, SU_S.end()));
        delete[] sig_Y, delete[] sig_X;
    };
    return w(S, _S);
};

double Graph::local_search (set<int>& S, set<int>& _S) {
    bool change = true;
    while (change) {
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
    return w(S, _S);
};

double Graph::maxcut_GRASP (int max_itr, bool print_steps=false, bool print_part=false) {
    double w_ = -INFINITY;
    set<int> S, _S, S_, _S_;
    bool cons_print = false;
    for (int i=0; i<max_itr; i++) {
        // double w = (this->*construct_maxcut)(S, _S);
        double w1 = simple_greedy_maxcut (S, _S);
        if (w1 > w_) S_ = S, _S_ = _S, w_ = w1, S.clear(), S_.clear();
        if (print_steps && i==0) cout << w1 << "\t";
        double w2 = semi_greedy_maxcut (S, _S);
        if (w2 > w_) S_ = S, _S_ = _S, w_ = w2, S.clear(), S_.clear();
        if (print_steps && i==0) cout << w2 << "\t";
        double w3 = random_maxcut (S, _S);
        if (w3 > w_) S_ = S, _S_ = _S, w_ = w3, S.clear(), S_.clear();
        if (print_steps && i==0) cout << w2 << "\t";
        S = S_, _S = _S_;
        double w = local_search (S, _S);
        if (w > w_) {
            if (print_steps && !cons_print) cout << w << "\t", cons_print = true;
            w_ = w;  
            S_ = S, _S_ = _S;
        };  
    };
    if (print_part) print_partition (S_, _S_);
    return w_;
};

void Graph::print_partition (set<int>& S, set<int>& _S) {
    cout << endl; cout << endl;  
    for (set<int>::iterator k = S.begin(); k!=S.end(); k++) cout << *k + 1<< " ";
    cout << endl;
    for (set<int>::iterator k = _S.begin(); k!=_S.end(); k++) cout << *k + 1 << " ";
    cout << endl; cout << endl;  
};

void Graph::print_maxcut_GRASP_steps (int max_itr) {
    double w =  maxcut_GRASP (max_itr, true);
    cout << max_itr << "\t" << w << endl;
};

int main (int argc, char** argv) {
    srand(0);
    int V, E, n_itr;
    n_itr = argc>1? atoi(argv[1]) : 50;
    cin >> V >> E;
    Graph* g = new Graph(V);
    for (int i=0; i<E; i++) {
        int u, v, W;
        cin >> u >> v >> W;
        g->add_edge(u-1, v-1, W);
    };
    cout << argv[1] << "\t" << V << "\t" << E << "\t";
    g->print_maxcut_GRASP_steps (n_itr);
    return 0;
};