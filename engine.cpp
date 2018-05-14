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
				Element *to;
				int input_number;
				bool new_value;
		} Wire;

		double x, y, x_size, y_size, middle;
		double inputs_step, outputs_step; //(graphics)
		double x_from_where_dragging, y_from_where_dragging;
		vector<bool*> inputs, outputs; //values
		vector<bool> new_outputs;
		vector<Wire*> output_wires;

		Element(Function *function, double x, double y, double x_size, double y_size, int inputs_number, int outputs_number):
			x(x), y(y), x_size(x_size), y_size(y_size), function(function)
		{	
			//filling inputs and outputs by zeros
			for (int i = 0; i < outputs_number; i++)
			{
				outputs.push_back(boolinter(false));
				new_outputs.push_back(false);
				output_wires.push_back(NULL);
			}
			for (int i = 0; i < inputs_number; i++) inputs.push_back(boolinter(0));

			evaluated = false; //yet

			//calculating inputs and outputs steps and middle
			inputs_step = y_size / inputs.size(); outputs_step = y_size / outputs.size();
			middle = x + x_size/2;
		}

		void evaluate_chain() //evaluate self and next elemenets which must be reevaluated
		{
			if (evaluated) return;
			function->evaluate(inputs, new_outputs); //evaluate self
			evaluated = true;
			vector<Element*> next_elements_wich_must_be_evaluated;
			for (int i = 0; i < new_outputs.size(); i++)
			{
				if (output_wires[i] && (new_outputs[i] != *outputs[i])) //if there is an output wire and new value
					next_elements_wich_must_be_evaluated.push_back(output_wires[i]->to); //then we must (re)evaluate element to which this wire is connected
				*outputs[i] = new_outputs[i]; //remembering new output
			}
			//(re)evaluating next elements
			for (int i = 0; i < next_elements_wich_must_be_evaluated.size(); i++) next_elements_wich_must_be_evaluated[i]->evaluate_chain();
		}

		void unevaluate() { evaluated = false; }

		void draw()
		{
			//output wires
			double from_x = x + x_size,
				   from_y = y+outputs_step/2;
			for (int i = 0; i < output_wires.size(); i++, from_y += outputs_step)
			{
				if (output_wires[i] == NULL) continue; //if there is no wire in this input
				
				//calculating from wich point we must draw the wire
				double	to_x = output_wires[i]->to->x,
						to_y = output_wires[i]->to->y + output_wires[i]->to->inputs_step*(double(output_wires[i]->input_number)+0.5);
				
				double	middle = (to_x + from_x) / 2; //where wire must turn and go vertically
				
				//background black line
				glLineWidth(7);
				glColor3ub(0, 0, 0);
				glBegin(GL_LINES);
				glVertex2f(to_x, to_y); glVertex2f(middle, to_y);
				glVertex2f(middle, to_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(from_x, from_y);
				glEnd();

				//white line
				glLineWidth(1);
				glColor3ub(255, 255, 255);
				glBegin(GL_LINES);
				glVertex2f(to_x, to_y); glVertex2f(middle, to_y);
				glVertex2f(middle, to_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(from_x, from_y);
				glEnd();
			}

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
		}

		void draw_caption()
		{
			glColor3ub(function->pack->contour_color.R, function->pack->contour_color.G, function->pack->contour_color.B);
			glRasterPos2f(x, y+y_size+1);
			glutBitmapString(GLUT_BITMAP_8_BY_13, function->name);
		}

		bool change_someput_value(double mouse_x, double mouse_y)
		{
			if (mouse_x < middle) //input
			{
				int input_number = int((mouse_y - y) / inputs_step);
				if (*inputs[input_number]) *inputs[input_number] = false;
				else *inputs[input_number] = true;
				evaluate_chain();
				return true;
			}
			return false;
		}
};

double x_where_mouse_was_pressed, y_where_mouse_was_pressed; //for dragging
vector<Element*> dragged_elements; //(now)

double Mouse_x, Mouse_y; //for drawing wire wich is just adding
bool adding_wire = false; Element *from_element; int output_number; //is we adding wire from element from output with such number (now)

class Scheme
{
	private:
		vector<Element*> elements;
		vector<bool*> inputs, outputs;
		bool captions_enabled;

	public:
		Scheme(): captions_enabled(true)
		{}

		void unevaluate()
		{
			for (int i = 0; i < elements.size(); i++) elements[i]->unevaluate();
		}

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
				x_where_mouse_was_pressed = x;
				y_where_mouse_was_pressed = y;
				dragged_elements.push_back(*element);
			}
		}

		void draw()
		{
			//wire which is adding now
			if (adding_wire)
			{
				double from_y = from_element->y + from_element->outputs_step*(output_number+0.5);
				double middle = (Mouse_x + from_element->x) / 2;
		
				glLineWidth(7);
				glColor3ub(0, 0, 0);
				glBegin(GL_LINES);
				glVertex2f(Mouse_x, Mouse_y); glVertex2f(middle, Mouse_y);
				glVertex2f(middle, Mouse_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(from_element->x + from_element->x_size, from_y);
				glEnd();

				glLineWidth(1);
				glColor3ub(255, 255, 255);
				glBegin(GL_LINES);
				glVertex2f(Mouse_x, Mouse_y); glVertex2f(middle, Mouse_y);
				glVertex2f(middle, Mouse_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(from_element->x + from_element->x_size, from_y);
				glEnd();
			}

			//elements
			for (int i = 0; i < elements.size(); i++) elements[i]->draw();
	
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
					if (elements[i]->change_someput_value(mouse_x, mouse_y)) unevaluate();
					break;
				}
		}

		void finish_adding_wire(double mouse_x, double mouse_y)
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					if (mouse_x < elements[i]->middle) //connecting wire to input
					{
						int input_number = int((mouse_y - elements[i]->y) / elements[i]->inputs_step);
		
						free(from_element->outputs[output_number]); from_element->outputs[output_number] = elements[i]->inputs[input_number]; //connecting values	
						from_element->output_wires[output_number] = new Element::Wire; //creating new wire structure
						from_element->output_wires[output_number]->to = elements[i]; //filling it
						from_element->output_wires[output_number]->input_number = input_number;
						from_element->evaluate_chain();
						unevaluate();
					}
					return;
				}
		}

		void try_to_add_wire(double mouse_x, double mouse_y)
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					if (mouse_x > elements[i]->middle) //adding wire from output
					{
						output_number = int((mouse_y - elements[i]->y) / elements[i]->outputs_step); //a bit of magic
						if (elements[i]->output_wires[output_number]) //if wire alredy connected then removing it
						{
							free(elements[i]->output_wires[output_number]); elements[i]->output_wires[output_number] = NULL; //deleting information about wire
							elements[i]->outputs[output_number] = new bool; //disconnecting values
						}
						from_element = elements[i];
						adding_wire = true;
					}
					return;
				}
		}
};

vector<Scheme> tabs;
Scheme *active_tab;
