#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <map>
#include <functional>
#include <vector>

class Parser {
public:
    Parser();
    Parser(bool radianMode = false);
    double evaluate(const std::string& expression);

private:
    std::vector<std::string> tokenize(const std::string& expr);
    std::vector<std::string> toPostfix(const std::vector<std::string>& tokens);
    double evaluatePostfix(const std::vector<std::string>& postfix);

    bool isRadian = false;
    bool isOperator(const std::string& token);
    bool isFunction(const std::string& token);
    int precedence(const std::string& op);
    double applyOperator(const std::string& op, double a, double b);
    double applyFunction(const std::string& func, double value);
    bool isNumber(const std::string& token);


    std::map<std::string, std::function<double(double)>> functions;
};

#endif // PARSER_H
