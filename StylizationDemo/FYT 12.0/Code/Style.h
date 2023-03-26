#ifndef STYLE_H
#define STYLE_H

#include <string>
#include <vector>
using namespace std;

class Style{
public:
	Style();
	void LoadStyle(string givenName = "");
	void SaveStyle();
	BOOL GetClipFileName(LPTSTR szName);
	BOOL SaveClipFileName(LPTSTR szName);
	vector<string> staticModelStyleName;
	vector<string> animatedModelStyleName;
	vector<string> objModelStyleName;
};

extern Style* style;
#endif