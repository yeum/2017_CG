#include <glut.h>

#include <fstream>

#include <cstdlib>

#include <stdio.h>   // sprintf

using namespace std;

#define MAX_CUBE 30
#define CUBE_SIZE 20
#define JOB_TABLE_SIZE 5
#define MAX_HEIGHT 10

struct XYZPOS {
	int x, y, z;
};

struct BUILD {
	int x, y, z;
	bool put;
};

enum GameMode { Main, Play, Tool };

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void TimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void SpecialFunc(int key, int x, int y);
void SetupRC();
void SetupRC_byMode();
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

float Width, Height;
float DegreeX, DegreeY, DegreeZ;
float MoveX, MoveY, MoveZ;
XYZPOS table_top[4], table_bottom[4], job_table[4];
XYZPOS table2_top[4], table2_bottom[4], job_table2[4];
BUILD build_cube[JOB_TABLE_SIZE][MAX_HEIGHT][JOB_TABLE_SIZE];
BUILD quest_cube[JOB_TABLE_SIZE][MAX_HEIGHT][JOB_TABLE_SIZE];
XYZPOS picked_cube, build_idx;
int picked_idx;
XYZPOS mouse;
int Mode, PreMode;
float timer;

bool LEFT_BUTTON, RIGHT_BUTTON;
bool SAVE_ON;


void main(int argc, char *argv[])
{
	Width = 800;
	Height = 600;

	DegreeX = 0;
	DegreeY = 0;
	DegreeZ = 0;
	MoveX = -300;
	MoveY = -50;
	MoveZ = -600;

	PreMode = GameMode::Main;
	Mode = GameMode::Main;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ���� 
	glutInitWindowPosition(100, 100); // �������� ��ġ���� 
	glutInitWindowSize(800, 600); // �������� ũ�� ���� 
	glutCreateWindow("Example"); // ������ ���� (������ �̸�) 
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ��� ���� 
	glutSpecialFunc(SpecialFunc);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	SetupRC();   // ���� ���� �ʱ�ȭ �Լ�
	SetupRC_byMode();   // ���� ���� �ʱ�ȭ �Լ�
	glutMainLoop();
}

void SetupRC() {
	// �ʿ��� ������, ��ǥ�� ���� �ʱ�ȭ
	// ��� ���� �ʱ�ȭ

	// �÷��� ���̺�
	table_top[0].y = table_top[1].y = table_top[2].y = table_top[3].y = 0;
	table_top[0].x = table_top[3].x = 150;
	table_top[1].x = table_top[2].x = -150;
	table_top[0].z = table_top[1].z = -100;
	table_top[2].z = table_top[3].z = 100;

	table_bottom[0].y = table_bottom[1].y = table_bottom[2].y = table_bottom[3].y = -30;
	table_bottom[0].x = table_bottom[3].x = 150;
	table_bottom[1].x = table_bottom[2].x = -150;
	table_bottom[0].z = table_bottom[1].z = -100;
	table_bottom[2].z = table_bottom[3].z = 100;

	job_table[0].y = job_table[1].y = job_table[2].y = job_table[3].y = 1;
	job_table[0].x = job_table[3].x = 50;
	job_table[1].x = job_table[2].x = -50;
	job_table[0].z = job_table[1].z = -50;
	job_table[2].z = job_table[3].z = 50;

	// ���� ���̺�
	table2_top[0].y = table2_top[1].y = table2_top[2].y = table2_top[3].y = 0;
	table2_top[0].x = table2_top[3].x = 750;
	table2_top[1].x = table2_top[2].x = 450;
	table2_top[0].z = table2_top[1].z = -100;
	table2_top[2].z = table2_top[3].z = 100;

	table2_bottom[0].y = table2_bottom[1].y = table2_bottom[2].y = table2_bottom[3].y = -30;
	table2_bottom[0].x = table2_bottom[3].x = 750;
	table2_bottom[1].x = table2_bottom[2].x = 450;
	table2_bottom[0].z = table2_bottom[1].z = -100;
	table2_bottom[2].z = table2_bottom[3].z = 100;

	job_table2[0].y = job_table2[1].y = job_table2[2].y = job_table2[3].y = 1;
	job_table2[0].x = job_table2[3].x = 650;
	job_table2[1].x = job_table2[2].x = 550;
	job_table2[0].z = job_table2[1].z = -50;
	job_table2[2].z = job_table2[3].z = 50;
}

void SetupRC_byMode()
{
	if (Mode == GameMode::Tool)
	{
		picked_idx = 0;
		build_idx = { 2,0,4 };

		// ���κ����ġ ����迭 build_cube[x][y][z];
		// x�� ���ʿ��� �������� ���ι���
		// z�� ���ʿ��� �ٱ��������� ���ι���
		for (int i = 0; i < JOB_TABLE_SIZE; ++i)
		{// x
			for (int j = 0; j < MAX_HEIGHT; ++j)
			{// y
				for (int k = 0; k < JOB_TABLE_SIZE; ++k)
				{// z
					build_cube[i][j][k].x = table2_top[0].x - (table2_top[0].x - table2_top[1].x) / 2 + (i - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
					build_cube[i][j][k].z = (k - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
					build_cube[i][j][k].y = j*CUBE_SIZE;
					build_cube[i][j][k].put = false;
				}
			}
		}

		picked_cube.x = table2_top[0].x - (table2_top[0].x - table2_top[1].x) / 2;
		picked_cube.y = job_table2[0].y + CUBE_SIZE / 2;
		picked_cube.z = job_table2[3].z - CUBE_SIZE / 2;
	}
	else
	{
		picked_idx = 0;
		build_idx = { 2,0,4 };

		// ���κ����ġ ����迭 build_cube[x][y][z];
		// x�� ���ʿ��� �������� ���ι���
		// z�� ���ʿ��� �ٱ��������� ���ι���
		for (int i = 0; i < JOB_TABLE_SIZE; ++i)
		{// x
			for (int j = 0; j < MAX_HEIGHT; ++j)
			{// y
				for (int k = 0; k < JOB_TABLE_SIZE; ++k)
				{// z
					build_cube[i][j][k].x = (i - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
					build_cube[i][j][k].z = (k - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
					build_cube[i][j][k].y = j*CUBE_SIZE;
					build_cube[i][j][k].put = false;
				}
			}
		}

		picked_cube.x = 0;
		picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
		picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
	}
}

void SpecialFunc(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		if (Mode == GameMode::Play)
		{
			if (picked_cube.z > job_table[0].z + CUBE_SIZE)
			{
				picked_cube.z -= CUBE_SIZE;
				build_idx.z--;
			}
		}
		else if (Mode == GameMode::Tool)
		{
			if (picked_cube.z > job_table2[0].z + CUBE_SIZE)
			{
				picked_cube.z -= CUBE_SIZE;
				build_idx.z--;
			}
		}
	}
	else if (key == GLUT_KEY_DOWN)
	{
		if (Mode == GameMode::Play)
		{
			if (picked_cube.z < job_table[3].z - CUBE_SIZE)
			{
				picked_cube.z += CUBE_SIZE;
				build_idx.z++;
			}
		}
		else if (Mode == GameMode::Tool)
		{
			if (picked_cube.z < job_table2[3].z - CUBE_SIZE)
			{
				picked_cube.z += CUBE_SIZE;
				build_idx.z++;
			}
		}
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		if (Mode == GameMode::Play)
		{
			if (picked_cube.x < job_table[0].x - CUBE_SIZE)
			{
				picked_cube.x += CUBE_SIZE;
				build_idx.x++;
			}
		}
		else if (Mode == GameMode::Tool)
		{
			if (picked_cube.x < job_table2[0].x - CUBE_SIZE)
			{
				picked_cube.x += CUBE_SIZE;
				build_idx.x++;
			}
		}
	}
	else if (key == GLUT_KEY_LEFT)
	{
		if (Mode == GameMode::Play)
		{
			if (picked_cube.x > job_table[1].x + CUBE_SIZE)
			{
				picked_cube.x -= CUBE_SIZE;
				build_idx.x--;
			}
		}
		else if (Mode == GameMode::Tool)
		{
			if (picked_cube.x > job_table2[1].x + CUBE_SIZE)
			{
				picked_cube.x -= CUBE_SIZE;
				build_idx.x--;
			}
		}
	}

	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'i' || key == 'I')
	{
		SetupRC();
		Reshape(Width, Height);
	}
	else if (key == 'x' || key == 'X')
	{
		DegreeX -= 1;
		Reshape(Width, Height);
	}
	else if (key == 'y' || key == 'Y')
	{
		DegreeY -= 1;
		Reshape(Width, Height);
	}
	else if (key == 'z' || key == 'Z')
	{
		DegreeZ -= 1;
		Reshape(Width, Height);
	}
	else if (key == 'w' || key == 'W')
	{
		if (picked_cube.y < (MAX_HEIGHT - 1) * CUBE_SIZE)
		{
			picked_cube.y += CUBE_SIZE;
			build_idx.y++;
		}
	}
	else if (key == 's' || key == 'S')
	{
		if (picked_cube.y > job_table[0].y + CUBE_SIZE)
		{
			picked_cube.y -= CUBE_SIZE;
			build_idx.y--;
		}
	}
	else if (key == '-')
	{
		MoveZ -= 10;
		Reshape(Width, Height);
	}
	else if (key == '=')
	{
		MoveZ += 10;
		Reshape(Width, Height);
	}
	else if (key == 32)
	{// �����̽��� �Է� => �� ����
		if (build_cube[build_idx.x][build_idx.y][build_idx.z].put == false)
		{
			if (Mode == GameMode::Play)
			{
				picked_cube.x = 0;
				picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
				picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
			}
			else if (Mode == GameMode::Tool)
			{
				picked_cube.x = table2_top[0].x - (table2_top[0].x - table2_top[1].x) / 2;
				picked_cube.y = job_table2[0].y + CUBE_SIZE / 2;
				picked_cube.z = job_table2[3].z - CUBE_SIZE / 2;
			}
			build_cube[build_idx.x][build_idx.y][build_idx.z].put = true;
			build_idx = { 2,0,4 };
		}

	}
	else if (key == 'l' || key == 'L')
	{// l or L ���� �� �ҷ�����
		char buf[100]{ 0 };      // ���ϸ��� ������ ����
		int file_num = 0;      // ���° �������� ���

		for (file_num = 0; file_num < 100; ++file_num) {      // 0���� �ִ� 100���� ������ ����� ����.
			sprintf_s(buf, "build_cube_%d.txt", file_num);      // ���ۿ� file_num��° ���ϸ��� ����
			ifstream read;
			read.open(buf);
			if (!read) {   // read�� ���� ������ �������� ������� break
				break;
			}
			read.close();
		}
		sprintf_s(buf, "build_cube_%d.txt", rand() % file_num);   // file_num �������� ������ �����Ƿ� �� ���̿��� rand�� �Ѵ�.

		FILE *fp;
		fopen_s(&fp, buf, "r");
		int a, b, c;
		while (!feof(fp)) {
			a = -1, b = -1, c = -1;
			fscanf_s(fp, "%d,%d,%d", &a, &b, &c);
			printf("%d %d %d\n", a, b, c);
			if (a == -1 || b == -1 || c == -1) {

			}
			/*else {
			quest_cube[a][b][c].put = true;
			}*/
			else {
				build_cube[a][b][c].put = true;
			}
		}

	}
	else if (key == 8)
	{// �� ���� ��ġ���� �齺���̽��� > �� �����
		if (build_cube[build_idx.x][build_idx.y][build_idx.z].put == true)
		{
			if (Mode == GameMode::Play)
			{
				picked_cube.x = 0;
				picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
				picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
			}
			else if (Mode == GameMode::Tool)
			{
				picked_cube.x = table2_top[0].x - (table2_top[0].x - table2_top[1].x) / 2;
				picked_cube.y = job_table2[0].y + CUBE_SIZE / 2;
				picked_cube.z = job_table2[3].z - CUBE_SIZE / 2;
			}
			build_cube[build_idx.x][build_idx.y][build_idx.z].put = false;
			build_idx = { 2,0,4 };


		}
	}
	else if (key == 13)
	{// �� �� �װ� ���� > ����
		ofstream WriteFile;
		char buf[100]{ 0 };      // ���ϸ��� ������ ����
		int file_num = 0;      // ���° �������� ���

		for (file_num = 0; file_num < 100; ++file_num) {      // 0���� �ִ� 100���� ������ ����� ����.
			sprintf_s(buf, "build_cube_%d.txt", file_num);      // ���ۿ� file_num��° ���ϸ��� ����
			ifstream read;
			read.open(buf);
			if (!read) {   // read�� ���� ������ �������� ������� break
				break;
			}
			read.close();
		}

		WriteFile.open(buf, ios_base::trunc);
		WriteFile.close();
		for (int i = 0; i < JOB_TABLE_SIZE; ++i)
		{
			for (int j = 0; j < MAX_HEIGHT; ++j)
			{
				for (int k = 0; k < JOB_TABLE_SIZE; ++k)
				{
					if (build_cube[i][j][k].put)
					{
						ofstream WriteFile;
						WriteFile.open(buf, ios_base::app);
						WriteFile << i << ',' << j << ',' << k << endl;
						WriteFile.close();
						build_cube[i][j][k].put = false;
					}
				}
			}
		}

		SAVE_ON = true;
		build_idx = { 2,0,4 };
	}
	else if (key == '1')
	{
		if (Mode != GameMode::Play)
		{
			DegreeX = 0;
			DegreeY = 0;
			DegreeZ = 0;
			PreMode = Mode;
			Mode = GameMode::Play;
			SetupRC_byMode();

			char buf[100]{ 0 };      // ���ϸ��� ������ ����
			int file_num = 0;      // ���° �������� ���

			for (file_num = 0; file_num < 100; ++file_num) {      // 0���� �ִ� 100���� ������ ����� ����.
				sprintf_s(buf, "build_cube_%d.txt", file_num);      // ���ۿ� file_num��° ���ϸ��� ����
				ifstream read;
				read.open(buf);
				if (!read) {   // read�� ���� ������ �������� ������� break
					break;
				}
				read.close();
			}
			sprintf_s(buf, "build_cube_%d.txt", rand() % file_num);   // file_num �������� ������ �����Ƿ� �� ���̿��� rand�� �Ѵ�.

			FILE *fp;
			fopen_s(&fp, buf, "r");
			int a, b, c;
			while (!feof(fp)) {
				a = -1, b = -1, c = -1;
				fscanf_s(fp, "%d,%d,%d", &a, &b, &c);
				printf("%d %d %d\n", a, b, c);
				if (a == -1 || b == -1 || c == -1) {

				}
				/*else {
				quest_cube[a][b][c].put = true;
				}*/
				else {
					build_cube[a][b][c].put = true;
				}
			}
		}
	}
	else if (key == '2')
	{
		if (Mode != GameMode::Tool)
		{
			DegreeX = 0;
			DegreeY = 0;
			DegreeZ = 0;
			PreMode = Mode;
			Mode = GameMode::Tool;
			SetupRC_byMode();
		}
	}

	else if (key == 27)
	{// ����
		exit(0);
	}
	glutPostRedisplay();
}

void TimerFunction(int value)
{
	if (PreMode == GameMode::Main && Mode == GameMode::Play)
	{
		MoveX += 20;
		MoveZ += 20;

		if (MoveX >= 0)
			MoveX = 0;
		if (MoveZ >= -300)
			MoveZ = -300;
	}
	else if (PreMode == GameMode::Main && Mode == GameMode::Tool)
	{
		MoveX -= 20;
		MoveZ += 20;

		if (MoveX <= -600)
			MoveX = -600;
		if (MoveZ >= -300)
			MoveZ = -300;
	}
	else if (PreMode == GameMode::Play && Mode == GameMode::Tool)
	{
		MoveX -= 20;

		if (MoveX <= -600)
			MoveX = -600;
	}
	else if (PreMode == GameMode::Tool && Mode == GameMode::Play)
	{
		MoveX += 20;

		if (MoveX >= 0)
			MoveX = 0;
	}

	if (SAVE_ON)
		timer++;
	Reshape(Width, Height);
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}

// ������ ��� �Լ�
GLvoid drawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	GLfloat ambientLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat diffuseLight[] = { 1 ,1, 1, 1.0 };
	GLfloat specular[] = { 0.4f, 0.4f, 0.4f, 1.0 };
	GLfloat lightPos[] = { -150, 50, 150.0, 1.0 };
	GLfloat lightPos2[] = { 650, 100, 150.0, 1.0 };

	glEnable(GL_LIGHTING);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	if (Mode == GameMode::Play)
		glEnable(GL_LIGHT0);
	else
		glDisable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
	if (Mode == GameMode::Tool)
		glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//glLoadIdentity();
	glPushMatrix();
	{

		glPushMatrix();
		{   ///////�÷��� ���̺� �׸���///////
			glTranslatef(0, -50, 0);
			glPushMatrix();
			{
				glColor3f(0.5f, 0.3f, 0.0f);
				glBegin(GL_QUADS);   // ����
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				// �Ʒ���
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glEnd();

				glColor3f(0.3f, 0.1f, 0.0f);
				glBegin(GL_QUADS);
				// ����
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);

				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);

				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);

				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glEnd();

				glPushMatrix();
				{   //////�����̺�//////
					// �� ĭ�� 20 //
					glColor3f(1, 1, 1);
					glBegin(GL_LINES);      // ������
					for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
					{
						glVertex3f(job_table[0].x - 20 * i, job_table[0].y, job_table[0].z);
						glVertex3f(job_table[0].x - 20 * i, job_table[3].y, job_table[3].z);
					}
					glEnd();

					glBegin(GL_LINES);      // ������
					for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
					{
						glVertex3f(job_table[1].x, job_table[1].y, job_table[1].z + 20 * i);
						glVertex3f(job_table[3].x, job_table[3].y, job_table[1].z + 20 * i);
					}
					glEnd();
				}
				glPopMatrix();
			}
			glPopMatrix();

			glPushMatrix();
			{   ///////���� ���̺� �׸���///////
				glColor3f(0.5f, 0.3f, 0.0f);
				glBegin(GL_QUADS);   // ����
				glVertex3f(table2_top[0].x, table2_top[0].y, table2_top[0].z);
				glVertex3f(table2_top[1].x, table2_top[1].y, table2_top[1].z);
				glVertex3f(table2_top[2].x, table2_top[2].y, table2_top[2].z);
				glVertex3f(table2_top[3].x, table2_top[3].y, table2_top[3].z);
				// �Ʒ���
				glVertex3f(table2_bottom[1].x, table2_bottom[1].y, table2_bottom[1].z);
				glVertex3f(table2_bottom[0].x, table2_bottom[0].y, table2_bottom[0].z);
				glVertex3f(table2_bottom[3].x, table2_bottom[3].y, table2_bottom[3].z);
				glVertex3f(table2_bottom[2].x, table2_bottom[2].y, table2_bottom[2].z);
				glEnd();

				glColor3f(0.3f, 0.1f, 0.0f);
				glBegin(GL_QUADS);
				// ����
				glVertex3f(table2_top[1].x, table2_top[1].y, table2_top[1].z);
				glVertex3f(table2_top[0].x, table2_top[0].y, table2_top[0].z);
				glVertex3f(table2_bottom[0].x, table2_bottom[0].y, table2_bottom[0].z);
				glVertex3f(table2_bottom[1].x, table2_bottom[1].y, table2_bottom[1].z);

				glVertex3f(table2_top[2].x, table2_top[2].y, table2_top[2].z);
				glVertex3f(table2_top[1].x, table2_top[1].y, table2_top[1].z);
				glVertex3f(table2_bottom[1].x, table2_bottom[1].y, table2_bottom[1].z);
				glVertex3f(table2_bottom[2].x, table2_bottom[2].y, table2_bottom[2].z);

				glVertex3f(table2_top[0].x, table2_top[0].y, table2_top[0].z);
				glVertex3f(table2_top[3].x, table2_top[3].y, table2_top[3].z);
				glVertex3f(table2_bottom[3].x, table2_bottom[3].y, table2_bottom[3].z);
				glVertex3f(table2_bottom[0].x, table2_bottom[0].y, table2_bottom[0].z);

				glVertex3f(table2_top[3].x, table2_top[3].y, table2_top[3].z);
				glVertex3f(table2_top[2].x, table2_top[2].y, table2_top[2].z);
				glVertex3f(table2_bottom[2].x, table2_bottom[2].y, table2_bottom[2].z);
				glVertex3f(table2_bottom[3].x, table2_bottom[3].y, table2_bottom[3].z);
				glEnd();

				glPushMatrix();
				{   //////�����̺�//////
					// �� ĭ�� 20 //
					glColor3f(1, 1, 1);
					glBegin(GL_LINES);      // ������
					for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
					{
						glVertex3f(job_table2[0].x - 20 * i, job_table2[0].y, job_table2[0].z);
						glVertex3f(job_table2[0].x - 20 * i, job_table2[3].y, job_table2[3].z);
					}
					glEnd();

					glBegin(GL_LINES);      // ������
					for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
					{
						glVertex3f(job_table2[1].x, job_table2[1].y, job_table2[1].z + 20 * i);
						glVertex3f(job_table2[3].x, job_table2[3].y, job_table2[1].z + 20 * i);
					}
					glEnd();
				}

				glPopMatrix();
			}
			glPopMatrix();

			glPushMatrix();
			{   //�÷��̾ �����̴� ť��
				glColor3f(1, 1, 0);
				glTranslatef(picked_cube.x, picked_cube.y, picked_cube.z);
				glutSolidCube(CUBE_SIZE);
			}
			glPopMatrix();


			// �ҷ��� ��
			//glEnable(GL_BLEND);
			//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
			//glColor4f(0.7f, 0.5f, 0.0f, 0.5f);
			//for (int i = 0; i < JOB_TABLE_SIZE; ++i)
			//{
			//   for (int j = 0; j < MAX_HEIGHT; ++j)
			//   {
			//      for (int k = 0; k < JOB_TABLE_SIZE; ++k)
			//      {
			//         if (quest_cube[i][j][k].put)
			//         {
			//            glPushMatrix();
			//            {
			//               glTranslatef(quest_cube[i][j][k].x, quest_cube[i][j][k].y + CUBE_SIZE / 2, quest_cube[i][j][k].z);
			//               glutSolidCube(CUBE_SIZE);
			//            }
			//            glPopMatrix();
			//         }
			//      }
			//   }
			//}



			//�÷��̾ ���� ť��

			glColor3f(0.7f, 0.5f, 0.0f);
			for (int i = 0; i < JOB_TABLE_SIZE; ++i)
			{
				for (int j = 0; j < MAX_HEIGHT; ++j)
				{
					for (int k = 0; k < JOB_TABLE_SIZE; ++k)
					{
						if (build_cube[i][j][k].put)
						{
							glPushMatrix();
							{
								glTranslatef(build_cube[i][j][k].x, build_cube[i][j][k].y + CUBE_SIZE / 2, build_cube[i][j][k].z);
								glutSolidCube(CUBE_SIZE);
							}
							glPopMatrix();
						}
					}
				}
			}


		}
		glPopMatrix();

		glPushMatrix();
		{
			if (SAVE_ON && timer < 10)
			{
				char *string = "SAVE";
				glRasterPos2f(590, 0);  // ���� ����� ��ġ ���� 
				int len = (int)strlen(string);
				for (int i = 0; i < len; i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
			}
			else
			{
				SAVE_ON = false;
				timer = 0;
			}
		}
		glPopMatrix();
	}

	glPopMatrix();

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);  // ���� ����
	glFrontFace(GL_CCW);   // �ո� ���� 

	gluPerspective(60.0, w / h, 1.0, 1000.0);
	gluLookAt(0, 0, 0, 0, -0.2, -1, 0.0, 1.0, 0.0);
	glTranslatef(MoveX, MoveY, MoveZ);
	glRotatef(DegreeX, 1, 0, 0);
	glRotatef(DegreeY, 0, 1, 0);
	glRotatef(DegreeZ, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);

	Width = w;
	Height = h;
}

void Mouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		mouse.x = x;

		LEFT_BUTTON = true;
	}
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
	{
		mouse.y = y;
		RIGHT_BUTTON = true;
	}

	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
	{
		LEFT_BUTTON = false;
	}
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
	{
		RIGHT_BUTTON = false;
	}

	glutPostRedisplay();

}

void Motion(int x, int y) {

	if (LEFT_BUTTON)
	{
		DegreeY += (x - mouse.x) * 0.005;
		Reshape(Width, Height);
	}
	if (RIGHT_BUTTON)
	{
		DegreeX += (y - mouse.y) * 0.005;
		Reshape(Width, Height);
	}
}