// MatchPoints.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "MatchPoints.h"
#include <vector>

using namespace std;

static vector<vector<Point64>> g_vec1;
static vector<vector<Point64>> g_vec2;

static vector<Point64> g_sub1;
static vector<Point64> g_sub2;

static vector<Point64> g_all1;
static vector<Point64> g_all2;

FILE* g_file;
static int g_fileisopen = 0;

#define CV_PI	3.1415926535897932384626433832795

BOOL MatchData(vector<vector<Point64>> vec1, vector<vector<Point64>> vec2, double POINT_DIS, double err);
void Get6Points(Point64 pt_center, Point64* pt, double r);
BOOL PointInCircle(Point64 point, Point64 pt_center, double r);
BOOL FindPtInMatrix(vector<vector<Point64>> vec, Point64 pt, double err, int& row, int& col);
int compareX(const void* pt1, const void* pt2);
int compareY(const void* pt1, const void* pt2);
void SortPoints(vector<Point64>& vec, int dir);
void MovePoints(vector<vector<Point64>> &vec, Point64 shift, Point64 ratio);
void MoveLine(vector<Point64> &vec, Point64 shift, double ratio);
void LSM(vector<Point64> vec, double& k, double& b);
void RotatePoints(vector<Point64> &vec, Point64 center, double degree);

MATCHPOINTS_API void OpenLog()
{
	fopen_s(&g_file, ".\\debug.txt", "w");
	g_fileisopen = 1;
}

MATCHPOINTS_API void CloseLog()
{
	fclose(g_file);
	g_fileisopen = 0;
}

MATCHPOINTS_API void SetPoints(int idx, double x, double y, int isendline)
{
	Point64 pt;
	pt.x = x;
	pt.y = y;
	if (idx == 1)
	{
		if (isendline)
		{
			g_sub1.push_back(pt);
			g_vec1.push_back(g_sub1);
			g_sub1.clear();
		}
		g_sub1.push_back(pt);
	}
	if (idx == 2)
	{
		if (isendline)
		{
			g_sub2.push_back(pt);
			g_vec2.push_back(g_sub2);
			g_sub2.clear();
		}
		g_sub2.push_back(pt);
	}
}

MATCHPOINTS_API void ClearPoints(int idx)
{
	if (idx == 1)
	{
		g_vec1.clear();
	}
	if (idx == 2)
	{
		g_vec2.clear();
	}
}

MATCHPOINTS_API int MatchPoints(double point_dis, double err)
{
	if (g_fileisopen)
	{
		int count = 0;
		fprintf_s(g_file, "vector 1 points:\n");
		for (size_t i = 0; i < g_vec1.size(); i++)
		{
			for (size_t j = 0; j < g_vec1[i].size(); j++)
			{
				fprintf_s(g_file, "%lf\t%lf\n", g_vec1[i][j].x,  g_vec1[i][j].y);
				count++;
			}
		}
		fprintf_s(g_file, "vector 1 points count: %d\n\n", count);

		count = 0;
		fprintf_s(g_file, "vector 2 points:\n");
		for (size_t i = 0; i < g_vec2.size(); i++)
		{
			for (size_t j = 0; j < g_vec2[i].size(); j++)
			{
				fprintf_s(g_file, "%lf\t%lf\n", g_vec2[i][j].x,  g_vec2[i][j].y);
				count++;
			}
		}
		fprintf_s(g_file, "vector 2 points count: %d\n\n", count);
	}
	if (g_fileisopen)
	{
		fprintf_s(g_file, "Start Match\n");
	}
	if (MatchData(g_vec1, g_vec2, point_dis, err))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void LSM(vector<Point64> vec, double& k, double& b)
{
	double sum_x = 0;
	double sum_x2 = 0;
	double sum_y = 0;
	double sum_xy = 0;

	size_t i;
	for (i = 0; i < vec.size(); i++)
	{
		sum_x += vec[i].x;
		sum_x2 += vec[i].x * vec[i].x;
		sum_y += vec[i].y;
		sum_xy += vec[i].x * vec[i].y;
	}

	b = (sum_x2 * sum_y - sum_x * sum_xy) / (vec.size() * sum_x2 - sum_x * sum_x);
	k = (vec.size() * sum_xy - sum_x * sum_y) / (vec.size() * sum_x2 - sum_x * sum_x);
}

MATCHPOINTS_API int MatchFromFile(char* path, double point_dis, double err, double deg)
{
	FILE* pfile;
	Point64 pt;
	if (!fopen_s(&pfile, path, "r"))
	{
		vector<Point64> vec_point;
		fseek(pfile, 0L, SEEK_SET);
		while (!feof(pfile))
		{
			if (fscanf_s(pfile, "%lf%lf", &pt.x, &pt.y) != EOF)
			{
				vec_point.push_back(pt);
			}
		}
		fclose(pfile);

		size_t len = vec_point.size();
		Point64* pts = new Point64[len];
		for (size_t i = 0; i < len; i++)
		{
			pts[i].x = vec_point[i].x;
			pts[i].y = vec_point[i].y;
		}
		qsort(pts, len, sizeof(Point64), compareX);

		size_t threshold = 0;
		g_vec2.clear();
		vec_point.clear();
		for (size_t i = 0; i < len; i++)
		{
			if (pts[threshold].x + 0.5 < pts[i].x)
			{
				threshold = i;
				g_vec2.push_back(vec_point);
				vec_point.clear();
			}
			vec_point.push_back(pts[i]);
		}
		delete pts;
		for (size_t i = 0; i < g_vec2.size(); i++)
		{
			SortPoints(g_vec2[i], 1);
		}

		RotatePoints(g_vec1[0], g_vec1[0][0], deg);
		len = g_vec1[0].size();
		pts = new Point64[len];
		for (size_t i = 0; i < len; i++)
		{
			pts[i].x = g_vec1[0][i].x;
			pts[i].y = g_vec1[0][i].y;
		}
		qsort(pts, len, sizeof(Point64), compareX);

		threshold = 0;
		g_vec1.clear();
		vec_point.clear();
		for (size_t i = 0; i < len; i++)
		{
			if (pts[threshold].x + 0.5 < pts[i].x)
			{
				threshold = i;
				g_vec1.push_back(vec_point);
				vec_point.clear();
			}
			vec_point.push_back(pts[i]);
		}
		delete pts;

		double k, b, degree;

		for (size_t i = 0; i < g_vec1.size(); i++)
		{
			RotatePoints(g_vec1[i], g_vec1[i][0], -90);
			LSM(g_vec1[i], k, b);
			degree = atan(k) * 180 / CV_PI;
			RotatePoints(g_vec1[i], g_vec1[i][0], 90.0 - degree);
			SortPoints(g_vec1[i], 1);
		}
		Point64 pt_shift, pt_ratio;
		pt_shift.x = g_vec1[0][0].x - g_vec2[0][0].x;
		pt_shift.y = g_vec1[0][0].y - g_vec2[0][0].y;
		pt_ratio.x = 1;
		pt_ratio.y = 1;
		double ratio = 1;
		for (size_t i = 0; i < g_vec2.size(); i++)
		{
			MoveLine(g_vec2[i], pt_shift, ratio);
		}

		for (size_t i = 0; i < g_vec1.size(); i++)
		{
			pt_shift.x = g_vec2[i][0].x - g_vec1[i][0].x;
			pt_shift.y = g_vec2[i][0].y - g_vec1[i][0].y;
			ratio = (g_vec2[i].back().y - g_vec2[i].front().y) /
				(g_vec1[i].back().y - g_vec1[i].front().y);
			MoveLine(g_vec1[i], pt_shift, ratio);
		}

		return MatchPoints(point_dis, err);
	}
	return -1;
}

void MoveLine(vector<Point64> &vec, Point64 shift, double ratio)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		vec[i].x += shift.x;
		vec[i].y += shift.y;
		vec[i].y = (vec[i].y - vec[0].y) * ratio + vec[0].y;
	}
}

void RotatePoints(vector<Point64> &vec, Point64 center, double degree)
{
	double x, y;
	for (size_t i = 0; i < vec.size(); i++)
	{
		x = (vec[i].x - center.x) * cos(degree / 180.0 * CV_PI) - 
			(vec[i].y - center.y) * sin(degree / 180.0 * CV_PI) + center.x;
		y = (vec[i].x - center.x) * sin(degree / 180.0 * CV_PI) + 
			(vec[i].y - center.y) * cos(degree / 180.0 * CV_PI) + center.y;
		vec[i].x = x;
		vec[i].y = y;
	}
}

int compareX(const void* pt1, const void* pt2)
{
	if (abs(((Point64*)pt1)->x - ((Point64*)pt2)->x) < 0.00000001)
	{
		return 0;
	}
	if (((Point64*)pt1)->x > ((Point64*)pt2)->x)
	{
		return 1;
	}
	if (((Point64*)pt1)->x < ((Point64*)pt2)->x)
	{
		return -1;
	}
	return 0;
}

int compareY(const void* pt1, const void* pt2)
{
	if (abs(((Point64*)pt1)->y - ((Point64*)pt2)->y) <0.00000001)
	{
		return 0;
	}
	if (((Point64*)pt1)->y > ((Point64*)pt2)->y)
	{
		return 1;
	}
	if (((Point64*)pt1)->y < ((Point64*)pt2)->y)
	{
		return -1;
	}
	return 0;
}

void MovePoints(vector<vector<Point64>> &vec, Point64 shift, Point64 ratio)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		for (size_t j = 0; j < vec[i].size(); j++)
		{
			vec[i][j].x += shift.x;
			vec[i][j].x = (vec[i][j].x - vec[0][0].x) * ratio.x + vec[0][0].x;
			vec[i][j].y += shift.y;
			vec[i][j].y = (vec[i][j].y - vec[0][0].y) * ratio.y + vec[0][0].y;
		}
	}
}

void SortPoints(vector<Point64>& vec, int dir)
{
	size_t len = vec.size();
	Point64* pts = new Point64[len];
	for (size_t i = 0; i < len; i++)
	{
		pts[i] = vec[i];
	}
	if (dir == 0)
		qsort(pts, len, sizeof(Point64), compareX);
	else
		qsort(pts, len, sizeof(Point64), compareY);
	vec.clear();
	for (size_t i = 0; i < len; i++)
	{
		vec.push_back(pts[i]);
	}
	delete pts;
}

BOOL MatchData(vector<vector<Point64>> vec1, vector<vector<Point64>> vec2, double POINT_DIS, double err)
{
	if (vec1.empty())
	{
		return vec2.empty() ? TRUE : FALSE;
	}
	if (vec2.empty())
	{
		return FALSE;
	}
	Point64 pt1[6];
	int row1, col1;
	if (PointInCircle(vec2[0][0], vec1[0][0], err))	//先断点中心是否在误差范围内
	{
		Get6Points(vec1[0][0], pt1, POINT_DIS);		//得到周围6点的坐标
		for (size_t i = 0; i < 6; i++)
		{
			if (FindPtInMatrix(vec1, pt1[i], POINT_DIS / 5, row1, col1))		//得到坐标对应行和列
			{
				if (!PointInCircle(vec2[col1][row1], vec1[col1][row1], err))
				{
					return FALSE;
				}
			}
		}
		vec1[0].erase(vec1[0].begin());
		if (vec1[0].empty())
		{
			vec1.erase(vec1.begin());
		}
		vec2[0].erase(vec2[0].begin());
		if (vec2[0].empty())
		{
			vec2.erase(vec2.begin());
		}
		return MatchData(vec1, vec2, POINT_DIS, err);
	}
	else
	{
		return FALSE;
	}
}

void Get6Points(Point64 pt_center, Point64* pt, double r)
{
	pt[0].x = pt_center.x + r * cos(30.0 / 180.0 * CV_PI);
	pt[0].y = pt_center.y + r * sin(30.0 / 180.0 * CV_PI);

	pt[1].x = pt_center.x + r * cos(90.0 / 180.0 * CV_PI);
	pt[1].y = pt_center.y + r * sin(90.0 / 180.0 * CV_PI);

	pt[2].x = pt_center.x + r * cos(150.0 / 180.0 * CV_PI);
	pt[2].y = pt_center.y + r * sin(150.0 / 180.0 * CV_PI);

	pt[3].x = pt_center.x + r * cos(210.0 / 180.0 * CV_PI);
	pt[3].y = pt_center.y + r * sin(210.0 / 180.0 * CV_PI);

	pt[4].x = pt_center.x + r * cos(270.0 / 180.0 * CV_PI);
	pt[4].y = pt_center.y + r * sin(270.0 / 180.0 * CV_PI);

	pt[5].x = pt_center.x + r * cos(330.0 / 180.0 * CV_PI);
	pt[5].y = pt_center.y + r * sin(330.0 / 180.0 * CV_PI);
}

BOOL PointInCircle(Point64 point, Point64 pt_center, double r)
{
	if (g_fileisopen)
	{
		fprintf_s(g_file, "%lf\n", sqrt((point.x - pt_center.x) * (point.x - pt_center.x) + (point.y - pt_center.y) * (point.y - pt_center.y)));
	}
	return (point.x - pt_center.x) * (point.x - pt_center.x) + (point.y - pt_center.y) * (point.y - pt_center.y) <= r * r;
}

BOOL FindPtInMatrix(vector<vector<Point64>> vec, Point64 pt, double err, int& row, int& col)
{
	size_t i, j;
	for (i = 0; i < vec.size(); i++)
	{
		if (pt.x < vec[i][0].x - err)
		{
			return FALSE;
		}
		if (abs(pt.x - vec[i][0].x) <= err)
		{
			for (j = 0; j < vec[i].size(); j++)
			{
				if (pt.y < vec[i][j].y - err)
				{
					return FALSE;
				}
				if (abs(pt.y - vec[i][j].y) <= err)
				{
					row = j;
					col = i;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


MATCHPOINTS_API int GetPointsCount(int idx)
{
	if (idx == 1)
	{
		g_all1.clear();
		for (size_t i = 0; i < g_vec1.size(); i++)
		{
			for (size_t j = 0; j < g_vec1[i].size(); j++)
			{
				g_all1.push_back(g_vec1[i][j]);
			}
		}
		return g_all1.size();
	}
	if (idx == 2)
	{
		g_all2.clear();
		for (size_t i = 0; i < g_vec2.size(); i++)
		{
			for (size_t j = 0; j < g_vec2[i].size(); j++)
			{
				g_all2.push_back(g_vec2[i][j]);
			}
		}
		return g_all2.size();
	}
	return 0;
}

MATCHPOINTS_API void GetAllPoints(int idx, int no, double &x, double &y)
{
	if (idx == 1)
	{
		x = g_all1[no].x;
		y = g_all1[no].y;
	}
	if (idx == 2)
	{
		x = g_all2[no].x;
		y = g_all2[no].y;
	}
}

MATCHPOINTS_API_CPP std::vector<std::vector<Point64>> GetPoints(int idx)
{
	std::vector<std::vector<Point64>> vec;
	if (idx == 1)
	{
		vec = g_vec1;
	}
	if (idx == 2)
	{
		vec = g_vec2;
	}
	return vec;
}