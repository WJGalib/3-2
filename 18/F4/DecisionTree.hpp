#include<iostream>
#include<vector>
#include<cmath>

using namespace std;

class DatasetPartition;

class Example {
    int n_attr;
    vector<int>* attr_values;

public: 
    Example (int n_attr) {
        this->n_attr = n_attr;
        attr_values = new vector<int> (n_attr);
    };

    void set_attr_val (int attr_id, int attr_val) {
        // cout << "hi from set attr " << endl;
        attr_values->at(attr_id) = attr_val;
    };

    int get_attr_val (int attr_id) {
        return attr_values->at(attr_id);
    };
};


class DatasetPartition {
    int n_attr, *n_attr_val, target_attr_id;
    vector<Example*> examples;
    double* entropy;
    DatasetPartition* parent;

public:

    vector<vector<DatasetPartition*>*>* attr_partitions;

    DatasetPartition (int n_attr, int* n_attr_val, int target_attr_id, DatasetPartition* parent = nullptr) {
        this->n_attr = n_attr, this->target_attr_id = target_attr_id;
        this->n_attr_val = n_attr_val;
        this->parent = parent;
        this->entropy = nullptr;
        this->attr_partitions = new vector<vector<DatasetPartition*>*> (this->n_attr, nullptr);
    };

    int get_n_attr() {
        return n_attr;
    };

    int* get_n_attr_val() {
        return n_attr_val;
    };

    int get_target_attr_id() {
        return target_attr_id;
    };

    DatasetPartition* get_parent() {
        return this->parent;
    };

    const vector<Example*>& get_examples() {
        return examples;
    };

    void add_example (Example* example) {
        this->examples.push_back(example);
    };

    void load_examples (vector<int*>* data) {
        for (int i=0; i<data->size(); i++) {
            Example* e = new Example(n_attr);
            // cout << data->size() << endl;
            for (int j=0; j<n_attr; j++) 
                e->set_attr_val (j, (data->at(i))[j]);
            this->add_example(e);
        };
        // for (int i=0; i<examples.size(); i++) {
        //     for (int j=0; j<n_attr; j++) {
        //         cout << examples[i]->get_attr_val(j) << " ";
        //     };
        //     cout << endl;
        // };
    };

    int get_example_decision (int i) {
        return examples[i]->get_attr_val(target_attr_id);
    };

    double get_entropy() {
        if (this->entropy) return *entropy;
        this->entropy = new double;
        int n_example = examples.size(), decision_types = n_attr_val[target_attr_id];   
        double S = 0;
        int* n_example_with_decision = new int[decision_types];
        for (int i=0; i<decision_types; i++) n_example_with_decision[i] = 0;
        for (int i=0; i<n_example; i++) {
            int dec_i = examples[i]->get_attr_val(target_attr_id);
            n_example_with_decision[dec_i]++; 
        };
        for (int i=0; i<decision_types; i++) {
            if (n_example_with_decision[i] > 0) {
                double p_dec_i = 1.0*n_example_with_decision[i]/n_example;
                S -= (p_dec_i * log2(p_dec_i));
            };
        };
        *this->entropy = S;
        //cout << "entropy=" << S << endl;
        delete[] n_example_with_decision;
        return *entropy;
    };

    vector<DatasetPartition*>* get_partition_by_attr (int attr_id) {
        if (this->attr_partitions->at(attr_id)) return this->attr_partitions->at(attr_id);       
        vector<DatasetPartition*>* partitions = new vector<DatasetPartition*>();
        if (attr_id == target_attr_id) return nullptr;
        for (int j=0; j<n_attr_val[attr_id]; j++) {
            DatasetPartition* attr_partition = new DatasetPartition(n_attr, n_attr_val, target_attr_id, this); 
            // cout << "hm" << endl;
            partitions->push_back(attr_partition);
            // cout << "hmm " << partitions->size() << endl;
        };
        for (int j=0; j<examples.size(); j++) 
            partitions->at(examples[j]->get_attr_val(attr_id))->add_example(examples[j]);
        //cout << "partitions by attr" << attr_id << ": " << endl;
        // for (int j=0; j<n_attr_val[attr_id]; j++) {
        //     cout << "partition " << j << ": " << endl;
        //     for (int i=0; i<partitions->at(j)->get_examples().size(); i++) {
        //         for (int k=0; k<n_attr; k++) {
        //             cout << partitions->at(j)->get_examples()[i]->get_attr_val(k) << " ";
        //         };
        //         cout << endl;
        //     };
        // };
        this->attr_partitions->at(attr_id) = partitions;
        return this->attr_partitions->at(attr_id);
    };

    double get_attr_gain (int attr_id) {
        vector<DatasetPartition*>* partitions = get_partition_by_attr(attr_id);
        double sum_part_entropy = 0;
        int n_example = examples.size();
        //cout << partitions->size() << endl;
        for (int i=0; i<n_attr_val[attr_id]; i++) {
            int n_part_example = partitions->at(i)->get_examples().size(); 
            sum_part_entropy += (1.0*n_part_example/n_example * partitions->at(i)->get_entropy());
        };
        //cout << "attr " << attr_id << " gain: " << get_entropy() - sum_part_entropy << endl;
        return ( get_entropy() - sum_part_entropy );
    };

    int plurality_value() {
        vector<int> count (n_attr_val[target_attr_id]);
        for (int i=0; i<count.size(); i++) count[i] = 0;
        for (int i=0; i<examples.size(); i++) {
            count[examples[i]->get_attr_val(target_attr_id)]++;
        };
        int plurality = 0, max_count = 0;
        for (int i=0; i<count.size(); i++) {
            if (count[i] >= max_count) 
                max_count = count[i], plurality = i;
        };
        return plurality;
    };

    int argmax_gain (vector<bool> attr_used) {
        double best_gain = -INFINITY;
        int arg_best_gain = -1;
        for (int i=0; i<n_attr; i++) {
            if (i==target_attr_id || attr_used[i]) continue;
            double attr_gain = get_attr_gain(i);
            if (attr_gain > best_gain) 
                best_gain = attr_gain, arg_best_gain = i;
        };
        // cout << arg_best_gain << endl;
        return arg_best_gain;
    };

    bool all_same_decision() {
        bool all_same = true;
        int decision = get_example_decision(0);

        for (int i=0; i<examples.size(); i++) {
            if (examples[i]->get_attr_val(target_attr_id) != decision) {
                all_same = false;
                break;
            };
        };
        return all_same;
    };
};

class DecisionTree {
    DatasetPartition* dataset;
    vector<DecisionTree*> subtree;
    vector<bool> attr_used;
    int decision, root_attr;

    bool all_atr_used() {
        bool all_used = true;
        for (int i=0; i<attr_used.size(); i++) {
            if (i == dataset->get_target_attr_id()) continue;
            // cout << attr_used[i];
            if (!attr_used.at(i)) {
                all_used = false;
                break;
            };
        };
        // cout << endl;
        return all_used;
    };

public:

    DecisionTree (DatasetPartition* dataset) {
        this->dataset = dataset;
        this->decision = -1;
        for (int i=0; i<this->dataset->get_n_attr(); i++) {
            // cout << this->attr_used.size() << endl;
            this->attr_used.push_back(false);
        };
    };

    DecisionTree (DatasetPartition* dataset, vector<bool> attr_used) {
        this->dataset = dataset;
        this->decision = -1;
        this->attr_used = attr_used;
        // cout << this->attr_used.size() << endl;
    };

    void learn (int depth = 0) {
        if (dataset->get_examples().empty()){
            if (dataset->get_parent()) 
                decision = dataset->get_parent()->plurality_value();
        } else if (dataset->all_same_decision()) {
            decision = dataset->get_example_decision(0);
        } else if (all_atr_used()) {
            // cout << "hii" << endl;
            decision = dataset->plurality_value();
        } else {
            root_attr = dataset->argmax_gain(this->attr_used);
            if (root_attr == -1) {
                decision = dataset->get_example_decision(0);
                return;
            };
            // cout << root_attr << endl;
            vector<DatasetPartition*>* partitions = dataset->get_partition_by_attr(root_attr);
            attr_used.at(root_attr) = true;
            for (int i=0; i<dataset->get_n_attr_val()[root_attr]; i++) {
                // cout << "hi depth " << depth << endl;
                DecisionTree* child = new DecisionTree(partitions->at(i), this->attr_used);
                // cout << "hii" << endl;
                child->learn(depth+1);
                // cout << "hii" << endl;

                this->subtree.push_back(child);
            };
            // attr_used.at(root_attr) = false;
            // cout << "hiii" << endl;
        };
    };

    int get_decision (Example* example) {
        if (this->decision != -1) return this->decision;
        int example_root_val = example->get_attr_val(this->root_attr);
        return this->subtree[example_root_val]->get_decision (example);
    };

    bool test_decision (Example* example) {
        return (get_decision(example) == example->get_attr_val(dataset->get_target_attr_id()));
    };

    void print (int depth = 0) {
        for (int i=0; i<depth; i++) cout << "-";
        if (decision == -1) {
            cout << "attr" << this->root_attr << "?" << endl;
            for (int i=0; i<this->subtree.size(); i++) {
                for (int i=0; i<depth+1; i++) cout << " ";
                cout << "attr" << this->root_attr << "==" << i << endl;
                subtree[i]->print (depth+1);
            };
        } else {
            cout << this->decision << endl;
        };
    };
}; 