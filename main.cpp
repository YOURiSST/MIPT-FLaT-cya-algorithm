#include <iostream>
#include "cya_algorithm.h"

using namespace grammar;

signed main() {


    StdReader stdReader;
    auto grammar = stdReader.GetGrammar();
    auto words = stdReader.GetWords();
    CYASolver cyaSolver(grammar);
    cyaSolver.ToCNF();
    for (const auto& word : words) {
        std::cout << (cyaSolver.Predict(word) ? "Yes" : "No") << "\n";
    }
    return 0;
}