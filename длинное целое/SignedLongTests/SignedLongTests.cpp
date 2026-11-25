#include "pch.h"
#include "CppUnitTest.h"

#include <climits>
#include <sstream>
#include <stdexcept>
#include <string>

#include "SignedLong.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SignedLongCppUnitTests
{
    std::string toString(const SignedLong& value)
    {
        std::ostringstream stream;
        stream << value;
        return stream.str();
    }

    TEST_CLASS(ConstructionTests)
    {
    public:
        TEST_METHOD(DefaultAndIntegerConstructors_CreateCanonicalValues)
        {
            Assert::AreEqual(std::string("0"), toString(SignedLong()));
            Assert::AreEqual(std::string("123456789012345"), toString(SignedLong(123456789012345LL)));
            Assert::AreEqual(std::string("-42500"), toString(SignedLong(-42500LL)));
            Assert::AreEqual(std::string("9223372036854775807"), toString(SignedLong(LLONG_MAX)));
            Assert::AreEqual(std::string("-9223372036854775808"), toString(SignedLong(LLONG_MIN)));
        }

        TEST_METHOD(StringConstructor_ParsesSignLeadingZerosAndWhitespace)
        {
            Assert::AreEqual(std::string("-123"), toString(SignedLong("  -000123  ")));
            Assert::AreEqual(std::string("456"), toString(SignedLong("+000456")));
            Assert::AreEqual(std::string("0"), toString(SignedLong("+000")));
            Assert::AreEqual(std::string("0"), toString(SignedLong("-0")));
        }

        TEST_METHOD(StringConstructor_RejectsInvalidInput)
        {
            Assert::ExpectException<std::invalid_argument>([&]() {
                std::string empty;
                SignedLong value(empty);
                (void)value;
            });

            Assert::ExpectException<std::invalid_argument>([&]() {
                SignedLong value("12abc");
                (void)value;
            });
        }

        TEST_METHOD(CopyConstructorAndAssignment_ProduceIndependentValues)
        {
            SignedLong original("12345678901234567890");
            SignedLong copied(original);
            Assert::IsTrue(copied == original);

            copied += 1LL;
            Assert::IsTrue(copied != original);

            SignedLong assigned;
            assigned = original;
            assigned = assigned;
            Assert::IsTrue(assigned == original);

            original += 1LL;
            Assert::IsTrue(assigned != original);
        }
    };

    TEST_CLASS(ConversionTests)
    {
    public:
        TEST_METHOD(ConversionToLongLong_ReturnsFittedValues)
        {
            Assert::AreEqual(0LL, static_cast<long long>(SignedLong()));
            Assert::AreEqual(-42500LL, static_cast<long long>(SignedLong(-42500LL)));
            Assert::AreEqual(LLONG_MAX, static_cast<long long>(SignedLong(LLONG_MAX)));
            Assert::AreEqual(LLONG_MIN, static_cast<long long>(SignedLong(LLONG_MIN)));
        }

        TEST_METHOD(ConversionToLongLong_ThrowsOnOverflow)
        {
            SignedLong tooBigPositive("999999999999999999999999999999");
            Assert::ExpectException<std::overflow_error>([&]() {
                volatile long long value = static_cast<long long>(tooBigPositive);
                (void)value;
            });

            SignedLong tooBigNegative("-999999999999999999999999999999");
            Assert::ExpectException<std::overflow_error>([&]() {
                volatile long long value = static_cast<long long>(tooBigNegative);
                (void)value;
            });
        }
    };

    TEST_CLASS(ArithmeticTests)
    {
    public:
        TEST_METHOD(AdditionAndSubtraction_HandleSignsAndCarry)
        {
            Assert::AreEqual(std::string("100000000000000000000"),
                toString(SignedLong("99999999999999999999") + SignedLong(1LL)));
            Assert::AreEqual(std::string("-60"), toString(SignedLong("-100") + SignedLong(40LL)));
            Assert::AreEqual(std::string("-50"), toString(SignedLong("100") - SignedLong("150")));
            Assert::AreEqual(std::string("-60"), toString(SignedLong("-100") - SignedLong("-40")));

            SignedLong value("12345678901234567890");
            Assert::AreEqual(std::string("0"), toString(value + (-value)));
        }

        TEST_METHOD(Multiplication_HandlesSignsAndZero)
        {
            SignedLong a("1000000000000000000");
            SignedLong b("250000000000000000");

            Assert::AreEqual(std::string("250000000000000000000000000000000000"), toString(a * b));
            Assert::AreEqual(std::string("-30"), toString(SignedLong("-5") * SignedLong(6LL)));
            Assert::AreEqual(std::string("0"), toString(a * SignedLong(0LL)));
        }

        TEST_METHOD(Division_TruncatesAndRejectsZero)
        {
            SignedLong a("1000000000000000000");
            SignedLong b("250000000000000000");

            Assert::AreEqual(std::string("4"), toString(a / b));
            Assert::AreEqual(std::string("-33"), toString(SignedLong("-100") / SignedLong(3LL)));
            Assert::AreEqual(std::string("-33"), toString(SignedLong("100") / SignedLong(-3LL)));
            Assert::AreEqual(std::string("0"), toString(SignedLong("5") / SignedLong("10")));

            Assert::ExpectException<std::domain_error>([&]() {
                SignedLong result = a / 0LL;
                (void)result;
            });
        }

        TEST_METHOD(CompoundArithmeticWithLongLong_MatchesExpectedSequence)
        {
            SignedLong value("1000000000000000000");

            value += 50LL;
            value -= 20LL;
            value *= 2LL;
            value /= 3LL;

            Assert::AreEqual(std::string("666666666666666686"), toString(value));
        }
    };

    TEST_CLASS(IncrementDecrementTests)
    {
    public:
        TEST_METHOD(PrefixAndPostfixIncrement_ReturnExpectedValues)
        {
            SignedLong value(10LL);

            Assert::AreEqual(10LL, static_cast<long long>(value++));
            Assert::AreEqual(11LL, static_cast<long long>(value));
            Assert::AreEqual(12LL, static_cast<long long>(++value));
        }

        TEST_METHOD(PrefixAndPostfixDecrement_ReturnExpectedValues)
        {
            SignedLong value(10LL);

            Assert::AreEqual(10LL, static_cast<long long>(value--));
            Assert::AreEqual(9LL, static_cast<long long>(value));
            Assert::AreEqual(8LL, static_cast<long long>(--value));
        }

        TEST_METHOD(IncrementAndDecrement_CrossBoundaries)
        {
            SignedLong carry("999");
            Assert::AreEqual(std::string("1000"), toString(++carry));
            Assert::AreEqual(std::string("1000"), toString(carry++));
            Assert::AreEqual(std::string("1001"), toString(carry));

            SignedLong zero;
            Assert::AreEqual(std::string("-1"), toString(--zero));
            Assert::AreEqual(std::string("0"), toString(++zero));
        }
    };

    TEST_CLASS(ComparisonTests)
    {
    public:
        TEST_METHOD(ComparisonWithSignedLong_CoversRelationalOperators)
        {
            SignedLong small("-100");
            SignedLong large("100");
            SignedLong same("100");

            Assert::IsTrue(small < large);
            Assert::IsTrue(small <= large);
            Assert::IsTrue(large > small);
            Assert::IsTrue(large >= small);
            Assert::IsTrue(large == same);
            Assert::IsFalse(large != same);
            Assert::IsTrue(SignedLong("-100") < SignedLong("-5"));
        }

        TEST_METHOD(ComparisonWithLongLong_CoversRelationalOperators)
        {
            SignedLong negative(-5LL);
            SignedLong positive(10LL);

            Assert::IsTrue(negative < positive);
            Assert::IsTrue(negative <= 0LL);
            Assert::IsTrue(positive > negative);
            Assert::IsTrue(positive >= 10LL);
            Assert::IsTrue(positive == 10LL);
            Assert::IsTrue(positive != 99LL);
            Assert::IsFalse(negative > 0LL);
        }

        TEST_METHOD(ZeroComparison_NormalizesSignedZero)
        {
            Assert::IsTrue(SignedLong("-0") == SignedLong(0LL));
            Assert::IsTrue(SignedLong("-0") == 0LL);
            Assert::IsFalse(SignedLong("-0") < SignedLong(0LL));
        }
    };

    TEST_CLASS(StreamTests)
    {
    public:
        TEST_METHOD(StreamOutput_WritesSignAndDigits)
        {
            Assert::AreEqual(std::string("-98765432109876543210"),
                toString(SignedLong("-98765432109876543210")));
            Assert::AreEqual(std::string("0"), toString(SignedLong("+000")));
        }

        TEST_METHOD(StreamInput_ReadsValuesAndKeepsStateOnError)
        {
            std::istringstream valid("  -12345678901234567890 ");
            SignedLong parsed;
            Assert::IsTrue(static_cast<bool>(valid >> parsed));
            Assert::AreEqual(std::string("-12345678901234567890"), toString(parsed));

            std::istringstream invalid("not_a_number");
            SignedLong fallback(77LL);
            invalid >> fallback;
            Assert::IsTrue(invalid.fail());
            Assert::AreEqual(77LL, static_cast<long long>(fallback));
        }
    };
}
