#include "pch.h"
#include "CppUnitTest.h"
#include "SignedLong.h"
#include <sstream>
#include <stdexcept>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SignedLongTestsNamespace
{
    TEST_CLASS(SignedLongTests)
    {
    public:
        TEST_METHOD(ConstructorsAndRuleOfThree)
        {
            SignedLong zero;
            Assert::AreEqual(std::string("0"), (std::ostringstream() << zero).str());

            SignedLong pos(123456789012345LL);
            SignedLong copyPos(pos);
            Assert::IsTrue(pos == copyPos);
            Assert::IsFalse(pos != copyPos);

            SignedLong assigned;
            assigned = pos;
            assigned = assigned;
            Assert::IsTrue(assigned == pos);

            SignedLong fromStr("-98765432109876543210");
            Assert::AreEqual(std::string("-98765432109876543210"), (std::ostringstream() << fromStr).str());
        }

        TEST_METHOD(ConversionToLongLong)
        {
            SignedLong val(-42500LL);
            Assert::AreEqual(-42500LL, static_cast<long long>(val));

            SignedLong tooBig("999999999999999999999999999999");
            Assert::ExpectException<std::overflow_error>([&]() {
                volatile long long x = static_cast<long long>(tooBig);
                (void)x;
            });
        }

        TEST_METHOD(ArithmeticWithSignedLongAndLongLong)
        {
            SignedLong a("1000000000000000000");
            SignedLong b("250000000000000000");

            Assert::AreEqual(std::string("1250000000000000000"), (std::ostringstream() << (a + b)).str());
            Assert::AreEqual(std::string("750000000000000000"), (std::ostringstream() << (a - b)).str());
            Assert::AreEqual(std::string("250000000000000000000000000000000000"), (std::ostringstream() << (a * b)).str());
            Assert::AreEqual(std::string("4"), (std::ostringstream() << (a / b)).str());

            SignedLong c(a);
            c += 50LL;
            c -= 20LL;
            c *= 2LL;
            c /= 3LL;
            Assert::AreEqual(std::string("666666666666666686"), (std::ostringstream() << c).str());

            Assert::ExpectException<std::domain_error>([&]() {
                SignedLong divZero = a / 0LL;
                (void)divZero;
            });
        }

        TEST_METHOD(IncrementDecrementAndComparisons)
        {
            SignedLong x(10LL);
            Assert::AreEqual(10LL, static_cast<long long>(x++));
            Assert::AreEqual(12LL, static_cast<long long>(++x));
            Assert::AreEqual(12LL, static_cast<long long>(x--));
            Assert::AreEqual(10LL, static_cast<long long>(--x));

            SignedLong y(-5LL);
            Assert::IsTrue(y < x);
            Assert::IsTrue(y <= x);
            Assert::IsTrue(x > y);
            Assert::IsTrue(x >= y);
            Assert::IsTrue(y < 0LL);
            Assert::IsTrue(x == 10LL);
            Assert::IsTrue(x != 99LL);
        }

        TEST_METHOD(StreamInputAndInvalidInput)
        {
            std::istringstream iss("-12345678901234567890");
            SignedLong n;
            Assert::IsTrue(static_cast<bool>(iss >> n));
            Assert::AreEqual(std::string("-12345678901234567890"), (std::ostringstream() << n).str());

            std::istringstream bad("not_a_number");
            SignedLong fallback(77LL);
            bad >> fallback;
            Assert::IsTrue(bad.fail());
            Assert::AreEqual(77LL, static_cast<long long>(fallback));
        }
    };
}