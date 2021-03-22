#pragma once

#include <QDebug>
#include <QString>
#include <map>

namespace Test
{

class TestFunction
{
public:
    TestFunction() = default;
    virtual ~TestFunction() = default;

    template<typename TestType, typename TestList>
    void runTests(TestType* testType, TestList testList)
    {
        for (auto const& [testName, testFunction] : testList)
        {
            qDebug().noquote() << "Running test" << testName;
            (testType->*testFunction)();
        }
        qDebug() << '\n';
    }
};


#define TEST_COMPARE(Actual, Expected) do {if (!compare(Actual,Expected)) { qDebug().noquote().nospace() << __FILE__ << '(' << __LINE__ << ')' << " Expected " << Expected << " but got " << Actual; }} while (0)

// Provide some type safety
template<typename E1, typename E2>
constexpr inline bool compare(E1 e1, E2 e2) noexcept
{
    return (e1 == e2);
}

};
