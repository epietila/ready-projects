/* Match 3
 * This is a game implemented for a C++ programming course in 12/2020. The 
 * idea of the game is to match three tiles of the same colour as long as possible. 
 * The game board size is ROWS x COLUMNS and the player always swaps two tiles on the board.
 * Tiles cannot be moved over the board limits. Player can choose if the removed
 * tiles will be replaced or not. 
 *
 * Contributors:
 * Essi Pietilä @epietila
 * Maarit Harsu, course teacher 
 *
 * NB: Sections implemented by Maarit Harsu, ie. the template,  will be marked
 * by a letter T in the beginning of the comment. 
 *
 * Rules and more documentation in Finnish in instructions.txt
 * */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <vector>
#include <deque>
#include <random>
#include <QTimer>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

// T
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_coordinatesLineEdit_editingFinished();

    void on_horizontalButton_clicked();

    void on_verticalButton_clicked();

    // Checks if there are three same fruits in a row after a flip.
    void checkThree();

    // Updates time label every second
    void timerTimeout();

    // Starts game by enabling flips
    void startButtonClicked();

private:
    Ui::MainWindow *ui;

    // T
    // Scene for the game grid
    QGraphicsScene* scene_;

    // T (values changed by Essi)
    // Margins for the drawing area (the graphicsView object)
    const int TOP_MARGIN = 160;
    const int LEFT_MARGIN = 120;

    // T (values defined by Essi)
    // Size of a square containing a fruit
    const int SQUARE_SIDE = 20;
    // Number of vertical cells (places for fruits)
    const int ROWS = 6;
    // Number of horizontal cells (places for fruits)
    const int COLUMNS = 8;

    // T
    // Constants describing scene coordinates
    const int BORDER_UP = 0;
    const int BORDER_DOWN = SQUARE_SIDE * ROWS;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = SQUARE_SIDE * COLUMNS;

    // The delay for dropping fruits in milliseconds
    const int DELAY = 1000;

    // Timer for keeping track of time used in the game
    QTimer* timer;

    // Seconds used
    double secs_ = 0;

    // Point counter
    int points_ = 0;

    // Constants for different fruits and the number of them
    // Add/remove fruits as you wish, or you can remove the whole enum type
    // Actually only the value NUMBER_OF_FRUITS is needed in the
    // template code
    enum Fruit_kind {PLUM,
                     STRAWBERRY,
                     APPLE,
                     LEMON,
                     BLUEBERRY,
                     ORANGE,
                     NUMBER_OF_FRUITS};

    // T
    // For randomly selecting fruits for the grid
    std::default_random_engine randomEng_;
    std::uniform_int_distribution<int> distr_;

    // T
    // Writes the titles for the grid rows and columns
    void init_titles();

    // T
    // Draws a single fruit near the right bottom corner of the grid
    // At the moment, this function is not called in the template,
    // but try to do so, if you want to use real fruits instead of rectangles.
    void draw_fruit();

    /* Adds fruit to the game
     * Parameteres:
     *  x: x coordinate for the fruit to be added
     *  y: y coordinate for the fruit to be added
     */
    void chooseFruit(int& x, int& y, QBrush& brush);


    /* Gets coordinates from input of line edit and returns 'true' if they are of
    * correct format.
    * Parameters:
    *   column: variable to transmit colun number according to the coordinates.
    *   row: variable to transmit row number according to the coordinates.
    * Return: 'true' if coordinates are valid
    */
    bool getCoordinates(int& column, int& row, const bool &horizontal);

    /* Gets coordinates from input of line edit and returns 'true' if they are of
    * correct format.
    * Parameters:
    *   x: column number for the fruit to be changed
    *   y: row number for the fruit to be changed
    *   brush: brush for defining the new fruit
    */
    void changeFruit(int &x, int &y, const QBrush &brush);
    
    /* Drops fruits down to fill empty spots
    * Parameters:
    *   x: column number for the first fruit to be dropped
    *   y: row number for the first fruit to be dropped
    *   horizontal:
    */
    void dropFruits(int &x, int &y, const bool &horizontal);

    // Vector containing pointers to the fruits
    std::vector<std::vector<QGraphicsRectItem*>> vec_ = {};
};
#endif // MAINWINDOW_HH
