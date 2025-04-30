#include "screen.h"
#include <fstream>   // ��� std::ofstream, std::ifstream
#include <sstream>   // ��� std::stringstream (������� �����)
#include <iostream>  // ��� std::cerr
#include <vector>    // ��� ���������� �������� � loadFromFile
#include <limits>    // ��� numeric_limits

Screen::Screen(double width, double height) noexcept
    : m_width(width > 0 ? width : 100.0), // ������� �������� � ������������
    m_height(height > 0 ? height : 100.0),
    m_lastError("") {}

// ��������������� ������� �������� ���������
bool Screen::checkOverlap(const Rectangle& rect) const noexcept {
    if (rect.getNotOverlap()) { // ��������� ������ ���� ���������� ����
        for (const auto& existingRect : m_rectangles) {
            if (rect.overlaps(existingRect)) {
                return true; // ��������� �������
            }
        }
    }
    return false; // ��������� ��� (��� ���� �� ����������)
}

// ��������������� ������� ��������� ����������
bool Screen::checkRectanglePlacement(const Rectangle& rect, bool throwOnError) {
    // 6. �������� ������ �� ������� ������
    if (rect.getX() < 0 || rect.getY() < 0 ||
        rect.getX() + rect.getWidth() > m_width ||
        rect.getY() + rect.getHeight() > m_height)
    {
        m_lastError = "Rectangle is out of screen bounds.";
        if (throwOnError) {
            throw ScreenError(m_lastError, rect); // ������� ScreenError
        }
        return false;
    }

    // 7. �������� ���������
    if (checkOverlap(rect)) {
        m_lastError = "Rectangle with notOverlap=true overlaps with an existing rectangle.";
        if (throwOnError) {
            throw ScreenError(m_lastError, rect); // ������� ScreenError
        }
        return false;
    }

    m_lastError = ""; // ������ ���
    return true;
}


void Screen::addRectangle(const Rectangle& rect) {
    // ��������� ��� �������� ����� ������������ ������� ��� ������� ��������
    checkRectanglePlacement(rect, true); // true - ������� ���������� ��� ������

    // ���� �������� ������ ������� (�� ���� ������� ����������), ���������
    // std::vector::push_back ������������ ������� �������� ��� �� ����
    // (���� �� ������� std::bad_alloc)
    m_rectangles.push_back(rect);
}

bool Screen::tryAddRectangle(const Rectangle& rect) noexcept {
    // 8. �������� ��������, �� �� ������� ���������� (false)
    // ��������� �������� ��������� � m_lastError ���� ���-�� �� ���
    if (checkRectanglePlacement(rect, false)) {
        try {
            // ���������� ��� ��� ����� ������� std::bad_alloc, ���������� �� ������ ������
            m_rectangles.push_back(rect);
            m_lastError = ""; // ���� ���������� ������ ��� ������
            return true;
        }
        catch (const std::bad_alloc&) {
            m_lastError = "Memory allocation failed while adding rectangle.";
            // � �������� ���������� �������� ����� ����������� ��� ����������
            return false;
        }
        catch (...) {
            // ����������� ���������� (������������ �� push_back)
            m_lastError = "An unexpected error occurred during addition.";
            return false;
        }
    }
    // checkRectanglePlacement ��� ���������� m_lastError
    return false;
}

std::string Screen::getLastError() const noexcept {
    return m_lastError;
}

void Screen::saveSVG(const std::string& filename) const {
    // 4. ���������� std::ofstream � ��������� ������
    std::ofstream outFile;

    // �������� ��������� ���������� ��� ������ outFile ��� �������
    outFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    try {
        outFile.open(filename); // ����� ������� ����������, ���� ���� �� ����� ���� ������/������

        // ��������� SVG
        outFile << "<svg width=\"" << m_width << "\" height=\"" << m_height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
        outFile << "  <rect width=\"100%\" height=\"100%\" fill=\"lightgrey\" />\n"; // ���

        // ������ ��� ��������������
        for (const auto& rect : m_rectangles) {
            rect.drawSVG(outFile); // ����� drawSVG ��� ����� � �����
            // ������ ������ ����� ������� ���������� ��-�� .exceptions()
        }

        // ����� SVG
        outFile << "</svg>\n";

        // outFile ��������� ������������� ��� ������ �� ����� try (RAII)
        // ���� �� �� exceptions(), ����� ���� �� ��������� outFile.fail() ����� ������ ������

    }
    catch (const std::ios_base::failure& e) {
        // ������������� ���������� ������ � ������� ���� � �����������
        // ���������� ����������� runtime_error, �.�. �� ��������� ������� Rectangle
        throw std::runtime_error("Error saving to SVG file '" + filename + "': " + e.what());
    }
    // ������ ���������� (��������, std::bad_alloc) ����������� ����
}


// --- ����� 2: ������ �� ����� ---
void Screen::loadFromFile(const std::string& filename) {
    std::ifstream inFile;
    // �������� ��������� ���������� ��� ������ ������, ����� ����� � �.�.
    inFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // �� �������� eofbit, �.�. ����� ������ � �����

    std::vector<Rectangle> loadedRectangles; // ��������� ������ ��� ������� ��������
    int lineNumber = 0;
    std::string currentLine;
    std::string lastValidColor = ""; // ���������� ��������� ��������� ����

    try {
        inFile.open(filename); // ����� ������� ����������

        while (std::getline(inFile, currentLine)) {
            lineNumber++;
            std::stringstream ss(currentLine);
            double cx, cy, w, h;
            std::string colorStr = ""; // ���� ��� ������� ������

            // �������� ��������� 4 ������������ �����
            if (!(ss >> cx >> cy >> w >> h)) {
                // ������ ������� ������ (������������ �����)
                throw FileParseError(filename, lineNumber, "Invalid format - expected 'cx cy w h [color]'");
            }

            // �������� ��������� �������������� ����
            if (ss >> colorStr) {
                lastValidColor = colorStr; // ��������� ��������� �������� ����
            }
            else {
                // ���� ���� �� ������, ���������� ��������� �����������
                colorStr = lastValidColor;
            }

            // ���������, �� �������� �� ������ ������ � ������
            std::string remaining;
            if (ss >> remaining) {
                throw FileParseError(filename, lineNumber, "Extra data found on line after expected fields.");
            }

            // --- �������� � �������� �������������� (������ ���������� ��� �������) ---
             // ���������� �� ����� - ����� (cx, cy), ������������ � ����� ������� ���� (x, y)
            double x = cx - w / 2.0;
            double y = cy - h / 2.0;

            // �������� ������� ������������� �� ��������� ������
            // ����������� Rectangle �������� �������� �������� � �����
            loadedRectangles.emplace_back(x, y, w, h, colorStr, false); // ��������� � notOverlap=false �� ���������

        } // ����� while getline

        // --- �������� ���� ����������� ��������������� (������� � ���������) ---
        // ��������� ������ ����� ������������� ������������ ������ ������ � ��� ������������ �� ������
        for (const auto& newRect : loadedRectangles) {
            // ��������� ����� �� ������� � ��������� �� ��� ������������ ��������������
            if (newRect.getX() < 0 || newRect.getY() < 0 ||
                newRect.getX() + newRect.getWidth() > m_width ||
                newRect.getY() + newRect.getHeight() > m_height)
            {
                // �� ���������� ScreenError, ��� ��� ������ ������� � ������
                throw FileParseError(filename, -1, "Rectangle loaded from file is out of screen bounds."); // -1 �.�. ����� ������ ��� �� ��� �����
            }
            // ����������: �� ������� �� ����������� ��������� ��������� ����� ���������������� �� ������ �����.
            // ���� ��� �����, �������� ����� ���� ��� �������� newRect ������ ������ � loadedRectangles.
        }


        // --- ������� ��������: ���� ��� ��������� � ��������� ������� ---
        // ��������� ��� ����������� �������������� � �������� ������ ������ ������
        m_rectangles.insert(m_rectangles.end(), loadedRectangles.begin(), loadedRectangles.end());


    }
    catch (const std::ios_base::failure& e) {
        // ������ ������ ����� (���� �� ������, ������ ������� � �.�.)
        if (inFile.eof() && lineNumber == 0 && loadedRectangles.empty()) {
            // ���� ������ - ��� �� ������ ������, ������ ������ �� ������
        }
        else {
            throw FileParseError(filename, lineNumber, "File read error: " + std::string(e.what()));
        }
    }
    catch (const std::invalid_argument& e) {
        // ������ ��������� �� Rectangle (�������, ����)
        throw FileParseError(filename, lineNumber, "Invalid rectangle data: " + std::string(e.what()));
    }
    catch (const std::out_of_range& e) {
        // ������ ��������� �� Rectangle (������� > 1000)
        throw FileParseError(filename, lineNumber, "Invalid rectangle data: " + std::string(e.what()));
    }
    catch (const FileParseError& e) {
        // ������ ������������� ���� ������ ��������
        throw;
    }
    catch (const std::exception& e) {
        // ������ ��������� ����������� ���������� (��������, bad_alloc ��� �������� ���������� �������)
        throw FileParseError(filename, lineNumber, "An unexpected error occurred during loading: " + std::string(e.what()));
    }
    // ��� ����� ������ � ����� try, ������ m_rectangles �� ����� �������,
    // ��� ��� �� �������� � ��������� �������� loadedRectangles.
}
