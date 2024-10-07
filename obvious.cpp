// compilation requirements in README.md"

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

class Image
{
    static inline int selected_number_images{ 0 };

    bool selectable_by_an_ai{};
    bool selected{};

public:
    sf::Texture texture{};

    Image(bool ai_selectable)
        : selectable_by_an_ai{ ai_selectable }
        , selected{ false }
    {
    }

    static int get_selected_number_images()
    {
        return selected_number_images;
    }

    static void reset_selected_number_images()
    {
        selected_number_images = 0;
    }

    bool is_an_image_that_would_select_an_AI()
    {
        return selectable_by_an_ai;
    }

    bool is_selected()
    {
        return selected;
    }

    void toggle_image_selection()
    {
        selected ? --selected_number_images : ++selected_number_images;
        selected = !selected;
    }
};

class AiDetector
{
    static inline bool user_is_an_ai{ false };
    static inline int times_verified{ 0 };

public:
    static bool is_user_an_ai()
    {
        return user_is_an_ai;
    }

    static int get_times_verified()
    {
        return times_verified;
    }

    static void increase_times_verified()
    {
        ++times_verified;
    }

    static void set_user_as_ai()
    {
        user_is_an_ai = true;
    }
};

// is like a json
void select_image
(
    sf::RectangleShape &verify_box,
    sf::Text &verify_text,
    std::vector<sf::RectangleShape> &image_shapes,
    std::vector<Image> &images,
    sf::Event &event,
    const int index
)
{
    for (int i{ 0 }; i < 4; ++i)
    {
        if (image_shapes[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
        {
            if (images[index + i].is_selected())
            {
                image_shapes[i].setOutlineThickness(0.f);
            }
            else
            {
                image_shapes[i].setOutlineThickness(5.f);
            }

            images[index + i].toggle_image_selection();

            if (Image::get_selected_number_images() > 0)
            {
                verify_box.setFillColor(sf::Color{ 73, 128, 167 });
                verify_text.setFillColor(sf::Color::White);
            }
            else
            {
                verify_box.setFillColor(sf::Color{ 126, 170, 214 });
                verify_text.setFillColor(sf::Color{ 204, 218, 236 });
            }
        }
    }
}

void verify_verify_box
(
    sf::RectangleShape &verify_box,
    sf::Text &verify_text,
    sf::Text &instructions_text,
    std::vector<sf::RectangleShape> &image_shapes,
    std::vector<Image> &images,
    sf::Event &event,
    int &index
)
{
    if ((Image::get_selected_number_images() > 0)
        && verify_box.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
    {
        AiDetector::increase_times_verified();

        for (int i{ index }; i < index + 4; ++i)
        {
            if (images[i].is_selected())
            {
                if (images[i].is_an_image_that_would_select_an_AI())
                {
                    AiDetector::set_user_as_ai();
                }
            }

            if (AiDetector::get_times_verified() < 2)
            {
                image_shapes[i].setOutlineThickness(0.f);
                image_shapes[i].setTexture(&(images[i + 4].texture));

                instructions_text.setString("Which one would you eat?");
            }
        }

        Image::reset_selected_number_images();

        verify_box.setFillColor(sf::Color{ 126, 170, 214 });

        verify_text.setFillColor(sf::Color{ 204, 218, 236 });
        verify_text.setPosition(411.f, 46.f);
        verify_text.setString("VERIFY");

        index = 4;
    }
}

int main()
{
    std::random_device generator{};
    std::seed_seq seed
    {
        generator(), generator(), generator(), generator(), generator(), generator(), generator(),
        static_cast<std::seed_seq::result_type>
        (
            // using steady_clock being aware that they can make jumps in time
            std::chrono::steady_clock::now().time_since_epoch().count()
        )
    };

    std::mt19937 mt{ seed };

    int temporal_number{};
    std::vector<int> asset_selected{};
    while (asset_selected.size() < 3)
    {
        temporal_number = std::uniform_int_distribution{ 1, 11 } (mt);
        // oh no! a line of code over 100 characters in length
        if (std::find(asset_selected.begin(), asset_selected.end(), temporal_number) == asset_selected.end())
        {
            asset_selected.push_back(temporal_number);
        }
    }

    asset_selected.push_back(std::uniform_int_distribution{ 12, 18 } (mt));

    std::shuffle(asset_selected.begin(), asset_selected.end(), mt);

    int counter{ 0 };
    std::vector<Image> images{};
    for (int asset_number : asset_selected)
    {
        images.push_back(Image{ (asset_number < 12) });

        // another line more than 100 characters long :/
        if (!images[counter].texture.loadFromFile("assets\\images\\" + std::to_string(asset_number) + ".png"))
        {
            return 1;
        }

        ++counter;
    }

    // counter = 4
    for (int i{ 19 }; i <= 22; ++i)
    {
        images.push_back(Image{ (i < 22) });

        // this line is exactly 100 characters long, living on the edge
        if (!images[counter].texture.loadFromFile("assets\\images\\" + std::to_string(i) + ".png"))
        {
            return 1;
        }

        ++counter;
    }

    std::shuffle(images.begin() + 4, images.end(), mt);

    std::vector<sf::RectangleShape> image_shapes{};
    for (int i{0}; i < 4; ++i)
    {
        image_shapes.push_back
        (
            sf::RectangleShape{ sf::Vector2f{ 250.f, 250.f } }
        );

        // this one is also more than 100 characters long
        image_shapes[i].setPosition(sf::Vector2f{ (i % 2) ? 25.f : 300.f, ((i / 2) * 275.f) + 125.f });
        image_shapes[i].setTexture(&(images[i].texture));
        image_shapes[i].setOutlineColor(sf::Color{ 63, 100, 133 });
    }

    sf::RenderWindow window
    {
        sf::VideoMode{ 575, 675, sf::VideoMode::getDesktopMode().bitsPerPixel },
        "Obvious Captcha"
    };

    window.setVerticalSyncEnabled(true);

    sf::RectangleShape instructions_box{ sf::Vector2f{ 525.f, 75.f } };
    instructions_box.setFillColor(sf::Color{ 169, 198, 217 });
    instructions_box.setPosition(25.f, 25.f);

    sf::RectangleShape verify_box{ sf::Vector2f{ 150.f, 45.f } };
    verify_box.setFillColor(sf::Color{ 126, 170, 214 });
    verify_box.setPosition(385.f, 40.f);

    sf::RectangleShape translucent_box{ sf::Vector2f{ 575.f, 675.f } };
    translucent_box.setFillColor(sf::Color{ 255, 255, 255, 126 });

    sf::RectangleShape final_box{ sf::Vector2f{ 475.f, 250.f } };
    final_box.setFillColor(sf::Color{ 242, 242, 242 });
    final_box.setOutlineThickness(-5.f);
    final_box.setOutlineColor(sf::Color{ 52, 52, 52 });
    final_box.setPosition(50.f, 212.f);

    sf::Font font{};
    if (!font.loadFromFile("assets\\fonts\\comic.ttf"))
    {
        return 1;
    }

    sf::Text instructions_text{};
    instructions_text.setFillColor(sf::Color::White);
    instructions_text.setFont(font);
    instructions_text.setCharacterSize(26);
    instructions_text.setPosition(45.f, 46.f);
    instructions_text.setString("Which one is faster?");

    sf::Text verify_text{};
    verify_text.setFillColor(sf::Color{ 204, 218, 236 });
    verify_text.setFont(font);
    verify_text.setCharacterSize(26);
    verify_text.setPosition(422.f, 46.f);
    verify_text.setString("NEXT");

    sf::Text final_text{};
    final_text.setFillColor(sf::Color{ 52, 52, 52 });
    final_text.setFont(font);
    final_text.setCharacterSize(42);

    while (window.isOpen())
    {
        for (sf::Event event{}; window.pollEvent(event);)
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::MouseButtonPressed:
                if (AiDetector::get_times_verified() > 1)
                {
                    break;
                }

                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    static int index{ 0 };
                    select_image(verify_box, verify_text, image_shapes, images, event, index);
                    // which is worse? a line of 119 characters or adding 9 more lines with the json style
                    // that comment took 107 characters
                    verify_verify_box(verify_box, verify_text, instructions_text, image_shapes, images, event, index);
                }

                break;

            default:
                break;
            }
        }

        window.clear(sf::Color{ 242, 242, 242 });

        for (const sf::RectangleShape &shape : image_shapes)
        {
            window.draw(shape);
        }

        window.draw(instructions_box);
        window.draw(verify_box);

        window.draw(instructions_text);
        window.draw(verify_text);

        if (AiDetector::get_times_verified() == 2)
        {
            static bool configured_text{ false };
            if (!configured_text)
            {
                final_text.setString
                (
                    AiDetector::is_user_an_ai()
                        ? "AI DETECTED.\nOnly an AI would\nselect these images."
                        : "You have proven\nnot to be an AI."
                );

                final_text.setPosition
                (
                    (575.f - final_text.getGlobalBounds().width) / 2.f,
                    (650.f - final_text.getGlobalBounds().height) / 2.f
                );

                configured_text = true;
            }

            window.draw(translucent_box);
            window.draw(final_box);
            window.draw(final_text);
        }

        window.display();
    }

    return 0;
}