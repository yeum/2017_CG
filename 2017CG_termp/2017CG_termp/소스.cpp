#include <GL/glut.h>
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

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void TimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void SpecialFunc(int key, int x, int y);
void SetupRC();

float DegreeX, DegreeY, DegreeZ;
float MoveZ;
XYZPOS table_top[4], table_bottom[4], job_table[4];
BUILD build_cube[10][5][5];
XYZPOS picked_cube, build_idx;
int picked_idx;

void main(int argc, char *argv[])
{
	//�ʱ�ȭ �Լ��� 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ���� 
	glutInitWindowPosition(100, 100); // �������� ��ġ���� 
	glutInitWindowSize(800, 600); // �������� ũ�� ���� 
	glutCreateWindow("Example"); // ������ ���� (������ �̸�) 
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ��� ���� 
	glutSpecialFunc(SpecialFunc);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸� �Լ� ����
	SetupRC();	// ���� ���� �ʱ�ȭ �Լ�
	glutMainLoop();
}

void SetupRC() {
	// �ʿ��� ������, ��ǥ�� ���� �ʱ�ȭ
	// ��� ���� �ʱ�ȭ
	DegreeX = 0;
	DegreeY = 0;
	DegreeZ = 0;
	MoveZ = 0;

	picked_idx = 0;
	build_idx = { 2,0,4 };

	for (int i = 0; i < MAX_HEIGHT; ++i)
	{// y
		for (int j = 0; j < JOB_TABLE_SIZE; ++j)
		{// z
			for (int k = 0; k < JOB_TABLE_SIZE; ++k)
			{// x
				build_cube[i][j][k].x = (k - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
				build_cube[i][j][k].z = (j - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
				build_cube[i][j][k].y = i*CUBE_SIZE;
				build_cube[i][j][k].put = false;
			}
		}
	}

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

	picked_cube.x = 0;
	picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
	picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
}

void SpecialFunc(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		if (picked_cube.z > job_table[0].z + CUBE_SIZE)
		{
			picked_cube.z -= CUBE_SIZE;
			build_idx.z--;
		}
	}
	else if (key == GLUT_KEY_DOWN)
	{
		if (picked_cube.z < job_table[3].z - CUBE_SIZE)
		{
			picked_cube.z += CUBE_SIZE;
			build_idx.z++;
		}
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		if (picked_cube.x < job_table[0].x - CUBE_SIZE)
		{
			picked_cube.x += CUBE_SIZE;
			build_idx.x++;
		}
	}
	else if (key == GLUT_KEY_LEFT)
	{
		if (picked_cube.x > job_table[1].x + CUBE_SIZE)
		{
			picked_cube.x -= CUBE_SIZE;
			build_idx.x--;
		}
	}

	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'i' || key == 'I')
		SetupRC();
	else if (key == 'x' || key == 'X')
		DegreeX -= 1;
	else if (key == 'y' || key == 'Y')
		DegreeY -= 1;
	else if (key == 'z' || key == 'Z')
		DegreeZ -= 1;
	else if (key == 'w' || key == 'W')
	{
		if (picked_cube.y < (MAX_HEIGHT-1) * CUBE_SIZE)
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
		MoveZ -= 10;
	else if (key == '=')
		MoveZ += 10;
	else if (key == 32)
	{// �����̽��� �Է� => �� ����
		if (build_cube[build_idx.x][build_idx.y][build_idx.z].put == false)
		{
			picked_cube.x = 0;
			picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
			picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
			build_cube[build_idx.x][build_idx.y][build_idx.z].put = true;
			build_idx = { 2,0,4 };
		}
	}

	glutPostRedisplay();
}

void TimerFunction(int value)
{

	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}

// ������ ��� �Լ�
GLvoid drawScene(GLvoid)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	//glLoadIdentity();
	glPushMatrix();
	{
		glRotatef(DegreeX, 1, 0, 0);
		glRotatef(DegreeY, 0, 1, 0);
		glRotatef(DegreeZ, 0, 0, 1);
		glTranslatef(0, 0, MoveZ);

		glPushMatrix();
		{	///////���̺� �׸���///////
			glColor3f(0.5f, 0.3f, 0.0f);
			glTranslatef(0, -50, 0);
			glPushMatrix();
			{
				glBegin(GL_QUADS);	// ����
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glBegin(GL_QUADS);	// ����
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glBegin(GL_QUADS);
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glBegin(GL_QUADS);
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glBegin(GL_QUADS);
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glBegin(GL_QUADS);	// �Ʒ���
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{	//////�����̺�//////
				// �� ĭ�� 20 //
				glColor3f(1, 1, 1);
				glBegin(GL_LINES);		// ������
				for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
				{
					glVertex3f(job_table[0].x - 20 * i, job_table[0].y, job_table[0].z);
					glVertex3f(job_table[0].x - 20 * i, job_table[3].y, job_table[3].z);
				}
				glEnd();

				glBegin(GL_LINES);		// ������
				for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
				{
					glVertex3f(job_table[1].x, job_table[1].y, job_table[1].z + 20 * i);
					glVertex3f(job_table[3].x, job_table[3].y, job_table[1].z + 20 * i);
				}
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{	//�÷��̾ �����̴� ť��
				glColor3f(1, 1, 0);
				glTranslatef(picked_cube.x, picked_cube.y, picked_cube.z);
				glutSolidCube(CUBE_SIZE);
			}
			glPopMatrix();

			
			//�÷��̾ ���� ť��
			glColor3f(0.7f, 0.5f, 0.0f);
			for (int i = 0; i < MAX_HEIGHT; ++i)
			{
				for (int j = 0; j < JOB_TABLE_SIZE; ++j)
				{
					for (int k = 0; k < JOB_TABLE_SIZE; ++k)
					{
						if (build_cube[i][j][k].put)
						{
							glPushMatrix();
							{
								glTranslatef(build_cube[i][j][k].x, build_cube[i][j][k].y, build_cube[i][j][k].z);
								glutSolidCube(CUBE_SIZE);
							}
							glPopMatrix();
						}
					}
				}
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
	glEnable(GL_CULL_FACE);  // ���δ� �߶󳽴�  
							 //��������
	gluPerspective(60.0, w / h, 1.0, 1000.0);
	glTranslatef(0.0, 0.0, -300.0);

	//��������
	//	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -300.0, 300.0);

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0, 0, 0,	 0, -0.5, -1.0,	 0.0, 1.0, 0.0);
}