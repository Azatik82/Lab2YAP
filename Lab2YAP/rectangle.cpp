#include "rectangle.h"
#include <stdexcept>
#include <algorithm> // для std::find
#include <vector>
#include <iostream> // Для вывода в SVG

// Определение статического массива допустимых цветов
// Можно расширить этот список
const std::vector<std::string> Rectangle::allowedColors = {
    "red", "green", "blue", "yellow", "black", "white", "purple", "orange"
};

// --- Валидация ---
void Rectangle::validateDimensions(double w, double h) {
    if (w <= 0 || h <= 0) {
        throw std::invalid_argument("Rectangle dimensions (width, height) must be positive.");
    }
    if (w > 1000 || h > 1000) {
        // Используем другое исключение или сообщение для разных ограничений
        throw std::out_of_range("Rectangle dimensions (width, height) must not exceed 1000.");
    }
}

void Rectangle::validateColor(const std::string& color) {
    if (color.empty()) { // Пустой цвет разрешен
        return;
    }
    // Ищем цвет в списке допустимых
    // Используем std::find для вектора
    if (std::find(allowedColors.begin(), allowedColors.end(), color) == allowedColors.end()) {
        throw std::invalid_argument("Invalid color specified: " + color + ". Must be one of the allowed colors or empty.");
    }
}

// --- Конструктор ---
Rectangle::Rectangle(double x, double y, double w, double h, std::string color, bool notOverlap)
    : m_x(x), m_y(y), m_width(0), m_height(0), m_color(""), m_notOverlap(notOverlap) // Инициализация по умолчанию для гарантий
{
    // 1. Проверка размеров
    validateDimensions(w, h);
    // 2. Проверка цвета
    validateColor(color);

    // Если все проверки прошли, присваиваем значения
    m_width = w;
    m_height = h;
    m_color = color;
    // m_x, m_y, m_notOverlap уже инициализированы
}

// --- Методы ---
void Rectangle::setColor(const std::string& color) {
    // 3. Используем ту же функцию валидации
    validateColor(color);
    m_color = color; // Присваиваем, только если валидация прошла
}

// 7. Проверка наложения (простая, без учета поворотов)
bool Rectangle::overlaps(const Rectangle& other) const noexcept {
    // Проверяем, что один прямоугольник слева от другого
    if (m_x + m_width <= other.m_x || other.m_x + other.m_width <= m_x) {
        return false;
    }
    // Проверяем, что один прямоугольник выше другого
    if (m_y + m_height <= other.m_y || other.m_y + other.m_height <= m_y) {
        return false;
    }
    // Если ни одно из условий не выполнено, значит есть наложение
    return true;
}

// Вывод в SVG формат
void Rectangle::drawSVG(std::ostream& out) const {
    out << "  <rect x=\"" << m_x << "\" y=\"" << m_y
        << "\" width=\"" << m_width << "\" height=\"" << m_height << "\"";
    if (!m_color.empty()) {
        out << " fill=\"" << m_color << "\"";
    }
    else {
        out << " fill=\"none\" stroke=\"black\""; // Если цвета нет, рисуем контур
    }
    out << " />\n";
    // Потенциально std::ostream может бросить исключение, если буфер переполнен или ошибка записи
    if (out.fail()) {
        // В реальном приложении можно было бы бросить исключение,
        // но в drawSVG это может быть излишне. Оставим проверку в Screen::saveSVG.
        // std::cerr << "Warning: Failed writing rectangle to SVG stream." << std::endl;
    }
}