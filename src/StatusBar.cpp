#include "StatusBar.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <string>
#include <SFML/System/Clock.hpp> // [ДОБАВЛЕНО] Подключаем таймер для мигания
#include <cmath>                 // [ДОБАВЛЕНО] Подключаем математику для функции sin
#include <cstdint>




struct StatusBar::Impl {
    sf::Font font;
    sf::Text scoreText;
    sf::RectangleShape background;
    sf::Text m_volumeText;  // [ДОБАВЛЕНО] Текст для отображения громкости
    sf::Clock blinkClock;        // [ДОБАВЛЕНО] Таймер, который считает время с начала игры

    // В SFML 3 инициализируем текст СТРОГО в списке инициализации через двоеточие
    Impl() 
        : scoreText(font, "PITON GAME", 18) 
        , m_volumeText(font, "Volume: 40%", 18)
    {
        // В SFML 3 вместо loadFromFile используется openFromFile
        if (!font.openFromFile("res/font.ttf")) {
            // Ошибка обработается автоматически, SFML выведет предупреждение в консоль
        }

        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition({15.f, 8.f});

        // [ДОБАВЛЕНО] Сдвигаем текст громкости в правый угол статус-бара
        m_volumeText.setFillColor(sf::Color(200, 200, 200)); // Делаем чуть серее, чтобы не отвлекал
        m_volumeText.setPosition({650.f, 8.f});



        background.setSize({800.f, 40.f});
        background.setFillColor(sf::Color(45, 45, 45));
    }
};

StatusBar::StatusBar() 
    : m_impl(std::make_unique<Impl>()), m_score(0), m_isGameOver(false) 
{}

StatusBar::~StatusBar() = default;

void StatusBar::setScore(int score) {
    m_score = score;
}

void StatusBar::setGameOver(bool isGameOver) {
    m_isGameOver = isGameOver;
}


// [ДОБАВЛЕНО] Реализация метода изменения текста громкости
void StatusBar::setVolume(float volume) {
    int volInt = static_cast<int>(volume);
    m_impl->m_volumeText.setString("Volume: " + std::to_string(volInt) + "%");
    
}




void StatusBar::render(sf::RenderWindow& window) {
    window.draw(m_impl->background);

    if (m_isGameOver) {
        m_impl->scoreText.setString("GAME OVER!  Score: " + std::to_string(m_score) + "  [Press Space to Restart]");
        m_impl->scoreText.setFillColor(sf::Color(255, 100, 100));


        // --- [ДОБАВЛЕНО] ЛОГИКА ПЛАВНОГО МИГАНИЯ ---
        // Получаем прошедшее время в секундах
        float time = m_impl->blinkClock.getElapsedTime().asSeconds();
        
        // Функция sin(time * скорость) колеблется от -1.0 до 1.0. 
        // Умножение на 5.0f делает мигание более динамичным.
        // Преобразуем этот диапазон в 0.0 - 1.0:
        float alphaFactor = (std::sin(time * 5.0f) + 1.0f) / 2.0f;
        
        // Переводим в диапазон для SFML (от 0 до 255)
        std::uint8_t alpha = static_cast<std::uint8_t>(alphaFactor * 255.f);
        
        // Применяем полученную прозрачность к красному цвету текста
        m_impl->scoreText.setFillColor(sf::Color(255, 100, 100, alpha));




    } else {
        m_impl->scoreText.setString("PITON GAME  |  Score: " + std::to_string(m_score));
        m_impl->scoreText.setFillColor(sf::Color::White);
    }

    window.draw(m_impl->scoreText);
    window.draw(m_impl->m_volumeText);

}

