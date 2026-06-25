#include "Game.hpp"
#include "GameWindow.hpp"
#include "StatusBar.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "SoundManager.hpp"
#include "MessageBox.hpp"
#include "MapManager.hpp"
#include "Portal.hpp"
#include "ConfigManager.hpp"
#include <SFML/Graphics/Font.hpp> 
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>




struct Game::Impl {
    GameWindow window;
    StatusBar  statusBar;
    Snake      snake;
    Food       food;
    MapManager mapManager; // [ДОБАВЛЕНО] Менеджер карты препятствий
    Portal     portal;     // [ДОБАВЛЕНО] Портал для телепортации змейки
    sf::View   gameView;
    std::unique_ptr<sf::Music> bgMusic; // [ДОБАВЛЕНО] Указатель на объект sf::Music для фоновой музыки
    float musicVolume = 40.f;           // [ДОБАВЛЕНО] Текущий уровень громкости фоновой музыки

      // [ДОБАВЛЕНО] Графический MessageBox
      std::unique_ptr<MessageBox> messageBox;
      sf::Font sharedFont; // Шрифт для нашего MessageBox


    Impl() : window(800, 640, "Piton Game (SFML 3.1.0)") {
        gameView.setSize({800.f, 600.f});
        gameView.setCenter({400.f, 300.f});
        gameView.setViewport(sf::FloatRect({0.f, 0.0625f}, {1.f, 0.9375f}));
        bgMusic = std::make_unique<sf::Music>();// [ДОБАВЛЕНО] Инициализируем указатель
        // Загружаем шрифт и передаем его в MessageBox
        if (sharedFont.openFromFile("res/font.ttf")) {
            messageBox = std::make_unique<MessageBox>(sharedFont);
        }


    }
};



Game::Game() 
    : m_impl(std::make_unique<Impl>())
    , m_state(GameState::Playing)
    , m_score(0)
    , m_currentLevel(1) // [ДОБАВЛЕНО] Инициализируем текущий уровень
    , m_timeSinceLastUpdate(sf::Time::Zero)
    , m_baseTimePerFrame(sf::seconds(1.f / 10.f)) // [ДОБАВЛЕНО] Запоминаем базовую скорость (10 шагов в сек)
{
// Внутри Game::Game()
m_timePerFrame = m_baseTimePerFrame;
m_effectTimer = sf::Time::Zero;

m_impl->food.spawnMultiple(25, 100, 100); // Спавним пул из 25 яблок
    ConfigManager::getInstance().load();
    SoundManager::getInstance().loadSound("eat", "res/eat_1.ogg");
    SoundManager::getInstance().loadSound("sgame", "res/doodle_pop.ogg");
    SoundManager::getInstance().loadSound("dead", "res/dead.ogg");

    // [ОБНОВЛЕНО] Генерируем препятствия для 1 уровня
    m_impl->mapManager.generateLevel(m_currentLevel);

    //m_impl->food.respawn(40, 30);
    // Спавним еду так, чтобы она не попала в камень (напишем цикл проверки)
    m_impl->food.spawnMultiple(25, 100, 100); // Спавним пул из 25 яблок
    // (Позже мы добавим в класс Food проверку на карту, чтобы еда не спавнилась в камнях)   
    
    // [ИСПРАВЛЕНО] Запускаем вашу стартовую музыку фоном по кругу
    if (m_impl->bgMusic->openFromFile("res/doodle_pop.ogg")) { 
        m_impl->bgMusic->setLooping(true); // Музыка будет автоматически начинаться сначала
        m_impl->bgMusic->setVolume(m_impl->musicVolume);   // 50% громкости, чтобы не оглушала
        m_impl->statusBar.setVolume(m_impl->musicVolume);
        m_impl->bgMusic->play();           // Поехали!
    }




}



Game::~Game() = default;



void Game::resetGame() {
        
    m_impl->portal.deactivate(); // [ДОБАВЛЕНО] Закрываем портал при полном перезапуске
    m_currentLevel = 1; // [ДОБАВЛЕНО] Сброс на 1 уровень
    m_impl->snake = Snake();                  // Теперь этот сброс работает без ошибок
    
    m_impl->mapManager.generateLevel(m_currentLevel);// [ОБНОВЛЕНО] Пересоздаем карту препятствий
    m_impl->food.spawnMultiple(25, 100, 100); // Пересоздаем 25 яблок
    m_score = 0;
    m_impl->statusBar.setScore(m_score);
    m_impl->statusBar.setGameOver(false);
    m_state = GameState::Playing;
    m_impl->bgMusic->setPitch(1.0f);
    m_impl->bgMusic->play();                    // [ДОБАВЛЕНО] Запускаем фоновую музыку
    m_impl->statusBar.setVolume(m_impl->musicVolume);

    // Сброс параметров скорости
    m_baseTimePerFrame = sf::seconds(1.f / 10.f);
    m_timePerFrame = m_baseTimePerFrame;
    m_effectTimer = sf::Time::Zero;    

    // [ДОБАВЛЕНО] Мгновенно телепортируем камеру на голову змейки при перезапуске
    const float tileSize = 20.f;
    sf::Vector2i headPos = m_impl->snake.getHeadPosition();
    m_impl->gameView.setCenter({ headPos.x * tileSize + tileSize / 2.f, headPos.y * tileSize + tileSize / 2.f });

}


void Game::run() {
    while (m_impl->window.isOpen()) {
        sf::Time deltaTime = m_clock.restart();
        m_timeSinceLastUpdate += deltaTime;

        // Отсчитываем время действия эффекта (Желтого или Оранжевого яблока)
        if (m_effectTimer > sf::Time::Zero) {
            m_effectTimer -= deltaTime;
            if (m_effectTimer <= sf::Time::Zero) {
                m_timePerFrame = m_baseTimePerFrame; // Возвращаем скорость
                m_impl->snake.setColorScheme(sf::Color(0, 190, 0), sf::Color(50, 255, 50)); // [ШАГ Б] Возвращаем зеленый цвет
            }
        }

        // Ваш отдельный метод обработки событий
        processEvents();

        while (m_timeSinceLastUpdate > m_timePerFrame) {
            m_timeSinceLastUpdate -= m_timePerFrame;
            update(m_timePerFrame);
        }

        // Плавный LERP камеры
        if (m_state == GameState::Playing) {
            const float tileSize = 20.f;
            sf::Vector2i headPos = m_impl->snake.getHeadPosition();
            sf::Vector2f targetCenter{ headPos.x * tileSize + tileSize / 2.f, headPos.y * tileSize + tileSize / 2.f };
            sf::Vector2f currentCenter = m_impl->gameView.getCenter();

            float interpolationFactor = 8.0f * deltaTime.asSeconds();
            if (interpolationFactor > 1.0f) interpolationFactor = 1.0f; 

            sf::Vector2f newCenter = currentCenter + (targetCenter - currentCenter) * interpolationFactor;
            m_impl->gameView.setCenter(newCenter);
        }

        render();
    }
}


void Game::processEvents() {
    while (const std::optional<sf::Event> event = m_impl->window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_impl->window.close();
        }

        // 1. БЛОК КЛАВИАТУРЫ
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_impl->window.close();
            }
            
            if (m_state == GameState::GameOver && keyPressed->code == sf::Keyboard::Key::Space) {
                resetGame();
            }

            // Клавиша '-' (Минус) — делаем тише
            if (keyPressed->code == sf::Keyboard::Key::Hyphen) {
                m_impl->musicVolume -= 5.f;
                if (m_impl->musicVolume < 0.f) m_impl->musicVolume = 0.f; 
                m_impl->bgMusic->setVolume(m_impl->musicVolume);
                m_impl->statusBar.setVolume(m_impl->musicVolume);
            }
            
            // Клавиша '=' (Равно/Плюс) — делаем громче
            if (keyPressed->code == sf::Keyboard::Key::Equal) {
                m_impl->musicVolume += 5.f;
                if (m_impl->musicVolume > 100.f) m_impl->musicVolume = 100.f; 
                m_impl->bgMusic->setVolume(m_impl->musicVolume);
                m_impl->statusBar.setVolume(m_impl->musicVolume);
            }

            if (m_state == GameState::GameOver) {
                if (keyPressed->code == sf::Keyboard::Key::Left) {
                    m_impl->musicVolume -= 5.f;
                    if (m_impl->musicVolume < 0.f) m_impl->musicVolume = 0.f;
                    m_impl->bgMusic->setVolume(m_impl->musicVolume);
                    m_impl->statusBar.setVolume(m_impl->musicVolume);
                }
                if (keyPressed->code == sf::Keyboard::Key::Right) {
                    m_impl->musicVolume += 5.f;
                    if (m_impl->musicVolume > 100.f) m_impl->musicVolume = 100.f;
                    m_impl->bgMusic->setVolume(m_impl->musicVolume);
                    m_impl->statusBar.setVolume(m_impl->musicVolume);
                }
            }
        } // <--- ТУТ СТРОГО ЗАКРЫВАЕТСЯ БЛОК КЛАВИАТУРЫ!

        // 2. [ПРОВЕРЬТЕ ТУТ] БЛОК МЫШКИ (должен быть отдельным, на уровне с keyPressed)
        if (m_state == GameState::GameOver) {
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    
                    // В SFML 3 координаты лежат прямо в mousePressed->position (это тип sf::Vector2i)
                    sf::Vector2f mouseWorld = m_impl->window.getRenderWindow().mapPixelToCoords(
                        mousePressed->position, 
                        m_impl->gameView
                    );

                    // Проверяем попадание по кнопке ОК
                    if (m_impl->messageBox && m_impl->messageBox->handleMouseClick(mouseWorld.x, mouseWorld.y)) {
                        resetGame(); // Рестарт!
                    }
                }
            }
        }
    }

    if (m_state == GameState::Playing) {
        m_impl->snake.handleInput();
    }
}



void Game::update(sf::Time deltaTime) {
    if (m_state == GameState::Playing) {
        // 1. Делаем шаг змейки вперед
        m_impl->snake.update();

        // 2. Проверяем столкновение с собой
        if (m_impl->snake.checkSelfCollision()) {
            m_impl->bgMusic->stop(); 
            SoundManager::getInstance().play("dead"); 
            // [ДОБАВЛЕНО] Проверяем и обновляем рекорд в JSON
            if (m_score > ConfigManager::getInstance().getHighScore()) {
                ConfigManager::getInstance().setHighScore(m_score);
            }
            // [ДОБАВЛЕНО] Загружаем свежие очки в MessageBox перед показом
            if (m_impl->messageBox) {
                m_impl->messageBox->setScores(m_score, ConfigManager::getInstance().getHighScore());
            }



            m_state = GameState::GameOver;
            m_impl->statusBar.setGameOver(true);
            return;
        }

        // 3. Проверяем столкновение со стенами (100х100)
        sf::Vector2i headPos = m_impl->snake.getHeadPosition();
        if (headPos.x < 0 || headPos.x >= 100 || headPos.y < 0 || headPos.y >= 100) {
            m_impl->bgMusic->stop(); 
            SoundManager::getInstance().play("dead"); 
            // [ДОБАВЛЕНО] Проверяем и обновляем рекорд в JSON
            if (m_score > ConfigManager::getInstance().getHighScore()) {
                ConfigManager::getInstance().setHighScore(m_score);
            }
           // [ДОБАВЛЕНО] Загружаем свежие очки в MessageBox перед показом
            if (m_impl->messageBox) {
                m_impl->messageBox->setScores(m_score, ConfigManager::getInstance().getHighScore());
            }




            m_state = GameState::GameOver;
            m_impl->statusBar.setGameOver(true);
            return;
        }
                // 3.1 [ДОБАВЛЕНО] СТОЛКНОВЕНИЕ С КАМНЯМИ НА КАРТЕ
        if (m_impl->mapManager.isCellOccupied(headPos)) {
            m_impl->bgMusic->stop(); 
            SoundManager::getInstance().play("dead"); 

            if (m_score > ConfigManager::getInstance().getHighScore()) {
                ConfigManager::getInstance().setHighScore(m_score);
            }
            if (m_impl->messageBox) {
                m_impl->messageBox->setScores(m_score, ConfigManager::getInstance().getHighScore());
            }

            m_state = GameState::GameOver;
            m_impl->statusBar.setGameOver(true);
            return; // Конец игры!
        }

        // 4. Проверяем, съела ли она яблоко
        const auto& apples = m_impl->food.getApples();
        for (size_t i = 0; i < apples.size(); ++i) {
            if (headPos == apples[i].position) {
                SoundManager::getInstance().play("eat");

                switch (apples[i].type) {
                    case Food::Type::SpeedBoost:
                        m_score += 25; 
                        m_timePerFrame = sf::seconds(1.f / 20.f); 
                        m_effectTimer = sf::seconds(5.f);         
                        m_impl->snake.grow();
                        // Стартовый золотой цвет перед началом мерцания
                        m_impl->snake.setColorScheme(sf::Color(255, 180, 0), sf::Color(255, 235, 50));
                        break;

                    case Food::Type::SlowDown:
                        m_score += 5;
                        m_timePerFrame = sf::seconds(1.f / 6.f);  
                        m_effectTimer = sf::seconds(5.f);         
                        m_impl->snake.grow();
                        m_impl->snake.setColorScheme(sf::Color(210, 90, 0), sf::Color(255, 130, 40));
                        break;

                    case Food::Type::Shrink:
                        m_score += 10;
                        m_impl->snake.shrink(); 
                        break;

                    case Food::Type::Normal:
                        m_score += 10;
                        m_impl->snake.grow();
                        break;
                }

                m_impl->statusBar.setScore(m_score);
                m_impl->food.replaceApple(i, 100, 100);

                // Базовое усложнение игры от счета
                float stepsPerSecond = 10.f + (m_score / 100) * 1.0f;
                if (stepsPerSecond > 25.f) stepsPerSecond = 25.f;
                m_baseTimePerFrame = sf::seconds(1.f / stepsPerSecond);

                if (m_effectTimer <= sf::Time::Zero) {
                    m_timePerFrame = m_baseTimePerFrame;
                }

                float newPitch = 1.0f + (m_score / 100) * 0.05f;
                if (newPitch > 1.5f) newPitch = 1.5f;
                m_impl->bgMusic->setPitch(newPitch);

                break; 
            }
        }

        // --- [ДОБАВЛЕНО СЮДА] МЕРЦАНИЕ СУПЕР-СКОРОСТИ В UPDATE ---
        // Так как метод update работает в игровом такте, мы можем плавно менять цвет змейки прямо здесь
        if (m_effectTimer > sf::Time::Zero && m_timePerFrame == sf::seconds(1.f / 20.f)) {

            // Используем внутреннее игровое время часов для синусоиды
            float time = m_clock.getElapsedTime().asSeconds();
            // Делаем пульсацию яркости желтого цвета
            int pulse = static_cast<int>((std::sin(time * 25.0f) + 1.0f) * 35.f); 
            
            m_impl->snake.setColorScheme(
                sf::Color(255, 150 + pulse, 0), 
                sf::Color(255, 190 + pulse, 50)
            );
        }


        // --- [ДОБАВЛЕНО] ЛОГИКА ПОРТАЛА ПЕРЕХОДА ---
        
        // 1. Проверяем условие открытия портала: набрано кратно 100 очков, и портал еще не активен
        // (Проверка m_score > 0 нужна, чтобы он не открывался в самом начале игры при 0 очков)
        if (m_score > 0 && m_score % 100 == 0 && !m_impl->portal.isActive()) {
            
            // Запускаем поиск безопасной точки для портала (чтобы он не спавнился внутри камня)
            bool portalSpawned = false;
            while (!portalSpawned) {
                m_impl->portal.spawn(100, 100);
                // Если точка не занята камнем — оставляем
                if (!m_impl->mapManager.isCellOccupied(m_impl->portal.getPosition())) {
                    portalSpawned = true;
                }
            }
            
            // Играем звук, извещающий, что портал на карте открылся!
            SoundManager::getInstance().play("sgame"); 
        }

        // 2. Проверяем, наступила ли гусеница на открытый портал
        if (m_impl->portal.isActive() && headPos == m_impl->portal.getPosition()) {
            
            m_currentLevel++; // Повышаем уровень!
            
            // Закрываем текущий портал
            m_impl->portal.deactivate();
            
            // Генерируем новую карту препятствий (камней станет больше!)
            m_impl->mapManager.generateLevel(m_currentLevel);
            
            // Переразмещаем яблоки на новой чистой карте
            m_impl->food.spawnMultiple(25, 100, 100);
            
            // Воспроизводим звук успешного перехода через телепорт
            SoundManager::getInstance().play("sgame");
            
            // Змейка (гусеница) сохраняет длину, но мы можем сбросить её позицию в центр (10, 10),
            // чтобы она гарантированно не оказалась внутри нового сгенерированного камня.
            // (Для этого можно было бы написать метод телепортации змейки, но пока оставим её 
            // ползти дальше, так как камни вокруг (10,10) защищены. Но для безопасности сбросим 
            // только еду и препятствия, а гусеница продолжит движение со своими сохраненными бонусами!)
        }






        

    }


}



void Game::renderGrid(sf::RenderWindow& window) {
    const int gridWidth = 100;  
    const int gridHeight = 100;
    const float tileSize = 20.f;
    const float worldSizeX = gridWidth * tileSize;
    const float worldSizeY = gridHeight * tileSize;

    // 1. Отрисовка однотонного тёмно-коричневого пола на всю карту (весит 1 объект вместо 10 000!)
    sf::RectangleShape floor({worldSizeX, worldSizeY});
    floor.setPosition({0.f, 0.f});
    floor.setFillColor(sf::Color(43, 29, 20)); // Красивый глубокий тёмно-коричневый цвет (Шоколадный/Земляной)
    window.draw(floor);

    // 2. Отрисовка прочной тёмно-серой границы (бордюра) по всему периметру карты
    sf::RectangleShape worldBorder({worldSizeX, worldSizeY});
    worldBorder.setPosition({0.f, 0.f});
    worldBorder.setFillColor(sf::Color::Transparent);
    
    // Задаем тёмно-серый цвет и толщину обводки
    worldBorder.setOutlineColor(sf::Color(70, 70, 70)); // Умеренно тёмно-серый
    worldBorder.setOutlineThickness(8.f);                // Сделали бордюр потолще (8 пикселей)
    
    window.draw(worldBorder);
}



void Game::render() {
    m_impl->window.clear(30, 30, 30);

    sf::RenderWindow& rawWindow = m_impl->window.getRenderWindow();
    
    // 1. Отрисовка статус-бара (использует фиксированный вид по умолчанию)
    rawWindow.setView(rawWindow.getDefaultView());
    m_impl->statusBar.render(rawWindow);

    // 2. [ОБНОВЛЕНО] Принудительно устанавливаем обновленный вид камеры для игры, 
    // центр которой изменился в методе update()
    rawWindow.setView(m_impl->gameView);




    if (m_state == GameState::Playing) {
        renderGrid(rawWindow); // [ДОБАВЛЕНО] Вызываем отрисовку шахматного поля и рамки
        m_impl->mapManager.render(rawWindow); // [ДОБАВЛЕНО] Рисуем камни на поле геймплея
        m_impl->portal.render(rawWindow); // [ДОБАВЛЕНО] Рисуем портал
        m_impl->food.render(rawWindow);
        m_impl->snake.render(rawWindow);
    } 
    else if (m_state == GameState::GameOver) {
        // [ОБНОВЛЕНО] Сначала рисуем поле на заднем плане, чтобы игра не исчезала совсем
        renderGrid(rawWindow);
        m_impl->mapManager.render(rawWindow);// [ДОБАВЛЕНО] Рисуем камни на фоне окна проигрыша
        m_impl->portal.render(rawWindow); // [ДОБАВЛЕНО] Рисуем портал
        m_impl->food.render(rawWindow);
        m_impl->snake.render(rawWindow);

        // А поверх него рисуем наш стильный MessageBox, передавая центр камеры
        sf::Vector2f cameraCenter = m_impl->gameView.getCenter();
        if (m_impl->messageBox) {
            m_impl->messageBox->render(rawWindow, cameraCenter.x, cameraCenter.y);
        }
    }

    m_impl->window.display();
}
