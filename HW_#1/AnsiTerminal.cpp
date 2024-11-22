#include "AnsiTerminal.h"
#include <iostream>
#include <unistd.h>   // For read()
#include <termios.h>  // For terminal control

AnsiTerminal::AnsiTerminal() {
    // Save the original terminal settings
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;

    // Disable canonical mode and echo for real-time input reading
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

AnsiTerminal::~AnsiTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void AnsiTerminal::printAt(int row, int col, const std::string &text) {
    std::cout << "\033[" << row << ";" << col << "H" << text << std::flush;
}

void AnsiTerminal::printInvertedAt(int row, int col, const std::string &text) {
    std::cout << "\033[" << row << ";" << col << "H\033[7m" << text << "\033[0m" << std::flush;
}

void AnsiTerminal::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

char AnsiTerminal::getKeystroke() {
    char ch;
    read(STDIN_FILENO, &ch, 1);  
    return ch;
}

char AnsiTerminal::getSpecialKey() {
    char ch = getKeystroke();

    // Check for escape sequences
    if (ch == '\033') {  // Escape character
        char next_ch;
        if (read(STDIN_FILENO, &next_ch, 1) == 1 && next_ch == '[') {
            char arrow_key;
            if (read(STDIN_FILENO, &arrow_key, 1) == 1) {
                switch (arrow_key) {
                    case 'A': return (char)1; // Up arrow
                    case 'B': return (char)2; // Down arrow
                    case 'C': return (char)3; // Right arrow
                    case 'D': return (char)4; // Left arrow
                }
            }
        }
        // If it was just ESC alone (not an arrow), return ESC
        return '\033';
    }

    // Return the character as-is if it's a regular key
    return ch;
}

bool AnsiTerminal::isArrowKey(const char ch) {
    return ch == (char)1 || ch == (char)2 || ch == (char)3 || ch == (char)4;
}
