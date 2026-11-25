/**
 * @file RubiksCube.cpp
 * @brief Реализация методов и операторов класса RubiksCube.
 */

#include "RubiksCube.h"
#include <cstdlib>
#include <ctime>

RubiksCube::RubiksCube() {
	colors = new int[SIZE];

	for (int i = 0; i < NUM_FACES; ++i) {
		for (int j = 0; j < FACE_SIZE; ++j) {
			colors[i * FACE_SIZE + j] = i;
		}
	}
}

// конструктор копирования
RubiksCube::RubiksCube(const RubiksCube& other) {
	colors = new int[SIZE];
	for (int i = 0; i < SIZE; ++i) {
		colors[i] = other.colors[i];
	}
}

// оператор присваивания
RubiksCube& RubiksCube::operator=(const RubiksCube& other) {
	if (this != &other) {
		int* newColors = new int[SIZE]; 

		for (int i = 0; i < SIZE; ++i) {
			newColors[i] = other.colors[i];
		}

		delete[] colors;
		colors = newColors;
	}
	return *this;
}

RubiksCube::~RubiksCube() {
	delete[] colors;
}

bool RubiksCube::isValidColor(int color) const {
	return color >= 0 && color < NUM_FACES;
}

void RubiksCube::rotateFaceData(int faceindex, bool clockwise) {
	if (faceindex < 0 || faceindex >= NUM_FACES) {
		return;
	}

	int base = faceindex * FACE_SIZE; 
	int temp[9];

	for (int i = 0; i < 9; ++i) {
		temp[i] = colors[base + i];
	}

	// поворот по часовой стрелке / против часовой стрелки
	if (clockwise) {
		colors[base + 0] = temp[6]; colors[base + 1] = temp[3]; colors[base + 2] = temp[0];
		colors[base + 3] = temp[7]; colors[base + 4] = temp[4]; colors[base + 5] = temp[1];
		colors[base + 6] = temp[8]; colors[base + 7] = temp[5]; colors[base + 8] = temp[2];
	}
	else {
		colors[base + 0] = temp[2]; colors[base + 1] = temp[5]; colors[base + 2] = temp[8];
		colors[base + 3] = temp[1]; colors[base + 4] = temp[4]; colors[base + 5] = temp[7];
		colors[base + 6] = temp[0]; colors[base + 7] = temp[3]; colors[base + 8] = temp[6];
	}
}

void RubiksCube::shiftEdgesCycle(const int* p1, const int* p2, const int* p3, const int* p4, bool clockwise) {
	int temp_colors[3];

	if (clockwise) {
		for (int i = 0; i < 3; ++i) temp_colors[i] = colors[p1[i]];
		// Сдвигаем цвета: p1 <- p4 <- p3 <- p2 <- p1
		for (int i = 0; i < 3; ++i) colors[p1[i]] = colors[p4[i]];
		for (int i = 0; i < 3; ++i) colors[p4[i]] = colors[p3[i]];
		for (int i = 0; i < 3; ++i) colors[p3[i]] = colors[p2[i]];
		for (int i = 0; i < 3; ++i) colors[p2[i]] = temp_colors[i];
	}
	else {
		for (int i = 0; i < 3; ++i) temp_colors[i] = colors[p1[i]];
		// Сдвигаем цвета в обратную сторону: p1 <- p2 <- p3 <- p4 <- p1
		for (int i = 0; i < 3; ++i) colors[p1[i]] = colors[p2[i]];
		for (int i = 0; i < 3; ++i) colors[p2[i]] = colors[p3[i]];
		for (int i = 0; i < 3; ++i) colors[p3[i]] = colors[p4[i]];
		for (int i = 0; i < 3; ++i) colors[p4[i]] = temp_colors[i];
	}
}

void RubiksCube::rotateEdges(int faceIndex, bool clockwise) {
	int p1[3], p2[3], p3[3], p4[3];

	if (faceIndex == 0) { // Верх
		for (int i = 0; i < 3; ++i) {
			p1[i] = 2 * 9 + i;
			p2[i] = 5 * 9 + i;
			p3[i] = 3 * 9 + i;
			p4[i] = 4 * 9 + i;
		}
		shiftEdgesCycle(p1, p2, p3, p4, clockwise);
	}
	else if (faceIndex == 1) { // Низ
		for (int i = 0; i < 3; ++i) {
			p1[i] = 2 * 9 + 6 + i;
			p2[i] = 4 * 9 + 6 + i;
			p3[i] = 3 * 9 + 6 + i;
			p4[i] = 5 * 9 + 6 + i;
		}
		shiftEdgesCycle(p1, p2, p3, p4, clockwise);
	}
	else if (faceIndex == 2) { // Перед
		for (int i = 0; i < 3; ++i) {
			p1[i] = 0 * 9 + 6 + i;
			p2[i] = 5 * 9 + i * 3;
			p3[i] = 1 * 9 + 2 - i;
			p4[i] = 4 * 9 + 8 - i * 3;
		}
		shiftEdgesCycle(p1, p2, p3, p4, clockwise);
	}
	else if (faceIndex == 3) { // Зад
		for (int i = 0; i < 3; ++i) {
			p1[i] = 0 * 9 + 2 - i;
			p2[i] = 4 * 9 + i * 3;
			p3[i] = 1 * 9 + 6 + i;
			p4[i] = 5 * 9 + 8 - i * 3;
		}
		shiftEdgesCycle(p1, p2, p3, p4, clockwise);
	}
	else if (faceIndex == 4) { // Лево
		for (int i = 0; i < 3; ++i) {
			p1[i] = 0 * 9 + i * 3;
			p2[i] = 2 * 9 + i * 3;
			p3[i] = 1 * 9 + i * 3;
			p4[i] = 3 * 9 + 8 - i * 3;
		}
		shiftEdgesCycle(p1, p2, p3, p4, clockwise);
	}
	else if (faceIndex == 5) { // Право
		for (int i = 0; i < 3; ++i) {
			p1[i] = 0 * 9 + 2 + i * 3;
			p2[i] = 3 * 9 + 6 - i * 3;
			p3[i] = 1 * 9 + 2 + i * 3;
			p4[i] = 2 * 9 + 2 + i * 3;
		}
		shiftEdgesCycle(p1, p2, p3, p4, clockwise);
	}
}

// Перемешивает кубик случайными ходами
void RubiksCube::scramble(int moves) {
	if (moves <= 0) {
		return;
	}

	// Инициализируем генератор случайных чисел текущим временем
	static bool initialized = false;
	if (!initialized) {
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		initialized = true;
	}

	for (int i = 0; i < moves; ++i) {
		int face = std::rand() % 6;      
		bool dir = (std::rand() % 2) == 1;  
		rotateFace(face, dir);            
	}
}


bool RubiksCube::isSolved() const {
	for (int i = 0; i < 6; ++i) {
		int centerColor = colors[i * FACE_SIZE + 4]; 

	
		for (int j = 0; j < FACE_SIZE; ++j) {
			if (colors[i * FACE_SIZE + j] != centerColor) {
				return false; 
			}
		}
	}
	return true; 
}

// Возвращает цвет стикера по индексу (с защитой от выхода за границы)
int RubiksCube::getColor(int index) const {
	if (index >= 0 && index < SIZE) {
		return colors[index];
	}
	return -1; 
}

// Загружает состояние кубика из текстового файла 
bool RubiksCube::loadFromFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		return false; 
	}

	RubiksCube temp; 
	if (!(file >> temp)) {
		file.close();
		return false;
	}

	*this = temp;
	file.close();
	return true;
}

bool RubiksCube::operator==(const RubiksCube& other) const {
	for (int i = 0; i < SIZE; ++i) {
		if (this->colors[i] != other.colors[i]) {
			return false;
		}
	}
	return true;
}

bool RubiksCube::operator!=(const RubiksCube& other) const {
	return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const RubiksCube& cube) {
	os << "\n";
	os << "╔══════════════════════════════════════╗\n";
	os << "║        СОСТОЯНИЕ КУБИКА РУБИКА       ║\n";
	os << "╚══════════════════════════════════════╝\n\n";

	os << "              ┌─────────┐\n";
	for (int row = 0; row < 3; ++row) {
		os << "              │ ";
		for (int col = 0; col < 3; ++col) {
			os << cube.colors[0 * 9 + row * 3 + col] << " ";
		}
		os << "│\n";
	}
	os << "              └─────────┘\n";

	os << "    ┌─────────┬─────────┬─────────┬─────────┐\n";
	for (int row = 0; row < 3; ++row) {
		os << "    │ ";
		for (int col = 0; col < 3; ++col) os << cube.colors[4 * 9 + row * 3 + col] << " ";
		os << "│ ";
		for (int col = 0; col < 3; ++col) os << cube.colors[2 * 9 + row * 3 + col] << " ";
		os << "│ ";
		for (int col = 0; col < 3; ++col) os << cube.colors[5 * 9 + row * 3 + col] << " ";
		os << "│ ";
		for (int col = 0; col < 3; ++col) os << cube.colors[3 * 9 + row * 3 + col] << " ";
		os << "│\n";
	}
	os << "    └─────────┴─────────┴─────────┴─────────┘\n";

	os << "              ┌─────────┐\n";
	for (int row = 0; row < 3; ++row) {
		os << "              │ ";
		for (int col = 0; col < 3; ++col) {
			os << cube.colors[1 * 9 + row * 3 + col] << " ";
		}
		os << "│\n";
	}
	os << "              └─────────┘\n\n";

	os << "  Грань 0: Верх (Up)     Грань 1: Низ (Down)\n";
	os << "  Грань 2: Перед (Front) Грань 3: Зад (Back)\n";
	os << "  Грань 4: Лево (Left)   Грань 5: Право (Right)\n";

	return os;
}

std::istream& operator>>(std::istream& is, RubiksCube& cube) {
	int buffer[RubiksCube::SIZE];
	for (int i = 0; i < RubiksCube::SIZE; ++i) {
		if (!(is >> buffer[i]) || !cube.isValidColor(buffer[i])) {
			is.setstate(std::ios::failbit);
			return is;
		}
	}
	for (int i = 0; i < RubiksCube::SIZE; ++i) {
		cube.colors[i] = buffer[i];
	}
	return is;
}

void RubiksCube::rotateFace(int faceIndex, bool clockwise) {
	if (faceIndex < 0 || faceIndex > 5) return; 
	rotateFaceData(faceIndex, clockwise);       
	rotateEdges(faceIndex, clockwise);          
}