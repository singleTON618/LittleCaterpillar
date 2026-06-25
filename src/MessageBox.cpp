#include "MessageBox.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

struct MessageBox::Impl {
    sf::RectangleShape background;
    sf::RectangleShape okButton;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text recordText;
    sf::Text buttonText;

    // В SFML 3 привязываем шрифт СТРОГО в списке инициализации
    Impl(const sf::Font& font)
        : titleText(font, "GAME OVER", 24)
        , scoreText(font, "Your Score: 0", 18)
        , recordText(font, "Best Score: 0", 18)
        , buttonText(font, "OK", 18)
    {
        // Настройка главного окна (размер 340x220)
        background.setSize({340.f, 220.f});
        background.setFillColor(sf::Color(45, 45, 45, 240)); // Полупрозрачный темно-серый
        background.setOutlineColor(sf::Color(120, 120, 120));
        background.setOutlineThickness(3.f);

        // Настройка кнопки ОК
        okButton.setSize({100.f, 35.f});
        okButton.setFillColor(sf::Color(70, 70, 70));
        okButton.setOutlineColor(sf::Color::White);
        okButton.setOutlineThickness(1.f);

        // Цвета текста
        titleText.setFillColor(sf::Color(255, 100, 100)); // Красный заголовок
        scoreText.setFillColor(sf::Color::White);
        recordText.setFillColor(sf::Color(255, 215, 0));  // Золотой цвет для рекорда
        buttonText.setFillColor(sf::Color::White);
    }
};

MessageBox::MessageBox(const sf::Font& font)
    : m_impl(std::make_unique<Impl>(font)) {}

MessageBox::~MessageBox() = default;

void MessageBox::setScores(int currentScore, int highScore) {
    m_impl->scoreText.setString("Your Score: " + std::to_string(currentScore));
    m_impl->recordText.setString("Best Score: " + std::to_string(highScore));
}

bool MessageBox::handleMouseClick(int mouseX, int mouseY) {
    // Проверяем, входят ли координаты мыши в глобальные границы кнопки ОК
    return m_impl->okButton.getGlobalBounds().contains({ static_cast<float>(mouseX), static_cast<float>(mouseY) });
}

void MessageBox::render(sf::RenderWindow& window, float centerX, float centerY) {
    // Центрируем все элементы динамически относительно переданных координат камеры (centerX, centerY)
    m_impl->background.setOrigin({170.f, 110.f});
    m_impl->background.setPosition({centerX, centerY});

    m_impl->titleText.setOrigin({m_impl->titleText.getLocalBounds().size.x / 2.f, 0.f});
    m_impl->titleText.setPosition({centerX, centerY - 85.f});

    m_impl->scoreText.setOrigin({m_impl->scoreText.getLocalBounds().size.x / 2.f, 0.f});
    m_impl->scoreText.setPosition({centerX, centerY - 30.f});

    m_impl->recordText.setOrigin({m_impl->recordText.getLocalBounds().size.x / 2.f, 0.f});
    m_impl->recordText.setPosition({centerX, centerY + 5.f});

    m_impl->okButton.setOrigin({50.f, 17.5f});
    m_impl->okButton.setPosition({centerX, centerY + 65.f});

    m_impl->buttonText.setOrigin({m_impl->buttonText.getLocalBounds().size.x / 2.f, m_impl->buttonText.getLocalBounds().size.y / 2.f});
    // В SFML 3 выравнивание текста по вертикали учитывает локальные границы чуть точнее:
    m_impl->buttonText.setPosition({centerX, centerY + 62.f});

    // Отрисовка на экране
    window.draw(m_impl->background);
    window.draw(m_impl->titleText);
    window.draw(m_impl->scoreText);
    window.draw(m_impl->recordText);
    window.draw(m_impl->okButton);
    window.draw(m_impl->buttonText);
}
