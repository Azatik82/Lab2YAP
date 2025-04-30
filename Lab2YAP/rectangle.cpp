#include "rectangle.h"
#include <stdexcept>
#include <algorithm> // ��� std::find
#include <vector>
#include <iostream> // ��� ������ � SVG

// ����������� ������������ ������� ���������� ������
// ����� ��������� ���� ������
const std::vector<std::string> Rectangle::allowedColors = {
    "red", "green", "blue", "yellow", "black", "white", "purple", "orange"
};

// --- ��������� ---
void Rectangle::validateDimensions(double w, double h) {
    if (w <= 0 || h <= 0) {
        throw std::invalid_argument("Rectangle dimensions (width, height) must be positive.");
    }
    if (w > 1000 || h > 1000) {
        // ���������� ������ ���������� ��� ��������� ��� ������ �����������
        throw std::out_of_range("Rectangle dimensions (width, height) must not exceed 1000.");
    }
}

void Rectangle::validateColor(const std::string& color) {
    if (color.empty()) { // ������ ���� ��������
        return;
    }
    // ���� ���� � ������ ����������
    // ���������� std::find ��� �������
    if (std::find(allowedColors.begin(), allowedColors.end(), color) == allowedColors.end()) {
        throw std::invalid_argument("Invalid color specified: " + color + ". Must be one of the allowed colors or empty.");
    }
}

// --- ����������� ---
Rectangle::Rectangle(double x, double y, double w, double h, std::string color, bool notOverlap)
    : m_x(x), m_y(y), m_width(0), m_height(0), m_color(""), m_notOverlap(notOverlap) // ������������� �� ��������� ��� ��������
{
    // 1. �������� ��������
    validateDimensions(w, h);
    // 2. �������� �����
    validateColor(color);

    // ���� ��� �������� ������, ����������� ��������
    m_width = w;
    m_height = h;
    m_color = color;
    // m_x, m_y, m_notOverlap ��� ����������������
}

// --- ������ ---
void Rectangle::setColor(const std::string& color) {
    // 3. ���������� �� �� ������� ���������
    validateColor(color);
    m_color = color; // �����������, ������ ���� ��������� ������
}

// 7. �������� ��������� (�������, ��� ����� ���������)
bool Rectangle::overlaps(const Rectangle& other) const noexcept {
    // ���������, ��� ���� ������������� ����� �� �������
    if (m_x + m_width <= other.m_x || other.m_x + other.m_width <= m_x) {
        return false;
    }
    // ���������, ��� ���� ������������� ���� �������
    if (m_y + m_height <= other.m_y || other.m_y + other.m_height <= m_y) {
        return false;
    }
    // ���� �� ���� �� ������� �� ���������, ������ ���� ���������
    return true;
}

// ����� � SVG ������
void Rectangle::drawSVG(std::ostream& out) const {
    out << "  <rect x=\"" << m_x << "\" y=\"" << m_y
        << "\" width=\"" << m_width << "\" height=\"" << m_height << "\"";
    if (!m_color.empty()) {
        out << " fill=\"" << m_color << "\"";
    }
    else {
        out << " fill=\"none\" stroke=\"black\""; // ���� ����� ���, ������ ������
    }
    out << " />\n";
    // ������������ std::ostream ����� ������� ����������, ���� ����� ���������� ��� ������ ������
    if (out.fail()) {
        // � �������� ���������� ����� ���� �� ������� ����������,
        // �� � drawSVG ��� ����� ���� �������. ������� �������� � Screen::saveSVG.
        // std::cerr << "Warning: Failed writing rectangle to SVG stream." << std::endl;
    }
}