#include "Utility.h"
#include "GameManager.h"
#include <glm/glm.hpp>

void ShowConsoleCursor(bool showFlag);

void ExecutePrints(bool _clearPrint = true);

int FixedUpdate();
void Draw();

void SendPrint(PrintString _ps);

void Print(PrintString _ps);

void SlowPrint(PrintString _ps, int _wait);

void GotoXY(int x, int y);

std::vector<PrintString> toClear;
std::vector<PrintString> toPrint;


void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

int main() {

	//Creating Different Windows
	sf::RenderWindow window(sf::VideoMode(800, 800), "Steering Behaviours - By Keane Carotenuto");
	Game::wind = &window;

	ShowConsoleCursor(false);

	//Manages the FixedUpdate() timing
	float stepTime = 0;
	bool drawn = false;
	sf::Clock clock;

	//FixedUpdate() call rate
	float step = (1.0f / 60.0f);

	sf::RectangleShape* test = new sf::RectangleShape();
	test->setPosition(sf::Vector2f(700, 700));
	test->setFillColor(sf::Color::Red);
	test->setSize(sf::Vector2f(10, 10));

	Game::toDraw.push_back(test);

	while (window.isOpen() == true)
	{
		util::CurrentTime = sf::Time().asSeconds();
		stepTime += clock.getElapsedTime().asSeconds();
		clock.restart();

		while (stepTime >= step)
		{
			ExecutePrints();

			//Main Loop of Game
			if (FixedUpdate() == 0) return 0;

			stepTime -= step;
			drawn = false;
		}

		//Draws After Updates
		if (drawn)
		{
			//sf::sleep(sf::seconds(0.01f));
		}
		else
		{
			Draw();

			drawn = true;
		}

		sf::Event newEvent;

		//Quit
		while (window.pollEvent(newEvent))
		{
			if (newEvent.type == sf::Event::Closed)
			{
				window.close();
			}

			if (newEvent.type == sf::Event::MouseMoved) {
				util::mousePos = sf::Mouse::getPosition(window);
			}
		}
	}

	return 0;
}

void ExecutePrints(bool _clearPrint)
{
	for (PrintString _ps : toClear) {
		Print({ _ps.m_pos, _ps.m_string, 0 });
	}
	toClear.clear();

	for (PrintString _ps : toPrint) {
		Print(_ps);
		toClear.push_back(_ps);
	}
	if (_clearPrint) toPrint.clear();
}

int FixedUpdate() {
	sf::RectangleShape* theRect = dynamic_cast<sf::RectangleShape*>(Game::toDraw[0]);

	SendPrint({ 5,5, std::to_string(util::mousePos.x) + ", " + std::to_string(util::mousePos.y), 15});

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		theRect->setPosition(sf::Vector2f(theRect->getPosition().x - 1, theRect->getPosition().y));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		theRect->setPosition(sf::Vector2f(theRect->getPosition().x + 1, theRect->getPosition().y));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		theRect->setPosition(sf::Vector2f(theRect->getPosition().x, theRect->getPosition().y - 1));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		theRect->setPosition(sf::Vector2f(theRect->getPosition().x, theRect->getPosition().y + 1));
	}

	return 1;
}

void Draw() {
	Game::wind->clear();

	for (sf::Drawable* item : Game::toDraw)
	{
		Game::wind->draw((*item));
	}

	//Update main window
	Game::wind->display();
}

#pragma region "Printing Functions"
/// <summary>
/// (AUTO CLEAR) Prints out text at position with colour 
/// </summary>
/// <param name="x"> pos</param>
/// <param name="y"> pos</param>
/// <param name="str"> string to print</param>
/// <param name="effect"> colour to print with</param>
void SendPrint(PrintString _ps) {
	toPrint.push_back(_ps);
};

#pragma region "Printing Functions"
/// <summary>
/// (NO CLEAR) Prints out text at position with colour
/// </summary>
/// <param name="x"> pos</param>
/// <param name="y"> pos</param>
/// <param name="str"> string to print</param>
/// <param name="effect"> colour to print with</param>
void Print(PrintString _ps){
	GotoXY(_ps.m_pos.x, _ps.m_pos.y);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _ps.m_col);
	std::cout << _ps.m_string;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
};

/// <summary>
/// (NO CLEAR) Slowly prints out text at pos with colour 
/// </summary>
/// <param name="x">pos</param>
/// <param name="y">pos</param>
/// <param name="_message">string to pring</param>
/// <param name="effect">colour to print with</param>
/// <param name="_wait">deltay after each character</param>
void SlowPrint(PrintString _ps, int _wait) {
	GotoXY(_ps.m_pos.x, _ps.m_pos.y);
	for (wchar_t _char : _ps.m_string) {
		Sleep(_wait);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _ps.m_col);
		std::cout << _char;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

/// <summary>
/// Moves console cursor to specific position
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void GotoXY(int x, int y) {
	COORD point = { (short)x,(short)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);
};
#pragma endregion