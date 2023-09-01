#include "DecisionTree.h"
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

const int rand_seed = 73;
int n_car_attr = 7, car_target_attr_id = 6;
int n_car_attr_val[7] = {4, 4, 4, 3, 3, 3, 4};

int main() {
    vector<int*>* exmp_data = new vector<int*>();
    string example, attr;
    ifstream fin ("car.data", ios::in);
    while(getline(fin, example)) {
        // cout << example << endl;
        int* example_values = new int [n_car_attr];
        stringstream str(example);
        for (int i=0; getline(str, attr, ','); i++) {
            // cout << attr << endl;
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
        // match_count = 0;
        // for (int i=0; i<learning_sample_size; i++) 
        //     match_count += (int)(tree->test_decision(learning_dataset->get_examples()[i]));
        // cout << 100.0*match_count/learning_sample_size << endl;
    };

    double mean_accuracy = accuracy_sum/20, variance = 0; 
    cout << endl << "Mean accuracy: " << mean_accuracy << "%" << endl;
    for (int i=0; i<20; i++) 
        variance += (pow(accuracy_list[i] - mean_accuracy, 2.0)/20);
    cout << "Standard deviation of accuracy: " << sqrt(variance) << "%" << endl;
    
    return 0;
};