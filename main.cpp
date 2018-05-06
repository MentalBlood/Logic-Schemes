#include <GL/freeglut.h>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <math.h>

using namespace std;

#include "engine.cpp"
#include "logic_functions.h"
#include "keys.cpp"

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Logic schemes interpreter");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutTimerFunc(dt, TimerFunction, 1);
	SetupRC();

	//glutKeyboardFunc(key);
	glutMouseFunc(mouse);
	glutMotionFunc(dragging);
	glutPassiveMotionFunc(motion);
	//setup_keys();
	
	elements.push_back(Element(0, 0, 10, 20, 10, 1, AND));
	elements.push_back(Element(0, 0, 14, 18, 5, 2, OR));
	elements.push_back(Element(20, 30, 8, 15, 3, 4, OR));
	
	glutMainLoop();
	return 0;
}
