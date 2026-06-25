#include "Snake.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
//#include "SoundManager.hpp"

Snake::Snake() 
    : m_direction(Direction::Right)
    , m_nextDirection(Direction::Right)
    , m_growPending(false)
    , m_headColor(sf::Color(0, 170, 0))   // [ДОБАВЛЕНО] Дефолтный цвет при старте
    , m_bodyColor(sf::Color(50, 255, 50))   // [ДОБАВЛЕНО] Дефолтный цвет при старте

{
    // Начальное тело змейки из 3 сегментов в центре экрана (в координатах сетки)
    m_body.push_back({10, 10}); // Голова
    m_body.push_back({9, 10});
    m_body.push_back({8, 10});  // Хвост
}



void Snake::handleInput() {
    // Меняем направление, запрещая разворот на 180 градусов (назад в себя)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && m_direction != Direction::Down) {
        m_nextDirection = Direction::Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && m_direction != Direction::Up) {
        m_nextDirection = Direction::Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && m_direction != Direction::Right) {
        m_nextDirection = Direction::Left;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && m_direction != Direction::Left) {
        m_nextDirection = Direction::Right;
    }
}



void Snake::update() {
    m_direction = m_nextDirection;

    // Вычисляем новую позицию головы
    sf::Vector2i newHead = m_body.front();
    switch (m_direction) {
        case Direction::Up:    newHead.y -= 1; break;
        case Direction::Down:  newHead.y += 1; break;
        case Direction::Left:  newHead.x -= 1; break;
        case Direction::Right: newHead.x += 1; break;
    }

    // Вставляем новую голову в начало вектора
    m_body.insert(m_body.begin(), newHead);

    // Если змейка не ела, удаляем старый кончик хвоста. Если ела — оставляем, длина растет.
    if (!m_growPending) {
        m_body.pop_back();
    } else {
        m_growPending = false; // Сбрасываем флаг роста
        // Воспроизводим звук поедания еды
       // SoundManager::getInstance().play("eat");
    }
}



void Snake::grow() {
    m_growPending = true;
}



bool Snake::checkSelfCollision() const {
    // Если голова (index 0) сталкивается с любым другим сегментом тела
     for (size_t i = 1; i < m_body.size(); ++i) {
        if (m_body[0] == m_body[i]) {
            return true;
        }
    }
    return false;
}



sf::Vector2i Snake::getHeadPosition() const {
    return m_body.front();
}



void Snake::render(sf::RenderWindow& window) {
    sf::RectangleShape segment(sf::Vector2f{ static_cast<float>(m_tileSize - 2), static_cast<float>(m_tileSize - 2) });
    
    for (size_t i = 0; i < m_body.size(); ++i) {
        if (i == 0) {
            segment.setFillColor(m_headColor); // [ИСПРАВЛЕНО] Используем динамический цвет головы
        } else {
            segment.setFillColor(m_bodyColor); // [ИСПРАВЛЕНО] Используем динамический цвет тела
        }

        segment.setPosition({ static_cast<float>(m_body[i].x * m_tileSize), static_cast<float>(m_body[i].y * m_tileSize) });
        window.draw(segment);
    }
}



void Snake::shrink() {
    if (m_body.size() > 3) m_body.pop_back();
    if (m_body.size() > 3) m_body.pop_back();
}



void Snake::setColorScheme(sf::Color headColor, sf::Color bodyColor) {
    m_headColor = headColor;
    m_bodyColor = bodyColor;
}

