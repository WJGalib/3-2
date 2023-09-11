#include<iostream>
#include<vector>
#include<cmath>
#include<fstream>
#include<sstream>
#include<bits/stdc++.h>


using namespace std;

class Example {
    int n_attr;
    vector<int>* attr_values;

public: 
    Example (int n_attr) {
        this->n_attr = n_attr;
        attr_values = new vector<int> (n_attr);
    };

    void set_attr_val (int attr_id, int attr_val) {
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
    vector<vector<DatasetPartition*>*>* attr_partitions;

public:

    DatasetPartition (int n_attr, int* n_attr_val, int target_attr_id, DatasetPartition* parent = nullptr) {
        this->n_attr = n_attr, this->target_attr_id = target_attr_id;
        this->n_attr_val = n_attr_val, this->parent = parent, this->entropy = nullptr;
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

    int get_example_decision (int i) {
        return examples[i]->get_attr_val(target_attr_id);
    };

    void add_example (Example* example) {
        this->examples.push_back (example);
    };

    void load_examples (vector<int*>* data) {
        for (int i=0; i<data->size(); i++) {
            Example* e = new Example(n_attr);
            for (int j=0; j<n_attr; j++) 
                e->set_attr_val (j, (data->at(i))[j]);
            this->add_example(e);
        };
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
        delete[] n_example_with_decision;
        return *entropy;
    };

    vector<DatasetPartition*>* get_partition_by_attr (int attr_id) {
        if (this->attr_partitions->at(attr_id)) return this->attr_partitions->at(attr_id);       
        vector<DatasetPartition*>* partitions = new vector<DatasetPartition*>();
        if (attr_id == target_attr_id) return nullptr;
        for (int j=0; j<n_attr_val[attr_id]; j++) {
            DatasetPartition* attr_partition = new DatasetPartition(n_attr, n_attr_val, target_attr_id, this); 
            partitions->push_back(attr_partition);
        };
        for (int j=0; j<examples.size(); j++) 
            partitions->at(examples[j]->get_attr_val(attr_id))->add_example(examples[j]);
        this->attr_partitions->at(attr_id) = partitions;
        return this->attr_partitions->at(attr_id);
    };

    double get_attr_gain (int attr_id) {
        vector<DatasetPartition*>* partitions = get_partition_by_attr(attr_id);
        double sum_part_entropy = 0;
        int n_example = examples.size();
        for (int i=0; i<n_attr_val[attr_id]; i++) {
            int n_part_example = partitions->at(i)->get_examples().size(); 
            sum_part_entropy += (1.0*n_part_example/n_example * partitions->at(i)->get_entropy());
        };
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
            if (!attr_used.at(i)) {
                all_used = false;
                break;
            };
        };
        return all_used;
    };

public:

    DecisionTree (DatasetPartition* dataset) {
        this->dataset = dataset;
        this->decision = -1;
        for (int i=0; i<this->dataset->get_n_attr(); i++)
            this->attr_used.push_back(false);
    };

    DecisionTree (DatasetPartition* dataset, vector<bool> attr_used) {
        this->dataset = dataset;
        this->decision = -1;
        this->attr_used = attr_used;
    };

    void learn (int depth = 0) {
        if (dataset->get_examples().empty()){
            if (dataset->get_parent()) 
                decision = dataset->get_parent()->plurality_value();
        } else if (dataset->all_same_decision()) {
            decision = dataset->get_example_decision(0);
        } else if (all_atr_used()) {
            decision = dataset->plurality_value();
        } else {
            root_attr = dataset->argmax_gain(this->attr_used);
            if (root_attr == -1) {
                decision = dataset->get_example_decision(0);
                return;
            };
            vector<DatasetPartition*>* partitions = dataset->get_partition_by_attr(root_attr);
            attr_used.at(root_attr) = true;
            for (int i=0; i<dataset->get_n_attr_val()[root_attr]; i++) {
                DecisionTree* child = new DecisionTree(partitions->at(i), this->attr_used);
                child->learn(depth+1);
                this->subtree.push_back(child);
            };
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

const int rand_seed = 73;
int n_car_attr = 7, car_target_attr_id = 6;
int n_car_attr_val[7] = {4, 4, 4, 3, 3, 3, 4};

int main (int argc, char** argv) {
    vector<int*>* exmp_data = new vector<int*>();
    string example, attr, input_file;
    if (argc > 1) input_file = argv[1];
    else input_file = "car.data";
    ifstream fin (input_file, ios::in);
    while(getline(fin, example)) {
        int* example_values = new int [n_car_attr];
        stringstream str(example);
        for (int i=0; getline(str, attr, ','); i++) {
            if (i==0 || i==1) {
                if (attr == "vhigh") example_values[i] = 3;
                else if (attr == "high") example_values[i] = 2;
                else if (attr == "med") example_values[i] = 1;
                else if (attr == "low") example_values[i] = 0;
            } else if (i == 2) {
                if (attr == "2") example_values[i] = 0;
                else if (attr == "3") example_values[i] = 1;
                else if (attr == "4") example_values[i] = 2;
                else if (attr == "5more") example_values[i] = 3;
            } else if (i == 3) {
                if (attr == "2") example_values[i] = 0;
                else if (attr == "4") example_values[i] = 1;
                else if (attr == "more") example_values[i] = 2;
            } else if (i == 4) {
                if (attr == "small") example_values[i] = 0;
                else if (attr == "med") example_values[i] = 1;
                else if (attr == "big") example_values[i] = 2;
            } else if (i == 5) {
                if (attr == "low") example_values[i] = 0;
                else if (attr == "med") example_values[i] = 1;
                else if (attr == "high") example_values[i] = 2;
            } else if (i == 6) {
                if (attr == "unacc") example_values[i] = 0;
                else if (attr == "acc") example_values[i] = 1;
                else if (attr == "good") example_values[i] = 2;
                else if (attr == "vgood") example_values[i] = 3;
            };
        };
        exmp_data->push_back(example_values);
    };

    double accuracy_sum = 0;
    double* accuracy_list = new double[20];
    cout << "Accuracy of 20 trials: " << endl << endl;
    for (int k=0; k<20; k++) {
        shuffle (exmp_data->begin(), exmp_data->end(), default_random_engine(rand_seed));
        vector<int*>* learning_data = new vector<int*> (exmp_data->begin(), exmp_data->begin()+exmp_data->size()*0.8);
        vector<int*>* test_data = new vector<int*> (exmp_data->begin()+exmp_data->size()*0.8, exmp_data->end());
        DatasetPartition* learning_dataset = new DatasetPartition (n_car_attr, n_car_attr_val, car_target_attr_id);
        learning_dataset->load_examples(learning_data);
        DecisionTree* tree = new DecisionTree(learning_dataset);
        tree->learn();
        // tree->print();
        DatasetPartition* test_dataset = new DatasetPartition (n_car_attr, n_car_attr_val, car_target_attr_id);
        test_dataset->load_examples(test_data);
        int test_sample_size = test_data->size(), learning_sample_size = learning_data->size(), match_count = 0;
        for (int i=0; i<test_sample_size; i++) 
            match_count += (int)(tree->test_decision(test_dataset->get_examples()[i]));
        double accuracy = 100.0 * match_count / test_sample_size;
        accuracy_list[k] = accuracy, accuracy_sum += accuracy;
        cout << "Trial " << k+1 << ": \t" << accuracy << "%" << endl;
    };

    double mean_accuracy = accuracy_sum/20, variance = 0; 
    cout << endl << "Mean accuracy: " << mean_accuracy << "%" << endl;
    for (int i=0; i<20; i++) 
        variance += (pow(accuracy_list[i] - mean_accuracy, 2.0)/20);
    cout << "Standard deviation of accuracy: " << sqrt(variance) << "%" << endl;
    
    return 0;
};