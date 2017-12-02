#include <GL/glut.h>
#include <fstream>

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

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void TimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void SpecialFunc(int key, int x, int y);
void SetupRC();
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);


float DegreeX, DegreeY, DegreeZ;
float MoveZ;
XYZPOS table_top[4], table_bottom[4], job_table[4];
BUILD build_cube[JOB_TABLE_SIZE][MAX_HEIGHT][JOB_TABLE_SIZE];
XYZPOS picked_cube, build_idx;
int picked_idx;
XYZPOS mouse;

bool LEFT_BUTTON, RIGHT_BUTTON;

void main(int argc, char *argv[])
{
	//초기화 함수들 
	//for (int i = 0; i < JOB_TABLE_SIZE; ++i)
	//{
	//   for (int j = 0; j < MAX_HEIGHT; ++j)
	//   {
	//      for (int k = 0; k < JOB_TABLE_SIZE; ++k)
	//      {
	//         build_cube[i][j][k].put = true;
	//         build_cube[i][j][k].x = 
	//         build_cube[i][j][k].y =
	//         build_cube[i][j][k].z =
	//      }
	//   }
	//}

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정 
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정 
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정 
	glutCreateWindow("Example"); // 윈도우 생성 (윈도우 이름) 
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정 
	glutSpecialFunc(SpecialFunc);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 설정
	SetupRC();   // 상태 변수 초기화 함수
	glutMainLoop();
}

void SetupRC() {
	// 필요한 변수들, 좌표값 등의 초기화
	// 기능 설정 초기화
	DegreeX = 0;
	DegreeY = 0;
	DegreeZ = 0;
	MoveZ = 0;

	picked_idx = 0;
	build_idx = { 2,0,4 };


	// 쌓인블록위치 저장배열 build_cube[x][y][z];
	// x는 왼쪽에서 오른쪽의 가로방향
	// z는 안쪽에서 바깥쪽으로의 세로방향
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
		MoveZ -= 10;
	else if (key == '=')
		MoveZ += 10;
	else if (key == 32)
	{// 스페이스바 입력 => 블럭 놓기
		if (build_cube[build_idx.x][build_idx.y][build_idx.z].put == false)
		{
			picked_cube.x = 0;
			picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
			picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
			build_cube[build_idx.x][build_idx.y][build_idx.z].put = true;
			build_idx = { 2,0,4 };


		}

	}
	else if (key == 'l' || key == 'L') {
		FILE *fp;
		fopen_s(&fp, "build_cube.txt", "r");
		int a, b, c;
		while (!feof(fp)) {
			a = -1, b = -1, c = -1;
			fscanf_s(fp, "%d,%d,%d", &a, &b, &c);
			printf("%d %d %d\n", a, b, c);
			if (a == -1 || b == -1 || c == -1) {

			}
			else {
				build_cube[a][b][c].put = true;
			}
		}

	}
	else if (key == 8)
	{// 블럭 놓인 위치에서 백스페이스바 > 블럭 지우기
		if (build_cube[build_idx.x][build_idx.y][build_idx.z].put == true)
		{
			picked_cube.x = 0;
			picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
			picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
			build_cube[build_idx.x][build_idx.y][build_idx.z].put = false;
			build_idx = { 2,0,4 };


		}
	}
	else if (key == 13)
	{// 블럭 다 쌓고 엔터 > 저장
		ofstream WriteFile;
		WriteFile.open("build_cube.txt", ios_base::trunc);
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
						WriteFile.open("build_cube.txt", ios_base::app);
						WriteFile << i << ',' << j << ',' << k << endl;
						WriteFile.close();
					}
				}
			}
		}
	}
	glutPostRedisplay();
}

void TimerFunction(int value)
{

	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}

// 윈도우 출력 함수
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
		{   ///////테이블 그리기///////
			glColor3f(0.5f, 0.3f, 0.0f);
			glTranslatef(0, -50, 0);
			glPushMatrix();
			{
				glBegin(GL_QUADS);   // 윗면
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glBegin(GL_QUADS);   // 옆면
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
				glBegin(GL_QUADS);   // 아랫면
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{   //////잡테이블//////
				// 한 칸에 20 //
				glColor3f(1, 1, 1);
				glBegin(GL_LINES);      // 세로줄
				for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
				{
					glVertex3f(job_table[0].x - 20 * i, job_table[0].y, job_table[0].z);
					glVertex3f(job_table[0].x - 20 * i, job_table[3].y, job_table[3].z);
				}
				glEnd();

				glBegin(GL_LINES);      // 가로줄
				for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
				{
					glVertex3f(job_table[1].x, job_table[1].y, job_table[1].z + 20 * i);
					glVertex3f(job_table[3].x, job_table[3].y, job_table[1].z + 20 * i);
				}
				glEnd();
			}
			glPopMatrix();

			glPushMatrix();
			{   //플레이어가 움직이는 큐브
				glColor3f(1, 1, 0);
				glTranslatef(picked_cube.x, picked_cube.y, picked_cube.z);
				glutSolidCube(CUBE_SIZE);
			}
			glPopMatrix();


			//플레이어가 쌓은 큐브

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

	}
	glPopMatrix();

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);  // 은면 제거
	glFrontFace(GL_CCW);   // 앞면 설정  
	glEnable(GL_CULL_FACE);  // 내부는 잘라낸다  
							 //원근투영
	gluPerspective(60.0, w / h, 1.0, 1000.0);
	glTranslatef(0.0, 0.0, -300.0);

	//직각투영
	//   glOrtho(-w / 2, w / 2, -h / 2, h / 2, -300.0, 300.0);

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0, 0, 0, 0, -0.5, -1.0, 0.0, 1.0, 0.0);
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
		DegreeY += (mouse.x - x) * 0.01;
	if (RIGHT_BUTTON)
		DegreeX += (mouse.y - y) * 0.01;

}