#include "pch.h"
#include "CppUnitTest.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "RubiksCube.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RubiksCubeCppUnitTests
{
    TEST_CLASS(CubeStateTests)
    {
    public:
        TEST_METHOD(DefaultConstructor_CreatesSolvedCube)
        {
            RubiksCube cube;

            Assert::IsTrue(cube.isSolved());

            for (int face = 0; face < 6; ++face)
            {
                for (int cell = 0; cell < 9; ++cell)
                {
                    Assert::AreEqual(face, cube.getColor(face * 9 + cell));
                }
            }
        }

        TEST_METHOD(GetColor_ReturnsColorOrMinusOneForInvalidIndex)
        {
            RubiksCube cube;

            Assert::AreEqual(0, cube.getColor(0));
            Assert::AreEqual(5, cube.getColor(53));
            Assert::AreEqual(-1, cube.getColor(-1));
            Assert::AreEqual(-1, cube.getColor(54));
        }
    };

    TEST_CLASS(CopyAndComparisonTests)
    {
    public:
        TEST_METHOD(CopyConstructor_CreatesIndependentState)
        {
            RubiksCube original;
            original.rotateFace(0, true);

            RubiksCube copied(original);
            Assert::IsTrue(copied == original);

            copied.rotateFace(2, true);
            Assert::IsTrue(copied != original);
            Assert::IsFalse(copied == original);
        }

        TEST_METHOD(AssignmentOperator_HandlesCopyAndSelfAssignment)
        {
            RubiksCube original;
            original.rotateFace(1, true);
            const RubiksCube expected(original);

            RubiksCube assigned;
            assigned = original;
            Assert::IsTrue(assigned == original);

            assigned = assigned;
            Assert::IsTrue(assigned == original);

            assigned.rotateFace(5, false);
            Assert::IsTrue(assigned != original);
            Assert::IsTrue(original == expected);
        }
    };

    TEST_CLASS(RotationTests)
    {
    public:
        TEST_METHOD(RotateFace_FourClockwiseTurnsRestoreState)
        {
            for (int face = 0; face < 6; ++face)
            {
                RubiksCube cube;
                const RubiksCube initial = cube;

                for (int step = 0; step < 4; ++step)
                {
                    cube.rotateFace(face, true);
                }

                Assert::IsTrue(cube == initial);
            }
        }

        TEST_METHOD(RotateFace_OppositeTurnsCancelEachOther)
        {
            for (int face = 0; face < 6; ++face)
            {
                RubiksCube cube;
                const RubiksCube initial = cube;

                cube.rotateFace(face, true);
                Assert::IsTrue(cube != initial);
                Assert::IsFalse(cube.isSolved());

                cube.rotateFace(face, false);
                Assert::IsTrue(cube == initial);
                Assert::IsTrue(cube.isSolved());
            }
        }

        TEST_METHOD(RotateFace_InvalidIndexDoesNothing)
        {
            RubiksCube cube;
            const RubiksCube initial = cube;

            cube.rotateFace(-1, true);
            cube.rotateFace(6, false);

            Assert::IsTrue(cube == initial);
        }
    };

    TEST_CLASS(ScrambleTests)
    {
    public:
        TEST_METHOD(Scramble_NonPositiveMovesDoNothing)
        {
            RubiksCube cube;
            const RubiksCube initial = cube;

            cube.scramble(0);
            Assert::IsTrue(cube == initial);

            cube.scramble(-5);
            Assert::IsTrue(cube == initial);
        }

        TEST_METHOD(Scramble_PositiveMovesChangeState)
        {
            RubiksCube cube;
            const RubiksCube initial = cube;

            cube.scramble(1);
            Assert::IsTrue(cube != initial);
            Assert::IsFalse(cube.isSolved());

            cube.scramble(25);
            Assert::IsTrue(cube != initial);
        }
    };

    TEST_CLASS(StreamAndFileTests)
    {
    public:
        TEST_METHOD(StreamOutput_WritesStateAndFaceLegend)
        {
            RubiksCube cube;
            cube.rotateFace(1, true);

            std::ostringstream output;
            output << cube;
            const std::string text = output.str();

            Assert::IsFalse(text.empty());
            Assert::IsTrue(text.find("0 ") != std::string::npos);
            Assert::IsTrue(text.find("5 ") != std::string::npos);
            Assert::IsTrue(text.find("(Up)") != std::string::npos);
        }

        TEST_METHOD(StreamInput_ReadsValidState)
        {
            RubiksCube source;
            source.rotateFace(3, true);
            source.rotateFace(4, false);

            std::ostringstream rawColors;
            for (int i = 0; i < 54; ++i)
            {
                rawColors << source.getColor(i) << (i + 1 < 54 ? " " : "");
            }

            std::istringstream input(rawColors.str());
            RubiksCube loaded;

            Assert::IsTrue(static_cast<bool>(input >> loaded));
            Assert::IsTrue(loaded == source);
        }

        TEST_METHOD(StreamInput_InvalidDataSetsFailbitAndKeepsState)
        {
            RubiksCube cube;
            const RubiksCube initial = cube;

            std::string badColorData;
            for (int i = 0; i < 53; ++i)
            {
                badColorData += "0 ";
            }
            badColorData += "9";

            std::istringstream badColorInput(badColorData);
            badColorInput >> cube;
            Assert::IsTrue(badColorInput.fail());
            Assert::IsTrue(cube == initial);

            std::istringstream shortInput("0 1 2 3");
            shortInput >> cube;
            Assert::IsTrue(shortInput.fail());
            Assert::IsTrue(cube == initial);
        }

        TEST_METHOD(LoadFromFile_ValidAndMissingFile)
        {
            const std::string validFile = "rubiks_cube_valid.txt";
            const std::string missingFile = "rubiks_cube_missing_file_12345.txt";

            RubiksCube expected;
            expected.rotateFace(3, true);
            expected.rotateFace(4, false);

            {
                std::ofstream output(validFile);
                for (int i = 0; i < 54; ++i)
                {
                    output << expected.getColor(i) << " ";
                }
            }

            RubiksCube cube;
            Assert::IsTrue(cube.loadFromFile(validFile));
            Assert::IsTrue(cube == expected);

            Assert::IsFalse(cube.loadFromFile(missingFile));
            Assert::IsTrue(cube == expected);

            std::remove(validFile.c_str());
        }

        TEST_METHOD(LoadFromFile_InvalidFileKeepsState)
        {
            const std::string invalidFile = "rubiks_cube_invalid.txt";

            RubiksCube cube;
            cube.rotateFace(2, true);
            const RubiksCube initial = cube;

            {
                std::ofstream output(invalidFile);
                output << "0 1 2 abc";
            }

            Assert::IsFalse(cube.loadFromFile(invalidFile));
            Assert::IsTrue(cube == initial);

            std::remove(invalidFile.c_str());
        }
    };
}
