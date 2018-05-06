#define KEYS_ARRAY_SIZE 223

void (*keys[KEYS_ARRAY_SIZE])() = {};
void nothing() {}

vector<Element*> dragged_elements;
double x_where_mouse_was_pressed, y_where_mouse_was_pressed;

double sq(double x) { return x*x; }

void mouse(int key, int realised, int mouse_x, int mouse_y)
{
	mouse_x -= glutGet(GLUT_WINDOW_WIDTH)/2;
	mouse_y = glutGet(GLUT_WINDOW_HEIGHT)/2 - mouse_y;
	mouse_x = int(double(mouse_x)/4); mouse_y = int(double(mouse_y)/4);
	if (!key) //left mouse key
	{
		if (!realised)	//pressed
		{
			x_where_mouse_was_pressed = double(mouse_x);
			y_where_mouse_was_pressed = double(mouse_y);
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i].x && mouse_x < elements[i].x + elements[i].x_size && mouse_y > elements[i].y && mouse_y < elements[i].y + elements[i].y_size)
				{
					dragged_elements.push_back(&elements[i]);
					elements[i].x_from_where_dragging = elements[i].x;
					elements[i].y_from_where_dragging = elements[i].y;
					break;
				}
		}
		else	//realised
			dragged_elements.clear();
	}
	else
	if (key == 1)	//middle mouse key
	{
		if (!realised)	//pressed
		for (int i = 0; i < elements.size(); i++)
			if (mouse_x > elements[i].x && mouse_x < elements[i].x + elements[i].x_size && mouse_y > elements[i].y && mouse_y < elements[i].y + elements[i].y_size)
			{
				elements[i].change_empty_someput_value(double(mouse_x), double(mouse_y));
				break;
			}
	}
	else	//right mouse key
	{
		if (!realised)	//pressed
		{
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i].x && mouse_x < elements[i].x + elements[i].x_size && mouse_y > elements[i].y && mouse_y < elements[i].y + elements[i].y_size)
				{
					start_adding_wire(&elements[i], double(mouse_x), double(mouse_y));
					break;
				}
		}
		else	//realised
		{
			if (adding_wire)
			for (int i = 0; i < elements.size(); i++)
				if (mouse_x > elements[i].x && mouse_x < elements[i].x + elements[i].x_size && mouse_y > elements[i].y && mouse_y < elements[i].y + elements[i].y_size)
				{
					finish_adding_wire(&elements[i], double(mouse_x), double(mouse_y));
					break;
				}
			adding_wire = false;
		}
	}
}

void dragging(int mouse_x, int mouse_y)
{
	mouse_x -= glutGet(GLUT_WINDOW_WIDTH)/2;
	mouse_y = glutGet(GLUT_WINDOW_HEIGHT)/2 - mouse_y;
	mouse_x = int(double(mouse_x)/4); mouse_y = int(double(mouse_y)/4);
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

void motion(int mouse_x, int mouse_y)
{
	//printf("motion");
}
