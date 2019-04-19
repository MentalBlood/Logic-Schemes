#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <string.h>
#include <cstdlib>

using namespace std;

bool workspace_render_needed = true, 
	 panel_render_needed = true, 
	 tabs_render_needed = true;

#include "logic_functions.cpp"
#include "engine.cpp"
#include "GUI.cpp"
#include "keys.cpp"

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutCreateWindow("Logic schemes interpreter");
	glutDisplayFunc(render_editor);
	glutReshapeFunc(ChangeSize);
	glutTimerFunc(dt, TimerFunction, 1);
	SetupRC();
	ChangeSize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	glutKeyboardFunc(key); setup_keys();
	glutMouseFunc(mouse);
	glutMotionFunc(dragging);
	//glutPassiveMotionFunc(motion);
	
	//default (basic) functions	
	Functions_Base default_functions_base;
	default_functions_base.packs.push_back(Function::Functions_Pack((unsigned char *)"basic", Color(50, 50, 50), Color(255, 255, 255)));
	default_functions_base.add_function((unsigned char*)"AND", AND, 2, 0, 1, 1, (unsigned char *)"basic");
	default_functions_base.add_function((unsigned char*)"OR", OR, 2, 0, 1, 1, (unsigned char *)"basic");
	default_functions_base.add_function((unsigned char*)"NOT", NOT, 1, 1, 1, 1, (unsigned char *)"basic");

	//sample tab
	new_tab();
	new_tab();
	new_tab();

	//elements panel
	Elements_Panel default_panel(&default_functions_base);
	active_panel = &default_panel;
	
	glutMainLoop(); //start
	return 0;
}
