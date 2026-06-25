#include "GameWindow.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

struct GameWindow::Impl {
    sf::RenderWindow window;
    Impl(unsigned int width, unsigned int height, const std::string& title) 
        : window(sf::VideoMode({width, height}), title) {}
};

GameWindow::GameWindow(unsigned int width, unsigned int height, const std::string& title)
    : m_impl(std::make_unique<Impl>(width, height, title)) 
{
    m_impl->window.setFramerateLimit(60);
}

GameWindow::~GameWindow() = default;

bool GameWindow::isOpen() const {
    return m_impl->window.isOpen();
}

void GameWindow::close() {
    m_impl->window.close();
}

std::optional<sf::Event> GameWindow::pollEvent() {
    return m_impl->window.pollEvent();
}

void GameWindow::clear(uint8_t r, uint8_t g, uint8_t b) {
    m_impl->window.clear(sf::Color(r, g, b));
}

void GameWindow::display() {
    m_impl->window.display();
}

sf::RenderWindow& GameWindow::getRenderWindow() {
    return m_impl->window;
}
