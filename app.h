#ifndef APP_H
#define APP_H

#include <math.h>
#include <memory>
#include <SFML/Graphics.hpp>
#include "baserender.h"


class App {
public:
    App(unsigned int width, unsigned int height);
    void run();
    void set_render(std::unique_ptr<BaseRender> && render);
private:
    unsigned int _width, _height;
    sf::RenderWindow _window;
    sf::Texture _texture;
    sf::Sprite _sprite;
    std::unique_ptr<BaseRender> _render_p;
    unsigned int _elapsed_frames = 0;
    unsigned long long _frame_time = 0;
    void render_image();
    void process_events();
};

#endif // APP_H
