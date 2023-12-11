#include "cya_algorithm.h"

grammar::Rule::Rule(const std::string& non_terminal, const std::vector<std::string>& product) :
    non_terminal(non_terminal), product(product) {}

grammar::Rule::Rule(char non_terminal_ch, const std::string& product_str) :
    non_terminal(std::string{non_terminal_ch}), product(product_str.size()) {
    for (size_t ind = 0; ind < product_str.size(); ++ind) {
        product[ind] = std::string{product_str[ind]};
    }
}

size_t grammar::Rule::GetProductSize() const {
    return product.size();
}

const std::string& grammar::Rule::GetNonTerminal() const {
    return non_terminal;
}

std::string& grammar::Rule::GetNonTerminal() {
    return non_terminal;
}

std::string grammar::Rule::GetStrProduct() const {
    std::string res = "";
    for (const auto& str: GetProduct()) {
        res += str;
    }
    return res;
}

const std::vector<std::string>& grammar::Rule::GetProduct() const {
    return product;
}
std::vector<std::string>& grammar::Rule::GetProduct() {
    return product;
}
bool grammar::Rule::operator==(const Rule& other) const {
    return non_terminal == other.GetNonTerminal() && product == other.GetProduct();
}


size_t std::hash<grammar::Rule>::operator()(const grammar::Rule& rule) const {
    std::string res = "";
    for (const auto& str: rule.GetProduct()) {
        res += str;
    }
    return std::hash<std::string>()(
        rule.GetNonTerminal() + "->" + res
    );
}


const std::string& grammar::Grammar::GetFAKESTART() const {
    return FAKESTART;
}

const std::string& grammar::Grammar::GetStartingNonTerminal() const {
    return starting_non_terminal;
}

std::string& grammar::Grammar::GetStartingNonTerminal() {
    return starting_non_terminal;
}

std::unordered_set<char>& grammar::Grammar::GetNonTerminals() {
    return non_terminals;
}

const std::unordered_set<char>& grammar::Grammar::GetNonTerminals() const {
    return non_terminals;
}

std::unordered_set<char>& grammar::Grammar::GetTerminals() {
    return terminals;
}

const std::unordered_set<char>& grammar::Grammar::GetTerminals() const {
    return terminals;
}

std::unordered_set<grammar::Rule>& grammar::Grammar::GetRules() {
    return rules;
}

const std::unordered_set<grammar::Rule>& grammar::Grammar::GetRules() const {
    return rules;
}

grammar::Grammar::Grammar(char starting_non_terminal_ch, const std::unordered_set<char>& non_terminals,
                          const std::unordered_set<char>& terminals, const std::unordered_set<Rule> rules) :
    starting_non_terminal(std::string{starting_non_terminal_ch}),
    non_terminals(non_terminals), terminals(terminals), rules(rules) {}
grammar::Grammar::Grammar() = default;

grammar::Grammar grammar::StdReader::GetGrammar() const {
    std::unordered_set<char> non_terminals;
    std::unordered_set<char> terminals;
    std::unordered_set<Rule> rules;
    char starting_non_terminal;

    size_t non_terminals_num, terminals_num, rules_num;

    std::cin >> non_terminals_num >> terminals_num >> rules_num;
    for (size_t i = 0; i < non_terminals_num; ++i) {
        char cur_non_terminal;
        std::cin >> cur_non_terminal;
        if (!IsNonTerminal(cur_non_terminal)) {
            throw std::invalid_argument("invalid non terminal, found " + std::string{cur_non_terminal});
        }
        non_terminals.emplace(cur_non_terminal);
    }
    for (size_t i = 0; i < terminals_num; ++i) {
        char cur_terminal;
        std::cin >> cur_terminal;
        if (!IsTerminal(cur_terminal)) {
            throw std::invalid_argument("invalid terminal, found " + std::string{cur_terminal});
        }
        terminals.emplace(cur_terminal);
    }
    for (size_t i = 0; i < rules_num; ++i) {
        std::string cur_rule;
        if (i == 0) {
            getline(std::cin, cur_rule);
        }
        getline(std::cin, cur_rule);

        if (cur_rule.size() < 5) {
            throw std::invalid_argument("rule size less than 5, it's impossible, found " + cur_rule);
        }
        if (cur_rule[1] != ' ' || cur_rule[2] != '-' || cur_rule[3] != '>' || cur_rule[4] != ' ') {
            throw std::invalid_argument(
                "rule format is strange, did not found *space*->*space* construction, found " + cur_rule);
        }
        if (!IsNonTerminal(cur_rule[0])) {
            throw std::invalid_argument("rule first element isn't a non-terminal, found " + cur_rule);
        }
        char non_terminal = cur_rule[0];
        if (!non_terminals.contains(non_terminal)) {
            throw std::invalid_argument(
                "we don't have non_terminal like this! found " + std::string{cur_rule[0]});
        }
        for (size_t rule_iter = 5; rule_iter < cur_rule.size(); ++rule_iter) {
            if (!non_terminals.contains(cur_rule[rule_iter]) && !terminals.contains(cur_rule[rule_iter])) {
            }

            if (!non_terminals.contains(cur_rule[rule_iter]) && !terminals.contains(cur_rule[rule_iter])) {
                throw std::invalid_argument("we don't have this symbol from target! found (rule + symbol)" +
                                            cur_rule + " + " + std::string{cur_rule[rule_iter]});
            }
        }
        rules.emplace(non_terminal, cur_rule.substr(5, cur_rule.size() - 5));

    }
    std::cin >> starting_non_terminal;
    if (!non_terminals.contains(starting_non_terminal)) {
        throw std::invalid_argument(
            "starting non terminal is invalid, there is no non terminal like this, found: " +
            std::string{starting_non_terminal});
    }
    grammar::Grammar result{starting_non_terminal, non_terminals, terminals, rules};
    return result;
}
std::vector<std::string> grammar::StdReader::GetWords() const {
    size_t num_words;
    std::cin >> num_words;
    std::vector<std::string> to_ret(num_words);
    for (auto& word: to_ret) {
        std::cin >> word;
    }
    return to_ret;
}

grammar::CYASolver::CYASolver(const grammar::Grammar& grammar) : grammar(grammar) {}

std::string grammar::CYASolver::NextAdditionalNT() {
    return "{" + std::to_string(++additional_non_terminals_num) + "}";
}

std::string grammar::CYASolver::CurAdditionalNT() {
    return "{" + std::to_string(additional_non_terminals_num) + "}";
}

void grammar::CYASolver::ToCNF() {
    std::unordered_set<Rule> to_add;

    for (auto& rule: grammar.GetRules()) {
        std::vector<std::string> instead_of;
        for (auto& right_elem: rule.GetProduct()) {
            if (right_elem.size() == 1 && Reader::IsTerminal(right_elem[0])) {
                to_add.emplace(NextAdditionalNT(), std::vector{std::string{right_elem[0]}});
                instead_of.emplace_back(CurAdditionalNT());
            } else {
                instead_of.emplace_back(right_elem);
            }
        }
        to_add.emplace(rule.GetNonTerminal(), instead_of);
    }
    std::swap(grammar.GetRules(), to_add);
    DeleteLong();
    DeleteEps();
    DeleteChainy();
}

void grammar::CYASolver::DeleteLong() {
    std::unordered_set<Rule> new_rules;
    for (const auto& rule: grammar.GetRules()) {
        if (rule.GetProductSize() <= 2) {
            new_rules.emplace(rule);
            continue;
        }
        // A -> N0N1N2
        // A -> N0Y1
        // Y1 -> N1N2

        // A -> N0N1N2N3
        // A -> N0Y1
        // Y1 -> N1Y2
        // Y2 -> N2N3
        new_rules.emplace(rule.GetNonTerminal(), std::vector{rule.GetProduct()[0], NextAdditionalNT()});
        for (size_t cur_nt_ind = 1; cur_nt_ind + 2 < rule.GetProductSize(); ++cur_nt_ind) {
            new_rules.emplace(CurAdditionalNT(), std::vector{
                rule.GetProduct()[cur_nt_ind],
                NextAdditionalNT()
            });
        }
        new_rules.emplace(CurAdditionalNT(), std::vector{
            rule.GetProduct()[rule.GetProductSize() - 2],
            rule.GetProduct()[rule.GetProductSize() - 1]
        });
    }
    std::swap(new_rules, grammar.GetRules());
}

void grammar::CYASolver::DeleteEps() {
    std::unordered_set<std::string> is_nt_eps_generative;
    std::unordered_set<Rule> eps_rules;
    size_t changes = 1; // should not be equal to zero in the beginning
    while (changes != 0) {
        changes = 0;
        for (const auto& rule: grammar.GetRules()) {
            if (rule.GetProductSize() == 0) {
                if (!is_nt_eps_generative.contains(rule.GetNonTerminal())) {
                    ++changes;
                    is_nt_eps_generative.emplace(rule.GetNonTerminal());
                    eps_rules.emplace(rule);
                }
                continue;
            }
            if (rule.GetProductSize() == 1) {
                if (!is_nt_eps_generative.contains(rule.GetNonTerminal()) &&
                    is_nt_eps_generative.contains(rule.GetProduct()[0])) {
                    eps_rules.emplace(rule);
                    is_nt_eps_generative.emplace(rule.GetNonTerminal());
                    ++changes;
                    continue;
                }
            }
            bool is_eps_gen = true;
            for (const auto& str: rule.GetProduct()) {
                if (!is_nt_eps_generative.contains(str)) {
                    is_eps_gen = false;
                    break;
                }
            }
            if (!is_eps_gen) {
                continue;
            }

            if (!is_nt_eps_generative.contains(rule.GetNonTerminal())) {
                ++changes;
                is_nt_eps_generative.emplace(rule.GetNonTerminal());
            }
        }
    }
    std::unordered_set<Rule> new_rules = grammar.GetRules();

    for (const auto& rule: grammar.GetRules()) {
        if (rule.GetProductSize() > 2) {
            throw std::logic_error("DEBUG : we have rule with > 2 product size wtf...");
        }

        if (rule.GetProductSize() == 2 && is_nt_eps_generative.contains(rule.GetProduct().front())) {
            new_rules.emplace(rule.GetNonTerminal(), std::vector{rule.GetProduct().back()});
        }

        if (rule.GetProductSize() == 2 && is_nt_eps_generative.contains(rule.GetProduct().back())) {
            new_rules.emplace(rule.GetNonTerminal(), std::vector{rule.GetProduct().front()});
        }
    }
    // if we have an S -> eps rule
    if (new_rules.contains(Rule{grammar.GetStartingNonTerminal(), std::vector<std::string>{}})) {
        new_rules.emplace(grammar.GetFAKESTART(), std::vector<std::string>{});
        new_rules.emplace(grammar.GetFAKESTART(), std::vector{grammar.GetStartingNonTerminal()});
        grammar.GetStartingNonTerminal() = grammar.GetFAKESTART();
    }
    for (const auto& rule: eps_rules) {
        if (!new_rules.contains(rule)) {
            throw std::logic_error("DEBUG : we don't have this rule, but need to delete it!!");
        }
        if (rule.GetNonTerminal() == grammar.GetFAKESTART()) {
            throw std::logic_error("DEBUG : we don't need to delete fake-start epsilon rule...!!!");
        }
        new_rules.erase(rule);
    }
    std::swap(new_rules, grammar.GetRules());
}


void grammar::CYASolver::DeleteChainy() {  // i think it's the best i can write :>
    bool is_changed = true; // should not be equal to zero in the beginning
    std::unordered_set<Rule> has_deleted;
    while (is_changed) {
        is_changed = false;
        std::unordered_set<Rule> to_add;
        for (const auto& rule: grammar.GetRules()) {
            if (rule.GetProductSize() == 1 && rule.GetProduct().front().size() == 1 &&
                Reader::IsTerminal(rule.GetProduct().front().front())) {
                continue;
            }
            if (rule.GetProductSize() != 1) {
                continue;
            }

            std::string mid_nt = rule.GetProduct().front();
            for (const auto& new_rule: grammar.GetRules()) {
                if (new_rule == rule) {
                    continue;
                }
                if (new_rule.GetNonTerminal() == mid_nt) {
                    if (new_rule.GetProductSize() == 1 && new_rule.GetStrProduct() == rule.GetNonTerminal()) {
                        continue;
                    }
                    if (!has_deleted.contains({rule.GetNonTerminal(), new_rule.GetProduct()})) {
                        to_add.emplace(rule.GetNonTerminal(), new_rule.GetProduct());
                    }
                }
            }

            if (!to_add.empty()) {
                is_changed = true;
                for (auto& add_rule: to_add) {
                    if (has_deleted.contains(add_rule)) {
                        continue;
                    }
                    grammar.GetRules().emplace(add_rule);
                }
                has_deleted.emplace(rule);
                grammar.GetRules().erase(rule);
                break;
            }
            to_add.clear();
        }
        to_add.clear();
    }
}

bool grammar::CYASolver::Predict(const std::string& to_predict) {
    size_t cnt = 0;
    std::unordered_map<std::string, size_t> nt_to_nums;
    for (auto& rule: grammar.GetRules()) {
        if (nt_to_nums.contains(rule.GetNonTerminal())) {
            continue;
        }
        nt_to_nums[rule.GetNonTerminal()] = cnt++;
    }
    std::vector<std::vector<std::vector<int>>> dp(cnt,
                                                  std::vector<std::vector<int>>(
                                                      to_predict.size(),
                                                      std::vector<int>(
                                                          to_predict.size() + 1, 0)));
    std::vector<std::vector<std::vector<std::pair<Rule, Rule>>>> pred(cnt,
                                                  std::vector<std::vector<std::pair<Rule,Rule>>>(
                                                      to_predict.size(),
                                                      std::vector<std::pair<Rule, Rule>>(
                                                          to_predict.size() + 1, {Rule{}, Rule{}})));

    for (int i = 0; i < to_predict.size(); ++i) {
        for (auto& rule : grammar.GetRules()) {
            if (rule.GetStrProduct().size() == 1 && Reader::IsTerminal(rule.GetStrProduct()[0]) &&
                rule.GetStrProduct()[0] == to_predict[i]) {
                dp[nt_to_nums[rule.GetNonTerminal()]][i][i + 1] = 1;
            }
        }
    }


    for (int len = 2; len < to_predict.size() + 1; ++len) {
        for (int left = 0; left + len < to_predict.size() + 1; ++left) {
            int right = left + len;
            for (auto& rule : grammar.GetRules()) {
                if (rule.GetProductSize() != 2) {
                    continue;
                }
                std::string ft_nt = rule.GetProduct()[0];
                std::string sc_nt = rule.GetProduct()[1];
                for (int k = left + 1; k < right; ++k) {
                    if (dp[nt_to_nums[ft_nt]][left][k] && dp[nt_to_nums[sc_nt]][k][right]) {
                        dp[nt_to_nums[rule.GetNonTerminal()]][left][right] = 1;
                        continue;
                    }
                }
            }
        }
    }

    return dp[nt_to_nums[grammar.GetStartingNonTerminal()]][0][to_predict.size()];
}

