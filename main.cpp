#include<time.h>
#include<string.h>
#include <stdlib.h>
#include <iostream> 
#include <fstream>  
#include <sstream>
#include <vector>
using namespace std;

#define POS_Y 20
#define PROPORTION 0.1
#define TBEGIN	 100//开始计时的次数
#define TIMES   400//总共更新的次数
#define P 0.5//减速概率   //无人驾驶车辆的概率与普通车不同

float Vmax = 3.58;
void gotoxy(int x, int y);
void back();
void show(int, int);
void clear(int);
void delay(int);
void delay1(int);
vector<vector<int> > initcars(int, int);
vector<vector<int> > move(int, vector<vector<int> >,int);
float caculate(int, int, vector<vector<int> >);
int FindFront(int, int, vector<vector<int> >);
void updateSpeed(const int, int&, int);
void updatePosition(int&, int, int, int,int, vector<vector<int> >&);
void ClearVector(vector< vector<int> >&);
int str2num(string);

int main()
{
	float velocity;
	srand(time(NULL));

	int initial_cars[223];
	int lane[223];
	int i = 0, a = 0;
	string value;
	int counter = 0;
	ifstream infile("data.csv");
	while (infile.good()){
		getline(infile, value, '\t');
		counter++;
		getline(infile, value, '\t');
		counter++;
		getline(infile, value, '\t');
		counter++;
		cout << counter << "," << value << endl;
		initial_cars[a] = str2num(value);
		getline(infile, value, '\n');
		counter++;
		cout << counter << "," << value << endl;
		lane[a] = str2num(value);
		a++;
	}
	infile.close();
	int k = 0, j = 0;
	FILE *fp;
	fp = fopen("record.txt", "ab");
	for (i = 0; i < 223; i++)
	{
		vector<vector<int> > cars(TIMES, vector<int>(lane[i]));
		vector<vector<int> > aftercars(TIMES, vector<int>(lane[i]));

		cars = initcars(initial_cars[i], lane[i]);
		cout << "the " << i << "time to move" << endl;
		aftercars = move(lane[i], cars,initial_cars[i]);
		velocity = caculate(initial_cars[i], lane[i], aftercars);
		fprintf(fp, "Density：%3d   Velocity: %.2f ", initial_cars[i], velocity);
		char line[] = "\r\n";
		fwrite(line, strlen(line), 1, fp);
		ClearVector(cars);
		ClearVector(aftercars);
	}
	fclose(fp);

	return 0;
}

//initialize cars
vector<vector<int> > initcars(int n, int lane)
{
	int i, j, location, speed;
	vector<vector<int> > cars(TIMES, vector<int>(lane));

	for (i = 0; i<TIMES; i++)
		for (j = 0; j<lane; j++)
			cars[i][j] = -1;

	for (i = 0; i<n; i++)
	{
		location = rand() % lane;

		if (cars[0][location] == -1)
		{
			cars[0][location] = rand() % int((Vmax + 1));
		}
		else   //if car already exists,put it on another place
			i--;
	}
	return cars;
}

//simulate car's movement
vector<vector<int> > move(int lane, vector<vector<int> > cars,int numofCars)
{
	vector<vector<int> > cars1;
	cars1 = cars;
	int i = 0, j, x, v, gap;
	for (i = 0; i<TIMES - 1; i++)
	{
		for (j = 0; j<lane; j++)
		{
			if (cars1[i][j] >= 0)
			{
				v = cars1[i][j];
				gap = FindFront(i, j, cars1);		
				//update speed
				updateSpeed(gap, v, numofCars);

				//update position
				updatePosition(x,v,j,i,lane,cars1);

			}
		}
	}
	return cars1;
}

//calculate the gap with the front car
int FindFront(int i, int j, vector<vector<int> > cars)
{
	int front = 0;
	while (cars[i][(++j) % 1000] == -1)
	{
		front++;
	}
	return front;
}


//calculate average velocity
float caculate(int n, int lane, vector<vector<int> > cars)
{
	long s = 0, i, j;
	float v = 0;

	for (i = TBEGIN; i<TIMES; i++)
		for (j = 0; j<lane; j++)
		{
			if (cars[i][j] >= 0)
			{
				s += cars[i][j];
			}

		}
	v = (float)s / n / (TIMES - TBEGIN);
	return v;
}

void updateSpeed(const int gap, int&v,int numofCars)
{
	double decPosibility = 0.0;
	double accePosibility = 0.0;
	double keepPosibility = 0.0;

	//use the posibility fitted before
	accePosibility = 100*(0.2 - 0.001*v) / 2.35;
	decPosibility = 100*(log(v / 30 + 1) / log(2)) / 2.35;
	keepPosibility = 100*(1 - accePosibility - decPosibility);
	if ((rand() % numofCars) >= (numofCars*PROPORTION))  //update non-auto-driving cars
	{
		if ((v + 1) <= Vmax)  // increasing speed
			v++;
		if (v > gap)    //maintaning speed
			v = gap;
		if (v > 0 && (rand() % 100 <= P * 100)) //decreasing speed
			v--;
	}
	else //update auto-driving cars
	{
		if (((v + 1) <= Vmax) && (rand() % 100 <= accePosibility))  //increasing speed
			v++;
		if (v > gap && (rand() % 100 < keepPosibility))   //maintaning speed 
			v = gap;
		if ((v + 1) > Vmax && (rand()%100 < decPosibility))   //decreasing speed
			v--;
	}
}
void updatePosition(int &x, int v, int j, int i,int lane, vector<vector<int> >&cars1)
{
	x = v + j;
	x = x%lane;
	if (cars1[i + 1][x] == -1)
	{
		cars1[i + 1][x] = v;
	}
	else
	{
		printf("craaaaaaaaaashed! second : %d ，block : %d \n", i, j);	
	    exit(1);
	}
}
void ClearVector(vector< vector<int> >& vt)
{
	vector< vector<int>  > vtTemp;
	vtTemp.swap(vt);
}
int str2num(string s)
{
	int num;
	stringstream ss(s);
	ss >> num;
	return num;
}
