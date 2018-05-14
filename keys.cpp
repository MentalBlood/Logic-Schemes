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

void mouse(int key, int realised, int mouse_x, int mouse_y)
{
	mouse_x -= glutGet(GLUT_WINDOW_WIDTH)/2;
	mouse_y = glutGet(GLUT_WINDOW_HEIGHT)/2 - mouse_y;
	mouse_x = int(double(mouse_x)/4); mouse_y = int(double(mouse_y)/4);
	
	if (!realised)
		if (active_panel->press(mouse_x, mouse_y, key)) return; //check if press on panel
	
	if (!key) //left mouse key
	{
		if (!realised) active_tab->try_to_drag(mouse_x, mouse_y); //pressed
		else active_tab->release(); //released
	}
	else
	if (key == 1) //middle mouse key
	{
		if (!realised) active_tab->try_to_change_someput_value(mouse_x, mouse_y); //pressed
	}
	else
	if (key == 2) //right mouse key
	{
		if (!realised) active_tab->try_to_add_wire(mouse_x, mouse_y);
		else //realised
		{
			if (adding_wire)
			{
				active_tab->finish_adding_wire(mouse_x, mouse_y);
				adding_wire = false;
			}
		}
	}
}

void dragging(int mouse_x, int mouse_y)
{
	mouse_x -= glutGet(GLUT_WINDOW_WIDTH)/2;
	mouse_y = glutGet(GLUT_WINDOW_HEIGHT)/2 - mouse_y;
	mouse_x = int(double(mouse_x)/4); mouse_y = int(double(mouse_y)/4);
	
	if (function_for_new_element) 
	{
		active_panel->give_birth_to_new_element(mouse_x, mouse_y);
		function_for_new_element = NULL;
	}
	
	if (adding_wire)
	{
		Mouse_x = mouse_x;
		Mouse_y = mouse_y;
	}

	for (int i = 0; i < dragged_elements.size(); i++)
	{
		dragged_elements[i]->x = dragged_elements[i]->x_from_where_dragging + double(mouse_x) - x_where_mouse_was_pressed;
		dragged_elements[i]->y = dragged_elements[i]->y_from_where_dragging + double(mouse_y) - y_where_mouse_was_pressed;
		dragged_elements[i]->middle = dragged_elements[i]->x + dragged_elements[i]->x_size/2;
	}
}
