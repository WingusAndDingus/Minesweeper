//#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "Board.cpp"
using namespace std;

/// FIND A WAY TO DETECT WHETHER A SPRITE HAS BEEN CLICKED
bool buttonPressed(sf::Sprite button, sf::Vector2i coords) {
    button.setOrigin(button.getLocalBounds().width / 2.0f, button.getLocalBounds().height / 2.0f);
    sf::Vector2f floatCoords(static_cast<float>(coords.x), static_cast<float>(coords.y));
    if (button.getGlobalBounds().contains(floatCoords)) {
        return true;
    }
    return false;
}

//Calculate total seconds based on Min and sec
int calculateSecs(int mins, int secs) {
    return (mins * 60) + secs;
}
//Calculate the opposite
string calculateString(int actualtime) {
    string result = "";
    int min = actualtime / 60;
    int secs = actualtime % 60;
    if (min < 10) {
        result += "0" + to_string(min) + ":";
    }
    else {
        result += to_string(min) + ":";
    }
    if (secs < 10) {
        result += "0" + to_string(secs);
    }
    else {
        result += to_string(secs);
    }
    return result;
}
// Create struct that holds each leaderboard line info
struct LeaderPerson
{
    int actualTime;
    string timeMin;
    string timeSec;
    string name;
    LeaderPerson(string line) {
        timeMin.push_back(line[0]);
        timeMin.push_back(line[1]);
        timeSec.push_back(line[3]);
        timeSec.push_back(line[4]);
        int min = stoi(timeMin);
        int sec = stoi(timeSec);
        actualTime = calculateSecs(min, sec);
        //Update Name
        for (int i = 6; i < line.size(); i++) {
            name.push_back(line[i]);
        }
    }
};


void setText(sf::Text& text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void setButton(sf::Sprite& sprite, float x, float y) {
    y = y + 32;
    sf::FloatRect spriteRect = sprite.getLocalBounds();
    sprite.setOrigin(spriteRect.left + spriteRect.width / 2.0f, spriteRect.top + spriteRect.height / 2.0f);
    sprite.setPosition(sf::Vector2f(x, y));
}

void drawCursor(sf::RenderWindow& window, sf::Text& text, int cursorPos) {
    sf::Text cursor;
    sf::Font font;
    font.loadFromFile("font.ttf");
    cursor.setFont(font);
    cursor.setString("|");
    cursor.setCharacterSize(18);
    cursor.setFillColor(sf::Color::Yellow); 
    cursor.setStyle(sf::Text::Bold);

    sf::Vector2f cursorPost;
    if (cursorPos == 0) {
        cursorPost = text.getPosition();
    }
    else {
        cursorPost = text.findCharacterPos(cursorPos);
    }

    cursor.setPosition(cursorPost.x, cursorPost.y);
    window.draw(cursor);
}

//Check Seconds

int main() {
    GameStatus gameStatus = GameStatus::Normal;
    sf::Time prevTime;
    prevTime = sf::seconds(0.0);
    sf::Time currTime;
    sf::Time paused1;
    sf::Time paused2;
    unsigned int GameTime = 0;
    unsigned int totalPaused = 0;
    unsigned int secs = 0;
    unsigned int mins = 0;
    unsigned int rowCount;
    unsigned int colCount;
    int index = -1;
    int numMines;
    bool once = false;
    bool debugged = false;
    bool paused = false;
    //Gather board Configuration from the file "config.cfg"
    ifstream configFile("config.cfg");
    if (!configFile.is_open()) {
        cerr << "Error: Board Configuration File Was Unabled To Be Opened." << endl;
        return 1;
    }
    string line;
    getline(configFile, line);
    colCount = stoi(line);
    getline(configFile, line);
    rowCount = stoi(line);
    getline(configFile, line);
    numMines = stoi(line);
    configFile.close();
    const int startMines = numMines;
    //Gather leaderboard info from file "leaderboard.txt"
    //Let's make it so it doesn't have to already have 5 names
    fstream leaderfile("leaderboard.txt");
    vector<LeaderPerson> leaderInfo;
    vector<string> lines;
    while (getline(leaderfile, line)) {
        LeaderPerson person(line);
        lines.push_back(line);
        leaderInfo.push_back(person);
    }
    leaderfile.close();

    string nameString;
    int Pos = 0;
    int width = colCount * 32;
    int lWidth = colCount * 16;
    int height = (rowCount * 32) + 100;
    int lHeight = (rowCount * 16) + 50;

    //sf::RenderWindow window2(sf::VideoMode(800, 600), "SFML window", sf::Style::Close);
    //sf::CircleShape shape(100.f);
    //shape.setFillColor(sf::Color::Red);
    // 
    //Buttons
    sf::Sprite happy;
    sf::Sprite debug;
    sf::Sprite pause;
    sf::Sprite leaderboard;
    sf::Sprite counter1;
    sf::Sprite counter2;
    sf::Sprite counter3;
    sf::Sprite neg;
    sf::Sprite timerMin1;
    sf::Sprite timerMin2;
    sf::Sprite timerSec1;
    sf::Sprite timerSec2;
    //Timer Minutes and Seconds
    sf::Texture happytext;
    sf::Texture debugtext;
    sf::Texture pauseText;
    sf::Texture playText;
    sf::Texture leaderText;
    sf::Texture digitsText;
    sf::Texture victoryText;
    sf::Texture lossText;
    sf::Texture digit;
    sf::IntRect rectNeg(210, 0, 21, 32);
    sf::IntRect rect(0, 0, 21, 32);
    sf::IntRect rect1(21, 0, 21, 32);
    sf::IntRect rect2(42, 0, 21, 32);
    sf::IntRect rect3(63, 0, 21, 32);
    sf::IntRect rect4(84, 0, 21, 32);
    sf::IntRect rect5(105, 0, 21, 32);
    sf::IntRect rect6(126, 0, 21, 32);
    sf::IntRect rect7(147, 0, 21, 32);
    sf::IntRect rect8(168, 0, 21, 32);
    sf::IntRect rect9(189, 0, 21, 32);

    happytext.loadFromFile("images/face_happy.png");
    debugtext.loadFromFile("images/debug.png");
    pauseText.loadFromFile("images/pause.png");
    playText.loadFromFile("images/play.png");
    leaderText.loadFromFile("images/leaderboard.png");
    digitsText.loadFromFile("images/digits.png");
    victoryText.loadFromFile("images/face_win.png");
    lossText.loadFromFile("images/face_lose.png");
    //Harder Stuff
    digit.loadFromFile("images/digits.png");


    happy.setTexture(happytext);
    debug.setTexture(debugtext);
    pause.setTexture(pauseText);
    leaderboard.setTexture(leaderText);
    //Harder Stuff
    counter1.setTexture(digit);
    counter1.setTextureRect(rect);
    counter2.setTexture(digit);
    counter2.setTextureRect(rect);
    counter3.setTexture(digit);
    counter3.setTextureRect(rect);
    neg.setTexture(digit);
    neg.setTextureRect(rectNeg);
    timerMin1.setTexture(digit);
    timerMin1.setTextureRect(rect);
    timerMin2.setTexture(digit);
    timerMin2.setTextureRect(rect);
    timerSec1.setTexture(digit);
    timerSec1.setTextureRect(rect);
    timerSec2.setTexture(digit);
    timerSec2.setTextureRect(rect);

    setButton(happy, (colCount / 2.0) * 32 - 32, 32 * (rowCount + 0.5));
    setButton(debug, (colCount) * 32.0 - 304, 32 * (rowCount + 0.5));
    setButton(pause, (colCount) * 32.0 - 240, 32 * (rowCount + 0.5));
    setButton(leaderboard, (colCount) * 32.0 - 176, 32 * (rowCount + 0.5));
    //Harder Stuff
    setButton(counter1, 33, 32 * (rowCount + 0.5));
    setButton(counter2, 33 + 21, 32 * (rowCount + 0.5));
    setButton(counter3, 33 + 42, 32 * (rowCount + 0.5));
    setButton(neg, 12, 32 * (rowCount + 0.5));
    setButton(timerMin1, (colCount) * 32 - 97, 32 * (rowCount + 0.5));
    setButton(timerMin2, (colCount) * 32 - 97 + 21, 32 * (rowCount + 0.5));
    setButton(timerSec1, colCount * 32 - 54, 32 * (rowCount + 0.5));
    setButton(timerSec2, colCount * 32 - 54 + 21, 32 * (rowCount + 0.5));

    sf::Font font;
    sf::Text title;
    sf::Text input;
    sf::Text name;
    if (!font.loadFromFile("font.ttf")) {
        cerr << "Error loading Font!" << endl;
        return 1;
    }
    title.setFont(font);
    input.setFont(font);
    name.setFont(font);
    title.setString("WELCOME TO MINESWEEPER!");
    input.setString("Enter your name:");

    title.setFillColor(sf::Color::White);
    input.setFillColor(sf::Color::White);
    name.setFillColor(sf::Color::Yellow);
    title.setCharacterSize(24);
    input.setCharacterSize(20);
    name.setCharacterSize(18);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    input.setStyle(sf::Text::Bold);
    name.setStyle(sf::Text::Bold);
    setText(title, (width) / 2.0, (height) / 2.0 - 150.0);
    setText(input, (width) / 2.0, (height) / 2.0 - 75.0);
    setText(name, (width) / 2.0, (height) / 2.0 - 45.0);

    //Set up Welcome Window
    sf::RenderWindow welcome(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close | sf::Style::Titlebar);
    //welcome.

    //Set up Game Window
    sf::RenderWindow game;
    //game.create(sf::VideoMode(width, height), "Game", sf::Style::Close | sf::Style::Titlebar);

    //Set up Leaderboard Window
    sf::RenderWindow leader;
    sf::Text leaderTitle;
    leaderTitle.setCharacterSize(20);
    leaderTitle.setString("LEADERBOARD");
    leaderTitle.setStyle(sf::Text::Bold | sf::Text::Underlined);
    leaderTitle.setFont(font);
    leaderTitle.setFillColor(sf::Color::White);
    setText(leaderTitle, (lWidth / 2.0), (lHeight / 2.0) - 120);
    string Cont = "";
    for (int i = 0; i < leaderInfo.size(); i++) {
        Cont += to_string(i + 1);
        Cont += ".\t";
        Cont += leaderInfo[i].timeMin + ":" + leaderInfo[i].timeSec + "\t";
        Cont += leaderInfo[i].name;
        Cont += "\n\n";
    }
    sf::Text leaderContent;
    leaderContent.setString(Cont);
    leaderContent.setCharacterSize(18);
    leaderContent.setFont(font);
    leaderContent.setStyle(sf::Text::Bold);
    leaderContent.setFillColor(sf::Color::White);
    setText(leaderContent, (lWidth / 2.0), (lHeight / 2.0) + 20);
    //leader.create(sf::VideoMode(lwidth, lHeight), "Leaderboard", sf::Style::Close | sf::Style::Titlebar);

    while (welcome.isOpen()) {
        sf::Event event;
        while (welcome.pollEvent(event)) {
            if (!leader.isOpen() && event.type == sf::Event::Closed) {
                welcome.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (nameString.size() != 0 && event.key.code == sf::Keyboard::Enter) {
                    welcome.close();
                    game.create(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close | sf::Style::Titlebar);
                }
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    if (event.text.unicode == 8) {
                        if (!nameString.empty() && Pos > 0) {
                            nameString.erase(Pos - 1, 1);
                            Pos--;
                        }
                    }
                    else if (isalpha(static_cast<char>(event.text.unicode)) && nameString.size() < 10) {
                        if (nameString.size() == 0) {
                            nameString.insert(Pos, 1, toupper(static_cast<char>(event.text.unicode)));
                            Pos++;
                        }
                        else {
                            nameString.insert(Pos, 1, tolower(static_cast<char>(event.text.unicode)));
                            Pos++;
                        }
                    }
                }
                name.setString(nameString);
            }
        }
        welcome.clear(sf::Color::Blue);

        welcome.draw(title);
        welcome.draw(input);
        setText(name, (width) / 2.0, ((height) / 2.0) - 45.0);
        welcome.draw(name);
        drawCursor(welcome, name, Pos);

        welcome.display();
    }

    //Game Starts
    Board board(colCount, rowCount, numMines);
    sf::Clock clock;
    while (game.isOpen()) {
        sf::Event event;
        currTime = clock.getElapsedTime();
        if (gameStatus == Normal && static_cast<int>(currTime.asSeconds() - prevTime.asSeconds()) == 1) {
            prevTime = currTime;
            currTime = sf::Time::Zero;
            //A second has passed
            secs++;
            if (secs == 60) {
                secs = 0;
                mins++;
            }
        }
        while (game.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                game.close();
            }
            //Detect MouseClick
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    
                    //Normal Button Operations
                    if (buttonPressed(happy, sf::Mouse::getPosition(game))) { //gameStatus != Paused && is just my own preference, do not submit it like that
                        //Reset the tiles map & Timer = Start new game  
                        once = false;
                        //Update LeaderBoard from Last Games
                        if (index == -1 && gameStatus == Victory) {
                            leaderInfo[leaderInfo.size() - 1].name.pop_back();
                            Cont = "";
                            for (int i = 0; i < leaderInfo.size(); i++) {
                                Cont += to_string(i + 1);
                                Cont += ".\t";
                                Cont += leaderInfo[i].timeMin + ":" + leaderInfo[i].timeSec + "\t";
                                Cont += leaderInfo[i].name;
                                Cont += "\n\n";
                            }
                            leaderContent.setString(Cont);
                        }
                        else if(gameStatus == Victory){
                            leaderInfo[index].name.pop_back();
                            Cont = "";
                            for (int i = 0; i < leaderInfo.size(); i++) {
                                Cont += to_string(i + 1);
                                Cont += ".\t";
                                Cont += leaderInfo[i].timeMin + ":" + leaderInfo[i].timeSec + "\t";
                                Cont += leaderInfo[i].name;
                                Cont += "\n\n";
                            }
                            leaderContent.setString(Cont);
                        }
                        happytext.loadFromFile("images/face_happy.png");
                        happy.setTexture(happytext);
                        secs = 0;
                        mins = 0;
                        totalPaused = 0;
                        clock.restart();
                        currTime = clock.getElapsedTime();
                        prevTime = currTime;
                        board.placeRandMines();
                        board.unBlow();
                        debugged = false;
                        numMines = startMines;
                        gameStatus = GameStatus::Normal;
                    }
                    else if (buttonPressed(debug, sf::Mouse::getPosition(game)) && gameStatus == GameStatus::Normal) {
                        if (gameStatus == Normal && !debugged) {
                            //Reveal all mines
                            board.blowUp();
                            debugged = true;
                        }
                        else {
                            //Hide all mines
                            board.unBlow();
                            debugged = false;
                        }
                    }
                    else if (buttonPressed(pause, sf::Mouse::getPosition(game)) && (gameStatus == GameStatus::Normal || gameStatus == GameStatus::Paused)) {
                        if (!paused) {
                            //Pause the game
                            gameStatus = Paused;
                            paused1 = clock.getElapsedTime();
                            pauseText.loadFromFile("images/play.png");
                            pause.setTexture(pauseText);
                            board.paused = true;
                            paused = true;
                        }
                        else {
                            //Unpause game
                            gameStatus = Normal;
                            paused2 = clock.getElapsedTime();
                            totalPaused += paused2.asSeconds() - paused1.asSeconds();
                            currTime = clock.getElapsedTime();
                            prevTime = currTime;
                            pauseText.loadFromFile("images/pause.png");
                            pause.setTexture(pauseText);
                            board.paused = false;
                            paused = false;
                        }
                    }
                    else if (buttonPressed(leaderboard, sf::Mouse::getPosition(game))) {
                        //Open Leaderboard window
                        
                        if (gameStatus == Normal) {
                            paused1 = clock.getElapsedTime();
                            board.paused = true;
                        }
                        if (!leader.isOpen()) {
                            leader.create(sf::VideoMode(lWidth, lHeight), "Leaderboard", sf::Style::Close | sf::Style::Titlebar);
                            game.clear(sf::Color::White);
                            //Draw Board Once so that it updates
                            board.draw(game);
                            game.draw(happy);
                            game.draw(debug);
                            game.draw(pause);
                            game.draw(leaderboard);
                            game.draw(counter1);
                            game.draw(counter2);
                            game.draw(counter3);
                            game.draw(timerMin1);
                            game.draw(timerMin2);
                            game.draw(timerSec1);
                            game.draw(timerSec2);
                            //Display
                            game.display();
                        }
                    }
                    else if (gameStatus == Normal) {
                        gameStatus = board.mouseLeft(sf::Mouse::getPosition(game));
                        if (gameStatus == Victory) {
                            GameTime = clock.getElapsedTime().asSeconds() - totalPaused;
                            //cout << " with a total of " << GameTime << endl;
                            gameStatus = Victory;
                            happytext.loadFromFile("images/face_win.png");
                            happy.setTexture(happytext);
                            //If You win
                            if (gameStatus == Victory && !once) {
                                board.unBlow();
                                //Check that they already have 5 people
                                if (leaderInfo.size() < 5) {
                                    lines.push_back(calculateString(GameTime) + "," + nameString);
                                    LeaderPerson user(lines[lines.size() - 1]);
                                    user.name += "*";
                                    leaderInfo.push_back(user);
                                    //Delete everything
                                    ofstream file("leaderboard.txt", ios::trunc);
                                    file.close();
                                    //Now update the file
                                    ofstream leaderfile("leaderboard.txt", ios::app);
                                    //Write changed vector to the file
                                    for (int i = 0; i < lines.size(); i++) {
                                        leaderfile << lines[i] << "\n";
                                    }
                                    leaderfile.close();
                                    //Format the leaderContent Again so it can be drawn right
                                    Cont = "";
                                    for (int i = 0; i < leaderInfo.size(); i++) {
                                        Cont += to_string(i + 1);
                                        Cont += ".\t";
                                        Cont += leaderInfo[i].timeMin + ":" + leaderInfo[i].timeSec + "\t";
                                        Cont += leaderInfo[i].name;
                                        Cont += "\n\n";
                                    }
                                    leaderContent.setString(Cont);
                                }
                                else {}
                                int scorePos = 0;
                                for (int J = 0; J < leaderInfo.size(); J++) {
                                    scorePos++;
                                    if (leaderInfo[J].actualTime > GameTime && !once) {
                                        once = true;
                                        //First delete everything in it
                                        ofstream file("leaderboard.txt", ios::trunc);
                                        file.close();
                                        //Now update the file
                                        ofstream leaderfile("leaderboard.txt", ios::app);
                                        if (leaderfile.is_open()) {
                                            //Change the corresponding line in leaderfile
                                            lines.push_back("00:00,bruh.");
                                            for (int i = lines.size() - 1; i > J; i--) {
                                                lines[i] = lines[i - 1];
                                            }
                                            lines[J] = calculateString(GameTime) + "," + nameString;
                                            lines.pop_back();
                                            //Write changed vector to the file
                                            for (int i = 0; i < lines.size(); i++) {
                                                leaderfile << lines[i] << "\n";
                                            }
                                            leaderfile.close();
                                            //Update the map with leader information
                                            LeaderPerson fake("00:00,Faker");
                                            leaderInfo.push_back(fake);
                                            for (int i = leaderInfo.size() - 1; i > J; i--) {
                                                leaderInfo[i] = leaderInfo[i - 1];
                                            }
                                            LeaderPerson user(lines[J]);
                                            user.name += "*";
                                            leaderInfo[J] = user;
                                            index = J;
                                            leaderInfo.pop_back();
                                            //Format the leaderContent Again so it can be drawn right
                                            Cont = "";
                                            for (int i = 0; i < leaderInfo.size(); i++) {
                                                Cont += to_string(i + 1);
                                                Cont += ".\t";
                                                Cont += leaderInfo[i].timeMin + ":" + leaderInfo[i].timeSec + "\t";
                                                Cont += leaderInfo[i].name;
                                                Cont += "\n\n";
                                            }
                                            leaderContent.setString(Cont);
                                        }
                                        else {
                                            cerr << "Error: Unable to edit the leaderboard file." << std::endl;
                                        }
                                    }
                                }
                            }
                            //Stop timer
                            if (!leader.isOpen()) {
                                leader.create(sf::VideoMode(lWidth, lHeight), "Leaderboard", sf::Style::Close | sf::Style::Titlebar);
                                game.clear(sf::Color::White);
                                //Draw Board Once so that it updates
                                board.draw(game);
                                game.draw(happy);
                                game.draw(debug);
                                game.draw(pause);
                                game.draw(leaderboard);
                                game.draw(counter1);
                                game.draw(counter2);
                                game.draw(counter3);
                                game.draw(timerMin1);
                                game.draw(timerMin2);
                                game.draw(timerSec1);
                                game.draw(timerSec2);
                                //Display
                                game.display();
                            }
                        }
                        else if (gameStatus == Loss) {
                            GameTime = clock.getElapsedTime().asSeconds() - totalPaused;
                            //cout << " with a total of " << GameTime << endl;
                            gameStatus = Loss;
                            happytext.loadFromFile("images/face_lose.png");
                            happy.setTexture(happytext);
                            //Stop timer
                        }
                    }
                }
                if (gameStatus == Normal && event.mouseButton.button == sf::Mouse::Right) {
                    int temp = board.mouseRight(sf::Mouse::getPosition(game));
                    //Update Counter/NumMines
                    if (temp == 0) {
                        numMines++;
                    }
                    else if (temp == 1) {
                        numMines--;
                    }
                }
            }
            while (leader.isOpen()) {
                while (leader.pollEvent(event)) {
                    //Do the leaderboard Stuff
                    if (event.type == sf::Event::Closed) {
                        leader.close();
                        if (gameStatus != Paused) {
                            paused2 = clock.getElapsedTime();
                            totalPaused += paused2.asSeconds() - paused1.asSeconds();
                        }
                        currTime = clock.getElapsedTime();
                        prevTime = currTime;
                        if (!paused) {
                            board.paused = false;
                        }
                    }
                }
                leader.clear(sf::Color::Blue);
                leader.draw(leaderTitle);
                leader.draw(leaderContent);
                leader.display();
            }
            event.type = sf::Event::GainedFocus;
            continue;
        }

        //Process Counter Drawing
        if (numMines == 0) {
            counter1.setTextureRect(rect);
            counter2.setTextureRect(rect);
            counter3.setTextureRect(rect);
        }
        if (abs(numMines) < 10) {
            counter1.setTextureRect(rect);
            counter2.setTextureRect(rect);
            if (abs(numMines) == 1) {
                counter3.setTextureRect(rect1);
            }
            else if (abs(numMines) == 2) {
                counter3.setTextureRect(rect2);
            }
            else if (abs(numMines) == 3) {
                counter3.setTextureRect(rect3);
            }
            else if (abs(numMines) == 4) {
                counter3.setTextureRect(rect4);
            }
            else if (abs(numMines) == 5) {
                counter3.setTextureRect(rect5);
            }
            else if (abs(numMines) == 6) {
                counter3.setTextureRect(rect6);
            }
            else if (abs(numMines) == 7) {
                counter3.setTextureRect(rect7);
            }
            else if (abs(numMines) == 8) {
                counter3.setTextureRect(rect8);
            }
            else if (abs(numMines) == 9) {
                counter3.setTextureRect(rect9);
            }
        }
        else {
            if (abs(numMines) % 10 == 0) {
                counter3.setTextureRect(rect);
            }
            else if (abs(numMines) % 10 == 1) {
                counter3.setTextureRect(rect1);
            }
            else if (abs(numMines) % 10 == 2) {
                counter3.setTextureRect(rect2);
            }
            else if (abs(numMines) % 10 == 3) {
                counter3.setTextureRect(rect3);
            }
            else if (abs(numMines) % 10 == 9) {
                counter3.setTextureRect(rect9);
            }
            else if (abs(numMines) % 10 == 4) {
                counter3.setTextureRect(rect4);
            }
            else if (abs(numMines) % 10 == 5) {
                counter3.setTextureRect(rect5);
            }
            else if (abs(numMines) % 10 == 6) {
                counter3.setTextureRect(rect6);
            }
            else if (abs(numMines) % 10 == 7) {
                counter3.setTextureRect(rect7);
            }
            else if (abs(numMines) % 10 == 8) {
                counter3.setTextureRect(rect8);
            }
            if (abs(numMines) < 100) {
                counter1.setTextureRect(rect);
                if (abs(numMines) / 10 == 1) {
                    counter2.setTextureRect(rect1);
                }
                else if (abs(numMines) / 10 == 2) {
                    counter2.setTextureRect(rect2);
                }
                else if (abs(numMines) / 10 == 3) {
                    counter2.setTextureRect(rect3);
                }
                else if (abs(numMines) / 10 == 9) {
                    counter2.setTextureRect(rect9);
                }
                else if (abs(numMines) / 10 == 4) {
                    counter2.setTextureRect(rect4);
                }
                else if (abs(numMines) / 10 == 5) {
                    counter2.setTextureRect(rect5);
                }
                else if (abs(numMines) / 10 == 6) {
                    counter2.setTextureRect(rect6);
                }
                else if (abs(numMines) / 10 == 7) {
                    counter2.setTextureRect(rect7);
                }
                else if (abs(numMines) / 10 == 8) {
                    counter2.setTextureRect(rect8);
                }
            }
            else {
                if ((abs(numMines) % 100) / 10 == 0) {
                    counter2.setTextureRect(rect);
                }
                if ((abs(numMines) % 100) / 10 == 1) {
                    counter2.setTextureRect(rect1);
                }
                else if ((abs(numMines) % 100) / 10 == 2) {
                    counter2.setTextureRect(rect2);
                }
                else if ((abs(numMines) % 100) / 10 == 3) {
                    counter2.setTextureRect(rect3);
                }
                else if ((abs(numMines) % 100) / 10 == 9) {
                    counter2.setTextureRect(rect9);
                }
                else if ((abs(numMines) % 100) / 10 == 4) {
                    counter2.setTextureRect(rect4);
                }
                else if ((abs(numMines) % 100) / 10 == 5) {
                    counter2.setTextureRect(rect5);
                }
                else if ((abs(numMines) % 100) / 10 == 6) {
                    counter2.setTextureRect(rect6);
                }
                else if ((abs(numMines) % 100) / 10 == 7) {
                    counter2.setTextureRect(rect7);
                }
                else if ((abs(numMines) % 100) / 10 == 8) {
                    counter2.setTextureRect(rect8);
                }
                if (abs(numMines) / 100 == 1) {
                    counter1.setTextureRect(rect1);
                }
                else if (abs(numMines) / 100 == 2) {
                    counter1.setTextureRect(rect2);
                }
                else if (abs(numMines) / 100 == 3) {
                    counter1.setTextureRect(rect3);
                }
                else if (abs(numMines) / 100 == 4) {
                    counter1.setTextureRect(rect4);
                }
                else if (abs(numMines) / 100 == 5) {
                    counter1.setTextureRect(rect5);
                }
                else if (abs(numMines) / 100 == 6) {
                    counter1.setTextureRect(rect6);
                }
                else if (abs(numMines) / 100 == 7) {
                    counter1.setTextureRect(rect7);
                }
                else if (abs(numMines) / 100 == 8) {
                    counter1.setTextureRect(rect8);
                }
                else if (abs(numMines) / 100 == 9) {
                    counter1.setTextureRect(rect9);
                }
            }
        }

        //Process Clock Drawing
        if (secs == 0) {
            timerSec1.setTextureRect(rect);
            timerSec2.setTextureRect(rect);
        }
        else if (secs < 10) {
            timerSec1.setTextureRect(rect);
            if (secs == 1) {
                timerSec2.setTextureRect(rect1);
            }
            else if (secs == 2) {
                timerSec2.setTextureRect(rect2);
            }
            else if (secs == 3) {
                timerSec2.setTextureRect(rect3);
            }
            else if (secs == 4) {
                timerSec2.setTextureRect(rect4);
            }
            else if (secs == 5) {
                timerSec2.setTextureRect(rect5);
            }
            else if (secs == 6) {
                timerSec2.setTextureRect(rect6);
            }
            else if (secs == 7) {
                timerSec2.setTextureRect(rect7);
            }
            else if (secs == 8) {
                timerSec2.setTextureRect(rect8);
            }
            else if (secs == 9) {
                timerSec2.setTextureRect(rect9);
            }
        }
        else {
            if (secs / 10 == 1) {
                timerSec1.setTextureRect(rect1);
            }
            else if (secs / 10 == 2) {
                timerSec1.setTextureRect(rect2);
            }
            else if (secs / 10 == 3) {
                timerSec1.setTextureRect(rect3);
            }
            else if (secs / 10 == 4) {
                timerSec1.setTextureRect(rect4);
            }
            else if (secs / 10 == 5) {
                timerSec1.setTextureRect(rect5);
            }
            else if (secs / 10 == 6) {
                timerSec1.setTextureRect(rect6);
            }
            else if (secs / 10 == 7) {
                timerSec1.setTextureRect(rect7);
            }
            else if (secs / 10 == 8) {
                timerSec1.setTextureRect(rect8);
            }
            else if (secs / 10 == 9) {
                timerSec1.setTextureRect(rect9);
            }
            if (secs % 10 == 0) {
                timerSec2.setTextureRect(rect);
            }
            else if (secs % 10 == 1) {
                timerSec2.setTextureRect(rect1);
            }
            else if (secs % 10 == 2) {
                timerSec2.setTextureRect(rect2);
            }
            else if (secs % 10 == 3) {
                timerSec2.setTextureRect(rect3);
            }
            else if (secs % 10 == 4) {
                timerSec2.setTextureRect(rect4);
            }
            else if (secs % 10 == 5) {
                timerSec2.setTextureRect(rect5);
            }
            else if (secs % 10 == 6) {
                timerSec2.setTextureRect(rect6);
            }
            else if (secs % 10 == 7) {
                timerSec2.setTextureRect(rect7);
            }
            else if (secs % 10 == 8) {
                timerSec2.setTextureRect(rect8);
            }
            else if (secs % 10 == 9) {
                timerSec2.setTextureRect(rect9);
            }
        }
        if (mins < 10) {
            timerMin1.setTextureRect(rect);
            if (mins == 0) {
                timerMin2.setTextureRect(rect);
            }
            if (mins == 1) {
                timerMin2.setTextureRect(rect1);
            }
            else if (mins == 2) {
                timerMin2.setTextureRect(rect2);
            }
            else if (mins == 3) {
                timerMin2.setTextureRect(rect3);
            }
            else if (mins == 4) {
                timerMin2.setTextureRect(rect4);
            }
            else if (mins == 5) {
                timerMin2.setTextureRect(rect5);
            }
            else if (mins == 6) {
                timerMin2.setTextureRect(rect6);
            }
            else if (mins == 7) {
                timerMin2.setTextureRect(rect7);
            }
            else if (mins == 8) {
                timerMin2.setTextureRect(rect8);
            }
            else if (mins == 9) {
                timerMin2.setTextureRect(rect9);
            }
        }
        else {
            if (mins / 10 == 1) {
                timerMin1.setTextureRect(rect1);
            }
            else if (mins / 10 == 2) {
                timerMin1.setTextureRect(rect2);
            }
            else if (mins / 10 == 3) {
                timerMin1.setTextureRect(rect3);
            }
            else if (mins / 10 == 4) {
                timerMin1.setTextureRect(rect4);
            }
            else if (mins / 10 == 5) {
                timerMin1.setTextureRect(rect5);
            }
            else if (mins / 10 == 6) {
                timerMin1.setTextureRect(rect6);
            }
            else if (mins / 10 == 7) {
                timerMin1.setTextureRect(rect7);
            }
            else if (mins / 10 == 8) {
                timerMin1.setTextureRect(rect8);
            }
            else if (mins / 10 == 9) {
                timerMin1.setTextureRect(rect9);
            }
            if (mins % 10 == 0) {
                timerMin2.setTextureRect(rect);
            }
            else if (mins % 10 == 1) {
                timerMin2.setTextureRect(rect1);
            }
            else if (mins % 10 == 2) {
                timerMin2.setTextureRect(rect2);
            }
            else if (mins % 10 == 3) {
                timerMin2.setTextureRect(rect3);
            }
            else if (mins % 10 == 4) {
                timerMin2.setTextureRect(rect4);
            }
            else if (mins % 10 == 5) {
                timerMin2.setTextureRect(rect5);
            }
            else if (mins % 10 == 6) {
                timerMin2.setTextureRect(rect6);
            }
            else if (mins % 10 == 7) {
                timerMin2.setTextureRect(rect7);
            }
            else if (mins % 10 == 8) {
                timerMin2.setTextureRect(rect8);
            }
            else if (mins % 10 == 9) {
                timerMin2.setTextureRect(rect9);
            }
        }

        game.clear(sf::Color::White);
        //Draw Board
        board.draw(game);
        game.draw(happy);
        game.draw(debug);
        game.draw(pause);
        game.draw(leaderboard);
        game.draw(counter1);
        game.draw(counter2);
        game.draw(counter3);
        game.draw(timerMin1);
        game.draw(timerMin2);
        game.draw(timerSec1);
        game.draw(timerSec2);
        if (numMines < 0) {
            game.draw(neg);
        }
        //Display
        game.display();
    }
    return 0;
}