#include <iostream>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <queue>
#include <fstream>
using namespace std;
using namespace sf;

enum Game_state
{
    NAME_INPUT,
    MENU,
    PLAY,
    GAME_OVER,
    LEADERBOARD
};

struct Node
{
    Node *left;
    Node *right;
    int score;
    string player_name;
    Node(int s, string name) : left(nullptr), right(nullptr), score(s), player_name(name) {}
};

class BST
{
    Node *parent;

    Node *insert_node(Node *node, int score, string name)
    {
        if (!node)
            return new Node(score, name);

        if (name == node->player_name)
        {
            node->score = max(node->score, score);
        }
        else if (score < node->score)
        {
            node->left = insert_node(node->left, score, name);
        }
        else if (score > node->score)
        {
            node->right = insert_node(node->right, score, name);
        }
        return node;
    }
    void inorder_traversal(Node *root, vector<pair<int, string>> &scores)
    {
        if (root)
        {
            inorder_traversal(root->right, scores);
            scores.push_back({root->score, root->player_name});
            inorder_traversal(root->left, scores);
        }
    }
    void clear(Node *root)
    {
        if (root)
        {
            clear(root->left);
            clear(root->right);
            delete root;
        }
    }

public:
    BST() : parent(nullptr) {}

    void insert(int score, string name)
    {
        parent = this->insert_node(parent, score, name);
    }
    vector<pair<int, string>> get_scores()
    {
        vector<pair<int, string>> SCORES;
        this->inorder_traversal(parent, SCORES);
        return SCORES;
    }
    ~BST()
    {
        this->clear(this->parent);
    }
};


class SaveFile
{
    fstream savefile;

public:
    void save_file(vector<pair<int, string>> &scores)
    {
        savefile.open("LEADERBOARD.txt", ios::out);
        if (savefile.is_open())
        {
            for (const auto &score : scores)
            {
                savefile << score.second << " " << score.first << endl;
                cout << score.second << " " << score.first << endl;
            }
            savefile.close();
        }
    }
    void loadFile(BST &leaderboard)
    {
        savefile.open("LEADERBOARD.txt", ios::in);
        if (savefile.is_open())
        {
            string name;
            int score;
            while (savefile >> name >> score)
            {
                leaderboard.insert(score, name);
            }
            savefile.close();
        }
    }
};

class Menu
{
    Sprite background;
    Texture background_texture;

    RectangleShape text_box;
    bool is_active;
    string name;
    Font font;
    Text text;
    Text title_text;

    CircleShape go_button;
    ConvexShape arrow;

    RectangleShape button;
    Text leaderboard_button_text;
    void init_button()
    {
        this->leaderboard_button_text.setFont(this->font);
        this->leaderboard_button_text.setFillColor(Color::White);
        this->leaderboard_button_text.setCharacterSize(40);
        this->leaderboard_button_text.setPosition(150.f, 400.f);
        this->leaderboard_button_text.setString("LEADERBOARD");
        FloatRect getbounds = leaderboard_button_text.getGlobalBounds();

        this->button.setSize(Vector2f(getbounds.width + 20.f, getbounds.height + 10.f));
        this->button.setFillColor(Color(25, 25, 25, 200));
        this->button.setOutlineColor(Color::White);
        this->button.setOutlineThickness(1.f);
        this->button.setPosition(this->leaderboard_button_text.getPosition().x - 10.f,
                                 this->leaderboard_button_text.getPosition().y + 8.f);
    }
    void init_go_button()
    {
        go_button.setRadius(20.f);
        go_button.setFillColor(Color(25, 25, 25, 200));
        go_button.setOutlineColor(Color::White);
        go_button.setOutlineThickness(2.f);
        go_button.setOrigin(20.f, 20.f);
        go_button.setPosition(400.f, 400.f);
        arrow.setPointCount(3);
        arrow.setFillColor(Color::White);
        arrow.setOutlineColor(Color::Black);
        arrow.setPoint(0, Vector2f(0, -25));
        arrow.setPoint(1, Vector2f(12.5, 0));
        arrow.setPoint(2, Vector2f(-12.5, 0));
        arrow.setRotation(90);
        arrow.setOrigin(0, -25); // Set origin to top-center of the triangle
        arrow.setPosition(go_button.getPosition().x + 15.f, go_button.getPosition().y);
    }

    void init_text_box()
    {
        this->text_box.setSize(Vector2f(300.f, 50.f));
        this->text_box.setFillColor(Color(25, 25, 25, 200));
        this->text_box.setOutlineColor(Color::White);
        this->text_box.setOutlineThickness(1.f);
        this->text_box.setPosition(250.f, 300.f);

        this->text.setFont(font);
        this->text.setCharacterSize(20);
        this->text.setFillColor(Color::White);
        this->text.setPosition(260.f, 310.f);
        is_active = false;
    }
    void init_gui()
    {
        this->background_texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/last.jpg");
        this->background.setTexture(this->background_texture);
        this->font.loadFromFile("ARCADE.ttf");
        this->title_text.setFont(this->font);
        this->title_text.setCharacterSize(50);
        this->title_text.setFillColor(Color::White);
        this->title_text.setPosition(200.f, 200.f);
    }

public:
    Menu()
    {
        this->init_gui();
        this->init_button();
        this->init_go_button();
        this->init_text_box();
    }

    string getName()
    {
        return name;
    }

    void handle_event(Event ev, RenderWindow &window)
    {
        if (ev.type == Event::MouseButtonPressed)
        {
            if (text_box.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window))))
            {
                is_active = true;
                cout << "TEXT BOX ACTIVE" << endl;
            }
            else
            {
                is_active = false;
                cout << "TEXT BOX INACTIVE" << endl;
            }
        }
        else if (ev.type == Event::TextEntered && is_active)
        {
            if (ev.text.unicode == '\b' && !name.empty())
            {
                name.pop_back(); // Handle backspace to delete characters
            }
            else if (ev.text.unicode < 128 && ev.text.unicode != '\b')
            {
                name += static_cast<char>(ev.text.unicode); // Append character to name
            }
            text.setString(name); // Update text display
        }
    }

    bool is_Mouse_hover_leaderboardbutton(RenderWindow &window)
    {
        Vector2i mouse_pos = Mouse::getPosition(window);
        FloatRect getbounds = button.getGlobalBounds();
        return getbounds.contains(static_cast<Vector2f>(mouse_pos));
    }
    bool is_Mouse_hover(RenderWindow &window)
    {
        Vector2i mouse_pos = Mouse::getPosition(window);
        FloatRect getbounds = go_button.getGlobalBounds();
        return getbounds.contains(static_cast<Vector2f>(mouse_pos));
    }

    void update_color(RenderWindow &window)
    {
        if (is_Mouse_hover(window))
        {
            go_button.setFillColor(Color(50, 50, 50, 200));
        }
        else
        {
            go_button.setFillColor(Color(25, 25, 25, 200));
        }
    }

    void update_color_leader(RenderWindow &window)
    {
        if (is_Mouse_hover_leaderboardbutton(window))
        {
            button.setFillColor(Color(50, 50, 50, 200));
        }
        else
        {
            button.setFillColor(Color(25, 25, 25, 200));
        }
    }
    void render_input_screen(RenderWindow &window)
    {
        window.draw(this->background);
        this->title_text.setString("SPACE INVADERS");
        window.draw(this->title_text);
        window.draw(this->text_box);
        window.draw(this->text);
        update_color(window);
        window.draw(this->go_button);
        window.draw(this->arrow);
    }
    void render_welcome_screeen(RenderWindow &window)
    {
        window.draw(this->background);
        this->title_text.setString("WELCOME " + name);
        update_color(window);
        window.draw(this->title_text);
        window.draw(this->go_button);
        window.draw(this->arrow);
    }
    void render_game_over_screen(RenderWindow &window)
    {
        window.draw(this->background);
        this->title_text.setString("GAME OVER");
        update_color_leader(window);
        window.draw(this->title_text);
        window.draw(this->button);
        window.draw(this->leaderboard_button_text);
    }
};

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
    Texture texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, shooting_texture, lil_stomper1, lil_stomper2, lil_stomper_3;
    int current_frame;
    float animation_time;
    float frame_speed;
    int num_of_frames;
    Vector2f velocity;
    bool is_running;
    bool is_flying;
    float gravity;
    float ground_y;
    bool shooting;
    bool power_up;

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
        this->shooting_texture.loadFromFile("shooter3.png");
        this->lil_stomper1.loadFromFile("lil.png");
        this->lil_stomper2.loadFromFile("lil_stomper2.png");
        this->lil_stomper_3.loadFromFile("lil_stomper3.png");
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
        shooting = false;
        this->power_up = false;
    }

    void activate_power_up(int ch)
    {
        if (ch)
            this->power_up = true;
        else
            this->power_up = false;
    }
    Sprite getSprite()
    {
        return this->sprite;
    }

    void update(float deltaTime)
    {

        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            shooting = true;
        }
        else
            shooting = false;
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
        if (power_up)
        {
            sprite.setTexture(this->lil_stomper1);
            sprite.setScale({2.f, 2.f});
            animation_time += deltaTime;
            if (animation_time > 1.5f)
            {
                current_frame = (current_frame + 1) % 2;
                if (current_frame == 0)
                    this->sprite.setTexture(this->lil_stomper1);
                else if (current_frame == 1)
                {
                    this->sprite.setTexture(this->lil_stomper2);
                }
                animation_time = 0.f;
            }
        }
        if (power_up && is_flying)
        {
            sprite.setTexture(this->lil_stomper_3);
        }

        if (shooting && !power_up)
        {
            sprite.setTexture(this->shooting_texture);
        }

        if (is_flying && !power_up)
        {
            sprite.setScale({1.f, 1.f});
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
        if (!is_flying && !shooting && !power_up)
        {
            sprite.setScale({1.f, 1.f});
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
    float disappeear_time;
    bool is_visible;
    Clock visibility_timer;
    Vector2f initial_position;

public:
    int points;
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
        this->initial_position = start_pos;
        this->speed = speed;
        this->sprite.setTexture(this->texture1);
        this->sprite.setPosition(start_pos);
        this->sprite.setScale(0.75, 0.75);
        this->current_frame = 0;
        this->frame_speed = 0.1f;
        this->animation_time = 0.0f;
        this->is_visible = true;
        this->disappeear_time = 8.0f;
        this->points = 10;
    }
    void set_position()
    {
        this->sprite.setPosition(this->initial_position);
    }

    Vector2f get_initial_pos()
    {
        return this->initial_position;
    }
    Sprite getSprite()
    {
        return this->sprite;
    }

    void set_points(int multiplier, int ch)
    {
        if (ch)
            this->points = this->points * multiplier;
        else
            this->points = this->points / multiplier;
    }

    void update_visibility()
    {
        if (!is_visible && visibility_timer.getElapsedTime().asSeconds() > disappeear_time)
        {
            is_visible = true;
        }
    }
    void update(float delta_time, bool player_running)
    {
        if (is_visible)
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

                // // Check if the coin goes off-screen
                if (sprite.getPosition().x + sprite.getGlobalBounds().width < 0)
                {
                    sprite.setPosition(800, sprite.getPosition().y); // reset it to right edge
                }
            }
        }
        update_visibility();
    }

    bool return_visibility()
    {
        return is_visible;
    }
    void disappear_coin()
    {
        is_visible = false;
        visibility_timer.restart();
    }
    void render(RenderWindow *window)
    {
        if (is_visible)
        {
            window->draw(this->sprite);
        }
    }
};

class Bullet
{
    Sprite sprite;
    Texture texture;
    Vector2f direction;
    float movement_speed;

    void init_texture()
    {
        if (!this->texture.loadFromFile("shoot.png"))
        {
            cout << "COULDN'T OPEN THE IMAGE" << endl;
        };
    }

public:
    Sprite getSprite()
    {
        return this->sprite;
    }
    Bullet(float pos_x, float dx, float pos_y, float dy, float movement)
    {
        this->init_texture();
        this->sprite.setTexture(this->texture);
        // this->sprite.setScale(0.6f,0.6f);
        this->sprite.setPosition(pos_x, pos_y);
        this->direction.x = dx;
        this->direction.y = dy;
        this->movement_speed = movement;
    }

    FloatRect getbounds() const
    {
        return this->sprite.getGlobalBounds();
    }

    void update()
    {
        this->sprite.move(this->movement_speed * direction);
    }

    void render(RenderWindow &target)
    {
        target.draw(this->sprite);
    }
};

class Enemy
{
    Sprite sprite;
    Texture texture101, texture102, texture103, texture104, texture201, texture202, texture203, texture204;
    Vector2f position;
    int points;
    float speed;
    float animation_time;
    int current_frame;

public:
    Enemy(Vector2f start__pos)
    {
        this->texture101.loadFromFile("robo.png");
        this->texture102.loadFromFile("robo2.png");
        this->texture103.loadFromFile("robo3.png");
        this->texture104.loadFromFile("robo4.png");
        this->texture201.loadFromFile("robo.png");
        this->texture202.loadFromFile("robo2.png");
        this->texture203.loadFromFile("robo3.png");
        this->texture204.loadFromFile("robo4.png");
        this->animation_time = 0.0f;
        this->position = start__pos;
        this->current_frame = 0;
        int type = rand() % 2;
        switch (type)
        {
        case 0:
            sprite.setTexture(this->texture101);
            this->speed = 100.f;
            this->points = 10;
            break;
        case 1:
            sprite.setTexture(this->texture201);
            this->speed = 100.f;
            this->points = 20;
            break;
        default:
            break;
        }
        sprite.setPosition(position);
    }

    void update_animation(float delta_time, Texture &texture101, Texture &texture102, Texture &texture103, Texture &texture104)
    {
        animation_time += delta_time;
        if (animation_time > 0.1f)
        {
            current_frame = (current_frame + 1) % 4;
            if (current_frame == 0)
            {
                this->sprite.setTexture(texture101);
            }
            else if (current_frame == 2)
            {
                this->sprite.setTexture(texture102);
            }
            else if (current_frame == 3)
            {
                this->sprite.setTexture(texture103);
            }
            else if (current_frame == 4)
            {
                this->sprite.setTexture(texture104);
            }
            this->animation_time = 0.0f;
        }
    }
    Sprite getSprite()
    {
        return this->sprite;
    }
    void update(float delta_time)
    {
        if (this->points == 10)
        {
            update_animation(delta_time, texture101, texture102, texture103, texture104);
        }
        else if (this->points == 20)
        {
            update_animation(delta_time, texture201, texture202, texture203, texture204);
        }
        sprite.move(-speed * delta_time, 0);
    }
    void render(RenderWindow &window)
    {
        window.draw(this->sprite);
    }
};

class power_ups
{
public:
    float duration;
    string type;
    Sprite sprite;
    Texture coin_textrue, lil_stomp_texture;
    void init_gui()
    {
        if (!this->coin_textrue.loadFromFile("power1.png"))
        {
            cout << "couldnt load image" << endl;
        }
        this->lil_stomp_texture.loadFromFile("power2.png");
    }
    virtual void update()
    {
        this->sprite.move(-0.2f, 0);
    }
    virtual void render(RenderWindow &window)
    {
        window.draw(this->sprite);
    }
    Sprite getSprite()
    {
        return this->sprite;
    }
};

class coin_2x : public power_ups
{
public:
    coin_2x(Vector2f position)
    {
        this->init_gui();
        this->sprite.setTexture(this->coin_textrue);
        this->sprite.setPosition(position);
        type = "coin_booster";
        duration = 10.f;
    }
    void apply_effect(Coin *coin)
    {
        coin->set_points(2, 1);
    }
    void remove_effect(Coin *coin)
    {
        coin->set_points(2, 0);
    }
};

class lil_stomper : public power_ups
{
public:
    lil_stomper(Vector2f position)
    {
        this->init_gui();
        duration = 15.f;
        type = "lil_stomper";
        this->sprite.setTexture(this->lil_stomp_texture);
        this->sprite.setPosition(position);
    }
    void apply_effect(PLayer *player)
    {
        player->activate_power_up(1);
    }
    void remove_effect(PLayer *player)
    {
        player->activate_power_up(0);
    }
};

class Game
{
    RenderWindow *window;
    PLayer *player;
    vector<Coin> coins;
    Clock clock;
    main_world *world;
    Event ev;
    Quad_Tree *quad_tree_coins;
    Quad_Tree *quad_tree_enemies;
    Quad_Tree *quad_tree_bullets;
    Quad_Tree *quad_tree_powerups;
    vector<Bullet *> bullets;

    Font font;
    Text point_text;
    int points;

    int hp;
    int hp_max;
    RectangleShape hp_bar;
    RectangleShape hp_bar_back;
    Menu *menu;
    Game_state state;

    vector<Enemy *> enemies;
    float spawn_timer_max;
    float spawn_timer;

    queue<power_ups *> active_powerups;
    bool is_power_Active;
    float powerup_spawn_timer;
    float powerup_spawn_timer_max;
    float activation_time;
    vector<power_ups *> powers;
    power_ups *cur_power;
    bool islilstmp_active;

    BST leaderboard;
    SaveFile file;
    Sprite background;
    Texture background_texture;
    void spawn_powerups()
    {
        powerup_spawn_timer += 1.f;
        if (powerup_spawn_timer >= powerup_spawn_timer_max)
        {

            int random_index = rand() % 2;
            powerup_spawn_timer = 0.f;
            if (random_index == 1)
            {
                powers.push_back(new coin_2x(Vector2f(this->player->getSprite().getPosition().x + 200.f + rand() % 200, player->getSprite().getPosition().y - 200.f - rand() % 250)));
            }
            else
            {
                powers.push_back(new lil_stomper(Vector2f(this->player->getSprite().getPosition().x + 200.f + rand() % 200, player->getSprite().getPosition().y - 200.f - rand() % 250)));
            }
        }
    }

    void render_leaderboard()
    {
        this->window->draw(this->background);
        Text title("Leaderboard", this->font, 50);
        title.setFillColor(Color::White);
        title.setPosition(200.f, 200.f);
        this->window->draw(title);
        vector<pair<int, string>> scores = this->leaderboard.get_scores();
        float y_position = 350.f;

        for (const auto &score : scores)
        {
            Text scoreText;
            scoreText.setFont(this->font);
            scoreText.setCharacterSize(30);
            scoreText.setFillColor(Color::White);
            scoreText.setString(score.second + " : " + to_string(score.first));
            scoreText.setPosition(100.f, y_position);
            this->window->draw(scoreText);
            y_position += 40.f;
        }
    }
    void init_hp_bar()
    {
        this->hp_bar.setSize(Vector2f(200.f, 25.f));
        this->hp_bar.setFillColor(Color::Blue);
        this->hp_bar.setPosition(Vector2f(20.f, 20.f));
        this->hp_bar_back = this->hp_bar;
        this->hp_bar_back.setFillColor(Color(25, 25, 25, 200));
    }

public:
    Game()
    {
        this->window = new RenderWindow(VideoMode(800, 700), "jetpack joyride");
        this->world = new main_world(window->getSize(), 200.f);
        this->player = new PLayer();
        this->quad_tree_coins = new Quad_Tree(0, FloatRect(0.f, 0.f, window->getSize().x, window->getSize().y));
        this->quad_tree_enemies = new Quad_Tree(0, FloatRect(0.f, 0.f, window->getSize().x, window->getSize().y));
        this->quad_tree_bullets = new Quad_Tree(0, FloatRect(0.f, 0.f, window->getSize().x, window->getSize().y));
        this->quad_tree_powerups = new Quad_Tree(0, FloatRect(0.f, 0.f, window->getSize().x, window->getSize().y));
        this->menu = new Menu();
        this->state = NAME_INPUT;
        this->activation_time = 0.f;
        this->powerup_spawn_timer = 0.f;
        this->powerup_spawn_timer_max = 5000.f;
        this->cur_power = nullptr;
        this->islilstmp_active = false;
        this->is_power_Active = false;
        this->background_texture.loadFromFile("C:/Users/sairam/Documents/data structures/graph/Texture/space3.jpeg");
        this->background.setTexture(this->background_texture);
        coins.push_back(Coin({200.f, 10.f}, 120.f));  // Coin in the upper line
        coins.push_back(Coin({250.f, 10.f}, 120.f));  // Coin in the upper line
        coins.push_back(Coin({300.f, 10.f}, 120.f));  // Coin in the upper line
        coins.push_back(Coin({350.f, 10.f}, 120.f));  // Coin in the middle line
        coins.push_back(Coin({350.f, 250.f}, 120.f)); // Coin in the middle line
        coins.push_back(Coin({350.f, 300.f}, 120.f)); // Coin in the middle line
        coins.push_back(Coin({400.f, 250.f}, 120.f)); // Coin in the middle line
        coins.push_back(Coin({400.f, 300.f}, 120.f)); // Coin in the middle line
        coins.push_back(Coin({350.f, 350.f}, 120.f)); // Coin in the middle line
        coins.push_back(Coin({700.f, 530.f}, 100.f)); // Coin in the lower line
        coins.push_back(Coin({750.f, 530.f}, 100.f)); // Coin in the lower line
        coins.push_back(Coin({650.f, 530.f}, 100.f)); // Coin in the lower line
        coins.push_back(Coin({790.f, 530.f}, 100.f)); // Coin in the lower line
        this->font.loadFromFile("ARCADE.TTF");
        this->point_text.setFont(this->font);
        this->point_text.setFillColor(Color::White);
        this->point_text.setPosition(600.f, 25.f);
        this->point_text.setCharacterSize(28);
        this->points = 0;
        this->spawn_timer = 0.f;
        this->spawn_timer_max = 5000.f;
        this->hp = 50;
        this->hp_max = 50;
        this->init_hp_bar();
        this->file.loadFile(leaderboard);
    }

    void replace_coins(Coin &coin)
    {
        coin.set_position();
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
            switch (state)
            {
            case NAME_INPUT:
                this->menu->handle_event(ev, *window);
                if (ev.type == Event::MouseButtonPressed && this->ev.mouseButton.button == Mouse::Left)
                {
                    if (this->menu->is_Mouse_hover(*window))
                    {
                        this->state = MENU;
                    }
                }

                break;
            case MENU:
                if (ev.type == Event::MouseButtonPressed && this->ev.mouseButton.button == Mouse::Left)
                {
                    if (this->menu->is_Mouse_hover(*window))
                    {
                        this->state = PLAY;
                    }
                }
                break;
            case PLAY:
                if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Space)
                {
                    this->bullets.push_back(new Bullet(this->player->getSprite().getPosition().x + this->player->getSprite().getGlobalBounds().width / 2.f,
                                                       1.f, this->player->getSprite().getPosition().y + 10, 0.f, 0.5f));
                }
                break;
            case GAME_OVER:
                if (ev.type == Event::MouseButtonPressed && this->ev.mouseButton.button == Mouse::Left)
                {
                    if (this->menu->is_Mouse_hover_leaderboardbutton(*window))
                    {
                        this->state = LEADERBOARD;
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    void spawn_enemies()
    {
        this->spawn_timer += 2.0f;
        if (this->spawn_timer >= this->spawn_timer_max)
        {
            enemies.push_back(new Enemy({this->player->getSprite().getPosition().x + 100.f + rand() % 200, player->getSprite().getPosition().y + 20.f}));
            this->spawn_timer = 0.f;
        }
    }

    void update_bullets()
    {
        for (int i = 0; i < bullets.size(); i++)
        {
            bullets[i]->update();
            if (bullets[i]->getbounds().left + bullets[i]->getbounds().width > 800.f)
            {
                delete this->bullets[i];
                this->bullets.erase(this->bullets.begin() + i);
            }
        }
    }

    void update_gui()
    {
        float hp_percent = static_cast<float>(this->hp) / static_cast<float>(this->hp_max);
        hp_bar.setSize({200.f * hp_percent, hp_bar.getSize().y});
    }

    void update_coins(vector<Sprite> &possible_collisions)
    {
        for (auto it = possible_collisions.begin(); it != possible_collisions.end(); it++)
        {
            if (player->getSprite().getGlobalBounds().intersects(it->getGlobalBounds()))
            {
                auto coin_it = std::find_if(coins.begin(), coins.end(),
                                            [&](Coin &coin)
                                            { return coin.getSprite().getPosition() == it->getPosition(); });
                this->points += coin_it->points;
                if (coin_it != coins.end())
                {
                    coin_it->disappear_coin();
                    this->replace_coins(*coin_it);
                }
                break;
            }
        }
    }

    void update_enemies(vector<Sprite> &possible_collisions_with_enemies)
    {
        for (auto it = possible_collisions_with_enemies.begin(); it != possible_collisions_with_enemies.end(); it++)
        {
            if (player->getSprite().getGlobalBounds().intersects(it->getGlobalBounds()))
            {

                if (!islilstmp_active)
                    hp -= 10;
                auto enemy_it = std::find_if(enemies.begin(), enemies.end(), [&](Enemy *enemy)
                                             { return enemy->getSprite().getPosition() == it->getPosition(); });
                if (enemy_it != enemies.end())
                {
                    enemies.erase(enemy_it);
                }

                break;
            }
        }
    }

    void update_powerups()
    {
        for (auto &power : powers)
        {
            power->update();
            if (power->sprite.getGlobalBounds().left < 0.f)
            {
                powers.erase(find(powers.begin(), powers.end(), power));
            }
        }
    }

    void render_powerups(RenderWindow &window)
    {
        for (auto &power : powers)
        {
            power->render(window);
        }
    }

    void update_powers(vector<Sprite> &possible_collisions_with_powers)
    {
        for (auto it = possible_collisions_with_powers.begin(); it != possible_collisions_with_powers.end(); it++)
        {
            if (player->getSprite().getGlobalBounds().intersects(it->getGlobalBounds()))
            {
                auto power_it = find_if(powers.begin(), powers.end(), [&](power_ups *power)
                                        { return power->getSprite().getPosition() == it->getPosition(); });
                active_powerups.push(*power_it);
                powers.erase(power_it);
                break;
            }
        }
        queue_handler();
    }

    void queue_handler()
    {
        if (!active_powerups.empty() && !is_power_Active)
        {
            is_power_Active = true;
            cur_power = active_powerups.front();
            active_powerups.pop();
        }
        if (cur_power)
        {
            activate_power_ups(cur_power);
        }
    }

    void activate_power_ups(power_ups *power)
    {
        if (activation_time == 0.f)
        {
            if (lil_stomper *stompPower = dynamic_cast<lil_stomper *>(power))
            {
                stompPower->apply_effect(player);
                this->islilstmp_active = true;
                cout << "Activated lil stomp   booster effect" << endl;
            }
            else if (coin_2x *coinPower = dynamic_cast<coin_2x *>(power))
            {
                for (auto &coin : coins)
                {
                    coinPower->apply_effect(&coin);
                    cout << "Activated coin booster effect" << endl;
                }
            }
        }
        activation_time += 0.0005f;
        if (activation_time >= power->duration)
        {
            is_power_Active = false;
            activation_time = 0.f;
            if (coin_2x *coin_power = dynamic_cast<coin_2x *>(power))
            {
                for (auto &coin : coins)
                {
                    coin_power->remove_effect(&coin);
                    cout << "removed coin booster effect" << endl;
                }
            }
            else if (lil_stomper *stompPower = dynamic_cast<lil_stomper *>(power))
            {
                stompPower->remove_effect(player);
                this->islilstmp_active = false;
                cout << "removed lil stomper effect" << endl;
            }
            cur_power = nullptr;
        }
    }

    void collision_with_bullet(vector<Sprite> &possible_collisions_with_bullets)
    {
        for (auto &bullet : bullets)
        {
            for (auto it = possible_collisions_with_bullets.begin(); it != possible_collisions_with_bullets.end(); ++it)
            {
                if (bullet->getbounds().intersects(it->getGlobalBounds()))
                {
                    auto enemy_it = std::find_if(enemies.begin(), enemies.end(),
                                                 [&](Enemy *enemy)
                                                 { return enemy->getSprite().getPosition() == it->getPosition(); });
                    if (enemy_it != enemies.end())
                    {
                        enemies.erase(enemy_it);
                    }

                    auto bullet_it = std::find(bullets.begin(), bullets.end(), bullet);
                    if (bullet_it != bullets.end())
                    {
                        delete *bullet_it;
                        bullets.erase(bullet_it);
                    }

                    break;
                }
            }
        }
    }

    void update(float delta_time)
    {
        if (state == PLAY)
        {
            this->spawn_enemies();
            this->spawn_powerups();
            this->update_bullets();

            //------------------------QUAD TREE-------------------------------
            // clearing quad tree for next frame
            quad_tree_coins->clear();
            quad_tree_bullets->clear();
            quad_tree_enemies->clear();
            quad_tree_powerups->clear();
            // insert the objects into quad tree , created separate objects of quad tree for coins and enemies
            for (auto &coin : coins)
            {
                if (coin.return_visibility())
                    quad_tree_coins->insert(coin.getSprite());
            }
            for (auto &enemy : enemies)
            {
                quad_tree_enemies->insert(enemy->getSprite());
            }
            for (auto &power : powers)
            {
                quad_tree_powerups->insert(power->sprite);
            }

            vector<Sprite> possible_collisions;
            vector<Sprite> possible_collisions_with_enemies;
            vector<Sprite> possible_collisions_with_bullets;
            vector<Sprite> possible_collisions_with_powerups;

            // retreival----------------
            quad_tree_coins->retrieve(possible_collisions, player->getSprite().getGlobalBounds());
            quad_tree_enemies->retrieve(possible_collisions_with_enemies, player->getSprite().getGlobalBounds());
            for (auto &bullet : bullets)
            {
                quad_tree_enemies->retrieve(possible_collisions_with_bullets, bullet->getSprite().getGlobalBounds());
            }
            for (auto &power : powers)
            {
                quad_tree_powerups->retrieve(possible_collisions_with_powerups, player->getSprite().getGlobalBounds());
            }
            //----------------------------------------------------------------------------------------------

            this->update_coins(possible_collisions);
            this->update_powers(possible_collisions_with_powerups);
            this->update_enemies(possible_collisions_with_enemies);
            this->collision_with_bullet(possible_collisions_with_bullets);

            bool player_is_running = Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right);
            this->world->set_moving(player_is_running);
            this->world->update(delta_time);

            for (int i = 0; i < enemies.size(); i++)
            {
                enemies[i]->update(delta_time);
                if (enemies[i]->getSprite().getGlobalBounds().left < 0.f)
                {
                    delete this->enemies[i];
                    this->enemies.erase(this->enemies.begin() + i);
                }
            }
            this->update_powerups();

            for (auto &coin : coins)
            {
                coin.update(delta_time, player_is_running);
            }

            this->player->update(delta_time);
            if (this->hp <= 0)
            {
                this->state = GAME_OVER;
                leaderboard.insert(this->points, menu->getName());
                vector<pair<int, string>> scores = leaderboard.get_scores();
                file.save_file(scores);
            }
            this->update_gui();
            point_text.setString("Points: " + std::to_string(points));
        }
    }

    void render_gui()
    {
        this->window->draw(this->point_text);
        this->window->draw(this->hp_bar_back);
        this->window->draw(this->hp_bar);
    }

    void render()
    {
        this->window->clear();
        if (state == NAME_INPUT)
        {
            this->menu->render_input_screen(*window);
        }
        else if (state == MENU)
        {
            this->menu->render_welcome_screeen(*window);
        }
        else if (state == PLAY)
        {
            this->world->render(*this->window);
            for (auto &coin : coins)
            {
                coin.render(window);
            }
            this->render_powerups(*window);
            for (int i = 0; i < bullets.size(); i++)
            {
                bullets[i]->render(*window);
            }
            for (int i = 0; i < enemies.size(); i++)
            {
                enemies[i]->render(*window);
            }

            this->player->render(this->window);
            this->render_gui();
        }
        else if (state == GAME_OVER)
        {
            menu->render_game_over_screen(*window);
        }
        else if (state == LEADERBOARD)
        {
            this->render_leaderboard();
        }
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
