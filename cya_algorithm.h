#pragma once

#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>

namespace grammar {
    class Rule {
    public:
        Rule() = default;
        Rule(const std::string& non_terminal, const std::vector<std::string>& product);
        Rule(char non_terminal, const std::string& product);
        size_t GetProductSize() const;
        const std::string& GetNonTerminal() const;
        std::string& GetNonTerminal();
        const std::vector<std::string>& GetProduct() const;
        std::string GetStrProduct() const;
        std::vector<std::string>& GetProduct();
        bool operator==(const Rule& other) const;

    private:
        std::string non_terminal;
        std::vector<std::string> product;
    };

}  // namespace grammar

namespace std {
    template<>
    struct std::hash<grammar::Rule> {
        size_t operator()(const grammar::Rule& rule) const;
    };
}

namespace grammar {
    class Grammar {
    public:
        const std::string& GetFAKESTART() const;

        const std::string& GetStartingNonTerminal() const;
        std::string& GetStartingNonTerminal();
        std::unordered_set<char>& GetNonTerminals();

        const std::unordered_set<char>& GetNonTerminals() const;

        std::unordered_set<char>& GetTerminals();

        const std::unordered_set<char>& GetTerminals() const;

        std::unordered_set<Rule>& GetRules();

        const std::unordered_set<Rule>& GetRules() const;

        Grammar(char starting_non_terminal, const std::unordered_set<char>& non_terminals,
                const std::unordered_set<char>& terminals, const std::unordered_set<Rule> rules);
        Grammar();


    private:
        const std::string FAKESTART = "S_FAKE";
        std::string starting_non_terminal;
        std::unordered_set<char> non_terminals;
        std::unordered_set<char> terminals;
        std::unordered_set<Rule> rules;
    };

    class Reader {
    public:
        virtual Grammar GetGrammar() const = 0;
        virtual std::vector<std::string> GetWords() const = 0;
    protected:
        static bool IsNonTerminal(char to_check) {
            return to_check >= 'A' && to_check <= 'Z';
        }
        static bool IsTerminal(char to_check) {
            return (to_check >= 'a' && to_check <= 'z') ||
                   (to_check >= '0' && to_check <= '9') ||
                   (to_check == '+' || to_check == '-' || to_check == '*' || to_check == '/') ||
                   (to_check == '(' || to_check == ')' || to_check == '[' || to_check == ']');
        }
        friend class CYASolver;
    };

    class StdReader : public Reader {
    public:
        virtual Grammar GetGrammar() const override final;
        virtual std::vector<std::string> GetWords() const override final;
    };

    class CYASolver {
    public:
        CYASolver(const Grammar& grammar);
        bool Predict(const std::string& to_predict);
        void ToCNF();
        const Grammar& GetGrammar() const {
            return grammar;
        }
    private:
        void DeleteEps();
        void DeleteLong();
        void DeleteChainy();
        void DeleteChainy2();

        Grammar grammar;
        size_t additional_non_terminals_num = 0;
        const std::string fake_non_terminal = "{}";
        std::string NextAdditionalNT();
        std::string CurAdditionalNT();
    };
} // namespace grammar



