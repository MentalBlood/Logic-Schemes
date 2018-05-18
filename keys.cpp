#define KEYS_ARRAY_SIZE 223

void (*keys[KEYS_ARRAY_SIZE])(int mouse_x, int mouse_y) = {};
void nothing(int mouse_x, int mouse_y) {}
void key(unsigned char c, int mouse_x, int mouse_y) { printf("%d\n", c); keys[c%KEYS_ARRAY_SIZE](mouse_x, mouse_y); }

void show_hide_captions(int mouse_x, int mouse_y) { active_tab->show_hide_captions(); }

void setup_keys()
{
	for (int i = 0; i < KEYS_ARRAY_SIZE; i++) keys[i] = nothing;
	keys[99] = show_hide_captions;
}

void mouse(int key, int realised, int base_mouse_x, int base_mouse_y)
{
	double mouse_x = (2*double(base_mouse_x)/glutGet(GLUT_WINDOW_WIDTH) - 1)*scale;
	double mouse_y = (1 - 2*double(base_mouse_y)/glutGet(GLUT_WINDOW_HEIGHT))*scale;
	if (aspectRatio < 1.0) mouse_y /= aspectRatio;
	else mouse_x *= aspectRatio;
	if (!realised)
	{
		if (!dragged_elements.empty() || active_tab->selecting_by_quad) return;
		else
		if (active_panel->press(mouse_x, mouse_y, key)) return; //check if press on panel
	}

	if (!key) //left mouse key
	{
		if (!realised) //pressed
		{
			if (!active_tab->try_to_drag_element(mouse_x, mouse_y)) active_tab->start_drag_field(mouse_x, mouse_y);
		}
		else active_tab->release(); //realised
	}
	else
	if (key == 1) //middle mouse key
	{
		if (!realised) //pressed
			if (!active_tab->try_to_change_input_value(mouse_x, mouse_y))
				active_panel->open_close();
	}
	else
	if (key == 2) //right mouse key
	{
		if (!realised) //pressed
		{
			if (active_tab->selecting)
			{
				if (!active_tab->select_element(mouse_x, mouse_y))
				{
					x_where_mouse_was_pressed = mouse_x;
					y_where_mouse_was_pressed = mouse_y;
					Mouse_x = mouse_x;
					Mouse_y = mouse_y;
					active_tab->selecting_by_quad = true;
					workspace_render_needed = true;
				}
				else
				if (active_tab->selection_is_empty())
				{
					active_tab->selecting = false;
					workspace_render_needed = true;
				}
			}
			else
			if (!active_tab->try_to_add_wire(mouse_x, mouse_y))
			{
				active_tab->selecting = true;
				active_tab->selecting_by_quad = true;
			}
		}
		else //realised
		{
			if (active_tab->selecting_by_quad)
			{
				active_tab->selecting_by_quad = false;
				if (active_tab->selection_is_empty()) active_tab->selecting = false;
				workspace_render_needed = true;
			}
			else
			if (adding_wire)
			{
				active_tab->finish_adding_wire(mouse_x, mouse_y);
				adding_wire = false;
				workspace_render_needed = true;
			}
		}
	}
}

void dragging(int base_mouse_x, int base_mouse_y)
{
	double mouse_x = (2*double(base_mouse_x)/glutGet(GLUT_WINDOW_WIDTH) - 1)*scale;
	double mouse_y = (1 - 2*double(base_mouse_y)/glutGet(GLUT_WINDOW_HEIGHT))*scale;
	if (aspectRatio < 1.0) mouse_y /= aspectRatio;
	else mouse_x *= aspectRatio;

	if (function_for_new_element) 
	{
		active_panel->give_birth_to_new_element(mouse_x, mouse_y);
		function_for_new_element = NULL;
	}
	
	if (adding_wire || active_tab->selecting_by_quad)
	{
		Mouse_x = mouse_x;
		Mouse_y = mouse_y;
	}

	for (int i = 0; i < dragged_elements.size(); i++)
	{
		dragged_elements[i]->x = dragged_elements[i]->x_from_where_dragging + mouse_x - x_where_mouse_was_pressed;
		dragged_elements[i]->y = dragged_elements[i]->y_from_where_dragging + mouse_y - y_where_mouse_was_pressed;
		dragged_elements[i]->middle = dragged_elements[i]->x + dragged_elements[i]->x_size/2;
	}

	workspace_render_needed = true;
}
