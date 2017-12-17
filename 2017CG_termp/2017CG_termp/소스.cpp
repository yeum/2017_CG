#include <GL/glut.h>
#include <fstream>
#include <cstdlib>
#include <stdio.h>   // sprintf
#include <windows.h> // 비트맵 파일 관련 자료 저장
#include <mmsystem.h>

using namespace std;

#define MAX_CUBE 30
#define CUBE_SIZE 20
#define JOB_TABLE_SIZE 5
#define MAX_HEIGHT 10
#define DESK_CUT_W 100
#define DESK_CUT_H 50

struct XYZPOS {
	int x, y, z;
};

struct BUILD {
	int x, y, z;
	bool put;
};

struct BUILDforQUEST {
	int x, y, z;
	bool put, correct;
};

enum GameMode { Main, Play, Tool };

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void TimerFunction(int value);
void PlayTimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void SpecialFunc(int key, int x, int y);
void SetupRC();
void SetupRC_byMode();
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info);

float Width, Height;
float DegreeX, DegreeY, DegreeZ;
float MoveX, MoveY, MoveZ;
XYZPOS table_top[4], table_bottom[4], job_table[4];
XYZPOS table2_top[4], table2_bottom[4], job_table2[4];
XYZPOS main_img[4];
BUILD build_cube[JOB_TABLE_SIZE][MAX_HEIGHT][JOB_TABLE_SIZE];
BUILDforQUEST quest_cube[JOB_TABLE_SIZE][MAX_HEIGHT][JOB_TABLE_SIZE];
XYZPOS picked_cube, build_idx;
int picked_idx;
XYZPOS mouse;
int Mode, PreMode;
float timer;
bool unclear, clear_sign;

bool LEFT_BUTTON, RIGHT_BUTTON;
bool SAVE_ON;

GLubyte* bitmap;
BITMAPINFO* bitmapInfo; // 비트맵 헤더 저장할 변수 
GLuint texture[2];

int hour, min, sec;

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

	PlaySound(L"Gemini.wav", NULL, SND_ASYNC | SND_LOOP);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정 
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정 
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정 
	glutCreateWindow("Stacker"); // 윈도우 생성 (윈도우 이름) 
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정 
	glutSpecialFunc(SpecialFunc);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 설정
	glutTimerFunc(1000, PlayTimerFunction, 2); // 타이머 함수 설정
	SetupRC();   // 상태 변수 초기화 함수
	SetupRC_byMode();   // 상태 변수 초기화 함수
	glutMainLoop();
}

void SetupRC() {
	// 필요한 변수들, 좌표값 등의 초기화
	// 기능 설정 초기화

	// 플레이 테이블
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

	// 제작 테이블
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

	main_img[0] = { 250,250,0 };
	main_img[1] = { -250,250,0 };
	main_img[2] = { -250,-250,0 };
	main_img[3] = { 250,-250,0 };

	glGenTextures(2, texture);
	for (int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		if (i == 0)
			bitmap = LoadDIBitmap("wood.bmp", &bitmapInfo);
		else if (i == 1)
			bitmap = LoadDIBitmap("main.bmp", &bitmapInfo);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void SetupRC_byMode()
{
	if (Mode == GameMode::Tool)
	{
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

					quest_cube[i][j][k].x = (i - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
					quest_cube[i][j][k].z = (k - JOB_TABLE_SIZE / 2) * CUBE_SIZE;
					quest_cube[i][j][k].y = j*CUBE_SIZE;
					quest_cube[i][j][k].put = false;
					quest_cube[i][j][k].correct = false;
				}
			}
		}

		picked_cube.x = 0;
		picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
		picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
	}
}

GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	// 바이너리 읽기 모드로 파일을 연다
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	// 비트맵 파일 헤더를 읽는다.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
		fclose(fp);
		return NULL;
	}

	// 파일이 BMP 파일인지 확인한다.
	if (header.bfType != 'MB')
	{
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER 위치로 간다.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// 비트맵 이미지 데이터를 넣을 메모리 할당을 한다.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
	{
		fclose(fp);
		exit(0);
		return NULL;
	}

	// 비트맵 인포 헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵의 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth
			* (*info)->bmiHeader.biBitCount + 7) / 8.0
		* abs((*info)->bmiHeader.biHeight);

	// 비트맵의 크기만큼 메모리를 할당한다.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}
	// 비트맵 데이터를 bit(GLubyte 타입)에 저장한다. 
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize)
	{
		free(*info);
		free(bits);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return bits;
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
	if ((key == 'w' || key == 'W' )&& Mode != GameMode::Main)
	{
		if (picked_cube.y < (MAX_HEIGHT - 1) * CUBE_SIZE)
		{
			picked_cube.y += CUBE_SIZE;
			build_idx.y++;
		}
	}
	else if ((key == 's' || key == 'S') && Mode != GameMode::Main)
	{
		if (picked_cube.y > job_table[0].y + CUBE_SIZE)
		{
			picked_cube.y -= CUBE_SIZE;
			build_idx.y--;
		}
	}
	else if (key == 32 && clear_sign == false)
	{// 스페이스바 입력 => 블럭 놓기
		if (build_cube[build_idx.x][build_idx.y][build_idx.z].put == false)
		{
			if (Mode == GameMode::Play)
			{
				picked_cube.x = 0;
				picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
				picked_cube.z = job_table[3].z - CUBE_SIZE / 2;

				if (quest_cube[build_idx.x][build_idx.y][build_idx.z].put)
					quest_cube[build_idx.x][build_idx.y][build_idx.z].correct = true;
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
	else if (key == 8 && clear_sign == false)
	{// 블럭 놓인 위치에서 백스페이스바 > 블럭 지우기
		if (build_cube[build_idx.x][build_idx.y][build_idx.z].put == true)
		{
			if (Mode == GameMode::Play)
			{
				picked_cube.x = 0;
				picked_cube.y = job_table[0].y + CUBE_SIZE / 2;
				picked_cube.z = job_table[3].z - CUBE_SIZE / 2;
				if (quest_cube[build_idx.x][build_idx.y][build_idx.z].put)
					quest_cube[build_idx.x][build_idx.y][build_idx.z].correct = false;

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
	{// 블럭 다 쌓고 엔터 > 저장
		ofstream WriteFile;
		char buf[100]{ 0 };      // 파일명을 저장할 버퍼
		int file_num = 0;      // 몇번째 파일인지 기록

		for (file_num = 0; file_num < 100; ++file_num) {      // 0에서 최대 100개의 파일을 만들수 있음.
			sprintf_s(buf, "build_cube_%d.txt", file_num);      // 버퍼에 file_num번째 파일명을 넣음
			ifstream read;
			read.open(buf);
			if (!read) {   // read로 만약 파일이 존재하지 않을경우 break
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
		unclear = false;
		if (clear_sign)
		{
			clear_sign = false;
			glutTimerFunc(1000, PlayTimerFunction, 2);
		}
		hour = min = sec = 0;
		DegreeX = 0;
		DegreeY = 0;
		DegreeZ = 0;
		PreMode = Mode;
		Mode = GameMode::Play;
		SetupRC_byMode();

		char buf[100]{ 0 };      // 파일명을 저장할 버퍼
		int file_num = 0;      // 몇번째 파일인지 기록

		for (file_num = 0; file_num < 100; ++file_num) {      // 0에서 최대 100개의 파일을 만들수 있음.
			sprintf_s(buf, "build_cube_%d.txt", file_num);      // 버퍼에 file_num번째 파일명을 넣음
			ifstream read;
			read.open(buf);
			if (!read) {   // read로 만약 파일이 존재하지 않을경우 break
				break;
			}
			read.close();
		}
		sprintf_s(buf, "build_cube_%d.txt", rand() % file_num);   // file_num 전까지는 파일이 있으므로 그 사이에서 rand를 한다.

		FILE *fp;
		fopen_s(&fp, buf, "r");
		int a, b, c;
		while (!feof(fp)) {
			a = -1, b = -1, c = -1;
			fscanf_s(fp, "%d,%d,%d", &a, &b, &c);
			printf("%d %d %d\n", a, b, c);
			if (a == -1 || b == -1 || c == -1) {

			}
			else {
				quest_cube[a][b][c].put = true;
			}
		}
		
	}
	else if (key == '2')
	{
		if (Mode != GameMode::Tool)
		{
			clear_sign = false;
			
			DegreeX = 0;
			DegreeY = 0;
			DegreeZ = 0;
			PreMode = Mode;
			Mode = GameMode::Tool;
			SetupRC_byMode();
		}
	}
	else if (key == 27)
	{// 종료
		exit(0);
	}

	if (Mode == GameMode::Play)
	{
		unclear = false;
		clear_sign = false;
		for (int i = 0; i < JOB_TABLE_SIZE; ++i)
		{// x
			for (int j = 0; j < MAX_HEIGHT; ++j)
			{// y
				for (int k = 0; k < JOB_TABLE_SIZE; ++k)
				{// z
					if ((quest_cube[i][j][k].put && quest_cube[i][j][k].correct == false) || (build_cube[i][j][k].put && quest_cube[i][j][k].put == false))
					{
						unclear = true;
						break;
					}
				}
				if (unclear)
					break;
			}
			if (unclear)
				break;
		}

		if (unclear == false)
			clear_sign = true;
	}
	glutPostRedisplay();
}

void TimerFunction(int value)
{
	if (Mode == GameMode::Play)
	{
		MoveX += 40;
		MoveZ += 20;

		if (MoveX >= 0)
			MoveX = 0;
		if (MoveZ >= -300)
			MoveZ = -300;
	}
	else if (Mode == GameMode::Tool)
	{
		MoveX -= 40;
		MoveZ += 20;
		
		if (MoveX <= -600)
			MoveX = -600;
		if (MoveZ >= -300)
			MoveZ = -300;
	}
	if (SAVE_ON)
		timer++;
	Reshape(Width, Height);
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}

void PlayTimerFunction(int value)
{
	sec++;
	if (sec >= 60)
	{
		min++;
		sec = 0;
	}
	if (min >= 60)
	{
		hour++;
		min = 0;
	}
	if(clear_sign==false)
		glutTimerFunc(1000, PlayTimerFunction, 2);
	glutPostRedisplay();
}

// 윈도우 출력 함수
GLvoid drawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat direction[] = { 0, -1, 0 };
	GLfloat lightPos[] = { 0, 100, 0, 1.0 };
	GLfloat lightPos2[] = { 600, 100, 0, 1.0 };
	GLfloat lightPos3[] = { 300, 200, 500, 1.0 };

	glEnable(GL_LIGHTING);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0f);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1.0f);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos3);
	glEnable(GL_LIGHT1);

	if (Mode == GameMode::Play)
	{
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}
	else if (Mode == GameMode::Tool)
	{
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos2);
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}
	else
	{
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
	}

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//glLoadIdentity();
	glPushMatrix();
	{

		glPushMatrix();
		{   /////// 테이블 그리기///////
			glTranslatef(0, -50, 0);
			glPushMatrix();
			{ ///// 플레이 테이블 /////
				glColor3f(1, 1, 1);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture[0]);
				glBegin(GL_QUADS);   // 윗면
				for (int i = 0; i < DESK_CUT_H; ++i)
				{
					for (int j = 0; j < DESK_CUT_W; ++j)
					{
						glNormal3f(0.0, 1.0, 0.0);
						glTexCoord2f((float)1/ DESK_CUT_W*(j+1), 1-((float)1/ DESK_CUT_H*i));  // 텍스처 위치 설정
						glVertex3f(table_top[1].x+(table_top[0].x-table_top[1].x)/ DESK_CUT_W *(j + 1), table_top[0].y, table_top[0].z + (table_top[3].z - table_top[0].z)/ DESK_CUT_H*i);
						glTexCoord2f((float)1 / DESK_CUT_W*j, 1-((float)1 / DESK_CUT_H*i));  // 텍스처 위치 설정
						glVertex3f(table_top[1].x+(table_top[0].x-table_top[1].x)/ DESK_CUT_W *j, table_top[1].y, table_top[0].z + (table_top[3].z - table_top[0].z)/ DESK_CUT_H*i);
						glTexCoord2f((float)1 / DESK_CUT_W*j, 1-((float)1 / DESK_CUT_H*(i+1)));  // 텍스처 위치 설정
						glVertex3f(table_top[1].x+(table_top[0].x-table_top[1].x)/ DESK_CUT_W *j, table_top[2].y, table_top[0].z + (table_top[3].z - table_top[0].z)/ DESK_CUT_H*(i + 1));
						glTexCoord2f((float)1 / DESK_CUT_W*(j + 1), 1-((float)1 / DESK_CUT_H*(i+1)));  // 텍스처 위치 설정
						glVertex3f(table_top[1].x+(table_top[0].x-table_top[1].x)/ DESK_CUT_W *(j + 1), table_top[3].y, table_top[0].z + (table_top[3].z - table_top[0].z) / DESK_CUT_H*(i + 1));
					}
				}
				glEnd();

				glColor3f(0.7f, 0.5f, 0.4f);
				glBegin(GL_QUADS);// 아랫면
				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glEnd();

				glBegin(GL_QUADS);
				// 옆면
				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);

				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[1].x, table_top[1].y, table_top[1].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[1].x, table_bottom[1].y, table_bottom[1].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);

				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[0].x, table_top[0].y, table_top[0].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[0].x, table_bottom[0].y, table_bottom[0].z);

				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[3].x, table_top[3].y, table_top[3].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table_top[2].x, table_top[2].y, table_top[2].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[2].x, table_bottom[2].y, table_bottom[2].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table_bottom[3].x, table_bottom[3].y, table_bottom[3].z);
				glEnd();
				glDisable(GL_TEXTURE_2D);

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
			}
			glPopMatrix();

			glPushMatrix();
			{   ///////제작 테이블 그리기///////
				glColor3f(1, 1, 1);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture[0]);
				glBegin(GL_QUADS);   // 윗면
				for (int i = 0; i < DESK_CUT_H; ++i)
				{
					for (int j = 0; j < DESK_CUT_W; ++j)
					{
						glNormal3f(0.0, 1.0, 0.0);
						glTexCoord2f((float)1 / DESK_CUT_W*(j + 1), 1 - ((float)1 / DESK_CUT_H*i));  // 텍스처 위치 설정
						glVertex3f(table2_top[1].x + (table2_top[0].x - table2_top[1].x) / DESK_CUT_W *(j + 1), table2_top[0].y, table2_top[0].z + (table2_top[3].z - table2_top[0].z) / DESK_CUT_H*i);
						glTexCoord2f((float)1 / DESK_CUT_W*j, 1 - ((float)1 / DESK_CUT_H*i));  // 텍스처 위치 설정
						glVertex3f(table2_top[1].x + (table2_top[0].x - table2_top[1].x) / DESK_CUT_W *j, table2_top[1].y, table2_top[0].z + (table2_top[3].z - table2_top[0].z) / DESK_CUT_H*i);
						glTexCoord2f((float)1 / DESK_CUT_W*j, 1 - ((float)1 / DESK_CUT_H*(i + 1)));  // 텍스처 위치 설정
						glVertex3f(table2_top[1].x + (table2_top[0].x - table2_top[1].x) / DESK_CUT_W *j, table2_top[2].y, table2_top[0].z + (table2_top[3].z - table2_top[0].z) / DESK_CUT_H*(i + 1));
						glTexCoord2f((float)1 / DESK_CUT_W*(j + 1), 1 - ((float)1 / DESK_CUT_H*(i + 1)));  // 텍스처 위치 설정
						glVertex3f(table2_top[1].x + (table2_top[0].x - table2_top[1].x) / DESK_CUT_W *(j + 1), table2_top[3].y, table2_top[0].z + (table2_top[3].z - table2_top[0].z) / DESK_CUT_H*(i + 1));
					}
				}
				glEnd();

				glColor3f(0.7f, 0.5f, 0.4f);
				glBegin(GL_QUADS);// 아랫면
				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[1].x, table2_bottom[1].y, table2_bottom[1].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[0].x, table2_bottom[0].y, table2_bottom[0].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[3].x, table2_bottom[3].y, table2_bottom[3].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[2].x, table2_bottom[2].y, table2_bottom[2].z);
				glEnd();

				glBegin(GL_QUADS);
				// 옆면
				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[1].x, table2_top[1].y, table2_top[1].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[0].x, table2_top[0].y, table2_top[0].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[0].x, table2_bottom[0].y, table2_bottom[0].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[1].x, table2_bottom[1].y, table2_bottom[1].z);

				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[2].x, table2_top[2].y, table2_top[2].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[1].x, table2_top[1].y, table2_top[1].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[1].x, table2_bottom[1].y, table2_bottom[1].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[2].x, table2_bottom[2].y, table2_bottom[2].z);

				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[0].x, table2_top[0].y, table2_top[0].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[3].x, table2_top[3].y, table2_top[3].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[3].x, table2_bottom[3].y, table2_bottom[3].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[0].x, table2_bottom[0].y, table2_bottom[0].z);

				glTexCoord2f(1, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[3].x, table2_top[3].y, table2_top[3].z);
				glTexCoord2f(0, 1);  // 텍스처 위치 설정
				glVertex3f(table2_top[2].x, table2_top[2].y, table2_top[2].z);
				glTexCoord2f(0, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[2].x, table2_bottom[2].y, table2_bottom[2].z);
				glTexCoord2f(1, 0);  // 텍스처 위치 설정
				glVertex3f(table2_bottom[3].x, table2_bottom[3].y, table2_bottom[3].z);
				glEnd();
				glDisable(GL_TEXTURE_2D);

				glPushMatrix();
				{   //////잡테이블//////
					// 한 칸에 20 //
					glColor3f(1, 1, 1);
					glBegin(GL_LINES);      // 세로줄
					for (int i = 0; i <= JOB_TABLE_SIZE; ++i)
					{
						glVertex3f(job_table2[0].x - 20 * i, job_table2[0].y, job_table2[0].z);
						glVertex3f(job_table2[0].x - 20 * i, job_table2[3].y, job_table2[3].z);
					}
					glEnd();

					glBegin(GL_LINES);      // 가로줄
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
			{   //플레이어가 움직이는 큐브
				glColor3f(1, 1, 0);
				glTranslatef(picked_cube.x, picked_cube.y, picked_cube.z);
				glutSolidCube(CUBE_SIZE);
			}
			glPopMatrix();


			// 불러온 블럭
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.5f, 0.3f, 0.0f, 0.5f);
			for (int i = 0; i < JOB_TABLE_SIZE; ++i)
			{
				for (int j = 0; j < MAX_HEIGHT; ++j)
				{
					for (int k = 0; k < JOB_TABLE_SIZE; ++k)
					{
						if (quest_cube[i][j][k].put && quest_cube[i][j][k].correct == false)
						{
							glPushMatrix();
							{
								glTranslatef(quest_cube[i][j][k].x, quest_cube[i][j][k].y + CUBE_SIZE / 2, quest_cube[i][j][k].z);
								glutSolidCube(CUBE_SIZE);
							}
							glPopMatrix();
						}
					}
				}
			}
			glDisable(GL_BLEND);

			//플레이어가 쌓은 큐브

			glColor4f(0.7f, 0.5f, 0.0f, 1.f);
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
				glRasterPos2f(590, 0);  // 문자 출력할 위치 설정 
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

		glPushMatrix();
		{
			if (clear_sign)
			{
				char *string = "CLEAR!!";
				char *string2 = "Please press '1' again";
				glRasterPos2f(-15, 80);  // 문자 출력할 위치 설정 
				int len = (int)strlen(string);
				for (int i = 0; i < len; i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
				glRasterPos2f(-35, 60);  // 문자 출력할 위치 설정 
				int len2 = (int)strlen(string2);
				for (int i = 0; i < len2; i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string2[i]);
			}
		}
		glPopMatrix();

		glPushMatrix();
		{
			glColor3f(1, 1, 1);
			if (Mode == GameMode::Play)
			{
				char string[8] = {0,};
				char b_h[3] = { 0 };
				char b_m[3] = { 0 };
				char b_s[3] = { 0 };
				itoa(hour, b_h, 10);
				if (hour < 10)
				{
					b_h[1] = b_h[0];
					b_h[0] = '0';
				}
				itoa(min, b_m, 10);
				if (min < 10)
				{
					b_m[1] = b_m[0];
					b_m[0] = '0';
				}
				itoa(sec, b_s, 10);
				if (sec < 10)
				{
					b_s[1] = b_s[0];
					b_s[0] = '0';
				}

				glRasterPos2f(-15, table_top[0].y + 100);  // 문자 출력할 위치 설정 
				for (int i = 0; i < 2; i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, b_h[i]);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (char)':');
				glRasterPos2f(-5, table_top[0].y + 100);  // 문자 출력할 위치 설정 
				for (int i = 0; i < 2; i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, b_m[i]);
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (char)':');
				glRasterPos2f(5, table_top[0].y + 100);  // 문자 출력할 위치 설정 
				for (int i = 0; i < 2; i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, b_s[i]);
			}
		}
		glPopMatrix();

		glPushMatrix();
		{
			if (Mode == GameMode::Main)
			{
				glEnable(GL_TEXTURE_2D);
				glTranslatef(300, -40, 180);
				glRotatef(-10, 1, 0, 0);
				glBindTexture(GL_TEXTURE_2D, texture[1]);
				glBegin(GL_QUADS);
				{
					glTexCoord2f(1, 1);  // 텍스처 위치 설정
					glVertex3f(main_img[0].x, main_img[0].y, main_img[0].z);
					glTexCoord2f(0, 1);  // 텍스처 위치 설정
					glVertex3f(main_img[1].x, main_img[1].y, main_img[1].z);
					glTexCoord2f(0, 0);  // 텍스처 위치 설정
					glVertex3f(main_img[2].x, main_img[2].y, main_img[2].z);
					glTexCoord2f(1, 0);  // 텍스처 위치 설정
					glVertex3f(main_img[3].x, main_img[3].y, main_img[3].z);
				}
				glEnd();
				glDisable(GL_TEXTURE_2D);
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

	gluPerspective(60.0, w / h, 1.0, 1000.0);
	gluLookAt(0, 0, 0, 0, -0.2, -1, 0.0, 1.0, 0.0);
	glTranslatef(MoveX, MoveY, MoveZ);
	if (Mode == GameMode::Tool)
		glTranslatef(600, 0, 0);
	glRotatef(DegreeX, 1, 0, 0);
	glRotatef(DegreeY, 0, 1, 0);
	glRotatef(DegreeZ, 0, 0, 1);
	if (Mode == GameMode::Tool)
		glTranslatef(-600, 0, 0);
	glMatrixMode(GL_MODELVIEW);

	Width = w;
	Height = h;
}

void Mouse(int button, int state, int x, int y)
{
	if (Mode != GameMode::Main)
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
	}
	glutPostRedisplay();

}

void Motion(int x, int y) {

	if (LEFT_BUTTON)
	{
		DegreeY += (x - mouse.x) /5;
		mouse.x = x;
		Reshape(Width, Height);
	}
	if (RIGHT_BUTTON)
	{
		DegreeX += (y - mouse.y) /5;
		mouse.y = y;
		Reshape(Width, Height);
	}
}