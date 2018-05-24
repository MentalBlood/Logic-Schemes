Function *function_for_new_element = NULL;

class Elements_Panel
{
	private:
		int current_page, current_inputs_number, current_outputs_number,
			last_element_index, first_element_index;
		unsigned char current_page_string[5], current_inputs_number_string[5], current_outputs_number_string[5];
		float parameters_size, element_button_size;
		Functions_Base *functions_base; //base with wich this panel operate

		//graphical placement data
		float x_size, border_x, x_size_in_pixels, 

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
		bool opened;
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
			active_tab->set_workspace_area(x_size_in_pixels + 1, 0, glutGet(GLUT_WINDOW_WIDTH) - x_size_in_pixels, glutGet(GLUT_WINDOW_HEIGHT));
		}
		
		void recalculate_graphical_placement_data()
		{
			x_size = windowWidth/3.25;
			x_size_in_pixels = float(glutGet(GLUT_WINDOW_WIDTH))/6.5;
			//if (aspectRatio < 1.0) x_size_in_pixels /= aspectRatio;
			border_x = -windowWidth + x_size;

			float pixel_scale = glutGet(GLUT_WINDOW_HEIGHT)*windowHeight;
			parameters_size = max(windowHeight*0.3, 120.0/pixel_scale);
			parameters_and_list_seperator_y = windowHeight - parameters_size - tabs_panel_size;
			parameters_vertical_separator_x = border_x - x_size/2;
			float parameters_step = parameters_size/3;
			parameters_horizontal_separator1_y = parameters_and_list_seperator_y + parameters_step;
			parameters_horizontal_separator2_y = parameters_horizontal_separator1_y + parameters_step;

			text_pos_x = -windowWidth + 0.5;
			values_pos_x = parameters_vertical_separator_x + 1.5;
			float text_margin_y = (parameters_step - 13.0/pixel_scale)/2;
			text_pos_1_y = parameters_and_list_seperator_y + text_margin_y;
			text_pos_2_y = parameters_horizontal_separator1_y + text_margin_y;
			text_pos_3_y = parameters_horizontal_separator2_y + text_margin_y;

			elements_step = max(10.0, 40.0/pixel_scale);
			page_graphical_length = windowHeight + parameters_and_list_seperator_y;
			elements_on_page = int(page_graphical_length / elements_step);
			elements_step = page_graphical_length / float(elements_on_page);
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

		void give_birth_to_new_element(float x, float y)
		{
			active_tab->add_element(function_for_new_element, x, y, 10, 20, 
					max(max(current_inputs_number, 1), function_for_new_element->min_inputs_number), max(max(current_outputs_number, 1), function_for_new_element->min_outputs_number), true);
		}

		void freeze_other_area()
		{
//			printf("panel scissor %d %d %d %d\n", 0, 0, int(x_size_in_pixels) + 1, int(glutGet(GLUT_WINDOW_HEIGHT)));
			glScissor(0, 0, int(x_size_in_pixels) + 1, int(glutGet(GLUT_WINDOW_HEIGHT) - tabs_panel_size_in_pixels));
			glEnable(GL_SCISSOR_TEST);
		}

		void draw()
		{
			if (!opened) return;

			glColor3ub(0, 0, 0);
			glLineWidth(1);

			//background
			glBegin(GL_QUADS);
			glVertex2f(-windowWidth, tabs_panel_border_y); glVertex2f(border_x, tabs_panel_border_y);
			glVertex2f(border_x, -windowHeight); glVertex2f(-windowWidth, -windowHeight);
			glEnd();

			//border
			glColor3ub(255, 255, 255);
			glBegin(GL_LINES);
			glVertex2f(border_x, tabs_panel_border_y); glVertex2f(border_x, -windowHeight);
			glEnd();

			//parameters separators
			glColor3ub(255, 255, 255);
			glBegin(GL_LINES);
			glVertex2f(-windowWidth, parameters_and_list_seperator_y); glVertex2f(border_x, parameters_and_list_seperator_y);
			glVertex2f(parameters_vertical_separator_x, parameters_and_list_seperator_y); glVertex2f(parameters_vertical_separator_x, tabs_panel_border_y);
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
			float up_side_y = parameters_and_list_seperator_y, 
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

		bool press(float mouse_x, float mouse_y, int key)
		{
			if (opened)
			{
				if (mouse_x < border_x)
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
						panel_render_needed = true;
					}
					else
					{
						//elements list
						if (key == 3) plus_page();
						else if (key == 4) minus_page();
						else if (!key)
						{
							int function_for_new_element_index =	first_element_index + 
																	int((parameters_and_list_seperator_y - mouse_y)/page_graphical_length * elements_on_page);
							if (function_for_new_element_index < functions.size()) function_for_new_element = functions[function_for_new_element_index];
						}
					}
					return true;
				}
			}
		}

		void open_close()
		{
			if (opened)
			{
				opened = false;
				active_tab->set_workspace_area(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)-tabs_panel_size_in_pixels);
				workspace_render_needed = true;
			}
			else
			{
				opened = true;
				active_tab->set_workspace_area(x_size_in_pixels + 1, 0, glutGet(GLUT_WINDOW_WIDTH) - x_size_in_pixels, glutGet(GLUT_WINDOW_HEIGHT)-tabs_panel_size_in_pixels);
				panel_render_needed = true;
			}
			tabs_render_needed = true;
		}

		void refresh_wokspace_area()
		{
			printf("%d : %d, %d\n", glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), int(x_size_in_pixels));
			if (opened) active_tab->set_workspace_area(int(x_size_in_pixels) + 1, 0, glutGet(GLUT_WINDOW_WIDTH) - int(x_size_in_pixels), glutGet(GLUT_WINDOW_HEIGHT)-tabs_panel_size_in_pixels);
			else active_tab->set_workspace_area(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)-tabs_panel_size_in_pixels);
		}
};

Elements_Panel *active_panel = NULL;

void SetupRC(void) { glClearColor(0.0f,0.0f,0.0f,1.0f); }

void ChangeSize(GLsizei w, GLsizei h)	//will be call every time window resizing
{
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspectRatio = (float)w / (float)h;
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
	if (active_panel)
	{
		active_panel->recalculate_graphical_placement_data();
		active_panel->refresh_wokspace_area();
		tabs_panel_size = windowHeight/15;
		tabs_step = (2*windowWidth-tabs_panel_size)/tabs.size();
		tabs_panel_size_in_pixels = glutGet(GLUT_WINDOW_HEIGHT)/30;
		tabs_panel_border_y = windowHeight - tabs_panel_size;
		tabs_panel_text_y = tabs_panel_border_y + (tabs_panel_size_in_pixels-8)/glutGet(GLUT_WINDOW_HEIGHT)*windowHeight;
		tabs_render_needed = true;
		panel_render_needed = true;
		workspace_render_needed = true;
	}
}

void render_workspace()
{
	workspace_render_needed = false;
	active_tab->freeze_other_area();
	glClear(GL_COLOR_BUFFER_BIT);
	active_tab->draw();
	if (active_tab->selecting_by_quad) active_tab->draw_selection_quad();
	glutSwapBuffers();
	glDisable(GL_SCISSOR_TEST);
}

void render_panel()
{
	panel_render_needed = false;
	active_panel->freeze_other_area();
	glClear(GL_COLOR_BUFFER_BIT);
	active_panel->draw();
	glutSwapBuffers();
	glDisable(GL_SCISSOR_TEST);
}

void render_tabs()
{
	tabs_render_needed = false;
	//freesing all other area
	glScissor(0, glutGet(GLUT_WINDOW_HEIGHT)-tabs_panel_size_in_pixels, glutGet(GLUT_WINDOW_WIDTH), tabs_panel_size_in_pixels);
	glEnable(GL_SCISSOR_TEST);
	//background
	glColor3ub(100, 100, 100);
	glBegin(GL_QUADS);
	glVertex2f(-windowWidth, tabs_panel_border_y); glVertex2f(windowWidth, tabs_panel_border_y);
	glVertex2f(windowWidth, windowHeight); glVertex2f(-windowWidth, windowHeight);
	glEnd();
	//new tab button
	glColor3ub(255, 255, 70);
	glBegin(GL_LINES);
	glVertex2f(windowWidth - tabs_panel_size/2, windowHeight - tabs_panel_size/5); glVertex2f(windowWidth - tabs_panel_size/2, windowHeight - 4*tabs_panel_size/5);
	glVertex2f(windowWidth - 4*tabs_panel_size/5, windowHeight - tabs_panel_size/2); glVertex2f(windowWidth - tabs_panel_size/5, windowHeight - tabs_panel_size/2);
	glEnd();
	//tabs
	float margin_x = -windowWidth;
	float multiplier = 8*windowWidth*2/glutGet(GLUT_WINDOW_WIDTH);
	for (int i = 0; i < tabs.size(); i++)
	{
		//active tab highlight
		if (tabs[i] == active_tab)
		{
			glColor3ub(0, 0, 0);
			glBegin(GL_QUADS);
			glVertex2f(margin_x, tabs_panel_border_y); glVertex2f(margin_x + tabs_step, tabs_panel_border_y);
			glVertex2f(margin_x + tabs_step, windowHeight); glVertex2f(margin_x, windowHeight);
			glEnd();
		}
		//tabs separator
		glColor3ub(255, 255, 255);
		glBegin(GL_LINES);
		glVertex2f(margin_x + tabs_step, windowHeight); glVertex2f(margin_x + tabs_step, tabs_panel_border_y);
		glEnd();
		//tab name
		glColor3ub(255, 255, 70);
		unsigned char *c = tabs[i]->name;
		while (*c) ++c;
		float text_x = margin_x + (tabs_step - (c - tabs[i]->name)*multiplier) / 2;
		glRasterPos2f(text_x, tabs_panel_text_y);
		glutBitmapString(GLUT_BITMAP_8_BY_13, tabs[i]->name);
		margin_x += tabs_step;
	}

	//finishing
	glutSwapBuffers();
	glDisable(GL_SCISSOR_TEST);
}

bool press_on_tabs_panel(float mouse_x, float mouse_y, int key)
{
	if (mouse_y < tabs_panel_border_y) return false;
	if (mouse_x > windowWidth - tabs_panel_size) //new tab button pressed
	{
		new_tab();
		return true;
	}
	int tab_number = int((mouse_x + windowWidth) / tabs_step);
	if (!key) //left mouse key => go to tab
	{
		if (active_tab == tabs[tab_number]) return true; //if selected tab already active
		active_tab = tabs[tab_number];
		active_panel->refresh_wokspace_area();
		workspace_render_needed = true;
	}
	else
	if (key == 2) //right mouse_key => delete tab
	{
		if (tabs[tab_number] == active_tab)
		{
			if (tab_number > 0) active_tab = tabs[tab_number-1];
			else
			if (tab_number < tabs.size()-1) active_tab = tabs[tab_number+1];
			else
			{
				new_tab();
				active_tab = tabs[tabs.size()-1];
			}
			active_panel->refresh_wokspace_area();
			workspace_render_needed = true;
		}
		delete tabs[tab_number];
		tabs.erase(tabs.begin() + tab_number);
		tabs_step = (2*windowWidth-tabs_panel_size)/tabs.size();
	}
	tabs_render_needed = true;
	return true;
}

void render_editor(void)
{
	if (workspace_render_needed) render_workspace();
	if (tabs_render_needed) render_tabs();
	if (panel_render_needed) render_panel();
}

void TimerFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(dt, TimerFunction, 1);
}
