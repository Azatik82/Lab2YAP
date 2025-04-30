#pragma once

#include "rectangle.h"
#include <vector>
#include <string>
#include <stdexcept> // ��� std::runtime_error
#include <fstream>   // ��� ������ � �������

// 5. ���� ����� ���������� ScreenError
class ScreenError : public std::runtime_error {
public:
    // ����������� ��������� ��������� � ����� ��������������
    ScreenError(const std::string& message, const Rectangle& rect)
        : std::runtime_error(message), m_rectangle(rect) {} // �������� ������

    // ����� ��� ��������� ������� � ������������ ��������������
    const Rectangle& getRectangle() const noexcept {
        return m_rectangle;
    }

    // ����� �������������� what() ��� ���������� ���������� � ��������������,
    // �� ����������� ������ - ������������ getter.

private:
    Rectangle m_rectangle; // ������ ����� �������
};

// ����� "������"
class Screen {
public:
    Screen(double width, double height) noexcept; // ����������� �� ������� ����������

    // ���������� �������������� (������� ����������)
    void addRectangle(const Rectangle& rect);

    // 8. "����������" ���������� (�� ������� ��������� ����������)
    bool tryAddRectangle(const Rectangle& rect) noexcept; // ��������� noexcept
    std::string getLastError() const noexcept; // �������� ����� ��������� ������

    // 4. ���������� � SVG � ���������� ������ �����
    void saveSVG(const std::string& filename) const;

    // ����� 2: ������ �� �����
    void loadFromFile(const std::string& filename);

    // ������� (�� ������� ����������)
    double getWidth() const noexcept { return m_width; }
    double getHeight() const noexcept { return m_height; }
    const std::vector<Rectangle>& getRectangles() const noexcept { return m_rectangles; }

private:
    double m_width;
    double m_height;
    std::vector<Rectangle> m_rectangles;
    std::string m_lastError; // ��� tryAddRectangle

    // ��������������� ������� ��� �������� ����� �����������
    // ���������� true, ���� �������� ��������, ����� false (� ������������� m_lastError)
    // ��� ������� ����������, ���� throwOnError = true
    bool checkRectanglePlacement(const Rectangle& rect, bool throwOnError);

    // ��������������� ������� ��� �������� ���������
    // ���������� true, ���� ��������� ����, ����� false
    bool checkOverlap(const Rectangle& rect) const noexcept;
};

// ��������� ���������� ��� ������ ������ ����� (����� 2)
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
