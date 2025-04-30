#include "screen.h"
#include <fstream>   // Для std::ofstream, std::ifstream
#include <sstream>   // Для std::stringstream (парсинг файла)
#include <iostream>  // Для std::cerr
#include <vector>    // Для временного хранения в loadFromFile
#include <limits>    // Для numeric_limits

Screen::Screen(double width, double height) noexcept
    : m_width(width > 0 ? width : 100.0), // Простая проверка в конструкторе
    m_height(height > 0 ? height : 100.0),
    m_lastError("") {}

// Вспомогательная функция проверки наложения
bool Screen::checkOverlap(const Rectangle& rect) const noexcept {
    if (rect.getNotOverlap()) { // Проверяем только если установлен флаг
        for (const auto& existingRect : m_rectangles) {
            if (rect.overlaps(existingRect)) {
                return true; // Наложение найдено
            }
        }
    }
    return false; // Наложения нет (или флаг не установлен)
}

// Вспомогательная функция валидации размещения
bool Screen::checkRectanglePlacement(const Rectangle& rect, bool throwOnError) {
    // 6. Проверка выхода за границы экрана
    if (rect.getX() < 0 || rect.getY() < 0 ||
        rect.getX() + rect.getWidth() > m_width ||
        rect.getY() + rect.getHeight() > m_height)
    {
        m_lastError = "Rectangle is out of screen bounds.";
        if (throwOnError) {
            throw ScreenError(m_lastError, rect); // Бросаем ScreenError
        }
        return false;
    }

    // 7. Проверка наложения
    if (checkOverlap(rect)) {
        m_lastError = "Rectangle with notOverlap=true overlaps with an existing rectangle.";
        if (throwOnError) {
            throw ScreenError(m_lastError, rect); // Бросаем ScreenError
        }
        return false;
    }

    m_lastError = ""; // Ошибок нет
    return true;
}


void Screen::addRectangle(const Rectangle& rect) {
    // Выполняем все проверки перед модификацией вектора для сильной гарантии
    checkRectanglePlacement(rect, true); // true - бросать исключение при ошибке

    // Если проверки прошли успешно (не было брошено исключение), добавляем
    // std::vector::push_back обеспечивает сильную гарантию сам по себе
    // (если не считать std::bad_alloc)
    m_rectangles.push_back(rect);
}

bool Screen::tryAddRectangle(const Rectangle& rect) noexcept {
    // 8. Вызываем проверку, но не бросаем исключение (false)
    // Результат проверки запишется в m_lastError если что-то не так
    if (checkRectanglePlacement(rect, false)) {
        try {
            // Добавление все еще может бросить std::bad_alloc, перехватим на всякий случай
            m_rectangles.push_back(rect);
            m_lastError = ""; // Явно сбрасываем ошибку при успехе
            return true;
        }
        catch (const std::bad_alloc&) {
            m_lastError = "Memory allocation failed while adding rectangle.";
            // В реальном приложении возможно стоит перебросить или логировать
            return false;
        }
        catch (...) {
            // Неожиданное исключение (маловероятно из push_back)
            m_lastError = "An unexpected error occurred during addition.";
            return false;
        }
    }
    // checkRectanglePlacement уже установила m_lastError
    return false;
}

std::string Screen::getLastError() const noexcept {
    return m_lastError;
}

void Screen::saveSVG(const std::string& filename) const {
    // 4. Используем std::ofstream и проверяем ошибки
    std::ofstream outFile;

    // Включаем генерацию исключений для потока outFile при ошибках
    outFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    try {
        outFile.open(filename); // Может бросить исключение, если файл не может быть открыт/создан

        // Заголовок SVG
        outFile << "<svg width=\"" << m_width << "\" height=\"" << m_height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
        outFile << "  <rect width=\"100%\" height=\"100%\" fill=\"lightgrey\" />\n"; // Фон

        // Рисуем все прямоугольники
        for (const auto& rect : m_rectangles) {
            rect.drawSVG(outFile); // Метод drawSVG сам пишет в поток
            // Ошибки записи также вызовут исключение из-за .exceptions()
        }

        // Конец SVG
        outFile << "</svg>\n";

        // outFile закроется автоматически при выходе из блока try (RAII)
        // Если бы не exceptions(), нужно было бы проверять outFile.fail() после каждой записи

    }
    catch (const std::ios_base::failure& e) {
        // Перехватываем исключение потока и бросаем свое с информацией
        // Используем стандартное runtime_error, т.к. не требуется хранить Rectangle
        throw std::runtime_error("Error saving to SVG file '" + filename + "': " + e.what());
    }
    // Другие исключения (например, std::bad_alloc) пробросятся выше
}


// --- Часть 2: Чтение из файла ---
void Screen::loadFromFile(const std::string& filename) {
    std::ifstream inFile;
    // Включаем генерацию исключений для ошибок чтения, конца файла и т.д.
    inFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Не включаем eofbit, т.к. будем читать в цикле

    std::vector<Rectangle> loadedRectangles; // Временный вектор для сильной гарантии
    int lineNumber = 0;
    std::string currentLine;
    std::string lastValidColor = ""; // Запоминаем последний указанный цвет

    try {
        inFile.open(filename); // Может бросить исключение

        while (std::getline(inFile, currentLine)) {
            lineNumber++;
            std::stringstream ss(currentLine);
            double cx, cy, w, h;
            std::string colorStr = ""; // Цвет для текущей строки

            // Пытаемся прочитать 4 обязательных числа
            if (!(ss >> cx >> cy >> w >> h)) {
                // Ошибка формата строки (недостаточно чисел)
                throw FileParseError(filename, lineNumber, "Invalid format - expected 'cx cy w h [color]'");
            }

            // Пытаемся прочитать необязательный цвет
            if (ss >> colorStr) {
                lastValidColor = colorStr; // Обновляем последний валидный цвет
            }
            else {
                // Если цвет не указан, используем последний запомненный
                colorStr = lastValidColor;
            }

            // Проверяем, не осталось ли лишних данных в строке
            std::string remaining;
            if (ss >> remaining) {
                throw FileParseError(filename, lineNumber, "Extra data found on line after expected fields.");
            }

            // --- Создание и проверка ПРЯМОУГОЛЬНИКА (бросит исключения при ошибках) ---
             // Координаты из файла - центр (cx, cy), конвертируем в левый верхний угол (x, y)
            double x = cx - w / 2.0;
            double y = cy - h / 2.0;

            // Пытаемся создать прямоугольник ВО ВРЕМЕННЫЙ ВЕКТОР
            // Конструктор Rectangle выполнит проверку размеров и цвета
            loadedRectangles.emplace_back(x, y, w, h, colorStr, false); // Добавляем с notOverlap=false по умолчанию

        } // конец while getline

        // --- Проверка всех загруженных прямоугольников (границы и наложения) ---
        // Проверяем каждый новый прямоугольник относительно ГРАНИЦ ЭКРАНА и УЖЕ СУЩЕСТВУЮЩИХ на экране
        for (const auto& newRect : loadedRectangles) {
            // Проверяем выход за границы и наложение на УЖЕ существующие прямоугольники
            if (newRect.getX() < 0 || newRect.getY() < 0 ||
                newRect.getX() + newRect.getWidth() > m_width ||
                newRect.getY() + newRect.getHeight() > m_height)
            {
                // Не используем ScreenError, так как ошибка связана с файлом
                throw FileParseError(filename, -1, "Rectangle loaded from file is out of screen bounds."); // -1 т.к. номер строки тут не так важен
            }
            // Примечание: По заданию не требовалось проверять наложения между прямоугольниками ИЗ САМОГО ФАЙЛА.
            // Если это нужно, добавьте здесь цикл для проверки newRect против других в loadedRectangles.
        }


        // --- Сильная гарантия: Если все прочитано и проверено успешно ---
        // Добавляем все загруженные прямоугольники в основной вектор ТОЛЬКО СЕЙЧАС
        m_rectangles.insert(m_rectangles.end(), loadedRectangles.begin(), loadedRectangles.end());


    }
    catch (const std::ios_base::failure& e) {
        // Ошибка чтения файла (файл не найден, ошибка доступа и т.д.)
        if (inFile.eof() && lineNumber == 0 && loadedRectangles.empty()) {
            // Файл пустой - это не ошибка чтения, просто ничего не делаем
        }
        else {
            throw FileParseError(filename, lineNumber, "File read error: " + std::string(e.what()));
        }
    }
    catch (const std::invalid_argument& e) {
        // Ошибка валидации из Rectangle (размеры, цвет)
        throw FileParseError(filename, lineNumber, "Invalid rectangle data: " + std::string(e.what()));
    }
    catch (const std::out_of_range& e) {
        // Ошибка валидации из Rectangle (размеры > 1000)
        throw FileParseError(filename, lineNumber, "Invalid rectangle data: " + std::string(e.what()));
    }
    catch (const FileParseError& e) {
        // Просто перебрасываем нашу ошибку парсинга
        throw;
    }
    catch (const std::exception& e) {
        // Другие возможные стандартные исключения (например, bad_alloc при создании временного вектора)
        throw FileParseError(filename, lineNumber, "An unexpected error occurred during loading: " + std::string(e.what()));
    }
    // При любой ошибке в блоке try, вектор m_rectangles не будет изменен,
    // так как мы работали с временным вектором loadedRectangles.
}
