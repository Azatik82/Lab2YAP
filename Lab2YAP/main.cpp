#include <iostream>
#include <vector>
#include <stdexcept> // ��� ��������� ����������� ����������
#include "screen.h"   // �������� screen.h, ������� �������� rectangle.h

// 9. ������������ noexcept
void function_that_might_throw() {
    std::cout << "Inside function_that_might_throw(). About to throw...\n";
    throw std::runtime_error("Exception from function_that_might_throw");
}

// ��� ������� �� ������ �� ������� ����������, �� �� ������� ��� ��� �����
// �����: �������� ���������� �� noexcept ������� ������ �������� � ������ std::terminate
void noexcept_function() noexcept {
    std::cout << "Inside noexcept_function(). I promised not to throw, but...\n";
    // function_that_might_throw(); // ����������������, ����� ������� std::terminate
                                   // ��� �������� ������� ����������:
    // throw std::logic_error("Surprise from noexcept!"); // ���� �������� � std::terminate
    std::cout << "Finished noexcept_function() without throwing (this time).\n";
}


int main() {
    // 2. ���������� try/catch ����
    try {
        Screen mainScreen(800, 600);
        std::cout << "Screen created: " << mainScreen.getWidth() << "x" << mainScreen.getHeight() << std::endl;

        // --- ������������ ���������� ������������ Rectangle (�.1, �.2) ---
        std::cout << "\n--- Testing Rectangle Constructor Exceptions ---\n";
        try {
            Rectangle r1(10, 10, -50, 50); // �������� ������
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Caught expected exception (invalid_argument): " << e.what() << std::endl;
        }

        try {
            Rectangle r2(10, 10, 50, 1200); // ������� ������� ������
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Caught expected exception (out_of_range): " << e.what() << std::endl;
        }

        try {
            Rectangle r3(10, 10, 50, 50, "unknown_color"); // �������� ����
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Caught expected exception (invalid color): " << e.what() << std::endl;
        }

        // --- �������� �������� � ���������� ---
        std::cout << "\n--- Testing Successful Addition ---\n";
        Rectangle rect1(50, 50, 100, 80, "red");
        mainScreen.addRectangle(rect1);
        std::cout << "Added rect1 (red)\n";

        Rectangle rect2(200, 100, 150, 100, "blue", true); // � ������ notOverlap
        mainScreen.addRectangle(rect2);
        std::cout << "Added rect2 (blue, notOverlap=true)\n";

        // --- ������������ setColor � �������� ������ (�.3) ---
        std::cout << "\n--- Testing setColor Exception ---\n";
        try {
            Rectangle tempRect(1, 1, 1, 1);
            tempRect.setColor("magenta"); // ������������ ����
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Caught expected exception (setColor): " << e.what() << std::endl;
        }

        // --- ������������ ���������� Screen (�.6, �.7) ---
        std::cout << "\n--- Testing Screen Add Exceptions ---\n";
        try {
            Rectangle rectOutOfBounds(750, 550, 100, 100); // ������� �� �������
            mainScreen.addRectangle(rectOutOfBounds);
        }
        catch (const ScreenError& e) {
            std::cerr << "Caught expected ScreenError (out of bounds): " << e.what() << std::endl;
            std::cerr << "  Rectangle details: w=" << e.getRectangle().getWidth() << ", h=" << e.getRectangle().getHeight() << std::endl;
        }

        try {
            // �������� �������� ������������� � notOverlap=true, ������� ������������� �� rect2
            Rectangle rectOverlap(220, 120, 50, 50, "yellow", true); // ������������� �� rect2
            mainScreen.addRectangle(rectOverlap);
        }
        catch (const ScreenError& e) {
            std::cerr << "Caught expected ScreenError (overlap): " << e.what() << std::endl;
            std::cerr << "  Rectangle details: x=" << e.getRectangle().getX() << ", y=" << e.getRectangle().getY() << std::endl;
        }

        // �������� ���������� ����� (��� ���������)
        Rectangle rectNoOverlap(400, 100, 50, 50, "green", true);
        mainScreen.addRectangle(rectNoOverlap);
        std::cout << "Added rectNoOverlap (green, notOverlap=true)\n";

        // --- ������������ tryAddRectangle (�.8) ---
        std::cout << "\n--- Testing tryAddRectangle ---\n";
        Rectangle rectOutOfBoundsTry(10, 1000, 20, 20); // �� ���������
        if (!mainScreen.tryAddRectangle(rectOutOfBoundsTry)) {
            std::cout << "tryAddRectangle failed as expected (out of bounds). Error: " << mainScreen.getLastError() << std::endl;
        }

        Rectangle rectOverlapTry(230, 130, 40, 40, "purple", true); // ������������� �� rect2
        if (!mainScreen.tryAddRectangle(rectOverlapTry)) {
            std::cout << "tryAddRectangle failed as expected (overlap). Error: " << mainScreen.getLastError() << std::endl;
        }

        Rectangle rectValidTry(500, 300, 60, 60, "orange");
        if (mainScreen.tryAddRectangle(rectValidTry)) {
            std::cout << "tryAddRectangle succeeded for rectValidTry (orange)." << std::endl;
        }
        else {
            std::cerr << "tryAddRectangle failed unexpectedly for rectValidTry. Error: " << mainScreen.getLastError() << std::endl;
        }


        // --- ������������ ���������� � ���� (�.4) ---
        std::cout << "\n--- Testing SVG Save ---\n";
        try {
            mainScreen.saveSVG("output.svg");
            std::cout << "Successfully saved screen to output.svg" << std::endl;
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Caught exception during SVG save: " << e.what() << std::endl;
        }

        // ������� ��������� � ������������ ����� (����� ����������� ���� �������������� ��� ������� ������)
        // � Linux/macOS: "/root/cant_write_here.svg"
        // � Windows: "C:\\Windows\\cant_write_here.svg" (���� ����� ����������)
        // ����������������, ����� �� �������� ������ �� ���������
        /*
        try {
             mainScreen.saveSVG("/root/cant_write_here.svg"); // ������ ��� Linux
        } catch (const std::runtime_error& e) {
            std::cerr << "Caught expected exception trying to save to restricted location: " << e.what() << std::endl;
        }
        */

        // --- ������������ ������ �� ����� (����� 2) ---
        std::cout << "\n--- Testing Load From File ---\n";
        // �������� ���� input.txt �� ��������� ����������:
        /* input.txt:
            100 100 50 50 red
            200 250 80 60 blue
            300 300 70 70
            350 350 90 90 orange
            50 400 40 40 # ���������� orange �� ���������� ������
            # ������ ������ ��� ������ � ������������ ������ �������������� getline
            900 500 30 30 white # ������� �� ������� 800x600
        */
        // �������� ���� input_invalid_format.txt:
        /* input_invalid_format.txt:
            100 100 50 50 red
            200 250 80 oops blue # ������ �������
        */
        // �������� ���� input_invalid_rect.txt:
        /* input_invalid_rect.txt:
            100 100 50 50 red
            200 200 -30 30 black # �������� ������
        */

        size_t rectCountBeforeLoad = mainScreen.getRectangles().size();
        try {
            std::cout << "Attempting to load from 'input.txt'..." << std::endl;
            mainScreen.loadFromFile("input.txt"); // ������ ������� ������ ��-�� ���������� ��������������
            std::cerr << "ERROR: Load from 'input.txt' should have failed but didn't!" << std::endl;

        }
        catch (const FileParseError& e) {
            std::cerr << "Caught expected FileParseError: " << e.what() << std::endl;
            std::cerr << "  File: " << e.getFilename() << ", Line: " << e.getLineNumber() << std::endl;
            // ���������, ��� ���������� ��������������� �� ���������� (������� ��������)
            if (mainScreen.getRectangles().size() == rectCountBeforeLoad) {
                std::cout << "  Screen state unchanged (strong guarantee respected)." << std::endl;
            }
            else {
                std::cerr << "  ERROR: Screen state was modified despite error!" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Caught unexpected exception during load: " << e.what() << std::endl;
        }

        // ���������� � ������� ������� (input_invalid_format.txt, input_invalid_rect.txt)
        // ���������, ��� FileParseError ������� ��������� � ��������� Screen �� ��������.

        // ������ �������� �������� (���� input_correct.txt �������� ������ �������� ������)
        // �������� input_correct.txt:
        /* input_correct.txt
            400 400 50 50 green
            500 500 60 60
            600 200 40 80 yellow
        */
        rectCountBeforeLoad = mainScreen.getRectangles().size();
        try {
            std::cout << "Attempting to load from 'input_correct.txt'..." << std::endl;
            mainScreen.loadFromFile("input_correct.txt");
            std::cout << "Successfully loaded from 'input_correct.txt'." << std::endl;
            std::cout << "New rectangle count: " << mainScreen.getRectangles().size() << std::endl;
            // �������� ��������� ����� ��������
            mainScreen.saveSVG("output_after_load.svg");
            std::cout << "Saved screen state after load to output_after_load.svg" << std::endl;

        }
        catch (const std::exception& e) {
            std::cerr << "Caught unexpected exception during load from 'input_correct.txt': " << e.what() << std::endl;
        }


        // --- ������������ noexcept (�.9) ---
        std::cout << "\n--- Testing noexcept Function ---\n";
        // ����� �������, ������� �������������� �� ������� ����������
        noexcept_function();

        // ����� �������, ���������� noexcept, �� ������� ����� ������� ����������
        // ������ ��� �������� � std::terminate, ��������� �������� ����������.
        // ����������� ����� ���������� ����������� catch �� ���������.
        std::cout << "Calling noexcept_function that might throw (uncomment inside to test std::terminate)...\n";
        // try {
        //     noexcept_function(); // ���� ������ noexcept_function() ����� ������� ����������, ��������� ������ �����
        // } catch (...) {
        //     std::cerr << "This catch block will likely NOT be reached if noexcept_function throws.\n";
        // }


    }
    catch (const ScreenError& e) {
        std::cerr << "\nUnhandled ScreenError in main: " << e.what() << std::endl;
        std::cerr << "  Associated Rectangle: w=" << e.getRectangle().getWidth() << ", h=" << e.getRectangle().getHeight() << std::endl;
        return 1; // ��� ������
    }
    catch (const FileParseError& e) {
        std::cerr << "\nUnhandled FileParseError in main: " << e.what() << std::endl;
        std::cerr << "  File: " << e.getFilename() << ", Line: " << e.getLineNumber() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "\nUnhandled standard exception in main: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "\nUnhandled unknown exception in main!" << std::endl;
        return 1;
    }

    std::cout << "\nProgram finished successfully." << std::endl;
    return 0; // �������� ����������
}
