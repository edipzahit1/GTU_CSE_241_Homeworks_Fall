#ifndef ANSI_TERMINAL_H
#define ANSI_TERMINAL_H

#include <string>
#include <termios.h>

/**
 * @class AnsiTerminal
 * @brief Provides an interface for ANSI terminal manipulation, including text printing, screen clearing, and key detection.
 */
class AnsiTerminal {
public:
    /**
     * @brief Constructor: Sets up the terminal for capturing keystrokes.
     */
    AnsiTerminal();

    /**
     * @brief Destructor: Restores the terminal settings to the original state.
     */
    ~AnsiTerminal();

    /**
     * @brief Print text at a specified row and column.
     * @param row The row position (0-based index).
     * @param col The column position (0-based index).
     * @param text The text to be printed.
     */
    void printAt(int row, int col, const std::string &text);

    /**
     * @brief Print text with inverted background at a specified row and column.
     * @param row The row position (0-based index).
     * @param col The column position (0-based index).
     * @param text The text to be printed with inverted colors.
     */
    void printInvertedAt(int row, int col, const std::string &text);

    /**
     * @brief Clear the terminal screen.
     */
    void clearScreen();

    /**
     * @brief Get a single keystroke from the terminal.
     * @return The character representing the keystroke.
     */
    char getKeystroke();

    /**
     * @brief Get the arrow key or special key input.
     *        Returns 'U', 'D', 'L', 'R' for Up, Down, Left, Right, respectively,
     *        or detects other key combinations such as Alt+Key, Ctrl+Key, etc.
     * @return A character representing the detected special key.
     */
    char getSpecialKey();

    /**
     * @brief Check if a character corresponds to an arrow key.
     * @param ch The character to check.
     * @return True if the character represents an arrow key; false otherwise.
     */
    bool isArrowKey(const char ch);

private:
    struct termios original_tio; ///< Holds the original terminal settings.
};

#endif // ANSI_TERMINAL_H
