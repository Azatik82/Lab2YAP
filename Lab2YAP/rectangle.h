#pragma once // Защита от двойного включения

#include <string>
#include <vector>
#include <stdexcept> // Для стандартных исключений
#include <ostream>   // Для drawSVG

class Rectangle {
public:
    // --- Конструкторы ---
    // Добавляем флаг notOverlap и цвет по умолчанию
    Rectangle(double x, double y, double w, double h, std::string color = "", bool notOverlap = false);

    // --- Валидация (статическая, чтобы использовать в конструкторе и setColor) ---
    static void validateDimensions(double w, double h);
    static void validateColor(const std::string& color);

    // --- Методы ---
    void setColor(const std::string& color);
    bool overlaps(const Rectangle& other) const noexcept; // Проверка наложения
    void drawSVG(std::ostream& out) const; // Вывод в SVG поток

    // --- Геттеры (помечены noexcept, т.к. не бросают исключений) ---
    double getX() const noexcept { return m_x; }
    double getY() const noexcept { return m_y; }
    double getWidth() const noexcept { return m_width; }
    double getHeight() const noexcept { return m_height; }
    std::string getColor() const noexcept { return m_color; }
    bool getNotOverlap() const noexcept { return m_notOverlap; }

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;
    std::string m_color;
    bool m_notOverlap; // Флаг для проверки наложения

    // Статический массив допустимых цветов
    static const std::vector<std::string> allowedColors;
};
