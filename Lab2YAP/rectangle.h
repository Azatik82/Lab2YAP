#pragma once // ������ �� �������� ���������

#include <string>
#include <vector>
#include <stdexcept> // ��� ����������� ����������
#include <ostream>   // ��� drawSVG

class Rectangle {
public:
    // --- ������������ ---
    // ��������� ���� notOverlap � ���� �� ���������
    Rectangle(double x, double y, double w, double h, std::string color = "", bool notOverlap = false);

    // --- ��������� (�����������, ����� ������������ � ������������ � setColor) ---
    static void validateDimensions(double w, double h);
    static void validateColor(const std::string& color);

    // --- ������ ---
    void setColor(const std::string& color);
    bool overlaps(const Rectangle& other) const noexcept; // �������� ���������
    void drawSVG(std::ostream& out) const; // ����� � SVG �����

    // --- ������� (�������� noexcept, �.�. �� ������� ����������) ---
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
    bool m_notOverlap; // ���� ��� �������� ���������

    // ����������� ������ ���������� ������
    static const std::vector<std::string> allowedColors;
};
