#include"Header.h"
#include"light.h"
#include"Obj.h"
#include"texture_sunkue.h"
#include"shader_sunkue.h"
#include"camera.h"
#include"PLANE.h"
#include"screen.h"
#include"Alpha_blending.h"

#define FLYING 1
#define FPS 60 

/*Funcs*/
GLvoid drawScene(GLvoid);
/**/
GLvoid Reshape(int w, int h);
GLvoid SpecialInput(int key, int x, int y);
GLvoid SpecialInput_up(int key, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid keyboard_up(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid MouseWheel(int button, int dir, int x, int y);
GLvoid Timer(int value);
/**/
GLvoid MakeShape();
GLvoid DefaultObj();
GLvoid print_message();
/**/


Obj coordinate, world, ground, building[buildingnum], cloud[cloudnum];
LIGHT sun, moon;
std::vector<LIGHT> thunder, bullet;

/*-----MAIN--*/
int main(int argc, char** argv) {
	srand((unsigned int)time(NULL));
	/* window */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(screen.width, screen.height);
	int winID = glutCreateWindow("OPEN GL");
	/* op gl */
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	if (!GLEW_VERSION_3_3) {

		std::cerr << "Error: OpenGL 3.3 API is not available.\n";
		std::exit(EXIT_FAILURE);
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	std::cout << "\n\n\n";

	/* glsl shader */
	shaderID = InitShader();
	if (!shaderID) {
		std::cerr << "Error: Shader Program ���� ����\n";
		std::exit(EXIT_FAILURE);
	}
	/* uniform */
	init_uniform_Loc();

	/*  */
	set_flip_texture(true);
	DefaultObj();
	MakeShape();

	/* states */
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Funcs */
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutSpecialFunc(SpecialInput);
	glutSpecialUpFunc(SpecialInput_up);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(keyboard_up);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMouseWheelFunc(MouseWheel);

	glutTimerFunc(50, Timer, 0);
	glutTimerFunc(12000 / FPS, Timer, 1);

	print_message();

	glutMainLoop();
}
GLvoid print_message() {
	std::cout <<
		"\
		-= -= -= -= -= -= -= -= -= -= -= -= -= -= -\n\
		time_stop[space]\n\
		head_light [ 1 2 3 ]\n\
		plane[w a s d up(8) down(5) left(4) right(6)]\n\
		rebuilding[tab]\n\
		camera[i j k l  mouse_wheel]\n\
		=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\
		";
}
GLvoid DefaultObj() {
	/*world*/
	LoadObj("sphere.obj", world, "8/8/8");
	LoadTexture(world, "skydome.jpg", 2048, 1024, 3);
	world.M.push_back(glm::scale(df, screen.size_of_world*100.0f));
	world.Reverse_nor();

	/*light*/
	LoadObj("sphere.obj", sun.obj, "8/8/8");
	sun.obj.Set_Color({ 10.0, 5.0, 5.0, 1.0 });
	sun.pos = { groundsize, 0.0, 0.0 };
	sun.col = { 100.0,100.0,100.0 };
	sun.obj.M.resize(2, df);
	sun.obj.M.push_back(glm::scale(df, glm::vec3(80.0f)));



	LoadObj("sphere.obj", moon.obj, "8/8/8");
	moon.obj.Set_Color({ 1.0, 1.0, 10.0, 1.0 });
	moon.pos = { -groundsize, 0.0, 0.0 };
	moon.col = { 0.5,0.5,100.0 };
	moon.obj.M.resize(2, df);
	moon.obj.M.push_back(glm::scale(df, glm::vec3(40.0f)));


	/*coord*/
	{
		Vertex V;
		GLfloat sizex = screen.size_of_world[0] * 0.8f;
		GLfloat sizey = screen.size_of_world[1] * 0.8f;
		GLfloat sizez = screen.size_of_world[2] * 0.8f;
		constexpr int VofC{ 100 };
		constexpr int dd{ VofC - 1 };
		constexpr int iy = 2 * VofC;
		constexpr int iz = 3 * VofC;
		for (GLfloat i = 0; i < iz; i++) {
			if (i < VofC) {
				V.col = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
				V.pos = glm::vec3(-sizex + 2 * i * sizex / dd, 0.0f, 0.0f);
			}
			else if (i < iy) {
				V.col = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				V.pos = glm::vec3(0.0f, -sizey + 2 * (i - VofC) * sizey / dd, 0.0f);
			}
			else {
				V.col = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);
				V.pos = glm::vec3(0.0f, 0.0f, -sizez + 2 * (i - iy) * sizez / dd);
			}
			coordinate.objData.vertices.push_back(V);
			coordinate.objData.verIndices.push_back(i);
		}
		coordinate.shape = GL_LINES;
		InitBuffer(coordinate);
	}
}
GLvoid MakeShape() {
	plane.init();
	{
		for (int i = 0; i < buildingnum; i++) {
			LoadObj("apartment.obj", building[i], "8/8/8");
			building[i].Set_Color({ 0.5f,0.5f,GLfloat(rand() % 10) / 10.0f,1.0f });
			building[i].M.resize(3, df);
			building[i].M.at(2) = glm::scale(df, glm::vec3(10.0f));
			building[i].M.at(1) = glm::rotate(df,glm::radians(GLfloat(rand()%360)), {0.0,1.0,0.0});
			building[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize)*2) - groundsize,ground_floor,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
	}
	{
		for (int i = 0; i < cloudnum; i++) {
			LoadObj("cloud.obj", cloud[i], "8/8/8");
			cloud[i].Set_Color({ 1.0f,1.0f,1.0f,0.5f });
			cloud[i].M.resize(3, df);
			cloud[i].M.at(2) = glm::scale(df, glm::vec3(1.0f));
			cloud[i].M.at(1) = glm::rotate(df, glm::radians(GLfloat(rand() % 360)), { 0.0,1.0,0.0 });
			cloud[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize) * 2) - groundsize,ground_floor + GLfloat(rand()%800)+1800.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
	}
	{
		LoadObj("cube.txt", ground, "8/8/8");
		LoadTexture(ground, "grass.jpg", 512, 512, 3);
		ground.M.push_back(glm::translate(df, { 0.0, ground_floor,0.0 }));
		ground.M.push_back(glm::scale(df, { groundsize,10.0,groundsize }));
	}
	/**/
};


bool cull_this_in_proj(Obj& obj, glm::mat4& PV) {
	// �ӵ����� ���� ���� �ڵ�, ����Ȯ������ �ϴ� ���.
	glm::vec3 pivot = obj.objData.vertices.at(1).pos;
	glm::vec4 gl_pos = PV * obj.world_M() * glm::vec4(pivot, 1.0f);
	gl_pos = gl_pos / gl_pos.w;
	if (1.0f < abs(gl_pos.x))return true;
	else if (1.0f < abs(gl_pos.y))return true;	
	else if (1.0f < gl_pos.z)return true;
	else return false;
}
/*�׸��� �Լ�*/
GLvoid drawScene() {
	/*�⺻ ���*/
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.view_M()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(screen.proj_M()));
	LIGHT::init_light_buffer();
	glUniform3fv(lightPosLoc, LIGHT::light_num, LIGHT::lights_pos);
	glUniform3fv(lightColLoc, LIGHT::light_num, LIGHT::lights_col);
	glUniform1iv(use_spotLoc, LIGHT::light_num, LIGHT::lights_use_spot);
	glUniform1fv(spot_cosLoc, LIGHT::light_num, LIGHT::lights_spot_cos);
	glUniform1fv(spot_out_cosLoc, LIGHT::light_num, LIGHT::lights_spot_out_cos);
	glUniform3fv(spot_dirLoc, LIGHT::light_num, LIGHT::lights_spot_dir);

	glUniform3f(ambientColorLoc, LIGHT::ambientColor.r, LIGHT::ambientColor.g, LIGHT::ambientColor.b);
	glUniform3f(viewPosLoc, camera.EYE.x, camera.EYE.y, camera.EYE.z);
	glUniform1f(ambientLoc, LIGHT::ambient);
	glUniform3f(specLoc, sun.spec.r, sun.spec.g, sun.spec.b);
	glUniform1i(texLoc, 0);
	glUniform1i(light_numLoc, LIGHT::light_num);

	//std::cout << light_num;
	std::vector<Obj*> Alpha_objs;
	Alpha_objs.reserve(16);
	/*�׸��� ����*/
	{
		glFrontFace(GL_CW);
		drawObj(world);
		glFrontFace(GL_CCW);
		drawObj(plane.obj);

		/*������*/
		drawObj(sun.obj);
		drawObj(moon.obj);
		drawObj(ground);
		for (int i = 0; i < buildingnum; i++) {
			drawObj(building[i]);
		}
		for (int i = 0; i < cloudnum; i++) {
			drawObj(cloud[i]);
		}
	}
	/*alpha*/
	/*���� ALpha_objs �� push �ϸ� ������ �� ��ο� ��*/
	{	
		/*
		����ȭ�� ��Ȯ�� ������ ��� �ߴ�.

		glm::mat4 PV = df * screen.proj_M() * camera.view_M();
		for (int i = 0; i < cloudnum; i++) {
			if (!cull_this_in_proj(cloud[i], PV)) {
				Alpha_objs.push_back(&cloud[i]);
			}
		}
		*/
	}
	/*�׸��� ��*/
	Sort_Alpha_blending(Alpha_objs);
	Draw_Alpha_blending(Alpha_objs);
	
	glutSwapBuffers();
}



/*�̺�Ʈ �Լ�*/
bool P_go, P_stop, P_YL, P_YR, P_RL, P_RR, P_PU, P_PD, bl_snow, stealth, timeStop;
bool up, down, left, right;
GLvoid Timer(int value) {
	constexpr GLfloat degree{ 5.0f };
	switch (value)
	{
	case 0: {
		if (timeStop == false) {
			constexpr GLfloat light_degree{ -1.0f };
			sun.pos = glm::rotate(df, glm::radians(light_degree), { 0.0,0.0,1.0 }) * glm::vec4{ sun.pos ,1.0 };
			sun.update();
			if (ground_floor < sun.pos.y) {
				GLfloat d = sun.pos.y / groundsize;
				LIGHT::ambient = d;
				LIGHT::ambientColor = { 1.0,d,1.0 };
				sun.on();
			}
			else sun.off();
			moon.pos = glm::rotate(df, glm::radians(light_degree), { 0.0,0.0,1.0 }) * glm::vec4{ moon.pos ,1.0 };
			moon.update();
			if (ground_floor < moon.pos.y) {
				LIGHT::ambient = 0.3f;
				LIGHT::ambientColor = { 0.2,0.2,1.0 };
				moon.on();
			}
			else moon.off();
		}
		
		if (stealth) {
			constexpr int tm{ 50 };
			constexpr int tm2{ 25 };
			static int time{ tm };
			if(tm2<time)plane.Stealth(true);
			else if(time%2) {
				plane.Stealth(true);
			}
			else {
				plane.Stealth(false);
			}
			time--;
			if (time < 0)stealth = false, time = tm;
		}
		
		
		glutTimerFunc(50, Timer, value);
		break;
	}
	case 1: {

		if (up) {
			glm::mat4 R = glm::rotate(df, glm::radians(-degree/(FPS/6)), camera.Right());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (down) {
			glm::mat4 R = glm::rotate(df, glm::radians(degree/(FPS / 6)), camera.Right());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (right) {
			glm::mat4 R = glm::rotate(df, glm::radians(degree/(FPS / 6)), camera.Up());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}
		if (left) {
			glm::mat4 R = glm::rotate(df, glm::radians(-degree/(FPS / 6)), camera.Up());
			plane.viewMat = R * plane.viewMat;
			camera.UP = glm::vec3(R * glm::vec4(camera.UP, 1.0f));
		}

		if (P_go) {
			plane.set_speed(0.08f);
		}
		if (P_stop) {
			plane.set_speed(-0.1f);
		}
		if (P_YL) {
			//ver 1
			//plane.Yaw(degree / (FPS / 6));
		}
		if (P_YR) {
			//ver 1
			//plane.Yaw(-degree / (FPS / 6));
		}
		if (P_RL) {
			plane.Roll(degree / (FPS / 24));
			//plane.Pitch(degree / (FPS / 4.8));
		}
		if (P_RR) {
			plane.Roll(-degree / (FPS / 24));
			//plane.Pitch(degree / (FPS / 4.8));
		}
		if (P_PU) {
			plane.Pitch(degree / (FPS / 4.8));
		}
		if (P_PD) {
			plane.Pitch(-degree / (FPS / 4.8));
		}
		plane.update();
		glutTimerFunc(1200 / FPS, Timer, value);
		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (button) {
	case GLUT_LEFT_BUTTON: {
		switch (state)
		{
		case GLUT_UP: {

			break;
		}
		case GLUT_DOWN: {

			break;
		}
		default: { break; }
		}
		break;
	}
	case GLUT_RIGHT_BUTTON: {
		break;
	}
	case GLUT_MIDDLE_BUTTON: {
		switch (state) {
		case GLUT_UP: {
			break;
		}
		case GLUT_DOWN: {
			break;
		}
		default: { break; }
		}
		break;
	}
	}
}
GLvoid Motion(int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	
	glutPostRedisplay();
}
GLvoid MouseWheel(int button, int dir, int x, int y) {
	if (dir > 0)
	{
		plane.view_dist_add(-10.0f);
	}
	else
	{
		plane.view_dist_add(10.0f);
	}
	glutPostRedisplay();
}
GLvoid SpecialInput(int key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case GLUT_KEY_UP: {
		P_PU = true;
		break;
	}
	case GLUT_KEY_DOWN: {
		P_PD = true;
		break;
	}
	case GLUT_KEY_RIGHT: {
		P_RR = true;
		break;
	}
	case GLUT_KEY_LEFT: {
		P_RL = true;
		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid SpecialInput_up(int key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case GLUT_KEY_UP: {
		P_PU = false;
		break;
	}
	case GLUT_KEY_DOWN: {
		P_PD = false;
		break;
	}
	case GLUT_KEY_RIGHT: {
		P_RR = false;
		break;
	}
	case GLUT_KEY_LEFT: {
		P_RL = false;
		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case 'i': {
		up = true;
		break;
	}
	case 'k': {
		down = true;
		break;
	}
	case 'j': {
		left = true;
		break;
	}
	case 'l': {
		right = true;
		break;
	}
	case '0': {
		if (!stealth) {
			stealth = true;
		}
		break;
	}
	case '\t': {
		for (int i = 0; i < buildingnum; i++) {
			building[i].M.at(0) = glm::translate(df, { GLfloat(rand() % int(groundsize)*2) - groundsize,-40.0f,GLfloat(rand() % int(groundsize) * 2) - groundsize });
		}
		break;
	}
	case '1': {
		plane.HeadLightOnOff();
		break;
	}
	case '2': {
		plane.head_light.spot_theta = 2.5f;
		plane.head_light.col = glm::vec3(1.0f);
		break;
	}
	case '3': {
		plane.head_light.spot_theta = 1.f;
		plane.head_light.col = glm::vec3(2.0f);
		break;
	}
	case 'C':
	case 'c': {
		plane.viewMat = df;
		camera.UP = { 0.0,1.0,0.0 };
		break;
	}
	case 'W':
	case 'w': {
		P_go = true;
		break;
	}
	case 'S':
	case 's': {
		P_stop = true;
		break;
	}
	case 'A':
	case 'a': {
		P_YL = true;
		break;
	}
	case 'D':
	case 'd': {
		P_YR = true;
		break;
	}
	case '4': {
		P_RL = true;
		break;
	}
	case '6': {
		P_RR = true;
		break;
	}
	case '8': {
		P_PD = true;
		break;
	}
	case '5': {
		P_PU = true;
		break;
	}	
	case ' ': {
		timeStop = timeStop ? false : true;

		break;
	}
	default: { break; }
	}
	glutPostRedisplay();
}
GLvoid keyboard_up(unsigned char key, int x, int y) {
	GLfloat GLx = { ((float)x / screen.width) * 2 - 1 }, GLy{ (-((float)y / screen.height) * 2) + 1 };
	switch (key)
	{
	case 'i': {
		up = false;
		break;
	}
	case 'k': {
		down = false;
		break;
	}
	case 'j': {
		left = false;
		break;
	}
	case 'l': {
		right = false;
		break;
	}
	case 'W':
	case 'w': {
		P_go = false;
		break;
	}
	case 'S':
	case 's': {
		P_stop = false;
		break;
	}
	case 'A':
	case 'a': {
		P_YL = false;
		break;
	}
	case 'D':
	case 'd': {
		P_YR = false;
		break;
	}
	case '4': {
		P_RL = false;
		break;
	}
	case '6': {
		P_RR = false;
		break;
	}
	case '8': {
		P_PD = false;
		break;
	}
	case '5': {
		P_PU = false;
		break;
	}
	default:
		break;
	}
	glutPostRedisplay();
}
GLvoid Reshape(int w, int h) {
	screen.width = w, screen.height = h;
	glViewport(0, 0, screen.width, screen.height);
}