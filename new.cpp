#include <iostream>
#include <SFML/Graphics.hpp>
#include <sstream>
using namespace std;
using namespace sf;

class Quad_Tree
{
    int max_objects;
    int max_levels;
    int current_level;
    vector<Sprite> objs;
    FloatRect bounds;
    Quad_Tree *nodes[4];

public:
    Quad_Tree(int level, FloatRect bounds)
    {
        this->current_level = level;
        this->max_levels = 5;
        this->max_objects = 4;
        for (int i = 0; i < 4; i++)
        {
            nodes[i] = nullptr;
        }
        this->bounds = bounds;
    }
    void clear()
    {
        objs.clear();
        for (int i = 0; i < 4; i++)
        {
            if (nodes[i] != nullptr)
            {
                nodes[i]->clear();
                delete nodes[i];
                nodes[i] = nullptr;
            }
        }
    }
    void split_tree()
    {
        float mid_width = bounds.width / 2.f;
        float mid_height = bounds.height / 2.f;
        float x = bounds.left;
        float y = bounds.top;
        nodes[0] = new Quad_Tree(this->current_level + 1, FloatRect(x, y, mid_width, mid_height));
        nodes[1] = new Quad_Tree(this->current_level + 1, FloatRect(x + mid_width, y, mid_width, mid_height));
        nodes[2] = new Quad_Tree(this->current_level + 1, FloatRect(x, y + mid_height, mid_width, mid_height));
        nodes[3] = new Quad_Tree(this->current_level + 1, FloatRect(x + mid_width, y + mid_height, mid_width, mid_height));
    }
    int get_index(FloatRect obj_bounds)
    {
        int index = -1;
        float mid_width = bounds.width / 2;
        float mid_height = bounds.height / 2;
        float x = bounds.left;
        float y = bounds.top;
        float mid_horizontal_point = x + mid_width;
        float mid_vertical_point = y + mid_height;
        bool in_top_half = obj_bounds.top + obj_bounds.height < mid_vertical_point;
        bool in_bottom_half = obj_bounds.top > mid_vertical_point;
        bool in_left_half = obj_bounds.left + obj_bounds.width < mid_horizontal_point;
        bool in_right_half = obj_bounds.left > mid_horizontal_point;
        if (in_top_half && in_left_half)
        {
            index = 0;
        }
        else if (in_top_half && in_right_half)
        {
            index = 1;
        }
        else if (in_bottom_half && in_left_half)
        {
            index = 2;
        }
        else if (in_bottom_half && in_right_half)
        {
            index = 3;
        }
        return index;
    }

    void insert(Sprite obj)
    {
        if (nodes[0] != nullptr)
        {
            int index = get_index(obj.getGlobalBounds());
            if (index != -1)
            {
                nodes[index]->insert(obj);
                return;
            }
        }
        objs.push_back(obj);
        if (objs.size() > max_objects && current_level < max_levels)
        {
            if (nodes[0] == nullptr)
            {
                split_tree();
            }
            int i = 0;
            while (i < objs.size())
            {
                int index = get_index(objs[i].getGlobalBounds());
                if (index != -1)
                {
                    nodes[index]->insert(objs[i]);
                    objs.erase(objs.begin() + i);
                }
                else
                    i++;
            }
        }
    }

    vector<Sprite> retrieve(vector<Sprite> &return_objs, FloatRect obj_bounds)
    {
        int index = get_index(obj_bounds);
        if (index != -1 && nodes[0] != nullptr)
        {
            nodes[index]->retrieve(return_objs, obj_bounds);
        }
        return_objs.insert(return_objs.end(), objs.begin(), objs.end());
        return return_objs;
    }
};

class main_world
{
    Sprite background1;
    Sprite background2;
    Texture background;
    float speed;
    bool ismoving;
    void init_gui(Vector2u window_size)
    {
        this->background.loadFromFile("bg.png");
        Vector2u texture_size = this->background.getSize();
        float scale_x = static_cast<float>(window_size.x) / texture_size.x;
        float scale_y = static_cast<float>(window_size.y) / texture_size.y;
        this->background1.setScale({scale_x, scale_y});
        this->background2.setScale({scale_x, scale_y});
        this->background1.setTexture(this->background);
        this->background2.setTexture(this->background);
        this->background1.setPosition(0.f, 0.f);
        this->background2.setPosition(static_cast<int>(window_size.x), 0.f);
        this->ismoving = false;
    }

public:
    main_world(Vector2u window_size, float speed)
    {
        this->init_gui(window_size);
        this->speed = speed;
    }

    void update(float time)
    {
        if (!ismoving)
            return;
        this->background1.move(-this->speed * time, 0.f);
        this->background2.move(-this->speed * time, 0.f);
        if (background1.getPosition().x + background1.getGlobalBounds().width < 0)
        {
            background1.setPosition(background2.getPosition().x + background2.getGlobalBounds().width, 0);
        }
        if (background2.getPosition().x + background2.getGlobalBounds().width < 0)
        {
            background2.setPosition(background1.getPosition().x + background1.getGlobalBounds().width, 0);
        }
    }
    void render(RenderTarget &window)
    {
        window.draw(this->background1);
        window.draw(this->background2);
    }
    void set_moving(bool moving)
    {
        this->ismoving = moving;
    }
};

class PLayer
{
    Sprite sprite;
    Texture texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8;
    int current_frame;
    float animation_time;
    float frame_speed;
    int num_of_frames;
    Vector2f velocity;
    bool is_running;
    bool is_flying;
    float gravity;
    float ground_y;

public:
    PLayer()
    {
        this->texture1.loadFromFile("run1.png");
        this->texture2.loadFromFile("run2.png");
        this->texture3.loadFromFile("run3.png");
        this->texture4.loadFromFile("run4.png");
        this->texture5.loadFromFile("runstills.png");
        this->texture6.loadFromFile("jet2.png");
        this->texture7.loadFromFile("jet3.png");
        this->texture8.loadFromFile("jet4.png");
        this->num_of_frames = 4;
        this->animation_time = 0.0f;
        this->frame_speed = 0.1f;
        this->current_frame = 0;
        this->sprite.setTexture(this->texture5);
        this->sprite.setPosition(150.f, 500); // Set initial fixed position
        this->gravity = 500.f;
        this->velocity = {0.f, 0.f};
        is_running = false;
        this->is_flying = false;
        this->ground_y = 500.f;
    }

    Sprite getSprite()
    {
        return this->sprite;
    }

    void update(float deltaTime)
    {
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            is_flying = true;
            // this->sprite.setTexture(texture6);
            velocity.y = -300.f;
        }
        else
        {
            is_flying = false;
            if (this->sprite.getPosition().y < ground_y) // basically we want to apply gravity until it reaches the ground
            {
                velocity.y += gravity * deltaTime;
            }
            else
            {
                velocity.y = 0;
                this->sprite.setPosition(this->sprite.getPosition().x, ground_y);
            }
        }
        if (this->sprite.getPosition().y < 0)
        {
            this->sprite.setPosition(this->sprite.getPosition().x, 0);
            velocity.y = 0; // Stop further upward movement
        }
        if (is_flying)
        {
            animation_time += deltaTime;
            if (animation_time > frame_speed)
            {
                current_frame = (current_frame + 1) % 3;
                if (current_frame == 0)
                    this->sprite.setTexture(this->texture8);
                else if (current_frame == 1)
                    this->sprite.setTexture(this->texture7);
                else if (current_frame == 2)
                    this->sprite.setTexture(this->texture8);
                animation_time = 0.0f;
            }
        }
        if (!is_flying)
        {
            if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right))
            {
                is_running = true;
            }
            else
            {
                is_running = false;
                this->sprite.setTexture(this->texture5);
            }

            if (is_running)
            {
                // Update running animation
                animation_time += deltaTime;
                if (animation_time > frame_speed)
                {
                    current_frame = (current_frame + 1) % 4;
                    if (current_frame == 0)
                        this->sprite.setTexture(this->texture1);
                    else if (current_frame == 1)
                        this->sprite.setTexture(this->texture2);
                    else if (current_frame == 2)
                        this->sprite.setTexture(this->texture3);
                    else if (current_frame == 3)
                        this->sprite.setTexture(this->texture4);
                    animation_time = 0.0f;
                }
            }
        }

        sprite.move(0, velocity.y * deltaTime);
    }

    void render(RenderWindow *window)
    {
        window->draw(this->sprite);
    }
};

class Coin
{
    Sprite sprite;
    Texture texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9;
    int current_frame;
    float frame_speed;
    float animation_time;
    float speed;

public:
    Coin(Vector2f start_pos, float speed)
    {
        this->texture1.loadFromFile("coin1.png");
        this->texture2.loadFromFile("coin2.0.png");
        this->texture3.loadFromFile("coin4 (2).png");
        this->texture4.loadFromFile("coin5 (2).png");
        this->texture5.loadFromFile("coin6 (2).png");
        this->texture6.loadFromFile("coin7 (2).png");
        this->texture7.loadFromFile("coin8 (2).png");
        this->texture8.loadFromFile("coin9 (2).png");
        this->texture9.loadFromFile("coin10 (2).png");
        this->speed = speed;
        this->sprite.setTexture(this->texture1);
        this->sprite.setPosition(start_pos);
        this->sprite.setScale(0.75, 0.75);
        this->current_frame = 0;
        this->frame_speed = 0.1f;
        this->animation_time = 0.0f;
        // this->speed = speed;
        // this->is_moving_up = move_up;
    }
    Sprite getSprite()
    {
        return this->sprite;
    }
    void update(float delta_time, bool player_running)
    {
        animation_time += delta_time;
        if (animation_time >= frame_speed)
        {
            current_frame = (current_frame + 1) % 9;
            switch (current_frame)
            {
            case 0:
                this->sprite.setTexture(texture1);
                break;
            case 1:
                this->sprite.setTexture(texture2);
                break;
            case 2:
                this->sprite.setTexture(texture3);
                break;
            case 3:
                this->sprite.setTexture(texture4);
                break;
            case 4:
                this->sprite.setTexture(texture5);
                break;
            case 5:
                this->sprite.setTexture(texture6);
                break;
            case 6:
                this->sprite.setTexture(texture7);
                break;
            case 7:
                this->sprite.setTexture(texture8);
                break;
            case 8:
                this->sprite.setTexture(texture9);
                break;
            default:
                break;
            }
            animation_time = 0.0f;
        }
        if (player_running)
        {
            sprite.move(-speed * delta_time, 0.f);

            // // Check if the coin goes off-screen and reposition it (wrap around)
            if (sprite.getPosition().x + sprite.getGlobalBounds().width < 0)
            {
                sprite.setPosition(800, sprite.getPosition().y); // Reposition to the right edge
            }
        }
    }
    void render(RenderWindow *window)
    {
        window->draw(this->sprite);
    }
};

class Enemy
{
    Sprite sprite;
    Texture texture;
};

class Game
{
    RenderWindow *window;
    PLayer *player;
    vector<Coin> coins;
    Clock clock;
    main_world *world;
    Event ev;
    Quad_Tree *quad_tree;

    Font font;
    Text point_text;
    int points;

    Coin *removed_coin;

public:
    Game()
    {
        this->window = new RenderWindow(VideoMode(800, 700), "jetpack joyride");
        this->world = new main_world(window->getSize(), 200.f);
        this->player = new PLayer();
        this->quad_tree = new Quad_Tree(0, FloatRect(0.f, 0.f, window->getSize().x, window->getSize().y));
        this->removed_coin=nullptr;
        coins.push_back(Coin({200.f, 10.f}, 120.f));  // Coin in the upper line
        coins.push_back(Coin({250.f, 10.f}, 120.f));  // Coin in the upper line
        coins.push_back(Coin({300.f, 10.f}, 120.f));  // Coin in the upper line
        coins.push_back(Coin({350.f, 10.f}, 120.f));  // Coin in the middle line
        coins.push_back(Coin({700.f, 530.f}, 120.f)); // Coin in the lower line
        coins.push_back(Coin({750.f, 530.f}, 120.f)); // Coin in the lower line
        coins.push_back(Coin({650.f, 530.f}, 120.f)); // Coin in the lower line
        coins.push_back(Coin({790.f, 530.f}, 120.f)); // Coin in the lower line
        this->font.loadFromFile("ARCADE.TTF");
        this->point_text.setFont(this->font);
        this->point_text.setFillColor(Color::White);
        this->point_text.setPosition(600.f, 25.f);
        this->point_text.setCharacterSize(28);
        this->points = 0;
    }

    void poll_events()
    {
        while (window->pollEvent(ev))
        {
            if (ev.type == Event::Closed)
            {
                window->close();
            }
            else if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape)
            {
                window->close();
            }
        }
    }

    void update(float delta_time)
    {
        quad_tree->clear();
        for (auto &coin : coins)
        {
            quad_tree->insert(coin.getSprite());
        }
        vector<Sprite> possible_collisions;
        // Coin *removed_coin = nullptr;
        quad_tree->retrieve(possible_collisions, player->getSprite().getGlobalBounds());
        for (auto it = possible_collisions.begin(); it != possible_collisions.end(); it++)
        {
            if (player->getSprite().getGlobalBounds().intersects(it->getGlobalBounds()))
            {
                points += 10;
                removed_coin = new Coin(it->getPosition(), 120.f);
                auto coin_it = std::find_if(coins.begin(), coins.end(),
                                            [&](Coin &coin)
                                            { return coin.getSprite().getPosition() == it->getPosition(); });
                if (coin_it != coins.end())
                {
                    removed_coin = new Coin(coin_it->getSprite().getPosition(), 120.f);
                    coins.erase(coin_it); // Remove it from the main vector
                }
                break;
            }
        }

        // for (int i = 0; i < possible_collisions.size();)
        // {
        //     bool coin_removed=false;
        //     for (auto it = coins.begin(); it !=coins.end(); it++)
        //     {
        //         if (player->getSprite().getGlobalBounds().intersects(it->getSprite().getGlobalBounds()))
        //         {
        //             points+=10;
        //             removed_coin=new Coin(it->getSprite().getPosition(),120.f);
        //             coins.erase(it);
        //             coin_removed=true;
        //             break;
        //         }

        //     }
        //     if (!coin_removed)
        //     {
        //         i++;
        //     }

        // }

        bool player_is_running = Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right);
        this->world->set_moving(player_is_running);
        this->world->update(delta_time);
        // this->coin->update(delta_time);
        for (auto &coin : coins)
        {
            coin.update(delta_time, player_is_running);
        }
        // if (removed_coin)
        // {
        //     coins.push_back(*removed_coin);
        //     delete removed_coin; // Clean up temporary storage
        //     removed_coin = nullptr;
        // }
        this->player->update(delta_time);
        point_text.setString("Points: " + std::to_string(points));
    }

    void render_gui()
    {
        this->window->draw(this->point_text);
    }

    void render()
    {
        this->world->render(*this->window);
        for (auto &coin : coins)
        {
            coin.render(window);
        }
        this->player->render(this->window);
        this->render_gui();
        // coins.push_back(*removed_coin);

    }

    void run()
    {
        while (this->window->isOpen())
        {
            float delta_time = clock.restart().asSeconds();
            poll_events();
            update(delta_time);
            render();
            window->display();
        }
    }
};

int main()
{
    Game game;
    game.run();

    return 0;
}