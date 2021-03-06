float windowWidth, windowHeight, scale = 100, dt = 5, aspectRatio;

void int_to_string(int x, unsigned char *string)
{
	for (unsigned char *c = string; ; c++)
	{
		*c = '0' + x%10;
		x /= 10;
		if (!x)
		{
			*(c+1) = 0;
			for (unsigned char *c1 = string; c1 <= c; c--, c1++) //inverting string
			{
				unsigned char temp = *c1;
				*c1 = *c;
				*c = temp;
			}
			return;
		}
	}
}

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
	public:
		Function *function;
		bool selected;

		class Wire
		{
			public:
				Wire(Element *from_element, Element *to_element, int output_number, int input_number): 
					from(from_element), output_number(output_number), new_value(false)
				{
					to.push_back(to_element);
					input_numbers.push_back(input_number);
				}

				~Wire()
				{
					to.clear();
					input_numbers.clear();
				}

				Element *from;
				vector<Element*> to;
				vector<int> input_numbers;
				int output_number;
				bool new_value;
		};

		float x, y, x_size, y_size, middle;
		float inputs_step, outputs_step; //(graphics)
		float x_from_where_dragging, y_from_where_dragging;
		vector<bool*> inputs, outputs; //values
		vector<bool> new_outputs;
		vector<Wire*> input_wires, output_wires;

		Element(Function *function, float x, float y, float x_size, float y_size, int inputs_number, int outputs_number):
			x(x), y(y), x_size(x_size), y_size(y_size), middle(x + x_size/2), function(function), selected(false)
		{	
			//filling inputs and outputs by zeros
			for (int i = 0; i < outputs_number; i++)
			{
				outputs.push_back(boolinter(false));
				new_outputs.push_back(false);
				output_wires.push_back(NULL);
			}
			for (int i = 0; i < inputs_number; i++)
			{
				inputs.push_back(boolinter(false));
				input_wires.push_back(NULL);
			}

			//calculating inputs and outputs steps and middle
			inputs_step = y_size / inputs.size(); outputs_step = y_size / outputs.size();

			evaluate_chain();
		}

		~Element()
		{
			//deleting input wires
			for (int i = 0; i < input_wires.size(); i++)
				if (input_wires[i]) delete_input_wire(i);
			//deleting output wires
			for (int i = 0; i < output_wires.size(); i++)
				if (output_wires[i]) delete_output_wire(i);
		}

		bool connect_input_to(int input_number, Element *to_element, int output_number)
		{
			printf("connect_input_to\n");
			//if input already connected to this element
			if ((input_wires[input_number] != NULL) && (input_wires[input_number] == to_element->output_wires[output_number])) return false;
			printf("not connected yet\n");

			//if there is no wire in needed output, then creating it
			if (to_element->output_wires[output_number] == NULL)
			{
				printf("there is no wire in output => creating it\n");
				to_element->output_wires[output_number] = new Wire(to_element, this, output_number, input_number);
				printf("created\n");
			}
			else
			{
				printf("needed output wire already existing, connecting from it\n");
				to_element->output_wires[output_number]->to.push_back(this);
				to_element->output_wires[output_number]->input_numbers.push_back(input_number);
			}

			//clearing and connecting
			printf("clearing\n");
			delete input_wires[input_number];
			printf("connecting\n");
			input_wires[input_number] = to_element->output_wires[output_number];
			printf("connected\n");
			return true;
		}

		void delete_input_wire(int input_number)
		{
			printf("delete_input_wire\n");

			//disconnecting values
			inputs[input_number] = boolinter(*inputs[input_number]);
			printf("OK\n");

			//deleting it from output
			int i = 0;
			Wire *output_wire = input_wires[input_number]->from->output_wires[input_wires[input_number]->output_number];
			//if it is the last output wire, then deleting it totally, else deleting only it's part
			if (output_wire->to.empty())
			{
				printf("output_wires->to.empty()\n");
				input_wires[input_number]->from->output_wires[input_wires[input_number]->output_number] = NULL;
				delete output_wire;
			}
			else
			{
				for (; i < output_wire->to.size(); i++)
					if (output_wire->to[i] == this && output_wire->input_numbers[i] == input_number) break;
				output_wire->to.erase(output_wire->to.begin() + i);
				output_wire->input_numbers.erase(output_wire->input_numbers.begin() + i);
			}
			//and from input
			input_wires[input_number] = NULL;
		}

		void delete_output_wire(int output_number)
		{
			printf("delete_output_wire\n");
			for (int i = 0; i < output_wires[output_number]->to.size(); i++)
			{
				//disconnecting values
				output_wires[output_number]->to[i]->inputs[output_wires[output_number]->input_numbers[i]] = new bool;

				//deleting it from input(s)
				output_wires[output_number]->to[i]->input_wires[output_wires[output_number]->input_numbers[i]] = NULL;
			}
			//and from output
			Wire *temp = output_wires[output_number];
			output_wires[output_number] = NULL;
			delete temp;
		}

		void select() { selected = true; }
		void unselect() { selected = false; }
		void select_unselect() { if (selected) selected = false; else selected = true; }

		void evaluate_elements_from_output(int output_number)
		{
			for (int i = 0; i < output_wires[output_number]->to.size(); i++)
				output_wires[output_number]->to[i]->evaluate_chain();
		}

		void evaluate_chain() //evaluate self and next elemenets which must be reevaluated
		{
			if (function) function->evaluate(inputs, new_outputs); //evaluate self
			vector<Element*> next_elements_wich_must_be_evaluated;
			for (int i = 0; i < new_outputs.size(); i++)
			{
				if (output_wires[i] && (new_outputs[i] != *outputs[i])) //if there is an output wire and new value
					for (int j = 0; j < output_wires[i]->to.size(); j++)
						next_elements_wich_must_be_evaluated.push_back(output_wires[i]->to[j]); //then we must (re)evaluate elements to which this wire is connected
				*outputs[i] = new_outputs[i]; //remembering new output
			}
			//(re)evaluating next elements
			for (int i = 0; i < next_elements_wich_must_be_evaluated.size(); i++) next_elements_wich_must_be_evaluated[i]->evaluate_chain();
		}

		void draw_self()
		{
			//selection
			glLineWidth(1);
			if (selected)
			{
				float margin = max(x_size/8, float(2.0 + 15.0/glutGet(GLUT_WINDOW_HEIGHT)*windowHeight));
				glColor3ub(30, 200, 30);
				glBegin(GL_LINE_STRIP);
				glVertex2f(x-margin, y-margin); glVertex2f(x+margin + x_size, y-margin);
				glVertex2f(x+margin + x_size, y+margin + y_size); glVertex2f(x-margin, y+margin + y_size);
				glVertex2f(x-margin, y-margin);
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
			float border_y = y+inputs_step;
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

		void draw_wires()
		{
			//output wires
			float from_x = x + x_size,
				   from_y = y+outputs_step/2;
			for (int i = 0; i < output_wires.size(); i++, from_y += outputs_step)
			{
				if (output_wires[i] == NULL) continue; //if there is no wires from this output

				for (int j = 0; j < output_wires[i]->to.size(); j++)
				{
					//calculating from wich point we must draw the wire
					float	to_x = output_wires[i]->to[j]->x,
							to_y = output_wires[i]->to[j]->y + output_wires[i]->to[j]->inputs_step*(float(output_wires[i]->input_numbers[j])+0.5);
				
					float	middle = (to_x + from_x) / 2; //where wire must turn and go vertically
				
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
			}
		}

		void draw_caption()
		{
			glColor3ub(function->pack->contour_color.R, function->pack->contour_color.G, function->pack->contour_color.B);
			glRasterPos2f(x, y+y_size+1);
			glutBitmapString(GLUT_BITMAP_8_BY_13, function->name);
		}

		bool change_input_value(float mouse_x, float mouse_y)
		{
			if (mouse_x < middle) //input
			{
				int input_number = int((mouse_y - y) / inputs_step);
				//if there is a wire in this input, then you can't change this value (cause this is not a scheme input)
				if (input_wires[input_number]) return true;
				if (*inputs[input_number]) *inputs[input_number] = false;
				else *inputs[input_number] = true;
				evaluate_chain();
				return true;
			}
			return false;
		}
};

float x_where_mouse_was_pressed, y_where_mouse_was_pressed; //for dragging
vector<Element*> dragged_elements; //(now)

float Mouse_x, Mouse_y; //for drawing wire wich is just adding
bool adding_wire = false, from_output; Element *first_element; int firstput_number;

class Scheme
{
	private:
		vector<Element*> elements;
		bool captions_enabled;
		int workspace_x, workspace_y, workspace_x_size, workspace_y_size;

	public:
		bool selecting,
			 selecting_by_quad;
		unsigned char *name;

		Scheme(unsigned char *name_arg): 
			captions_enabled(true), selecting(false), selecting_by_quad(false)
		{ strcpy(&name, &name_arg); }

		~Scheme()
		{
			for (int i = 0; i < elements.size(); i++) delete elements[i];
			elements.clear();
		}

		void set_workspace_area(int x, int y, int x_size, int y_size)
		{
			workspace_x = x; workspace_x_size = x_size;
			workspace_y = y; workspace_y_size = y_size;
		}

		bool select_element(float mouse_x, float mouse_y)
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					elements[i]->select_unselect();
					workspace_render_needed = true;
					return true;
				}
			return false;
		}

		void select_elements(float mouse_x1, float mouse_y1, float mouse_x2, float mouse_y2)
		{
			if (mouse_x1 > mouse_x2) swap(mouse_x1, mouse_x2);
			if (mouse_y1 < mouse_y2) swap(mouse_y1, mouse_y2);
			for (int i = 0; i < elements.size(); i++)
			{
				if	(	mouse_x1 < (elements[i]->x + elements[i]->x_size) && mouse_x2 > elements[i]->x && 
						mouse_y1 > elements[i]->y && mouse_y2 < (elements[i]->y + elements[i]->y_size))
					elements[i]->select();
				else elements[i]->unselect();
			}
		}

		void draw_selection_quad()
		{
			select_elements(x_where_mouse_was_pressed, y_where_mouse_was_pressed, Mouse_x, Mouse_y);
			glColor3ub(30, 200, 30);
			glLineWidth(1);
			glBegin(GL_LINE_STRIP);
			glVertex2f(x_where_mouse_was_pressed, y_where_mouse_was_pressed); glVertex2f(x_where_mouse_was_pressed, Mouse_y);
			glVertex2f(Mouse_x, Mouse_y); glVertex2f(Mouse_x, y_where_mouse_was_pressed);
			glVertex2f(x_where_mouse_was_pressed, y_where_mouse_was_pressed);
			glEnd();
		}

		bool selection_is_empty()
		{
			for (int i = 0; i < elements.size(); i++)
				if (elements[i]->selected) return false;
			return true;
		}

		void clear_selection() { for (int i = 0; i < elements.size(); i++) elements[i]->unselect(); }

		void add_element(Function *function, float x, float y, float x_size, float y_size, int inputs_number, int outputs_number, bool from_panel)
		{
			Element **element;
			element = new Element*;
			*element = new Element(function, x, y, x_size, y_size, inputs_number, outputs_number);
			if (from_panel)
			{	
				x_where_mouse_was_pressed = x;
				y_where_mouse_was_pressed = y;
				(*element)->x_from_where_dragging = x;
				(*element)->y_from_where_dragging = y;
				dragged_elements.push_back(*element);
			}
			elements.push_back(*element);
		}

		void delete_element(int element_number)
		{
			Element *temp = elements[element_number];
			elements.erase(elements.begin() + element_number);
			delete temp;
		}

		void freeze_other_area()
		{
//			printf("workspace scissor %d %d %d %d\n", workspace_x, workspace_y, workspace_x_size, workspace_y_size);
			glScissor(workspace_x, workspace_y, workspace_x_size, workspace_y_size);
			glEnable(GL_SCISSOR_TEST);
		}

		void draw()
		{
			//wire which is adding now
			if (adding_wire)
			{
				float from_x, from_y;
				if (from_output)
				{
					from_x = first_element->x + first_element->x_size, 
					from_y = first_element->y + first_element->outputs_step*(firstput_number+0.5);
				}
				else
				{
					from_x = first_element->x, 
					from_y = first_element->y + first_element->inputs_step*(firstput_number+0.5);
				}
				float middle = (Mouse_x + from_x) / 2;
		
				glLineWidth(7);
				glColor3ub(0, 0, 0);
				glBegin(GL_LINES);
				glVertex2f(Mouse_x, Mouse_y); glVertex2f(middle, Mouse_y);
				glVertex2f(middle, Mouse_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(from_x, from_y);
				glEnd();

				glLineWidth(1);
				glColor3ub(255, 255, 255);
				glBegin(GL_LINES);
				glVertex2f(Mouse_x, Mouse_y); glVertex2f(middle, Mouse_y);
				glVertex2f(middle, Mouse_y); glVertex2f(middle, from_y);
				glVertex2f(middle, from_y); glVertex2f(from_x, from_y);
				glEnd();
			}

			//elements
			for (int i = 0; i < elements.size(); i++) elements[i]->draw_wires();
			for (int i = 0; i < elements.size(); i++) elements[i]->draw_self();
			
			//captions
			if (captions_enabled)
				for (int i = 0; i < elements.size(); i++) elements[i]->draw_caption();
		}

		void show_hide_captions()
		{
			if (captions_enabled) captions_enabled = false;
			else captions_enabled = true;
		}

		bool try_to_drag_element(float mouse_x, float mouse_y)
		{
			
			for (int i = 0; i < elements.size(); i++)
				//checking if cursor is on element
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					//remembering mouse coordinates for dragging
					x_where_mouse_was_pressed = mouse_x;
					y_where_mouse_was_pressed = mouse_y;
					if (selecting)
					{
						if (elements[i]->selected)
						{
							//then start dragging selected elements
							for (i = 0; i < elements.size(); i++)
								if (elements[i]->selected)
								{
									elements[i]->x_from_where_dragging = elements[i]->x;
									elements[i]->y_from_where_dragging = elements[i]->y;
									dragged_elements.push_back(elements[i]);
								}
						}
						else
						{
							//start dragging only pressed element
							dragged_elements.clear();
							elements[i]->x_from_where_dragging = elements[i]->x;
							elements[i]->y_from_where_dragging = elements[i]->y;
							dragged_elements.push_back(elements[i]);
						}
					}
					else
					{
						//dragging only pressed element
						elements[i]->x_from_where_dragging = elements[i]->x;
						elements[i]->y_from_where_dragging = elements[i]->y;
						dragged_elements.push_back(elements[i]);
					}
					return true;
				}
			return false;
		}

		void release() { dragged_elements.clear(); }

		bool try_to_change_input_value(float mouse_x, float mouse_y)
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					//if click is on inputs, then change input value, else remove element
					if (!elements[i]->change_input_value(mouse_x, mouse_y)) delete_element(i);
					workspace_render_needed = true;
					return true;
				}
			return false;
		}

		void finish_adding_wire(float mouse_x, float mouse_y)
		{
			printf("finish adding wire\n");
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					if (first_element == elements[i]) return; //you can not connect element to itself
					
					if (from_output)
					{
						if (mouse_x < elements[i]->middle) //connecting wire to input
						{
							printf("connecting wire to input\n");
							int input_number = int((mouse_y - elements[i]->y) / elements[i]->inputs_step);

							//checking if the input is empty and if it is not, then removing wire
							if (elements[i]->input_wires[input_number] != NULL) elements[i]->delete_input_wire(input_number);

							//try to connect wire (false if it is already connected here)
							printf("try to connect wire\n");
							if (!elements[i]->connect_input_to(input_number, first_element, firstput_number)) return;
							//connecting values
							printf("ok, connecting values\n");
							elements[i]->inputs[input_number] = first_element->outputs[firstput_number];	
							//reevaluating
							printf("reevaluating\n");
							first_element->evaluate_chain();
						}
					}
					else
						if (mouse_x > elements[i]->middle) //connecting wire to output
						{
							printf("connecting wire to output\n");
							int output_number = int((mouse_y - elements[i]->y) / elements[i]->outputs_step);

							//connecting wire
							first_element->connect_input_to(firstput_number, elements[i], output_number);
							//connecting values
							first_element->inputs[firstput_number] = elements[i]->outputs[output_number];
							//reevaluating
							first_element->evaluate_chain();
						}
					return;
				}
		}

		bool try_to_add_wire(float mouse_x, float mouse_y)
		{
			printf("try_to_add_wire\n");
			x_where_mouse_was_pressed = mouse_x;
			y_where_mouse_was_pressed = mouse_y;
			Mouse_x = mouse_x;
			Mouse_y = mouse_y;
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i]->x && mouse_x < elements[i]->x + elements[i]->x_size && mouse_y > elements[i]->y && mouse_y < elements[i]->y + elements[i]->y_size)
				{
					if (mouse_x > elements[i]->middle) //adding wire from output
					{
						printf("adding wire from output\n");
						from_output = true;
						firstput_number = int((mouse_y - elements[i]->y) / elements[i]->outputs_step); //a bit of magic
					}
					else //adding wire from input
					{
						printf("adding wire from input\n");
						from_output = false;
						firstput_number = int((mouse_y - elements[i]->y) / elements[i]->inputs_step); //a bit of magic
						//if there is already a wire in input, then deleting it
						if (elements[i]->input_wires[firstput_number] != NULL) elements[i]->delete_input_wire(firstput_number);
					}
					first_element = elements[i];
					adding_wire = true;
					return true;
				}
			return false;
		}

		void start_drag_field(float mouse_x, float mouse_y)
		{
			x_where_mouse_was_pressed = mouse_x;
			y_where_mouse_was_pressed = mouse_y;
			for (int i = 0; i < elements.size(); i++)
			{
				elements[i]->x_from_where_dragging = elements[i]->x;
				elements[i]->y_from_where_dragging = elements[i]->y;
				dragged_elements.push_back(elements[i]);
			}
		}
};

Scheme *active_tab = NULL;
vector<Scheme*> tabs;
float tabs_step;

float tabs_panel_size, 
	  tabs_panel_size_in_pixels, 
	  tabs_panel_border_y, 
	  tabs_panel_text_y;

void new_tab()
{
	tabs.push_back(new Scheme((unsigned char*)"Unnamed scheme"));
	if (!active_tab) active_tab = tabs[tabs.size()-1];
	tabs_step = (2*windowWidth-tabs_panel_size)/tabs.size();
	tabs_render_needed = true;
}
