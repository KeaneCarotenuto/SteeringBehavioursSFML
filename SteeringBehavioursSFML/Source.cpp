#include "Utility.h"
#include "GameManager.h"
#include "Agent.h"

bool util::debugMode = false;

void ShowConsoleCursor(bool showFlag);

int main();

void SpawnBasics();

void ToggleControls();

void UpdateTypeText(std::string _newText);

void PrevBehaviour();

void NextBehaviour();

void ExecutePrints(bool _clearPrint = true);

int FixedUpdate(float deltaTime);
void Draw();

void SendPrint(PrintString _ps);

void Print(PrintString _ps);

void SlowPrint(PrintString _ps, int _wait);

void GotoXY(int x, int y);

std::vector<PrintString> toClear;
std::vector<PrintString> toPrint;

sf::Font pixelFont;
sf::Text currentType;
sf::Text instructions;

sf::Texture* texture_agent = new sf::Texture();

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

	//Loads textures for sprites
	if (!texture_agent->loadFromFile("Resources/Agent.png", sf::IntRect(0, 0, 32, 32)))
	{
		std::cout << "ERROR: Failed to load sprite";
	}

	Agent::mainWindow = Game::wind;

	SpawnBasics();

	if (!pixelFont.loadFromFile("Resources/pixelFont.ttf"))
	{
		std::cout << "ERROR: Failed to load font";
	}

	currentType.setFont(pixelFont);
	currentType.setFillColor(sf::Color::White);
	currentType.setOutlineColor(sf::Color::Black);
	currentType.setOutlineThickness(5.0f);
	currentType.setCharacterSize(30);
	UpdateTypeText("SEEK");
	currentType.setPosition(20, 20);

	instructions.setFont(pixelFont);
	instructions.setFillColor(sf::Color::White);
	instructions.setOutlineColor(sf::Color::Black);
	instructions.setOutlineThickness(5.0f);
	instructions.setCharacterSize(20);
	instructions.setLineSpacing(1.5f);
	instructions.setString(
		"[C] TO OPEN AND CLOSE CONTROLS\n"\
	);
	instructions.setOrigin(0, instructions.getGlobalBounds().height);
	instructions.setPosition(20, util::windowHeight - 10);


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

			if (newEvent.type == sf::Event::KeyPressed) {
				if (newEvent.key.code == sf::Keyboard::Key::Tab) {
					util::debugMode = !util::debugMode;
				}

				if (newEvent.key.code == sf::Keyboard::Key::LBracket) {
					PrevBehaviour();
				}
				if (newEvent.key.code == sf::Keyboard::Key::RBracket) {
					NextBehaviour();
				}

				if (newEvent.key.code == sf::Keyboard::Key::C) {
					ToggleControls();
				}

				if (newEvent.key.code == sf::Keyboard::Key::R) {
					Agent::DeleteAll();
					SpawnBasics();
				}

				if (newEvent.key.code == sf::Keyboard::Key::A) {

					Agent* _agent = new Agent("a" + std::to_string(Agent::allAgents.size()), (sf::Vector2f)util::mousePos, 1, rand() % 100 + 50, rand() % 100 + 50, texture_agent);

					Agent::AddAgent(_agent);

				}
				else if (newEvent.key.code == sf::Keyboard::Key::O) {

					Obstacle* _obst = new Obstacle("o" + std::to_string(Agent::allObstacles.size()), (sf::Vector2f)util::mousePos, rand() % 100 + 10);

					Agent::AddObstacle(_obst);

				}
				else if (newEvent.key.code == sf::Keyboard::Key::P) {

					Path::AddToPath(Agent::allPaths["p0"], (sf::Vector2f)util::mousePos, 20);
				}
			}
		}
	}

	return 0;
}

void SpawnBasics()
{
	for (int i = 0; i < 10; i++) {
		Agent* _agent = new Agent("a" + std::to_string(i), { float(rand() % 800), float(rand() % 800) }, 1, rand() % 100 + 50, rand() % 100 + 50, texture_agent);

		if (_agent->GetName() == "a0") {
			_agent->SetMaxVel(200);
			_agent->SetMaxAcc(200);
		}

		Agent::AddAgent(_agent);
	}

	Agent::AddObstacle(new Obstacle("o0", sf::Vector2f(rand() % 800, rand() % 800), rand() % 100 + 50));

	Agent::AddTarget(new Target("a0", Agent::allAgents["a0"]->GetPos(), Agent::allAgents["a0"]->GetVel(), 20));
	Agent::AddTarget(new Target("mouse", (sf::Vector2f)util::mousePos, sf::Vector2f(0, 0), 20));

	Agent::allAgents["a0"]->SetTarget(Agent::allTargets["mouse"]);

	Agent::AddPath(new Path("p0"));

	Agent::UpdateBehaviour(UpdateTypeText);
}

void ToggleControls()
{
	if (instructions.getString().getSize() < 50) {
		instructions.setString(
			"[TAB] TO ENABLE/DISABLE DEBUG VIEW\n"\
			"[A] TO SPAWN AGENT\n"\
			"[O] TO SPAWN OBSTACLE\n"\
			"[P] TO SPAWN NEW PATH POINT\n"\
			"[LEFT SQUARE BRACKET] TO GO TO PREVIOUS BEHAVIOUR\n"\
			"[RIGHT SQUARE BRACKET] TO GO TO NEXT BEHAVIOUR\n"\
			"[R] TO RESTART\n"\
			"[C] TO OPEN AND CLOSE CONTROLS\n"\
		);
	}
	else {
		instructions.setString(
			"[C] TO OPEN AND CLOSE CONTROLS\n"\
		);
	}

	instructions.setOrigin(0, instructions.getGlobalBounds().height);
	instructions.setPosition(20, util::windowHeight - 10);

	
}

void UpdateTypeText(std::string _newText)
{
	currentType.setString("MODE:\n" + _newText);
}

void PrevBehaviour() {
	Agent::behaviour = static_cast<SteeringType>((int)Agent::behaviour - 1);

	if (Agent::behaviour < SteeringType::First) {
		Agent::behaviour = SteeringType::Last;
	}

	Agent::UpdateBehaviour(UpdateTypeText);
}

void NextBehaviour() {
	Agent::behaviour = static_cast<SteeringType>((int)Agent::behaviour + 1);

	if (Agent::behaviour > SteeringType::Last) {
		Agent::behaviour = SteeringType::First;
	}

	Agent::UpdateBehaviour(UpdateTypeText);
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
	util::mousePos = sf::Mouse::getPosition(*Game::wind);
	util::mousePos.x = glm::clamp(util::mousePos.x, 0, util::windowWidth);
	util::mousePos.y = glm::clamp(util::mousePos.y, 0, util::windowHeight);
	
	Agent::allTargets["mouse"]->m_position = (sf::Vector2f)util::mousePos;
	Agent::allTargets["a0"]->m_position = Agent::allAgents["a0"]->GetPos();
	Agent::allTargets["a0"]->m_velocity = Agent::allAgents["a0"]->GetVel();

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

	Game::wind->draw(currentType);
	Game::wind->draw(instructions);

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