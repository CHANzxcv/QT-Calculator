#include "parser.h"
#include <stack>
#include <cmath>
#include <sstream>
#include <cctype>
#include <stdexcept>

Parser::Parser() : Parser(false) {}  // ✅ Default to DEG mode

Parser::Parser(bool radianMode) : isRadian(radianMode) {
    functions = {
        {"sin", [this](double x) {
             return std::sin(isRadian ? x : x * M_PI / 180.0);
         }},
        {"cos", [this](double x) {
             return std::cos(isRadian ? x : x * M_PI / 180.0);
         }},
        {"tan", [this](double x) {
             return std::tan(isRadian ? x : x * M_PI / 180.0);
         }},
        {"sqrt", [](double x) { return std::sqrt(x); }},
        {"sqr", [](double x) { return x * x; }},
        {"log", [](double x) { return std::log(x); }}
    };
}


bool Parser::isOperator(const std::string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^";
}

bool Parser::isFunction(const std::string& token) {
    return functions.find(token) != functions.end();
}

int Parser::precedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

double Parser::applyOperator(const std::string& op, double a, double b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") return a / b;
    if (op == "^") return std::pow(a, b);
    throw std::runtime_error("Unknown operator");
}

double Parser::applyFunction(const std::string& func, double value) {
    return functions[func](value);
}

// Tokenizer: splits string into numbers, functions, operators, parentheses
std::vector<std::string> Parser::tokenize(const std::string& expr) {
    std::vector<std::string> tokens;
    std::string token;

    for (size_t i = 0; i < expr.length(); ++i) {
        char c = expr[i];

        if (std::isspace(c)) continue;

        // Accumulate numbers and decimals
        if (std::isdigit(c) || c == '.') {
            token += c;
        } else {
            // Push number token if we just finished one
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }

            // Handle √ as sqrt
            if (c == '√') {
                tokens.push_back("sqrt");
            }
            // Handle function names (sin, cos, etc.)
            else if (std::isalpha(c)) {
                std::string func;
                while (i < expr.length() && std::isalpha(expr[i])) {
                    func += expr[i++];
                }
                --i;
                tokens.push_back(func);
            }
            // Convert 'x' or '×' to '*'
            else if (c == 'x' || c == '×') {
                tokens.push_back("*");
            }
            // Standard operators or parentheses
            else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')') {
                tokens.push_back(std::string(1, c));
            }
        }
    }

    // Push final number token if any
    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}


// Shunting Yard Algorithm: infix to postfix
std::vector<std::string> Parser::toPostfix(const std::vector<std::string>& tokens) {
    std::vector<std::string> output;
    std::stack<std::string> ops;

    for (const std::string& token : tokens) {
        if (isNumber(token)) {
            output.push_back(token);
        }
        else if (isFunction(token)) {
            ops.push(token);
        }
        else if (isOperator(token)) {
            while (!ops.empty() && isOperator(ops.top()) &&
                   precedence(ops.top()) >= precedence(token)) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(token);
        }
        else if (token == "(") {
            ops.push(token);
        }
        else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }

            if (!ops.empty() && ops.top() == "(")
                ops.pop();  // remove "("

            if (!ops.empty() && isFunction(ops.top())) {
                output.push_back(ops.top());
                ops.pop();  // function comes after closing paren
            }
        }
        else {
            throw std::runtime_error("Unknown token in expression: " + token);
        }
    }

    while (!ops.empty()) {
        if (ops.top() == "(" || ops.top() == ")")
            throw std::runtime_error("Mismatched parentheses");
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}


// Evaluates postfix expression
double Parser::evaluatePostfix(const std::vector<std::string>& postfix) {
    std::stack<double> stack;

    for (const std::string& token : postfix) {
        if (isNumber(token)) {
            stack.push(std::stod(token));
        }
        else if (isOperator(token)) {
            if (stack.size() < 2)
                throw std::runtime_error("Insufficient operands for operator");

            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();
            stack.push(applyOperator(token, a, b));
        }
        else if (isFunction(token)) {
            if (stack.empty())
                throw std::runtime_error("Missing operand for function");

            double a = stack.top(); stack.pop();
            stack.push(applyFunction(token, a));
        }
        else {
            throw std::runtime_error("Unknown token in expression: " + token);
        }
    }

    if (stack.size() != 1)
        throw std::runtime_error("Malformed expression");

    return stack.top();
}


double Parser::evaluate(const std::string& expression) {
    std::vector<std::string> tokens = tokenize(expression);
    std::vector<std::string> postfix = toPostfix(tokens);
    return evaluatePostfix(postfix);
}
bool Parser::isNumber(const std::string& token) {
    std::istringstream iss(token);
    double val;
    return (iss >> val) && iss.eof();
}

