#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "dxut.h"
#include "definition.h"
#include "Video.h"
#include "TextureRT.h"
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

void 								CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void								CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );

#endif

