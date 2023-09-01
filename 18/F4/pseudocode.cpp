// algo exactly similar to aiama

// do these yourself:
// data parsing
// splitting data into example and test cases 
// car class
// attribute definitions
// main func

// pardon my fear of copy checker :D
// pls change code to your own style :')


vector< vector<car> > split_by_attr(vector<car> carlist, ll attr){
    vector< vector<car> > splitted;
    for(ll i=0; i<typecnt[attr]; i++){
        vector<car> v;
        splitted.push_back(v);
    }
    ll n = carlist.size();
    for(ll i=0; i<n; i++){
        splitted[carlist[i].attrval[attr]].push_back(carlist[i]);
    }
    return splitted;
}

double entropy(vector<car> carlist){
    ll n = carlist.size();
    ll a[4]={0};
    for(ll i=0; i<n; i++){
        ll j = carlist[i].verdict;
        a[j]++;
    }
    double ans = 0;
    for(ll i=0; i<4; i++){
        if(a[i]>0){
            double f = (double)a[i]/(double)n;
            f*=log(1.0/(double)f)/log(2.0);
            ans+=f;
        }
    }
    return ans;
}

double gain(vector<car> carlist, ll attr){
    vector< vector<car> > splitted = split_by_attr(carlist,attr);
    double curr_entropy = entropy(carlist);
    for(ll i=0; i<typecnt[attr]; i++){
        curr_entropy-=entropy(splitted[i]);
    }
    return curr_entropy;
}

double best_attr(vector<bool> attribute,vector<car> carlist){
    // loop over existing attributes and call  gain(carlist,i)
    // return the attr that has the highest gain, 
}


ll plurality(vector<car> carlist){
    // most common verdict among all cars in carlist
}

bool attribute_empty(vector<bool> attribute){
    // return true if all attributes are used up i.e. false
}

bool all_same(vector<car> carlist){
    // return true if all cars in carlist has the same verdict
}

class tree{
    public:
    ll currattr;
    bool hasverdict;
    int verdict;
    vector<car> cars;
    vector<tree*> subtree;
    tree(){
        hasverdict = 0;
        currattr = 10000;
        verdict = 10000;
    }
    
    void decision(vector<car> curr_examples, vector<bool> attribute, vector<car> parent_examples){
        cars = curr_examples;
        if(curr_examples.size()==0){
            hasverdict = true;
            verdict = plurality(parent_examples);
        }
        else if(all_same(curr_examples) || attribute_empty(attribute)){
            hasverdict = true;
            verdict = plurality(curr_examples);
        }
        else{
            currattr = best_attr(attribute,curr_examples);
            vector< vector<car> > splitted = split_by_attr(curr_examples,currattr);
            attribute[currattr] = false;
            for(ll i=0; i<typecnt[currattr]; i++){
                tree* branch = new tree();
                branch->decision(splitted[i],attribute,curr_examples);
                subtree.push_back(branch);
            }
            attribute[currattr] = true;
        }
    }
    bool verdict_match(car newcar){
        if(hasverdict){
            return newcar.verdict == this->verdict;
        }
        return subtree[newcar.attrval[currattr]]->verdict_match(newcar);
    }
};