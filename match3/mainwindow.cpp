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
#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QDebug>
#include <QPixmap>
#include <vector>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // T
    // We need a graphics scene in which to draw rectangles
    scene_ = new QGraphicsScene(this);

    // T
    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(LEFT_MARGIN, TOP_MARGIN,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // T
    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a fruit is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // T
    int seed = time(0);
    randomEng_.seed(seed);
    distr_ = std::uniform_int_distribution<int>(0, NUMBER_OF_FRUITS - 1);
    distr_(randomEng_); // Wiping out the first random number (which is almost always 0)

    init_titles();

    // Configure coordinatesLineEdit ready for starting
    ui->coordinatesLineEdit->setVisible(false);
    ui->coordinatesLabel->setHidden(true);
    ui->horizontalButton->setDisabled(true);
    ui->verticalButton->setDisabled(true);
    ui->horizontalButton->setHidden(true);
    ui->verticalButton->setHidden(true);
    ui->delayCheckBox->setHidden(true);
    ui->fillingCheckBox->setHidden(true);

    connect(ui->quitButton, &QPushButton::clicked, this, &QMainWindow::close);
    connect(ui->startButton, &QPushButton::clicked, this,
            &MainWindow::startButtonClicked);

    // Timer for the game
    timer = new(QTimer);
    connect(timer, &QTimer::timeout, this, &MainWindow::timerTimeout);

    // Initialise variables for creating fruits.
    int x = 0;
    int y = 0;
    int columnTracker = 0;
    QBrush brush(Qt::black);
    QPen pen(Qt::black);

    // Add the fruit tiles to the sceneRect
    for (int rowTracker = 0; rowTracker < ROWS; ++rowTracker)
    {
        vec_.push_back({});
        for (columnTracker = 0; columnTracker < COLUMNS; ++columnTracker)
        {
            chooseFruit(columnTracker, rowTracker, brush);

            // Add new fruit to vec_
            QGraphicsRectItem* fruit = scene_->addRect(x, y, SQUARE_SIDE,
                                                       SQUARE_SIDE, pen, brush);
            vec_.at(rowTracker).push_back(fruit);
            x += SQUARE_SIDE;

        }
        y += SQUARE_SIDE;
        x = 0;
        columnTracker = 0;
    }
    ui->pointsLabel->setText("0");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

// T
void MainWindow::init_titles()
{
    // Displaying column titles, starting from A
    for(int i = 0, letter = 'A'; i < COLUMNS; ++i, ++letter)
    {
        int shift = 5;
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN + shift + i * SQUARE_SIDE,
                           TOP_MARGIN - SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }

    // Displaying row titles, starting from A
    for(int i = 0, letter = 'A'; i < ROWS; ++i, ++letter)
    {
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN - SQUARE_SIDE,
                           TOP_MARGIN + i * SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }
}

// T
void MainWindow::draw_fruit()
{
    // Vector of fruits
    const std::vector<std::string>
            fruits = {"cherries", "strawberry", "orange", "pear", "apple",
                      "bananas", "tomato", "grapes", "eggplant"};

    // Defining where the images can be found and what kind of images they are
    const std::string PREFIX(":/");
    const std::string SUFFIX(".png");

    // Converting image (png) to a pixmap
    int i = 0; // try different values in 0 <= i < fruits.size()
    std::string filename = PREFIX + fruits.at(i) + SUFFIX;
    QPixmap image(QString::fromStdString(filename));

    // Scaling the pixmap
    image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);

    // Setting the pixmap for a new label
    QLabel* label = new QLabel("test", this);
    label->setGeometry(LEFT_MARGIN + COLUMNS * SQUARE_SIDE,
                       TOP_MARGIN + ROWS * SQUARE_SIDE,
                       SQUARE_SIDE, SQUARE_SIDE);
    label->setPixmap(image);
}

void MainWindow::chooseFruit(int &x, int &y, QBrush& brush)
{
    bool fruitOk = false;

    // Loop to get new fruit colours until there is maximum two same
    // fruits in a row.
    while (!fruitOk)
    {
        fruitOk = true;
        switch (distr_(randomEng_))
        {
        case 0 :
            brush.setColor(Qt::red);
            break;
        case 1 :
            brush.setColor(Qt::blue);
            break;
        case 2 :
            brush.setColor(Qt::green);
            break;
        case 3 :
            brush.setColor(Qt::yellow);
            break;
        case 4 :
            brush.setColor(Qt::darkMagenta);
            break;
        case 5 :
            brush.setColor(Qt::cyan);
            break;
        }

        // Check if there would be three same fruits in a horizontal row
        if (x > 1)
        {
            if (vec_.at(y).at(x-1)->brush().color()
                    == brush.color() &&
               vec_.at(y).at(x-2)->brush().color()
                    == brush.color() )
            {
                fruitOk = false;
            }
        }
        // Check if there would be three same fruits in a vertical row
        if (y > 1)
        {
            if (vec_.at(y-1).at(x)->brush().color()
                    == brush.color() &&
               vec_.at(y-2).at(x)->brush().color()
                    == brush.color() )
            {
                fruitOk = false;
            }
        }

    }
}


// Get coordinates given on input and return 'true' if they're of correct form.
bool MainWindow::getCoordinates(int& column, int& row, const bool &horizontal)
{
    std::string coordinates = ui->coordinatesLineEdit->text().toLower()
            .toStdString();
    if (coordinates.length() == 2)
    {
        row = coordinates.at(1) - 'a';
        column = coordinates.at(0) - 'a';
        if (!horizontal && (0 < row  && row < ROWS) && (0 <= column &&
                                                         column < COLUMNS))
        {
            if (vec_.at(row).at(column)->brush().color() != Qt::white)
            {
                if ((vec_.at(row-1).at(column)->brush().color() != Qt::white))
                {
                    return true;
                }
            }
        }
        else if (horizontal && (0 <= row  && row < ROWS) && (0 <= column &&
                                                              column < COLUMNS-1))
        {
            if (vec_.at(row).at(column)->brush().color() != Qt::white)
            {
                if ((vec_.at(row).at(column+1)->brush().color() != Qt::white))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// Changes fruit color
void MainWindow::changeFruit(int &x, int &y, const QBrush &brush)
{
    vec_.at(y).at(x)->setBrush(brush);
}

void MainWindow::dropFruits(int &x, int &y, const bool &horizontal)
{
    QBrush brush(Qt::white);

    // Dropping if fruit row was horizontal
    if (horizontal)
    {
        for (int i = x; i < x+3; ++i)
        {
            for (int j = y; j > 0 ; --j)
            {
                changeFruit(i, j, vec_.at(j-1).at(i)->brush());
            }

            // Change the last fruit to another if player has opted so in a
            // checkBox or else empty it
            int j = 0;
            if (ui->fillingCheckBox->isChecked())
            {
                chooseFruit(i, j, brush);
            }
            changeFruit(i, j, brush);
        }
    }
    else
    {
        for (int j = y; j > 2 ; --j)
        {
            changeFruit(x, j, vec_.at(j-3).at(x)->brush());
        }
        for (int j = 0; j < 3; ++j)
        {
            // Change the last three fruits to others if player has opted so in
            //a checkBox or else empty them.
            if (ui->fillingCheckBox->isChecked())
            {
                chooseFruit(x, j, brush);
            }
            changeFruit(x, j, brush);
        }
    }
}

// Sets move buttons active when coordinates have been given
void MainWindow::on_coordinatesLineEdit_editingFinished()
{
    ui->verticalButton->setDisabled(false);
    ui->horizontalButton->setDisabled(false);
}

// Flips fruits horizontally
void MainWindow::on_horizontalButton_clicked()
{
    int row = 0;
    int column = 0;
    if (getCoordinates(column, row, true) && column < COLUMNS-1)
    {
        // Change colours of the wanted tiles ie. flip fruits
        QBrush oldColour = vec_.at(row).at(column)->brush();
        vec_.at(row).at(column)->setBrush(vec_.at(row).at(column+1)->brush());
        vec_.at(row).at(column+1)->setBrush(oldColour);

        // Set push buttons and line edit back to start mode
        ui->coordinatesLineEdit->clear();
        ui->verticalButton->setDisabled(true);
        ui->horizontalButton->setDisabled(true);

        // If filling of empty slots hasnt been chosen at the start, unable it.
        if (ui->fillingCheckBox->isEnabled() &&
                !ui->fillingCheckBox->isChecked())
        {
            ui->fillingCheckBox->setEnabled(false);
        }

        // Use delay in dropping fruits if player opted so
        if (ui->delayCheckBox->isChecked())
        {
            QTimer::singleShot(DELAY, this, SLOT(checkThree()));
            QTimer::singleShot(DELAY*2, this, SLOT(checkThree()));
            QTimer::singleShot(DELAY*3, this, SLOT(checkThree()));
        }
        else
        {
            checkThree();
            checkThree();
            checkThree();
        }
    }
}

// Flips fruits vertically
void MainWindow::on_verticalButton_clicked()
{
    int row = 0;
    int column = 0;
    if (getCoordinates(column, row, false) && row > 0)
    {
        // Change colours of the wanted tiles ie. flip fruits
        QBrush oldColour = vec_.at(row).at(column)->brush();
        vec_.at(row).at(column)->setBrush(vec_.at(row-1).at(column)->brush());
        vec_.at(row-1).at(column)->setBrush(oldColour);

        // Set push buttons and line edit back to start mode
        ui->coordinatesLineEdit->clear();
        ui->verticalButton->setDisabled(true);
        ui->horizontalButton->setDisabled(true);

        // If filling of empty slots hasnt been chosen at the start, unable it.
        if (ui->fillingCheckBox->isEnabled() &&
                !ui->fillingCheckBox->isChecked())
        {
            ui->fillingCheckBox->setEnabled(false);
        }

        // Use delay in dropping fruits if player opted so
        if (ui->delayCheckBox->isChecked())
        {
            QTimer::singleShot(DELAY, this, SLOT(checkThree()));
            QTimer::singleShot(DELAY*2, this, SLOT(checkThree()));
            QTimer::singleShot(DELAY*3, this, SLOT(checkThree()));
        }
        else
        {
            checkThree();
            checkThree();
            checkThree();
        }
    }
}

void MainWindow::checkThree()
{
    int inRow = 1;
    // Horizontal rows
    for (int row = 0; row < ROWS; ++row)
    {
        for (int column = 1; column < COLUMNS; ++column)
        {
            if (vec_.at(row).at(column)->brush().color() != Qt::white)
            {
                if (vec_.at(row).at(column)->brush().color() ==
                        vec_.at(row).at(column-1)->brush().color())
                {
                    ++inRow;
                    if (inRow == 3)
                    {
                        int x = column - 2;
                        dropFruits(x, row, true);
                        points_ += 3;
                        ui->pointsLabel->setText(QString::number(points_));
                        return;
                    }
                }
                else
                {
                    inRow = 1;
                }
            }
            else
            {
                inRow = 1;
            }

        }
    }

    inRow = 1;
    // Vertical rows
    for (int column = 0; column < COLUMNS; ++column)
    {
        for (int row = 1; row < ROWS; ++row)
        {
            if (vec_.at(row).at(column)->brush().color() != Qt::white)
            {
                if (vec_.at(row).at(column)->brush().color() ==
                        vec_.at(row-1).at(column)->brush().color())
                {
                    ++inRow;
                    if (inRow == 3)
                    {
                        dropFruits(column, row, false);
                        points_ += 3;
                        ui->pointsLabel->setText(QString::number(points_));
                    }
                }
                else
                {
                    inRow = 1;
                }
            }
            else
            {
                inRow = 1;
            }
        }
    }
}

void MainWindow::timerTimeout()
{
   secs_ += 1;
   ui->timeLabel->setText(QString::number(secs_));
}

void MainWindow::startButtonClicked()
{
    ui->timeLabel->setText(QString::number(secs_));
    timer->start(DELAY);

    ui->coordinatesLineEdit->setVisible(true);
    ui->coordinatesLabel->setHidden(false);
    ui->delayCheckBox->setHidden(false);
    ui->fillingCheckBox->setHidden(false);
    ui->startButton->setDisabled(true);
    ui->horizontalButton->setHidden(false);
    ui->verticalButton->setHidden(false);
}
