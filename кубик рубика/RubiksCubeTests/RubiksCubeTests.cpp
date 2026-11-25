#include "pch.h"
#include "CppUnitTest.h"
#include "RubiksCube.h"
#include <sstream>
#include <fstream>
#include <cstdio>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RubiksCubeTests
{
	TEST_CLASS(RubiksCubeUnitTests)
	{
	public:
		// 1. Конструктор по умолчанию и проверка собранного состояния
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

		// 2. getColor: корректные индексы и выход за границы
		TEST_METHOD(GetColor_ValidAndInvalidIndices)
		{
			RubiksCube cube;
			Assert::AreEqual(0, cube.getColor(0));
			Assert::AreEqual(5, cube.getColor(53));
			Assert::AreEqual(-1, cube.getColor(-1));
			Assert::AreEqual(-1, cube.getColor(54));
		}

		// 3. Конструктор копирования и оператор присваивания
		TEST_METHOD(CopyConstructorAndAssignment_IndependentCopies)
		{
			RubiksCube original;
			original.rotateFace(0, true);

			// Конструктор копирования
			RubiksCube copied(original);
			Assert::IsTrue(copied == original);
			Assert::IsFalse(copied != original);

			copied.rotateFace(2, true);
			Assert::IsTrue(copied != original);
			Assert::IsFalse(copied == original);

			// Оператор присваивания
			RubiksCube assigned;
			assigned = original;
			Assert::IsTrue(assigned == original);

			// Самоприсваивание не должно ломать объект
			assigned = assigned;
			Assert::IsTrue(assigned == original);
		}

		// 4. Поворот каждой из 6 граней по и против часовой стрелки + 4 поворота = исходное состояние
		TEST_METHOD(RotateFace_AllFacesClockwiseAndCounterClockwise_FourTimesReturnsToStart)
		{
			for (int face = 0; face < 6; ++face)
			{
				RubiksCube cube;
				const RubiksCube initial = cube;

				// 4 поворота по часовой стрелке возвращают грань и рёбра на место
				for (int step = 0; step < 4; ++step)
				{
					cube.rotateFace(face, true);
				}
				Assert::IsTrue(cube == initial);

				// 1 поворот меняет состояние кубика
				cube.rotateFace(face, true);
				Assert::IsTrue(cube != initial);
				Assert::IsFalse(cube.isSolved());

				// Обратный поворот возвращает в исходное состояние
				cube.rotateFace(face, false);
				Assert::IsTrue(cube == initial);
				Assert::IsTrue(cube.isSolved());
			}
		}

		// 5. Некорректный индекс грани в rotateFace
		TEST_METHOD(RotateFace_InvalidFaceIndex_DoesNothing)
		{
			RubiksCube cube;
			const RubiksCube initial = cube;

			cube.rotateFace(-1, true);
			cube.rotateFace(6, false);

			Assert::IsTrue(cube == initial);
		}

		// 6. Перемешивание (scramble)
		TEST_METHOD(Scramble_PositiveAndNonPositiveMoves)
		{
			RubiksCube cube;
			const RubiksCube initial = cube;

			// Неположительное число ходов не должно менять кубик
			cube.scramble(0);
			Assert::IsTrue(cube == initial);
			cube.scramble(-5);
			Assert::IsTrue(cube == initial);

			// Перемешивание на 25 ходов меняет собранный кубик
			cube.scramble(25);
			Assert::IsTrue(cube != initial);
			Assert::IsFalse(cube.isSolved());
		}

		// 7. Потоковый ввод/вывод (>> и <<)
		TEST_METHOD(StreamOperators_ReadAndWriteCube)
		{
			RubiksCube source;
			source.rotateFace(1, true);
			source.rotateFace(5, false);

			std::ostringstream oss;
			oss << source;
			const std::string output = oss.str();

			Assert::IsFalse(output.empty());
			Assert::IsTrue(output.find("0 ") != std::string::npos);
			Assert::IsTrue(output.find("5 ") != std::string::npos);

			// Формируем поток из 54 корректных чисел
			std::ostringstream rawColors;
			for (int i = 0; i < 54; ++i)
			{
				rawColors << source.getColor(i) << (i + 1 < 54 ? " " : "");
			}

			std::istringstream iss(rawColors.str());
			RubiksCube loaded;
			Assert::IsTrue(static_cast<bool>(iss >> loaded));
			Assert::IsTrue(loaded == source);
		}

		// 8. Некорректные данные при чтении из потока
		TEST_METHOD(StreamInputOperator_InvalidData_SetsFailbitAndKeepsCubeIntact)
		{
			RubiksCube cube;
			const RubiksCube initial = cube;

			// Неверный код цвета (9)
			std::string badColorData;
			for (int i = 0; i < 53; ++i) badColorData += "0 ";
			badColorData += "9";

			std::istringstream issBadColor(badColorData);
			issBadColor >> cube;
			Assert::IsTrue(issBadColor.fail());
			Assert::IsTrue(cube == initial);

			// Недостаточно чисел
			std::istringstream issShort("0 1 2 3");
			issShort >> cube;
			Assert::IsTrue(issShort.fail());
			Assert::IsTrue(cube == initial);
		}

		// 9. Загрузка из файла (успешная, несуществующий файл, битый файл)
		TEST_METHOD(LoadFromFile_ValidInvalidAndMissingFile)
		{
			const std::string validFile = "test_cube_valid.txt";
			const std::string invalidFile = "test_cube_invalid.txt";
			const std::string missingFile = "non_existent_cube_file_12345.txt";

			RubiksCube expected;
			expected.rotateFace(3, true);
			expected.rotateFace(4, false);

			{
				std::ofstream out(validFile);
				for (int i = 0; i < 54; ++i)
				{
					out << expected.getColor(i) << " ";
				}
			}

			RubiksCube cube;
			Assert::IsTrue(cube.loadFromFile(validFile));
			Assert::IsTrue(cube == expected);

			// Несуществующий файл возвращает false
			Assert::IsFalse(cube.loadFromFile(missingFile));

			// Битый файл возвращает false и не портит текущее состояние
			{
				std::ofstream badOut(invalidFile);
				badOut << "0 1 2 abc";
			}
			Assert::IsFalse(cube.loadFromFile(invalidFile));
			Assert::IsTrue(cube == expected);

			std::remove(validFile.c_str());
			std::remove(invalidFile.c_str());
		}
	};
}