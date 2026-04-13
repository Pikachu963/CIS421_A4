#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem> 

using namespace std;

// Structure to hold a tuple for R1 (3 attributes) or R2 (4 attributes)
struct Tuple {
    vector<int> values;
};

// Function to read tuples from a file
vector<Tuple> readRelation(string fileName, int numAttributes) {
    vector<Tuple> relation;
    ifstream file(fileName);
    
    if (!file) {
        // Shows which file name input is failing (if so)
        cout << "!! FAILED TO OPEN: " << fileName << " !!" << endl;
        return relation; 
    }

    int val;
    while (file >> val) {
        Tuple t;
        t.values.push_back(val);
        for (int i = 1; i < numAttributes; ++i) {
            if (file >> val) {
                t.values.push_back(val);
            }
        }
        if (t.values.size() == numAttributes) {
            relation.push_back(t);
        }
    }
    
    cout << "Successfully loaded " << relation.size() << " rows from " << fileName << endl;
    return relation;
}

int main() {
    string file1, file2;
    int nBuckets, attrR1, attrR2;

    // 1. User Input
    cout << "--- Hash-Join DBMS Simulator ---" << endl;
    cout << "Enter filename for R1: "; cin >> file1;
    cout << "Enter filename for R2: "; cin >> file2;
    cout << "Enter number of hash buckets (N): "; cin >> nBuckets;
    cout << "Join attribute for R1 (1-3): "; cin >> attrR1;
    cout << "Join attribute for R2 (1-4): "; cin >> attrR2;

    // Adjust for 0-based indexing
    attrR1--; attrR2--;

    // 2. Load Relations
    vector<Tuple> R1 = readRelation(file1, 3);
    vector<Tuple> R2 = readRelation(file2, 4);

    if (R1.empty() || R2.empty()) return 1;

    // 3. Build Phase (Hash R1)
    vector<vector<Tuple>> hashTable(nBuckets);
    for (const auto& t : R1) {
        int key = t.values[attrR1];
        int bucketIdx = key % nBuckets;
        hashTable[bucketIdx].push_back(t);
    }

    // 4. Probe Phase (Iterate R2 and Match)
    cout << "\n--- Join Results ---\n";
    int joinCount = 0;
    for (const auto& t2 : R2) {
        int key = t2.values[attrR2];
        int bucketIdx = key % nBuckets;

        // Only check the specific bucket
        for (const auto& t1 : hashTable[bucketIdx]) {
            if (t1.values[attrR1] == t2.values[attrR2]) {
                joinCount++;
                // Output joined tuple: (a1, a2, a3, b1, b2, b3, b4)
                for (int v : t1.values) cout << v << " ";
                for (int v : t2.values) cout << v << " ";
                cout << endl;
            }
        }
    }

    // 5. Calculate Selectivity
    double cartesianProduct = (double)R1.size() * R2.size();
    double selectivity = (cartesianProduct > 0) ? (joinCount / cartesianProduct) : 0;

    cout << "\n--- Statistics ---" << endl;
    cout << "Total Join Tuples: " << joinCount << endl;
    cout << "Selectivity: " << fixed << setprecision(6) << selectivity << endl;

    return 0;
}