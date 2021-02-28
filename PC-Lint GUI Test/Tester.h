#pragma once

#include <QDebug>
#include <QString>
#include <map>

namespace Test
{

class TestFunction
{
public:
    constexpr TestFunction() : m_failedTests(0), m_passedTests(0)
    {
    }
    virtual ~TestFunction() = default;

    template<typename TestType, typename TestList>
    void runTests(TestType* testType, TestList testList)
    {
        for (auto const& [testName, testFunction] : testList)
        {
            QString testResult;
            if ((testType->*testFunction)())
            {
                testResult = "[OK]";
                m_passedTests++;
            }
            else
            {
                testResult = "[FAIL]";
                m_failedTests++;
            }
            qDebug().noquote() << "Running test" << testName << "\t" << testResult;
        }
        qDebug().nospace() << "# Passed tests :" << m_passedTests;
        qDebug().nospace() << "# Failed tests :" << m_failedTests;
        qDebug().nospace() << "Total tests run:" << (m_passedTests+m_failedTests);

        qDebug() << '\n';

        // Reset
        m_failedTests = 0;
        m_passedTests = 0;
    }

protected:
    int m_failedTests;
    int m_passedTests;
};

template<typename T1>
constexpr inline bool compare(T1 t1, T1 t2) noexcept
{
    if (t1 != t2)
    {
        // TODO: Line, File and Function but has to be macro
        qDebug() << "Expected " << t1 << " but got " << t2;
        return false;
    }
    return true;
}

};
