#include <SFML/Graphics.hpp>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <math.h>

using namespace std;

#include "engine.cpp"
#include "logic_functions.h"
//#include "keys.cpp"

int sq(int a) { return a*a; }

int main(int argc, char **argv)
{
	elements.push_back(Element(50, 50, 100, 200, 7, 1, AND));

	sf::RenderWindow main_window(sf::VideoMode(1400, 900), "Logic Schemes");

	int where_mouse_was_pressed_x, where_mouse_was_pressed_y;
	vector<Element*> dragging_elements;

	while (main_window.isOpen())
	{
		sf::Event event;
		while (main_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) main_window.close();
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					for (int i = 0; i < elements.size(); i++)
						if (event.mouseButton.x < elements[i].self_graphics.getPosition().x + elements[i].x_size && event.mouseButton.x > elements[i].self_graphics.getPosition().x && event.mouseButton.y < elements[i].self_graphics.getPosition().y + elements[i].y_size && event.mouseButton.y > elements[i].self_graphics.getPosition().y)
						{
							for (int j = 0; j < elements[i].inputs_graphics.size(); j++)
								if (sq(elements[i].inputs_graphics[j].getPosition().x + elements[i].inputs_graphics[j].getRadius() + elements[i].self_graphics.getPosition().x - event.mouseButton.x) + sq(elements[i].inputs_graphics[j].getPosition().y + elements[i].inputs_graphics[j].getRadius() + elements[i].self_graphics.getPosition().y - event.mouseButton.y) <= sq(elements[i].inputs_graphics[j].getRadius()))
								{
									elements[i].inputs_graphics[j].setFillColor(elements[i].inputs_graphics[j].getOutlineColor());
									break;
								}
							where_mouse_was_pressed_x = event.mouseButton.x; where_mouse_was_pressed_y = event.mouseButton.y;
							dragging_elements.push_back(&elements[i]);
							elements[i].from_where_dragging_x = elements[i].self_graphics.getPosition().x; elements[i].from_where_dragging_y = elements[i].self_graphics.getPosition().y;
							break;
						}
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
					for (int i = 0; i < dragging_elements.size(); i++) dragging_elements.pop_back();
			}
			if (event.type == sf::Event::MouseMoved)
			{
				if (!dragging_elements.empty())
					for (int i = 0; i < dragging_elements.size(); i++)
						dragging_elements[i]->self_graphics.setPosition(dragging_elements[i]->from_where_dragging_x - where_mouse_was_pressed_x + event.mouseMove.x, dragging_elements[i]->from_where_dragging_y - where_mouse_was_pressed_y + event.mouseMove.y);
			}
		}

		main_window.clear();

		for (int i = 0; i < elements.size(); i++)
		{
			main_window.draw(elements[i].self_graphics);

			for (int j = 0; j < elements[i].inputs_graphics.size(); j++)
			{
				elements[i].inputs_graphics[j].move(elements[i].self_graphics.getPosition());
				main_window.draw(elements[i].inputs_graphics[j]);
				elements[i].inputs_graphics[j].move(-elements[i].self_graphics.getPosition());
			}
			for (int j = 0; j < elements[i].outputs_graphics.size(); j++)
			{
				elements[i].outputs_graphics[j].move(elements[i].self_graphics.getPosition());
				main_window.draw(elements[i].outputs_graphics[j]);
				elements[i].outputs_graphics[j].move(-elements[i].self_graphics.getPosition());
			}
		}

		main_window.display();
	}
	return 0;
}
