#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <irrlicht.h>
#include <irrString.h>
#include "driverChoice.h"

using namespace std;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

struct SAppContext
{
	IrrlichtDevice *device;
	s32				counter;
	IGUIListBox*	listbox;
};

enum
{
	GUI_ID_QUIT_BUTTON = 101,	// quit button id
	GUI_ID_ENTER_BUTTON,		// change center button id
	GUI_ID_XCOOR_EDITBOX,		// xcoor editbox id
	GUI_ID_YCOOR_EDITBOX,		// ycoor editbox id		
	GUI_ID_CIRCLE_BUTTON,
	GUI_ID_RECTANGLE_BUTTON,
	GUI_ID_GAME_BUTTON,
};

class MyEventReceiver : public IEventReceiver
{
public:
	// Bu bizim constructor'ýmýz.
	MyEventReceiver(SAppContext & context) : Context(context) { }

	// event oldugunda gerçekleþen fonksiyonumuz.
	virtual bool OnEvent(const SEvent& event);

private:
	SAppContext & Context;
};
