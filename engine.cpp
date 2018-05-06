GLdouble windowWidth, windowHeight, scale = 100, dt = 5;
GLdouble aspectRatio;

bool* boolinter(bool value)
{
	bool **b = (bool**)malloc(sizeof(bool*));
	*b = (bool*)malloc(sizeof(bool));
	**b = value;
	return *b;
}

void color_from_spectre(int color_number, int colors_total)
{
	color_number = double(color_number) / double(colors_total) * 1275;
	if (color_number <= 255) glColor3ub(255, int(color_number), 0);
	else if (color_number <= 510) glColor3ub(510 - int(color_number), 255, 0);
	else if (color_number <= 765) glColor3ub(0, 255, int(color_number) - 510);
	else if (color_number <= 1020) glColor3ub(0, 1020 - int(color_number), 255);
	else glColor3ub(int(color_number) - 1020, 0, 255);
}

class Element
{
	private:
		bool evaluated;
		void (*function)(vector<bool*> &, vector<bool*> &);

	public:	
		double x, y, x_size, y_size, middle;
		double inputs_step, outputs_step;
		double x_from_where_dragging, y_from_where_dragging;
		vector<bool*> inputs, outputs;
		vector<Element*> input_elements;

		Element(double x, double y, double x_size, double y_size, int inputs_number, int outputs_number, void (*function)(vector<bool*> &, vector<bool*> &)):
			x(x), y(y), x_size(x_size), y_size(y_size), function(function)
		{
			for (int i = 0; i < inputs_number; i++)
			{
				inputs.push_back(boolinter(false));
				input_elements.push_back(NULL);
			}
			for (int i = 0; i < outputs_number; i++) outputs.push_back(boolinter(0));
			evaluated = 0;
			inputs_step = y_size / inputs.size(); outputs_step = y_size / outputs.size();
			middle = x + x_size/2;
		}

		void evaluate()
		{
			for (int i = 0; i < inputs.size(); i++)
				if (input_elements[i])
					if (!input_elements[i]->evaluated) input_elements[i]->evaluate();
			function(inputs, outputs);
		}

		void draw()
		{
			glLineWidth(1);
			glColor3ub(255, 255, 255);

			glBegin(GL_LINE_STRIP);
			//element borders
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

		void change_empty_someput_value(double mouse_x, double mouse_y)
		{
			if (mouse_x < middle)
			{
				int input_number = int((mouse_y - y) / inputs_step);
				if (*inputs[input_number]) *inputs[input_number] = false;
				else *inputs[input_number] = true;
			}
			else
			{
				int output_number = int((mouse_y - y) / outputs_step);
				if (*outputs[output_number]) *outputs[output_number] = false;
				else *outputs[output_number] = true;
			}
		}
};

vector<bool*> scheme_inputs, scheme_outputs;

void draw_wire(double from_x, double from_y, double to_x, double to_y)
{
	glLineWidth(7);
	glColor3ub(0, 0, 0);
	double middle = (from_x + to_x) / 2;

	glBegin(GL_LINES);
	glVertex2f(from_x, from_y); glVertex2f(middle, from_y);
	glVertex2f(middle, from_y); glVertex2f(middle, to_y);
	glVertex2f(middle, to_y); glVertex2f(to_x, to_y);
	glEnd();

	glLineWidth(1);
	glColor3ub(255, 255, 255);

	glBegin(GL_LINES);
	glVertex2f(from_x, from_y); glVertex2f(middle, from_y);
	glVertex2f(middle, from_y); glVertex2f(middle, to_y);
	glVertex2f(middle, to_y); glVertex2f(to_x, to_y);
	glEnd();
}

class Wire
{
	private:
		Element *from_element, *to_element;
		int output_number, input_number;

	public:
		Wire(Element *from_element, Element *to_element, int output_number, int input_number): from_element(from_element), to_element(to_element), output_number(output_number), input_number(input_number) {}

		void draw()
		{
			draw_wire	(
							from_element->x + from_element->x_size, from_element->y + from_element->outputs_step*(double(output_number)+0.5), 
							to_element->x, to_element->y + to_element->inputs_step*(double(input_number)+0.5)
						);
		}
};

vector<Element> elements;
vector<Wire> wires;

double Mouse_x, Mouse_y;
bool adding_wire, *wire_to_value;
Element *to_element; int input_number;

void start_adding_wire(Element *element, double mouse_x, double mouse_y)
{
	if (mouse_x < element->middle)
	{
		to_element = element;
		input_number = int((mouse_y - element->y) / element->inputs_step);
		wire_to_value = element->inputs[input_number];
		adding_wire = true;
	}
}

void finish_adding_wire(Element *element, double mouse_x, double mouse_y)
{
	if (mouse_x > element->middle)
	{
		int output_number = int((mouse_y - element->y) / element->outputs_step);
		
		to_element->inputs[input_number] = element->outputs[output_number]; //connecting values
		wires.push_back(Wire(element, to_element, output_number, input_number)); //adding wire (graphics)
		to_element->input_elements[input_number] = element;
	}
}

void remove_wire(Element *element, double mouse_x, double mouse_y)
{
	
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (adding_wire)
	{
		glBegin(GL_LINES);
		draw_wire(Mouse_x, Mouse_y, to_element->x, to_element->y + to_element->inputs_step*(input_number+0.5));
		glEnd();
	}
	for (int i = 0; i < elements.size(); i++) elements[i].draw();
	for (int i = 0; i < wires.size(); i++) wires[i].draw();
	glutSwapBuffers();
}

void SetupRC(void) { glClearColor(0.0f,0.0f,0.0f,1.0f); }
 
void ChangeSize(GLsizei w, GLsizei h)	//will be call every time window resizing
{
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspectRatio = (GLdouble)w / (GLdouble)h;
	if (w <= h)
	{
		windowWidth = scale;
		windowHeight = scale / aspectRatio;
		glOrtho(-scale, scale, -windowHeight, windowHeight, 1.0, -1.0);
	}
	else
	{
		windowWidth = scale * aspectRatio;
		windowHeight = scale;
		glOrtho(-windowWidth, windowWidth, -scale, scale, 1.0, -1.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TimerFunction(int value)
{
	//do something
	glutPostRedisplay();
	glutTimerFunc(dt, TimerFunction, 1);
}
