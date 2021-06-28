#include "Utility.h"
#include "GameManager.h"
#include "Agent.h"

void ShowConsoleCursor(bool showFlag);

void ExecutePrints(bool _clearPrint = true);

int FixedUpdate(float deltaTime);
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
	srand(time(0));

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

	sf::Texture* texture_agent =  new sf::Texture();
	//Loads textures for sprites
	if (!texture_agent->loadFromFile("Resources/Agent.png", sf::IntRect(0, 0, 32, 32)))
	{
		std::cout << "ERROR: Failed to load sprite";
	}

	Agent::mainWindow = Game::wind;

	for (int i = 0; i < 10; i++) {
		Agent* _agent = new Agent("a" + std::to_string(i), { float(rand() % 800), float(rand() % 800) }, 1, rand() % 50 + 50, rand() % 50 + 50, texture_agent);

		if (_agent->GetName() == "a0") {
			_agent->SetMaxVel(200);
			_agent->SetMaxAcc(200);
		}

		Agent::AddAgent(_agent);
	}

	Agent::AddObstacle(new Obstacle("o1", sf::Vector2f(400, 400), 100));
	Agent::AddObstacle(new Obstacle("o2", sf::Vector2f(500, 400), 100));

	while (window.isOpen() == true)
	{
		util::CurrentTime = sf::Time().asSeconds();
		stepTime += clock.getElapsedTime().asSeconds();
		clock.restart();

		while (stepTime >= step)
		{
			ExecutePrints();

			//Main Loop of Game
			if (FixedUpdate(stepTime) == 0) return 0;

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

int FixedUpdate(float deltaTime) {
	sf::Vector2i oldMouse = util::mousePos;
	util::mousePos = sf::Mouse::getPosition(*Game::wind);

	sf::Vector2f mouseVel = (sf::Vector2f)(oldMouse - util::mousePos);

	std::map<std::string, Agent*>::iterator it;
	for (it = Agent::allAgents.begin(); it != Agent::allAgents.end(); ++it) {
		if (it->second->GetName() == "a0") {
			it->second->SetTarget({ (sf::Vector2f)util::mousePos, {0,0}, 0 });
			continue;
		}
		it->second->SetTarget({ Agent::allAgents["a0"]->GetPos() , Agent::allAgents["a0"]->GetVel() , 0 });
	}

	

	Agent::UpdateAll(deltaTime);

	return 1;
}

void Draw() {
	Game::wind->clear();

	Agent::RenderAll();

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