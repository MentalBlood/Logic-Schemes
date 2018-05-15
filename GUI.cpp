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

Function *function_for_new_element = NULL;

class Elements_Panel
{
	private:
		int current_page, current_inputs_number, current_outputs_number,
			last_element_index, first_element_index;
		unsigned char current_page_string[5], current_inputs_number_string[5], current_outputs_number_string[5];
		double x_size, parameters_size, element_button_size;
		bool opened;
		Functions_Base *functions_base; //base with wich this panel operate

		//graphical placement data
		double border_x, 

			   open_close_button_half_y_size, 
			   open_close_button_right_x, 
			   open_close_button_center, 
			   open_close_button_sign_half_size, 
			   
			   parameters_and_list_seperator_y, parameters_horizontal_separator1_y, 
			   parameters_vertical_separator_x, parameters_horizontal_separator2_y, 
			   
			   text_pos_x, values_pos_x, 
			   text_pos_1_y, 
			   text_pos_2_y, 
			   text_pos_3_y,
			   
			   elements_step,
			   page_graphical_length;
		int elements_on_page;

	public:
		vector<Function*> functions; //functions satisfying the parameters

		Elements_Panel(Functions_Base *functions_base): functions_base(functions_base), opened(true), current_page(1), current_inputs_number(0), current_outputs_number(0)
		{
			int_to_string(current_page, current_page_string);
			int_to_string(current_inputs_number, current_inputs_number_string);
			int_to_string(current_outputs_number, current_outputs_number_string);
			recalculate_graphical_placement_data();
			functions_base->write_functions_by_parameters_to_vector(current_inputs_number, current_outputs_number, functions);
			first_element_index = (current_page - 1)*elements_on_page;
			last_element_index = min(int(functions.size()), first_element_index + elements_on_page);
		}
		
		void recalculate_graphical_placement_data()
		{
			x_size = windowWidth/3.25;
			border_x = -windowWidth + x_size;
			
			double open_close_button_x_size = windowWidth/15;
			open_close_button_right_x = border_x + open_close_button_x_size;
			open_close_button_center = border_x + open_close_button_x_size/2;
			open_close_button_half_y_size = windowHeight/5;
			open_close_button_sign_half_size = open_close_button_x_size/3;
			
			double pixel_scale = glutGet(GLUT_WINDOW_HEIGHT)*windowHeight;
			parameters_size = max(windowHeight*0.3, 120.0/pixel_scale);
			parameters_and_list_seperator_y = windowHeight - parameters_size;
			parameters_vertical_separator_x = border_x - x_size/2;
			double parameters_step = parameters_size/3;
			parameters_horizontal_separator1_y = parameters_and_list_seperator_y + parameters_step;
			parameters_horizontal_separator2_y = parameters_horizontal_separator1_y + parameters_step;
			
			text_pos_x = -windowWidth + 0.5;
			values_pos_x = parameters_vertical_separator_x + 1.5;
			double text_margin_y = (parameters_step - 13.0/pixel_scale)/2;
			text_pos_1_y = parameters_and_list_seperator_y + text_margin_y;
			text_pos_2_y = parameters_horizontal_separator1_y + text_margin_y;
			text_pos_3_y = parameters_horizontal_separator2_y + text_margin_y;

			elements_step = max(10.0, 40.0/pixel_scale);
			page_graphical_length = windowHeight + parameters_and_list_seperator_y;
			elements_on_page = int(page_graphical_length / elements_step);
			elements_step = page_graphical_length / double(elements_on_page);
		}

		void refresh_functions_list() { functions_base->write_functions_by_parameters_to_vector(current_inputs_number, current_outputs_number, functions); }

		void refresh_page_data()
		{
			first_element_index = (current_page - 1)*elements_on_page;
			last_element_index = min(int(functions.size()), first_element_index + elements_on_page);
		}

		void plus_input()
		{
			if (current_inputs_number != 9999) ++current_inputs_number;
			else current_inputs_number = 0;
			int_to_string(current_inputs_number, current_inputs_number_string);
			refresh_functions_list();
			refresh_page_data();
		}

		void minus_input()
		{
			if (!current_inputs_number) return;
			--current_inputs_number;
			int_to_string(current_inputs_number, current_inputs_number_string);
			refresh_functions_list();
			refresh_page_data();
		}

		void plus_output()
		{
			if (current_outputs_number != 9999) ++current_outputs_number;
			else current_outputs_number = 0;
			int_to_string(current_outputs_number, current_outputs_number_string);
			refresh_functions_list();
			refresh_page_data();
		}

		void minus_output()
		{
			if (!current_outputs_number) return;
			--current_outputs_number;
			int_to_string(current_outputs_number, current_outputs_number_string);
			refresh_functions_list();
			refresh_page_data();
		}

		void plus_page()
		{
			if (last_element_index == functions.size()-1) return;
			int_to_string(current_page, current_page_string);
			refresh_page_data();
		}

		void minus_page()
		{
			if (!current_page) return;
			int_to_string(current_inputs_number, current_inputs_number_string);
			first_element_index = (current_page - 1)*elements_on_page;
			last_element_index = min(int(functions.size()), first_element_index + elements_on_page);
			refresh_page_data();
		}

		void give_birth_to_new_element(double x, double y)
		{
			active_tab->add_element(function_for_new_element, x, y, 10, 20, 
					max(max(current_inputs_number, 1), function_for_new_element->min_inputs_number), max(max(current_outputs_number, 1), function_for_new_element->min_outputs_number), true);
		}

		void draw()
		{
			glColor3ub(0, 0, 0);
			glLineWidth(1);

			if (!opened) //closed => only open button
			{
				//background
				glBegin(GL_QUADS);
				glVertex2f(-windowWidth, open_close_button_half_y_size); glVertex2f(open_close_button_right_x - x_size, open_close_button_half_y_size);
				glVertex2f(open_close_button_right_x - x_size, -open_close_button_half_y_size); glVertex2f(-windowWidth, -open_close_button_half_y_size);
				glEnd();

				//contour
				glColor3ub(255, 255, 255);
				glBegin(GL_LINE_STRIP);
				glVertex2f(-windowWidth, open_close_button_half_y_size); glVertex2f(open_close_button_right_x - x_size, open_close_button_half_y_size);
				glVertex2f(open_close_button_right_x - x_size, -open_close_button_half_y_size); glVertex2f(-windowWidth, -open_close_button_half_y_size);
				glEnd();
				
				//sign
				glColor3ub(255, 255, 70);
				glBegin(GL_LINES);
				glVertex2f(open_close_button_center - open_close_button_sign_half_size - x_size, 0); glVertex2f(open_close_button_center + open_close_button_sign_half_size - x_size, 0);
				glVertex2f(open_close_button_center - x_size, open_close_button_sign_half_size); glVertex2f(open_close_button_center - x_size, -open_close_button_sign_half_size);
				glEnd();
				return;
			}

			//background
			glBegin(GL_QUADS);
			glVertex2f(-windowWidth, windowHeight); glVertex2f(border_x, windowHeight);
			glVertex2f(border_x, -windowHeight); glVertex2f(-windowWidth, -windowHeight);
			glEnd();

			//close button
				//background
				glColor3ub(0, 0, 0);
				glBegin(GL_QUADS);
				glVertex2f(border_x, open_close_button_half_y_size); glVertex2f(open_close_button_right_x, open_close_button_half_y_size);
				glVertex2f(open_close_button_right_x, -open_close_button_half_y_size); glVertex2f(border_x, -open_close_button_half_y_size);
				glEnd();
				
				//contour
				glColor3ub(255, 255, 255);
				glBegin(GL_LINE_STRIP);
				glVertex2f(border_x, open_close_button_half_y_size); glVertex2f(open_close_button_right_x, open_close_button_half_y_size);
				glVertex2f(open_close_button_right_x, -open_close_button_half_y_size); glVertex2f(border_x, -open_close_button_half_y_size);
				glEnd();

				//sign
				glColor3ub(255, 255, 70);
				glBegin(GL_LINES);
				glVertex2f(open_close_button_center - open_close_button_sign_half_size, 0); glVertex2f(open_close_button_center + open_close_button_sign_half_size, 0);
				glEnd();
			
			//border
			glColor3ub(255, 255, 255);
			glBegin(GL_LINES);
			glVertex2f(border_x, windowHeight); glVertex2f(border_x, -windowHeight);
			glEnd();

			//parameters separators
			glColor3ub(255, 255, 255);
			glBegin(GL_LINES);
			glVertex2f(-windowWidth, parameters_and_list_seperator_y); glVertex2f(border_x, parameters_and_list_seperator_y);
			glVertex2f(parameters_vertical_separator_x, parameters_and_list_seperator_y); glVertex2f(parameters_vertical_separator_x, windowHeight);
			glVertex2f(-windowWidth, parameters_horizontal_separator1_y); glVertex2f(border_x, parameters_horizontal_separator1_y);
			glVertex2f(-windowWidth, parameters_horizontal_separator2_y); glVertex2f(border_x, parameters_horizontal_separator2_y);
			glEnd();

			//parameters text
			glRasterPos2f(text_pos_x, text_pos_1_y); glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)"page");
			glRasterPos2f(text_pos_x, text_pos_2_y); glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)"outputs");
			glRasterPos2f(text_pos_x, text_pos_3_y); glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)"inputs");

			//parameters values
			glColor3ub(255, 255, 70);
			glRasterPos2f(values_pos_x, text_pos_1_y); glutBitmapString(GLUT_BITMAP_8_BY_13, current_page_string);
			glRasterPos2f(values_pos_x, text_pos_2_y); glutBitmapString(GLUT_BITMAP_8_BY_13, current_outputs_number_string);
			glRasterPos2f(values_pos_x, text_pos_3_y); glutBitmapString(GLUT_BITMAP_8_BY_13, current_inputs_number_string);

			//elements list
			if (functions.empty()) return;
			double up_side_y = parameters_and_list_seperator_y, 
				   down_side_y = parameters_and_list_seperator_y - elements_step,
				   name_y_margin = (elements_step - 15.0/glutGet(GLUT_WINDOW_HEIGHT)*windowHeight)/2,
				   name_x;
			for (int i = first_element_index; i < last_element_index; i++)
			{
				//background
				glColor3ub(
						functions[i]->pack->background_color.R, 
						functions[i]->pack->background_color.G, 
						functions[i]->pack->background_color.B);
				glBegin(GL_QUADS);
				glVertex2f(-windowWidth, up_side_y); glVertex2f(border_x, up_side_y);
				glVertex2f(border_x, down_side_y); glVertex2f(-windowWidth, down_side_y);
				glEnd();

				//contour
				glColor3ub(
						functions[i]->pack->contour_color.R, 
						functions[i]->pack->contour_color.G, 
						functions[i]->pack->contour_color.B);
				glBegin(GL_LINE_STRIP);
				glVertex2f(-windowWidth, up_side_y); glVertex2f(border_x, up_side_y);
				glVertex2f(border_x, down_side_y); glVertex2f(-windowWidth, down_side_y);
				glEnd();

				//name
				unsigned char *c = functions[i]->name;
				while (*c) c++;
				name_x = -windowWidth + (x_size - (c - functions[i]->name + 2)*9.0/glutGet(GLUT_WINDOW_WIDTH)*windowWidth)/2;
				glRasterPos2f(name_x, down_side_y + name_y_margin); glutBitmapString(GLUT_BITMAP_9_BY_15, functions[i]->name);

				//shifting
				up_side_y -= elements_step;
				down_side_y -= elements_step;
			}
		}

		bool press(double mouse_x, double mouse_y, int key)
		{
			if (opened)
			{
				if (mouse_x > border_x)
				{
					//open/close button (maybe)
					if (mouse_x < open_close_button_right_x && mouse_y < open_close_button_half_y_size && mouse_y > -open_close_button_half_y_size)
					{
						//open/close button
						if (opened) opened = false;
						else opened = true;
						render_needed = true;
						return true;
					}
					else return false;
				}
				else
				{
					//panel
					if (mouse_y > parameters_horizontal_separator1_y)
					{
						//parameters
						if (key < 3) return false;
						else
						if (mouse_y < parameters_horizontal_separator2_y)
						{
							//outputs
							if (key == 4) minus_output();
							else plus_output();
						}
						else
						{
							//inputs
							if (key == 4) minus_input();
							else plus_input();
						}
					}
					else
					{
						//elements list
						if (key == 3) plus_page();
						else if (key == 4) minus_page();
						else
						{
							int function_for_new_element_index =	first_element_index + 
																	int((parameters_and_list_seperator_y - mouse_y)/page_graphical_length * elements_on_page);
							if (function_for_new_element_index < functions.size()) function_for_new_element = functions[function_for_new_element_index];
						}
					}
					return true;
				}
			}
			else
			{
				if (mouse_x < open_close_button_right_x - x_size && mouse_y < open_close_button_half_y_size && mouse_y > -open_close_button_half_y_size)
				{
					//open/close button
					if (opened) opened = false;
					else opened = true;
					render_needed = true;
					return true;
				}
				else return false;
			}
		}
};

Elements_Panel *active_panel = NULL;

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	active_tab->draw();
	active_panel->draw();
	if (active_tab->selecting_by_quad) active_tab->draw_selection_quad();
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
	if (active_panel) active_panel->recalculate_graphical_placement_data();
}

void TimerFunction(int value)
{
	if (render_needed)
	{
		render_needed = false;
		glutPostRedisplay();
	}
	glutTimerFunc(dt, TimerFunction, 1);
}
