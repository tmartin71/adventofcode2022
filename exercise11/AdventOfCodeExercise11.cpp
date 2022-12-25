#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <algorithm>

std::vector<std::string> ReadTextFile(std::string inputFilename)
{
    std::ifstream myfile;
    std::string line;

    std::vector<std::string> output;
    myfile.open(inputFilename);
    if (myfile.is_open())
    {
        while (getline(myfile,line))
        {
            output.push_back(std::string(line));
        }

        myfile.close();
    }
    return output;
}

std::vector<std::string> Split(const std::string& input, const char& commandDelimiter = ' ')
{
    std::vector<std::string> strings;
    std::istringstream f(input);
    std::string s;
    while(std::getline(f, s, commandDelimiter))
    {
        strings.push_back(s);
    }

    return strings;
}

std::string Trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}


/*
Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3
*/
const std::string MonkeyString("Monkey");
const std::string StartingItems("Starting items: ");
const std::string Operation("Operation: ");
const std::string Test("Test:");
const std::string IfTrue("If true:");
const std::string IfFalse("If false:");

class ExpressionNode;
class BinaryOpNode;
class VariableNode;
class ConstantNode;

class ExpressionVisitor
{
public:
    ExpressionVisitor() {}
    virtual ~ExpressionVisitor() {}

    virtual const long long int VisitBinaryOp(const BinaryOpNode&) const = 0;
    virtual const long long int VisitVariable(const VariableNode&) const = 0;
    virtual const long long int VisitConstant(const ConstantNode&) const = 0;
};

class ExpressionNode
{
public:
    ExpressionNode() {}
    virtual ~ExpressionNode() {}

    virtual const long long int AcceptEvaluator(const ExpressionVisitor&) const = 0;
};


long long int addition(const long long int a, const long long int b)
{
    return a + b;
}

long long int multiplication(const long long int a, const long long int b)
{
    return a * b;
}

std::function<long long int(long long int, long long int)> ParseBinaryOperation(const std::string& op)
{
    std::function<long long int(long long int, long long int)> opFn;
    if (op == "+")
    {
        opFn = addition;
    }
    else if (op == "*")
    {
        opFn = multiplication;
    }
    else
    {
        assert(false);
    }

    return opFn;
}

class BinaryOpNode : public ExpressionNode
{
public:
    BinaryOpNode(
        std::shared_ptr<ExpressionNode> rhs,
        std::shared_ptr<ExpressionNode> lhs,
        const std::string& op)
            : m_rhs(rhs)
            , m_lhs(lhs)
        {
            m_op = ParseBinaryOperation(op);
        }

    const long long int AcceptEvaluator(const ExpressionVisitor& v) const override
    {
        return v.VisitBinaryOp(*this);
    }

    std::shared_ptr<ExpressionNode> GetRHS() const
    {
        return m_rhs;
    }

    std::shared_ptr<ExpressionNode> GetLHS() const
    {
        return m_lhs;
    }

    const std::function<long long int(long long int, long long int)>& GetOp() const
    {
        return m_op;
    }

private:
    std::shared_ptr<ExpressionNode> m_rhs;
    std::shared_ptr<ExpressionNode> m_lhs;
    std::function<long long int(long long int, long long int)> m_op;
};

class VariableNode : public ExpressionNode
{
public:
    VariableNode(const std::string& name)
        : m_name(name){}
    ~VariableNode() = default;

    const long long int AcceptEvaluator(const ExpressionVisitor& v) const override
    {
        return v.VisitVariable(*this);
    }

    const std::string& GetName() const
    {
        return m_name;
    }

private:
    const std::string m_name;
};

class ConstantNode : public ExpressionNode
{
public:
    ConstantNode(const long long int value)
        : m_value(value) {}
    ~ConstantNode() = default;

    const long long int AcceptEvaluator(const ExpressionVisitor& v) const override
    {
        return v.VisitConstant(*this);
    }

    const long long int GetValue() const
    {
        return m_value;
    }

private:
    const long long int m_value;
};

class MonkeyExpressionEvaluator : public ExpressionVisitor
{
public:
    MonkeyExpressionEvaluator(
        std::shared_ptr<ExpressionNode> root)
            : m_root(root)
        {}

    ~MonkeyExpressionEvaluator() = default;

    const long long int Eval(const std::unordered_map<std::string, long long int> vars)
    {
        m_vars = vars;
        return m_root->AcceptEvaluator(*this);
    }

    const long long int VisitBinaryOp(const BinaryOpNode& n) const override
    {
        return n.GetOp()(n.GetRHS()->AcceptEvaluator(*this), n.GetLHS()->AcceptEvaluator(*this));
    }

    const long long int VisitVariable(const VariableNode& n) const override
    {
        const auto val = m_vars.find(n.GetName());
        assert(val != m_vars.end());
        return val->second;
    }
 
    const long long int VisitConstant(const ConstantNode& n) const override
    {
        return n.GetValue();
    }

private:
    std::shared_ptr<ExpressionNode> m_root;
    std::unordered_map<std::string, long long int> m_vars;
};

class MonkeyPredicate
{
public:
    MonkeyPredicate()
        : m_value(0) {}

    MonkeyPredicate(const uint32_t value)
        : m_value(value) {}
    ~MonkeyPredicate() = default;

    MonkeyPredicate(const MonkeyPredicate& rhs)
        : m_value(rhs.m_value) {}

    const uint32_t GetValue() const
    {
        return m_value;
    }

    bool operator() (const uint32_t testValue) const
    {
        return testValue % m_value == 0;
    }

private:
    uint32_t m_value;
};

class Monkey
{
public:
    Monkey(
        const uint32_t number,
        const std::vector<long long int>& startingItems,
        const std::shared_ptr<ExpressionNode> monkeyOpExpressionRoot,
        const MonkeyPredicate monkeyPredicate,
        const uint32_t throwToIfPredicateFalse,
        const uint32_t throwToIfPredicateTrue)
            : m_number(number)
            , m_monkeyOpExpressionRoot(monkeyOpExpressionRoot)
            , m_monkeyPredicate(monkeyPredicate)
            , m_throwToIfPredicateFalse(throwToIfPredicateFalse)
            , m_throwToIfPredicateTrue(throwToIfPredicateTrue)
    {
        for (const auto& item : startingItems)
        {
            m_items.push(item);
        }

        m_factor = m_monkeyPredicate.GetValue();
    }
    ~Monkey() = default;
    
    std::queue<long long int>& GetItems()
    {
        return m_items;
    }

    void ReceiveItem(const long long int itemNumber)
    {
        m_items.push(itemNumber);
    }

    std::shared_ptr<ExpressionNode> GetExpressionRoot() const
    {
        return m_monkeyOpExpressionRoot;
    }

    const MonkeyPredicate& GetPredicate()
    {
        return m_monkeyPredicate;
    }

    const uint32_t GetThrowToIfPredicateTrue() const
    {
        return m_throwToIfPredicateTrue;
    }

    const uint32_t GetThrowToIfPredicateFalse() const
    {
        return m_throwToIfPredicateFalse;
    }

    void InspectObject()
    {
        m_inspectedObjectCount++;
    }

    const long long int GetNumberObjectsInspected() const
    {
        return m_inspectedObjectCount;
    }

    const uint32_t GetFactor() const
    {
        return m_factor;
    }

private:
    std::queue<long long int> m_items;
    long long int m_inspectedObjectCount;
    const uint32_t m_number;
    const std::shared_ptr<ExpressionNode> m_monkeyOpExpressionRoot;
    const MonkeyPredicate m_monkeyPredicate;
    const uint32_t m_throwToIfPredicateFalse;
    const uint32_t m_throwToIfPredicateTrue;
    uint32_t m_factor;
};

std::vector<Monkey> ParseMonkeys()
{
const auto lines = ReadTextFile("input_exercise_11.txt");
    std::vector<Monkey> monkeys;

    uint32_t monkeyNumber;
    std::vector<long long int> items;
    std::shared_ptr<ExpressionNode> monkeyOpExpressionRoot;
    MonkeyPredicate monkeyPredicate;
    uint32_t throwToIfPredicateFalse;
    uint32_t throwToIfPredicateTrue;
    for (const auto& line : lines)
    {
        const auto trimmed = Trim(line);
        if (trimmed.rfind(MonkeyString, 0) == 0)
        {
            // Monkey <num>:
            const auto monkeyStr = Trim(trimmed, ":");
            const auto strings = Split(monkeyStr);
            assert(strings.size() == 2);

            monkeyNumber = std::stoi(strings[1].substr(0, 1));
        }
        else if (trimmed.rfind(StartingItems, 0) == 0)
        {
            // Starting Items: n, m, ...
            const auto startingItems = Split(trimmed, ':');
            assert(startingItems.size() == 2);
            const auto itemNumbers = Trim(startingItems[1]);
            const auto numberStrings = Split(itemNumbers, ',');

            std::vector<long long int> numbers;
            numbers.reserve(numberStrings.size());
            for (const auto& numberStr : numberStrings)
            {
                numbers.push_back(std::stoi(Trim(numberStr)));
            }

            items = numbers;
        }
        else if (trimmed.rfind(Operation, 0) == 0)
        {
            // Operation: new = old + 6
            const auto expression = Split(trimmed, '=');
            assert(expression.size() == 2);
            const auto tokens = Split(Trim(expression[1]));

            std::vector<std::shared_ptr<ExpressionNode> > inputs;
            std::string expressionFn;
            for (auto itr = tokens.begin(); itr != tokens.end(); ++itr)
            {
                if (*itr == "old")
                {
                    inputs.push_back(std::make_shared<VariableNode>(*itr));
                }
                else if (*itr == "+" || *itr == "*")
                {
                    expressionFn = *itr;
                }
                else
                {
                    try
                    {
                        auto val = std::stoi(*itr);
                        inputs.push_back(std::make_shared<ConstantNode>(val));
                    }
                    catch (...)
                    {
                        assert(false);
                    }
                }
            }

            assert(inputs.size() == 2);
            monkeyOpExpressionRoot = 
                std::make_shared<BinaryOpNode>(inputs[0], inputs[1], expressionFn);
        }
        else if (trimmed.rfind(Test, 0) == 0)
        {
            // Test: divisible by 19
            const auto tokens = Split(trimmed, ' ');
            assert(tokens.size() == 4);
            monkeyPredicate = MonkeyPredicate(std::stoi(tokens[3]));
        }
        else if (trimmed.rfind(IfTrue, 0) == 0)
        {
            // If true: throw to monkey 2
            const auto tokens = Split(trimmed, ' ');
            assert(tokens.size() == 6);
            throwToIfPredicateTrue = std::stoi(tokens[5]);
        }
        else if (trimmed.rfind(IfFalse, 0) == 0)
        {
            // If false: throw to monkey 0
            const auto tokens = Split(trimmed, ' ');
            assert(tokens.size() == 6);
            throwToIfPredicateFalse = std::stoi(tokens[5]);
        }
        else
        {
            Monkey m(monkeyNumber,
                        items,
                        monkeyOpExpressionRoot,
                        monkeyPredicate,
                        throwToIfPredicateFalse,
                        throwToIfPredicateTrue);
            monkeys.push_back(m);
        }
    }

    return monkeys;
}

void AdventOfCodeExercise10()
{
    auto monkeys = ParseMonkeys();

    // Simulation
    bool part1 = false;
    uint32_t nRounds;
    if (part1)
        nRounds = 20;
    else
        nRounds = 10000;

    uint32_t factorProduct = 1;
    for (const auto& monkey : monkeys)
    {
        factorProduct *= monkey.GetFactor();
    }

    for (int i = 0; i < nRounds; ++i)
    {
        for (auto& monkey : monkeys)
        {
            MonkeyExpressionEvaluator eval(monkey.GetExpressionRoot());
            while (!monkey.GetItems().empty())
            {
                const auto item = monkey.GetItems().front();
                monkey.GetItems().pop();
                monkey.InspectObject();
                auto worryLevel = item;
                std::unordered_map<std::string, long long int> vars;
                vars["old"] = worryLevel;

                worryLevel = eval.Eval(vars);
                if (part1)
                    worryLevel /= 3;
                else
                    worryLevel %= factorProduct;

                if (monkey.GetPredicate()(worryLevel))
                {
                    const auto throwTo = monkey.GetThrowToIfPredicateTrue();
                    monkeys[throwTo].ReceiveItem(worryLevel);
                }
                else if (!monkey.GetPredicate()(worryLevel))
                {
                    const auto throwTo = monkey.GetThrowToIfPredicateFalse();
                    monkeys[throwTo].ReceiveItem(worryLevel);
                }
            }
        }
    }

    std::vector<long long int> itemsInspected;
    itemsInspected.reserve(monkeys.size());
    for (const auto& monkey : monkeys)
    {
        itemsInspected.push_back(monkey.GetNumberObjectsInspected());
    }
    
    std::sort(itemsInspected.begin(), itemsInspected.end());
    const long long int totalPart1 = *(itemsInspected.end() - 2) * *(itemsInspected.end() - 1);

    std::cout << totalPart1 << std::endl;
}

int main()
{
    AdventOfCodeExercise10();
}