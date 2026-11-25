#ifndef RUBIKS_CUBE_H
#define RUBIKS_CUBE_H

#include <iostream>
#include <fstream>
#include <string>

/**
 * @brief Класс, реализующий головоломку «Кубик Рубика» 3x3x3.
 *
 * Состояние хранится в виде динамического массива из 54 элементов
 * (6 граней по 9 стикеров, значения цветов в диапазоне 0..5).
 */
class RubiksCube {
private:
    int* colors; 

    static const int SIZE = 54;      
    static const int FACE_SIZE = 9; 
    static const int NUM_FACES = 6;  

    void rotateFaceData(int faceIndex, bool clockwise);
    void shiftEdgesCycle(const int* p1, const int* p2, const int* p3, const int* p4, bool clockwise);
    void rotateEdges(int faceIndex, bool clockwise);
    bool isValidColor(int color) const;

public:
    RubiksCube();
    RubiksCube(const RubiksCube& other);
    RubiksCube& operator=(const RubiksCube& other);
    ~RubiksCube();

    void scramble(int moves = 20);
    void rotateFace(int faceIndex, bool clockwise);
    bool isSolved() const;
    int getColor(int index) const;
    bool loadFromFile(const std::string& filename);

    bool operator==(const RubiksCube& other) const;
    bool operator!=(const RubiksCube& other) const;

    friend std::ostream& operator<<(std::ostream& os, const RubiksCube& cube);
    friend std::istream& operator>>(std::istream& is, RubiksCube& cube);
};

#endif 