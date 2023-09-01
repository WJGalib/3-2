#include "DecisionTree.h"

int main() {
    int n_attr, target_attr_id, *n_attr_val, n_examples;
    cin >> n_attr >> target_attr_id;
    n_attr_val = new int [n_attr];
    for (int i=0; i<n_attr; i++) cin >> n_attr_val[i];

    // cout << "Enter data now: " << endl;
    DatasetPartition* dataset = new DatasetPartition (n_attr, n_attr_val, target_attr_id);
    cin >> n_examples;
    vector<int*>* exmp_data = new vector<int*>();
    for (int i=0; i<n_examples; i++) {
        int* example = new int [n_attr];
        for (int j=0; j<n_attr; j++) {
            cin >> example[j];
        };
        exmp_data->push_back(example);
        //cout << exmp_data->size() << endl;
    };
    dataset->load_examples (exmp_data);
    DecisionTree* tree = new DecisionTree(dataset);
    tree->learn();
    tree->print();

    return 0;
}