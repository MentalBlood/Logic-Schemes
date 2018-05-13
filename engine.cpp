double windowWidth, windowHeight, scale = 100, dt = 5;
double aspectRatio;

bool* boolinter(bool value) //makes boolean with given value and retuns pointer to it
{
	bool **b = (bool**)malloc(sizeof(bool*));
	*b = (bool*)malloc(sizeof(bool));
	**b = value;
	return *b;
}

bool scheme_inputs_outputs_enabled = true;

class Element
{
	private:
		bool evaluated;

	public:
		Function *function;

		typedef struct Wire
		{
				Element *from;
				int output_number;
		} Wire;

		double x, y, x_size, y_size, middle;
		double inputs_step, outputs_step; //(graphics)
		double x_from_where_dragging, y_from_where_dragging;
		vector<bool*> inputs, outputs; //values
		vector<Wire*> input_wires;

		Element(Function *function, double x, double y, double x_size, double y_size, int inputs_number, int outputs_number):
			x(x), y(y), x_size(x_size), y_size(y_size), function(function)
		{	
			//filling inputs and outputs by zeros
			for (int i = 0; i < inputs_number; i++)
			{
				inputs.push_back(boolinter(false));
				input_wires.push_back(NULL);
			}
			for (int i = 0; i < outputs_number; i++) outputs.push_back(boolinter(0));

			evaluated = false; //yet

			//calculating inputs and outputs steps and middle
			inputs_step = y_size / inputs.size(); outputs_step = y_size / outputs.size();
			middle = x + x_size/2;
		}

		void evaluate()
		{
			for (int i = 0; i < inputs.size(); i++)
				if (input_wires[i]) //if there is an input wire
					if (!input_wires[i]->from->evaluated) //and element on other side of it is not evaluated
						input_wires[i]->from->evaluate(); //than evaluate it
			function->evaluate(inputs, outputs); //evaluate self
		}

		void draw()
		{
			//background
			glColor3ub(function->pack->background_color.R, function->pack->background_color.G, function->pack->background_color.B);
			glBegin(GL_QUADS);
			glVertex2f(x, y); glVertex2f(x + x_size, y);
			glVertex2f(x + x_size, y + y_size); glVertex2f(x, y + y_size);
			glEnd();

			//borders
			glLineWidth(1);
			glColor3ub(function->pack->contour_color.R, function->pack->contour_color.G, function->pack->contour_color.B);
			glBegin(GL_LINE_STRIP);
			glVertex2f(x, y); glVertex2f(x + x_size, y);
			glVertex2f(x + x_size, y + y_size); glVertex2f(x, y + y_size);
			glVertex2f(x, y);
			glEnd();

			glBegin(GL_LINES);
			//middle line
			glVertex2f(middle, y); glVertex2f(middle, y + y_size);

			//inputs
			double border_y = y+inputs_step;
			for (int i = 0; i < inputs.size(); i++)
			{
				glVertex2f(x, border_y); glVertex2f(middle, border_y);
				if (!*inputs[i])
				{
					glVertex2f(x+x_size/4, border_y); glVertex2f(x+x_size/4, border_y - inputs_step);
				}
				border_y += inputs_step;
			}

			//outputs
			border_y = y+outputs_step;
			for (int i = 0; i < outputs.size(); i++)
			{
				if (!*outputs[i])
				{
					glVertex2f(middle+x_size/4, border_y); glVertex2f(middle+x_size/4, border_y - outputs_step);
				}
				glVertex2f(middle, border_y); glVertex2f(x + x_size, border_y);
				border_y += outputs_step;
			}
			glEnd();

			//input wires
			double to_y = y+inputs_step/2;
			for (int i = 0; i < input_wires.size(); i++, to_y += inputs_step)
			{
				if (input_wires[i] == NULL) continue; //if there is no wire in this input
				
				//calculating from wich point we must draw the wire
				double	from_x = input_wires[i]->from->x + input_wires[i]->from->x_size,
						from_y = input_wires[i]->from->y + input_wires[i]->from->outputs_step*(double(input_wires[i]->output_number)+0.5);
				
				double	middle = (from_x + x) / 2; //where wire must turn and go vertically
				
				//background black line
				glLineWidth(7);
				glColor3ub(0, 0, 0);
				glBegin(GL_LINES);
				glVertex2f(from_x, from_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(middle, to_y);
				glVertex2f(middle, to_y); glVertex2f(x, to_y);
				glEnd();

				//white line
				glLineWidth(1);
				glColor3ub(255, 255, 255);
				glBegin(GL_LINES);
				glVertex2f(from_x, from_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(middle, to_y);
				glVertex2f(middle, to_y); glVertex2f(x, to_y);
				glEnd();
			}
		}

		void draw_caption()
		{
			glColor3ub(function->pack->contour_color.R, function->pack->contour_color.G, function->pack->contour_color.B);
			glRasterPos2f(x, y+y_size+1);
			glutBitmapString(GLUT_BITMAP_8_BY_13, function->name);
		}

		void change_someput_value(double mouse_x, double mouse_y)
		{
			if (mouse_x < middle) //input
			{
				int input_number = int((mouse_y - y) / inputs_step);
				if (*inputs[input_number]) *inputs[input_number] = false;
				else *inputs[input_number] = true;
			}
			else //output
			{
				int output_number = int((mouse_y - y) / outputs_step);
				if (*outputs[output_number]) *outputs[output_number] = false;
				else *outputs[output_number] = true;
			}
		}
};

double x_where_mouse_was_pressed, y_where_mouse_was_pressed; //for dragging
vector<Element*> dragged_elements; //(now)

double Mouse_x, Mouse_y; //for drawing wire wich is just adding
bool adding_wire = false; Element *to_element; int input_number; //is we adding wire to element to input with such number (now)

class Scheme
{
	private:
		vector<Element*> elements;
		vector<bool*> inputs, outputs;
		bool captions_enabled;

	public:
		Scheme(): captions_enabled(true)
		{}

		void add_element(Function *function, double x, double y, double x_size, double y_size, int inputs_number, int outputs_number, bool dragging)
		{
			Element **element;
			element = new Element*;
			*element = new Element(function, x, y, x_size, y_size, inputs_number, outputs_number);
			elements.push_back(*element);
			(*element)->x_from_where_dragging = x;
			(*element)->y_from_where_dragging = y;
			if (dragging)
			{
				printf("dragging\n");
				x_where_mouse_was_pressed = x;
				y_where_mouse_was_pressed = y;
				dragged_elements.push_back(*element);
			}
		}

		void draw()
		{
			//elements
			for (int i = 0; i < elements.size(); i++) elements[i]->draw();

			//wire which is adding now
			if (adding_wire)
			{
				double to_y = to_element->y + to_element->inputs_step*(input_number+0.5);
				double middle = (Mouse_x + to_element->x) / 2;
		
				glLineWidth(7);
				glColor3ub(0, 0, 0);
				glBegin(GL_LINES);
				glVertex2f(Mouse_x, Mouse_y); glVertex2f(middle, Mouse_y);
				glVertex2f(middle, Mouse_y); glVertex2f(middle, to_y);
				glVertex2f(middle, to_y); glVertex2f(to_element->x, to_y);
				glEnd();

				glLineWidth(1);
				glColor3ub(255, 255, 255);
				glBegin(GL_LINES);
				glVertex2f(Mouse_x, Mouse_y); glVertex2f(middle, Mouse_y);
				glVertex2f(middle, Mouse_y); glVertex2f(middle, to_y);
				glVertex2f(middle, to_y); glVertex2f(to_element->x, to_y);
				glEnd();
			}
	
			//captions
			if (captions_enabled)
				for (int i = 0; i < elements.size(); i++)
					elements[i]->draw_caption();
		}

		void show_hide_captions()
		{
			if (captions_enabled) captions_enabled = false;
			else captions_enabled = true;
		}

		void try_to_drag(double mouse_x, double mouse_y)
		{
			x_where_mouse_was_pressed = double(mouse_x);
			y_where_mouse_was_pressed = double(mouse_y);
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					dragged_elements.push_back(elements[i]);
					elements[i]->x_from_where_dragging = elements[i]->x;
					elements[i]->y_from_where_dragging = elements[i]->y;
					break;
				}
		}

		void release()
		{
			dragged_elements.clear();
		}

		void try_to_change_someput_value(double mouse_x, double mouse_y)
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					elements[i]->change_someput_value(mouse_x, mouse_y);
					break;
				}
		}

		void finish_adding_wire(double mouse_x, double mouse_y)
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					if (mouse_x > elements[i]->middle) //connecting wire to output
					{
						int output_number = int((mouse_y - elements[i]->y) / elements[i]->outputs_step);
		
						free(to_element->inputs[input_number]); to_element->inputs[input_number] = elements[i]->outputs[output_number]; //connecting values	
						to_element->input_wires[input_number] = new Element::Wire; //creating new wire structure
						to_element->input_wires[input_number]->from = elements[i]; //filling it
						to_element->input_wires[input_number]->output_number = output_number;
					}
					return;
				}
		}

		void try_to_add_wire(double mouse_x, double mouse_y)
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					if (mouse_x < elements[i]->middle) //adding wire from input
					{
						input_number = int((mouse_y - elements[i]->y) / elements[i]->inputs_step); //a bit of magic
						if (elements[i]->input_wires[input_number]) //if wire alredy connected then removing it
						{
							free(elements[i]->input_wires[input_number]); elements[i]->input_wires[input_number] = NULL; //deleting information about wire
							elements[i]->inputs[input_number] = new bool; //disconnecting values
						}
						to_element = elements[i];
						adding_wire = true;
					}
					return;
				}
		}
};

vector<Scheme> tabs;
Scheme *active_tab;
