#include "Agent.h"

Agent::Agent(std::string _name, sf::Vector2f _pos, float _mass, float _maxVel, float _maxAcc, sf::RenderWindow* _window, sf::Texture* _texture)
{
	m_name = _name;
	m_position = _pos;
	m_mass = _mass;
	m_maxVelocity = _maxVel;
	m_maxAcceleration = _maxAcc;

	window = _window;
	texture = _texture;
	sprite.setTexture(*texture);
	sprite.setOrigin(16,16);
}

sf::Vector2f Agent::Seek()
{
	sf::Vector2f desiredVel = util::normalize(m_target.m_position - m_position) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Pursue()
{
	sf::Vector2f desiredVel = util::normalize(m_target.m_position - m_position + m_target.m_velocity) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Flee()
{
	sf::Vector2f desiredVel = util::normalize(m_position - m_target.m_position) * m_maxAcceleration;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

sf::Vector2f Agent::Arrive()
{
	sf::Vector2f offset = m_target.m_position - m_position;

	float distance = util::length(offset);

	float rampedSpeed = m_maxVelocity * (distance / m_target.m_slowingDistance);

	float clampedSpeed = std::min(rampedSpeed, m_maxVelocity);

	sf::Vector2f desiredVel = (clampedSpeed / distance) * offset;

	desiredVel = desiredVel - m_velocity;

	return desiredVel;
}

void Agent::Update(float deltaTime)
{
	m_acceleration = Pursue();

	if (util::length(m_acceleration) > m_maxAcceleration)
	{
		m_acceleration = util::normalize(m_acceleration);
		m_acceleration *= m_maxAcceleration;
	}

	m_velocity += m_acceleration * deltaTime;

	if (util::length(m_velocity) > m_maxVelocity)
	{
		m_velocity = util::normalize(m_velocity);
		m_velocity *= m_maxVelocity;
	}

	m_position += m_velocity * deltaTime;

	if (m_position.x > util::windowWidth) m_position.x = 0;
	if (m_position.x < 0) m_position.x = util::windowWidth;
	if (m_position.y > util::windowHeight) m_position.y = 0;
	if (m_position.y < 0) m_position.y = util::windowHeight;
}

void Agent::Render()
{
	sf::VertexArray lines(sf::LinesStrip, 2);
	lines[0].position = m_position;
	lines[0].color = sf::Color::Red;
	lines[1].position = m_position + m_velocity;
	lines[1].color = sf::Color::Red;
	window->draw(lines);

	lines[0].position = m_position;
	lines[0].color = sf::Color::Blue;
	lines[1].position = m_position + m_acceleration;
	lines[1].color = sf::Color::Blue;
	window->draw(lines);

	sprite.setPosition(m_position);
	sprite.setRotation(-atan2(m_velocity.x, m_velocity.y) * 180.0f / glm::pi<float>() + 180);
	window->draw(sprite);
}
