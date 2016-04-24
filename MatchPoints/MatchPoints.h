// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MATCHPOINTS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MATCHPOINTS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MATCHPOINTS_EXPORTS
#define MATCHPOINTS_API extern "C" __declspec(dllexport)
#define MATCHPOINTS_API_CPP __declspec(dllexport)
#else
#define MATCHPOINTS_API extern "C" __declspec(dllimport)
#define MATCHPOINTS_API_CPP __declspec(dllimport)
#endif

#include <vector>

typedef struct _points
{
	double x;
	double y;
}Point64;

MATCHPOINTS_API void SetPoints(int idx, double x, double y, int isendline);
MATCHPOINTS_API int MatchPoints(double point_dis, double err);
MATCHPOINTS_API int MatchFromFile(char* path, double point_dis, double err, double deg);
MATCHPOINTS_API void ClearPoints(int idx);
MATCHPOINTS_API void OpenLog();
MATCHPOINTS_API void CloseLog();
MATCHPOINTS_API int GetPointsCount(int idx);
MATCHPOINTS_API void GetAllPoints(int idx, int no, double &x, double &y);
MATCHPOINTS_API_CPP std::vector<std::vector<Point64>> GetPoints(int idx);
