#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "vector.h"

#define _USE_MATH_DEFINES
#include <math.h>


struct point2d {
	float x;
	float y;
};

struct triangle {
	int ind1 = 0;
	int ind2 = 0;
	int ind3 = 0;
	vecf3d normal;

	olc::Pixel colour = olc::BLACK;

	void setIndices(int i1, int i2, int i3) {
		ind1 = i1;
		ind2 = i2;
		ind3 = i3;
	}

	void setColour(olc::Pixel _colour) {
		colour = _colour;
	}

	void setNormal(vecf3d _normal) {
		normal = _normal;
	}
};


class Example : public olc::PixelGameEngine
{
	const int MAP_SIZE = 128;

	point2d* points2d;
	vecf3d* vectors3d;

	triangle* triangles = new triangle[MAP_SIZE * MAP_SIZE * 2];

	float light_angle = 0.0f;
	float light_rot_speed = (float) M_PI / 8.0f;

	float projection_d = 100000.0f;


	vecf3d eyeVector = { 132.0f, -61.0f, -215.0f };
	float radAngle = 5.665f;

	vecf3d lightVector = { 3000.0f, 0.0f, (float) MAP_SIZE / 2 };

	int x_2d_origin = ScreenWidth() / 2;
	int y_2d_origin = ScreenHeight() / 2;
	float screen_scale = 0.01f;

public:
	Example()
	{
		sAppName = "The Wulf Scape";
	}

public:
	bool OnUserDestroy() override
	{
		delete[] points2d;
		delete[] vectors3d;

		return true;
	}


	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		x_2d_origin = ScreenWidth() / 2;
		y_2d_origin = ScreenHeight() / 2;
		points2d = new point2d[(int)MAP_SIZE * MAP_SIZE];

		float* land = CreateLandscape(MAP_SIZE);
		vectors3d = PopulateVertexArrayWithLandscape(land, MAP_SIZE);
		CreateTriangleArray(MAP_SIZE);

		delete[] land;
		return true;
	}


	vecf3d* PopulateVertexArrayWithLandscape(float* landHeightArray, int map_size) {
		vecf3d* vectorArray = new vecf3d[(int)map_size * map_size];

		for (int j = 0; j < map_size; j++)
		{
			for (int i = 0; i < map_size; i++)
			{
				int index = j * map_size + i;
				vectorArray[index] = { (float)i - map_size / 2, landHeightArray[index], (float)j };
			}
		}
		return vectorArray;
	}


	void CreateTriangleArray(int map_size) {
		triangle* triangle_ptr = &triangles[0];

		for (int j = 0; j < map_size - 1; j++) {
			for (int i = 0; i < map_size - 1; i++) {
				int point1 = j * map_size + i;
				int point2 = j * map_size + i + 1;
				int point3 = (j + 1) * map_size + i + 1;
				int point4 = (j + 1) * map_size + i;

				triangle_ptr->setIndices(point1, point2, point3);
				triangle_ptr->setColour(olc::GREEN);
				CreateTriangleNormal(triangle_ptr);
				triangle_ptr++;
				triangle_ptr->setIndices(point3, point4, point1);
				triangle_ptr->setColour(olc::DARK_GREEN);
				CreateTriangleNormal(triangle_ptr);
				triangle_ptr++;
			}
		}
	}

	void CreateTriangleNormal(triangle* triangle_ptr) {
		vecf3d vectorA = (vectors3d[triangle_ptr->ind1] - vectors3d[triangle_ptr->ind2]);// .norm();
		vecf3d vectorB = (vectors3d[triangle_ptr->ind3] - vectors3d[triangle_ptr->ind2]); // .norm();
		triangle_ptr->setNormal(vectorA.cross(vectorB).norm());
	}



	void CreateScreenCoords(point2d& point, float x, float y) {
		point.x = (int) x * screen_scale + x_2d_origin;
		point.y = (int) -y * screen_scale + y_2d_origin;
	}


	bool OnUserUpdate(float fElapsedTime) override
	{
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::DARK_BLUE);
		HandleInput(fElapsedTime);
		MoveLight(fElapsedTime);
		TransformLandscape();
		DrawTriangles();
		DisplayStats();
		return true;
	}

	void MoveLight(float fElapsedTime) {

		light_angle += light_rot_speed * fElapsedTime;
		if (light_angle > M_PI * 2) light_angle -= (float) M_PI * 2;

		float light_x = cos(light_angle) * 10000.0f;
		float light_y = sin(light_angle) * 10000.0f;
		lightVector.x = light_x;
		lightVector.y = light_y;
	}

	void HandleInput(float fElapsedTime) {

		float rotateSpeed = 0.8f;

		if (IsFocused())
		{
			if (GetKey(olc::Key::UP).bHeld)
				eyeVector += {0.0f, 0.0f, 15.0f * fElapsedTime};

			if (GetKey(olc::Key::DOWN).bHeld)
				eyeVector -= {0.0f, 0.0f, 15.0f * fElapsedTime};

			if (GetKey(olc::Key::LEFT).bHeld)
				eyeVector -= {15.0f * fElapsedTime, 0.0f, 0.0f};

			if (GetKey(olc::Key::RIGHT).bHeld)
				eyeVector += {15.0f * fElapsedTime, 0.0f, 0.0f};

			if (GetKey(olc::Key::Q).bHeld)
				eyeVector += {0.0f, 15.0f * fElapsedTime, 0.0f};

			if (GetKey(olc::Key::A).bHeld)
				eyeVector += {0.0f, -15.0f * fElapsedTime, 0.0f};

			if (GetKey(olc::Key::W).bHeld) {
				radAngle += (rotateSpeed * fElapsedTime);
				if (radAngle > (2 * 3.142f)) radAngle -= 2 * 3.142f;
			}
			if (GetKey(olc::Key::S).bHeld) {
				radAngle -= (rotateSpeed * fElapsedTime);
				if (radAngle < -(2 * 3.142f)) radAngle += 2 * 3.142f;
			}
		}
	}

	vecf3d TransformVector(vecf3d original) {
		//vecf3d temp = (original + eyeVector).rotateZ(radAngle);
		vecf3d temp = original.rotateZ(radAngle).rotateY(0.4f) - eyeVector;

		return temp;
	}

	void TransformLandscape() {
		point2d* point2dptr = points2d;

		for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++) {
			vecf3d tempV = TransformVector(vectors3d[i]).project(projection_d);

			CreateScreenCoords(*point2dptr, tempV.x, tempV.y);
			point2dptr++;
		}
	}

	void printVector(vecf3d vec) {
		printf("%2.3f  %2.3f  %2.3f\n", vec.x, vec.y, vec.z);
	}


	bool CullTriangle(triangle* tri) {
		// determine visibility - done by working out if triangles screen coords are clockwise or not
		int x1 = (int) points2d[tri->ind1].x;
		int y1 = (int) points2d[tri->ind1].y;
		int x2 = (int) points2d[tri->ind2].x;
		int y2 = (int) points2d[tri->ind2].y;
		int x3 = (int) points2d[tri->ind3].x;
		int y3 = (int) points2d[tri->ind3].y;

		int e1 = (x2 - x1) * (y2 + y1);
		int e2 = (x3 - x2) * (y3 + y2);
		int e3 = (x1 - x3) * (y1 + y3);

		return (e1 + e2 + e3 < 0);
	}


	void DrawTriangles()
	{
		for (int i = 0; i < MAP_SIZE * MAP_SIZE * 2; i++) {
			int p1 = triangles[i].ind1;
			int p2 = triangles[i].ind2;
			int p3 = triangles[i].ind3;


			if (CullTriangle(&triangles[i])) continue;
			vecf3d pointInTriangle = vectors3d[p1];


			// determine light value
			vecf3d toLightVector = (lightVector - pointInTriangle).norm();
			float lightDot = triangles[i].normal.dot(toLightVector);
			if (lightDot < 0.1f) lightDot = 0.1f;
			if (lightDot > 1.0f) lightDot = 1.0f;
			int lightVal = (int)(255.0f * lightDot);


			FillTriangle(
				(int)points2d[triangles[i].ind1].x,
				(int)points2d[triangles[i].ind1].y,
				(int)points2d[triangles[i].ind2].x,
				(int)points2d[triangles[i].ind2].y,
				(int)points2d[triangles[i].ind3].x,
				(int)points2d[triangles[i].ind3].y,
				olc::Pixel(0, lightVal, 0)
			);
		}
	}


	void DisplayStats() {
		DrawString(10, 10, "Eyepoint:");
		DrawString(160, 10, std::to_string(eyeVector.x));
		DrawString(270, 10, std::to_string(eyeVector.y));
		DrawString(380, 10, std::to_string(eyeVector.z));

		DrawString(10, 30, "Angle:");
		DrawString(160, 30, std::to_string(radAngle));
	}

	// =========================================================================================================
	//  LANDSCAPING (NEW)
	// =========================================================================================================

	float* CreateEmptyFloatArray(int array_size) {
		float* array = new float[array_size];
		for (int i = 0; i < array_size; i++) array[i] = 0.0f;
		return array;
	}
	void SetLand(float* land, int i, int j, int map_size, float value) {

		i = i % map_size;
		j = j % map_size;
		land[j * map_size + i] = value;
	}

	float GetLand(float* land, int i, int j, int map_size) {
		i = i % map_size;
		j = j % map_size;
		return land[j * map_size + i];
	}

	void SetLandHeight(float* land, int i, int j, int map_size, float base_value, int rand_range) {
		if (GetLand(land, i, j, map_size) != 0.0f) return;
		float rand_height = (float)(rand() % rand_range - (rand_range / 2));
		SetLand(land, i, j, map_size, base_value + rand_height);
	}



	void LandscapeRecurse(float* land, int i, int j, int map_size, int size) {
		int halfsize = size / 2;

		SetLandHeight(land, i, j + halfsize, map_size, (GetLand(land, i, j, map_size) + GetLand(land, i, j + size, map_size)) / 2, halfsize);

		SetLandHeight(land, i + halfsize, j, map_size, (GetLand(land, i, j, map_size) + GetLand(land, i + size, j, map_size)) / 2, halfsize);


		SetLandHeight(land, i + size, j + halfsize, map_size, (GetLand(land, i + size, j, map_size) + GetLand(land, i + size, j + size, map_size)) / 2, halfsize);
		SetLandHeight(land, i + halfsize, j + size, map_size, (GetLand(land, i, j + size, map_size) + GetLand(land, i + size, j + size, map_size)) / 2, halfsize);
		SetLandHeight(land, i + halfsize, j + halfsize, map_size,
			(
				GetLand(land, i, j + halfsize, map_size) + GetLand(land, i + halfsize, j, map_size) +
				GetLand(land, i + size, j + halfsize, map_size) + GetLand(land, i + halfsize, j + size, map_size)
			) / 4,
			halfsize);

		if (halfsize > 1) {
			LandscapeRecurse(land, i, j, map_size, halfsize);
			LandscapeRecurse(land, i + halfsize, j, map_size, halfsize);
			LandscapeRecurse(land, i, j + halfsize, map_size, halfsize);
			LandscapeRecurse(land, i + halfsize, j + halfsize, map_size, halfsize);
		}
	}

	float* CreateLandscape(int map_size) {
		float* landHeightArray = CreateEmptyFloatArray(map_size * map_size);

		SetLandHeight(landHeightArray, 0, 0, map_size, 0, map_size);
		LandscapeRecurse(landHeightArray, 0, 0, map_size, map_size);
		return landHeightArray;
	}

	
};


int main() {
	Example demo;
	if (demo.Construct(1280, 720, 1, 1))
		demo.Start();

	return 0;
}
