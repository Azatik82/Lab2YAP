#pragma once

#include "rectangle.h"
#include <vector>
#include <string>
#include <stdexcept> // для std::runtime_error
#include <fstream>   // Для работы с файлами

// 5. Свой класс исключения ScreenError
class ScreenError : public std::runtime_error {
public:
    // Конструктор принимает сообщение и копию прямоугольника
    ScreenError(const std::string& message, const Rectangle& rect)
        : std::runtime_error(message), m_rectangle(rect) {} // Копируем объект

    // Метод для получения доступа к сохраненному прямоугольнику
    const Rectangle& getRectangle() const noexcept {
        return m_rectangle;
    }

    // Можно переопределить what() для добавления информации о прямоугольнике,
    // но стандартный подход - предоставить getter.

private:
    Rectangle m_rectangle; // Храним копию объекта
};

// Класс "экрана"
class Screen {
public:
    Screen(double width, double height) noexcept; // Конструктор не бросает исключений

    // Добавление прямоугольника (бросает исключения)
    void addRectangle(const Rectangle& rect);

    // 8. "Безопасное" добавление (не бросает ожидаемых исключений)
    bool tryAddRectangle(const Rectangle& rect) noexcept; // Добавляем noexcept
    std::string getLastError() const noexcept; // Получить текст последней ошибки

    // 4. Сохранение в SVG с обработкой ошибок файла
    void saveSVG(const std::string& filename) const;

    // Часть 2: Чтение из файла
    void loadFromFile(const std::string& filename);

    // Геттеры (не бросают исключений)
    double getWidth() const noexcept { return m_width; }
    double getHeight() const noexcept { return m_height; }
    const std::vector<Rectangle>& getRectangles() const noexcept { return m_rectangles; }

private:
    double m_width;
    double m_height;
    std::vector<Rectangle> m_rectangles;
    std::string m_lastError; // Для tryAddRectangle

    // Вспомогательная функция для проверки перед добавлением
    // Возвращает true, если проверка пройдена, иначе false (и устанавливает m_lastError)
    // или бросает исключение, если throwOnError = true
    bool checkRectanglePlacement(const Rectangle& rect, bool throwOnError);

    // Вспомогательная функция для проверки наложения
    // Возвращает true, если наложение есть, иначе false
    bool checkOverlap(const Rectangle& rect) const noexcept;
};

// Кастомное исключение для ошибок чтения файла (Часть 2)
class FileParseError : public std::runtime_error {
public:
    FileParseError(const std::string& filename, int lineNumber, const std::string& details)
        : std::runtime_error("Error parsing file '" + filename + "' at line " + std::to_string(lineNumber) + ": " + details),
        m_filename(filename), m_lineNumber(lineNumber) {}

    const std::string& getFilename() const noexcept { return m_filename; }
    int getLineNumber() const noexcept { return m_lineNumber; }

private:
    std::string m_filename;
    int m_lineNumber;
};
