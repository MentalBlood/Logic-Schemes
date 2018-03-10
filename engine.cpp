bool* boolinter(bool value)
{
	bool **b = (bool**)malloc(sizeof(bool*));
	*b = (bool*)malloc(sizeof(bool));
	**b = value;
	return *b;
}

void color_circle_from_spectre(sf::CircleShape *circle, int color_number, int colors_total)
{
	color_number = double(color_number) / double(colors_total) * 1275;
	if (color_number <= 255) circle->setOutlineColor(sf::Color(255, int(color_number), 0));
	else if (color_number <= 510) circle->setOutlineColor(sf::Color(510 - int(color_number), 255, 0));
	else if (color_number <= 765) circle->setOutlineColor(sf::Color(0, 255, int(color_number) - 510));
	else if (color_number <= 1020) circle->setOutlineColor(sf::Color(0, 1020 - int(color_number), 255));
	else circle->setOutlineColor(sf::Color(int(color_number) - 1020, 0, 255));
}

class Element
{
	private:
		vector<Element*> input_elements;
		vector<bool*> inputs_values, outputs_values;
		bool evaluated;
		void (*function)(vector<bool*> &, vector<bool*> &);

	public:
		int x_from_where_dragging, y_from_where_dragging;
		int x, y, x_size, y_size, from_where_dragging_x, from_where_dragging_y;
		vector<sf::CircleShape> inputs_graphics, outputs_graphics;
		sf::RectangleShape self_graphics;

		Element(double x, double y, double x_size, double y_size, int inputs_number, int outputs_number, void (*function)(vector<bool*> &, vector<bool*> &)):
			x_size(x_size), y_size(y_size), function(function)
		{
			for (int i = 0; i < inputs_number; i++) inputs_values.push_back(boolinter(0));
			for (int i = 0; i < outputs_number; i++) outputs_values.push_back(boolinter(0));
			evaluated = 0;
			self_graphics.setPosition(x, y); self_graphics.setSize(sf::Vector2f(x_size, y_size));
			self_graphics.setFillColor(sf::Color(0, 0, 0)); self_graphics.setOutlineThickness(3); self_graphics.setOutlineColor(sf::Color(250, 250, 250));
			calculate_inputs_coordinates(); calculate_outputs_coordinates();
		}

		void evaluate()
		{
			for (int i = 0; i < inputs_values.size(); i++)
				if (!input_elements[i]->evaluated) input_elements[i]->evaluate();
			function(inputs_values, outputs_values);
		}

		void calculate_inputs_coordinates()
		{
			double d1 = y_size / double(inputs_values.size()) / 2.5;
			double inputs_r = min(d1 / 2, double(x_size) / 6);
			double d2 = (y_size - 2*d1 - 2*double(inputs_values.size())*inputs_r) / (double(inputs_values.size()) - 1);
			double s =  inputs_r * 1.5;
			double circle_x = s - inputs_r, circle_y = d1, circle_y_increment = 2*inputs_r + d2;

			for (int i = 0; i < inputs_graphics.size(); i++) inputs_graphics.pop_back();
			for (int i = 0; i < inputs_values.size(); i++)
			{
				inputs_graphics.push_back(sf::CircleShape(inputs_r)); inputs_graphics[i].setPosition(circle_x, circle_y);
				inputs_graphics[i].setFillColor(sf::Color(0, 0, 0)); inputs_graphics[i].setOutlineThickness(max(1.0, inputs_r / 5));
				color_circle_from_spectre(&inputs_graphics[i], i, inputs_values.size());
				circle_y += circle_y_increment;
			}
		}

		void calculate_outputs_coordinates()
		{
			double d1 = y_size / double(outputs_values.size()) / 2.5;
			double outputs_r = min(d1 / 2, double(x_size) / 6);
			double d2 = (y_size - 2*d1 - 2*double(outputs_values.size())*outputs_r) / (double(outputs_values.size()) - 1);
			double s =  outputs_r * 1.5;
			double circle_x = x_size - s - outputs_r, circle_y = y_size - d1 - 2*outputs_r, circle_y_increment = 2*outputs_r + d2;

			for (int i = 0; i < outputs_graphics.size(); i++) outputs_graphics.pop_back();
			for (int i = 0; i < outputs_values.size(); i++)
			{
				outputs_graphics.push_back(sf::CircleShape(outputs_r)); outputs_graphics[i].setPosition(circle_x, circle_y);
				outputs_graphics[i].setFillColor(sf::Color(0, 0, 0)); outputs_graphics[i].setOutlineThickness(max(1.0, outputs_r / 5));
				color_circle_from_spectre(&outputs_graphics[i], i, outputs_values.size());
				circle_y -= circle_y_increment;
			}
		}
};

void connect_wire(bool **from, bool **to) { *to = *from; }

vector<Element> elements;
