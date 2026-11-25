#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#define NOMINMAX
#include <Windows.h>
#include "SignedLong.h"

namespace {

    void clearBadInput() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool readSignedLong(const std::string& prompt, SignedLong& out) {
        std::cout << prompt;
        if (!(std::cin >> out)) {
            clearBadInput();
            std::cout << "Ошибка: введите корректное целое число.\n";
            return false;
        }
        return true;
    }

    bool readLongLong(const std::string& prompt, long long& out) {
        std::cout << prompt;
        if (!(std::cin >> out)) {
            clearBadInput();
            std::cout << "Ошибка: введите корректное число типа long long.\n";
            return false;
        }
        return true;
    }

    void printMenu() {
        std::cout << "\n=== Меню: Длинное целое со знаком (SignedLong) ===\n"
            << "1. Ввести значения A и B\n"
            << "2. Показать A, B и проверку копирования/присваивания\n"
            << "3. Арифметика с другим SignedLong (+, -, *, /, +=, -=, *=, /=)\n"
            << "4. Арифметика с обычным целым long long (+, -, *, /, +=, -=, *=, /=)\n"
            << "5. Инкремент и декремент (++A, A++, --A, A--)\n"
            << "6. Сравнение A и B, а также A с числом long long\n"
            << "7. Приведение A к встроенному целому типу (long long)\n"
            << "0. Выход\n"
            << "Выберите пункт: ";
    }

} // namespace

/**
 * @brief Точка входа: консольное меню для работы с классом SignedLong.
 * @return Код завершения программы (0 при штатном выходе).
 */
int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    SignedLong a(0LL);
    SignedLong b(0LL);
    int choice = -1;

    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            clearBadInput();
            std::cout << "Ошибка ввода пункта меню.\n";
            continue;
        }

        if (choice == 0) {
            break;
        }

        try {
            switch (choice) {
            case 1: {
                readSignedLong("Введите число A: ", a);
                readSignedLong("Введите число B: ", b);
                break;
            }
            case 2: {
                SignedLong copyA(a);
                SignedLong assignedB;
                assignedB = b;
                std::cout << "A = " << a << "\n"
                    << "B = " << b << "\n"
                    << "Копия A (конструктор копирования) = " << copyA << "\n"
                    << "Присвоенное B (operator=) = " << assignedB << "\n";
                break;
            }
            case 3: {
                std::cout << "A + B = " << (a + b) << "\n"
                    << "A - B = " << (a - b) << "\n"
                    << "A * B = " << (a * b) << "\n"
                    << "A / B = " << (a / b) << "\n";

                SignedLong mod(a);
                mod += b;
                std::cout << "После A += B: " << mod << "\n";
                mod -= b;
                std::cout << "После -= B: " << mod << "\n";
                mod *= b;
                std::cout << "После *= B: " << mod << "\n";
                mod /= b;
                std::cout << "После /= B: " << mod << "\n";
                break;
            }
            case 4: {
                long long num = 0LL;
                if (!readLongLong("Введите обычное целое число: ", num)) {
                    break;
                }
                std::cout << "A + num = " << (a + num) << "\n"
                    << "A - num = " << (a - num) << "\n"
                    << "A * num = " << (a * num) << "\n"
                    << "A / num = " << (a / num) << "\n";

                SignedLong mod(a);
                mod += num;
                std::cout << "После A += num: " << mod << "\n";
                mod -= num;
                std::cout << "После -= num: " << mod << "\n";
                mod *= num;
                std::cout << "После *= num: " << mod << "\n";
                mod /= num;
                std::cout << "После /= num: " << mod << "\n";
                break;
            }
            case 5: {
                std::cout << "Исходное A: " << a << "\n"
                    << "A++ вернуло: " << (a++) << ", теперь A = " << a << "\n"
                    << "++A вернуло: " << (++a) << "\n"
                    << "A-- вернуло: " << (a--) << ", теперь A = " << a << "\n"
                    << "--A вернуло: " << (--a) << "\n";
                break;
            }
            case 6: {
                std::cout << std::boolalpha
                    << "A == B: " << (a == b) << ", A != B: " << (a != b) << "\n"
                    << "A <  B: " << (a < b) << ", A <= B: " << (a <= b) << "\n"
                    << "A >  B: " << (a > b) << ", A >= B: " << (a >= b) << "\n";

                long long num = 0LL;
                if (readLongLong("Введите число long long для сравнения с A: ", num)) {
                    std::cout << "A == num: " << (a == num) << ", A != num: " << (a != num) << "\n"
                        << "A <  num: " << (a < num) << ", A <= num: " << (a <= num) << "\n"
                        << "A >  num: " << (a > num) << ", A >= num: " << (a >= num) << "\n";
                }
                break;
            }
            case 7: {
                const long long asLL = static_cast<long long>(a);
                std::cout << "static_cast<long long>(A) = " << asLL << "\n";
                break;
            }
            default:
                std::cout << "Неизвестный пункт меню.\n";
                break;
            }
        }
        catch (const std::exception& ex) {
            std::cout << "Ошибка операции: " << ex.what() << "\n";
        }
    }

    return 0;
}