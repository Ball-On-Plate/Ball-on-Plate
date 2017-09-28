#include "EventListener.h"
#include "rs232.h"

bool MyEventReceiver::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();
		IGUIEnvironment* env = Context.device->getGUIEnvironment();

		switch (event.GUIEvent.EventType)
		{
		case EGET_BUTTON_CLICKED:
		{
									switch (id)
									{
									case GUI_ID_QUIT_BUTTON:
									{
															   Context.device->closeDevice();

															   RS232_CloseComport(2);

															   return true;
									}
									case GUI_ID_ENTER_BUTTON:
									{
																IGUIElement* root = env->getRootGUIElement();
																stringc Text_X = root->getElementFromId(GUI_ID_XCOOR_EDITBOX, true)->getText();
																stringc Text_Y = root->getElementFromId(GUI_ID_YCOOR_EDITBOX, true)->getText();

																int xCoor = atoi(Text_X.c_str());
																int yCoor = atoi(Text_Y.c_str());
																if (xCoor < 0 || xCoor > 300 || yCoor < 0 || yCoor > 400)
																	cerr << "invalid point " << endl;
																else
																{
																	std::string x = "";
																	std::string y = "";
																	if (xCoor < 100)
																	{
																		x = "0";
																		x += Text_X.c_str();
																	}
																	else
																	{
																		x = Text_X.c_str();
																	}

																	if (yCoor < 100)
																	{
																		y = "0";
																		y += Text_Y.c_str();
																	}
																	else
																	{
																		y = Text_Y.c_str();
																	}

																	std::string set(x + " " + y + "-");
																	cout << set << endl;
																	char * cstr = new char[set.length() + 1];
																	strcpy(cstr, set.c_str());

																	RS232_cputs(cport_nr, cstr);
																}
																root->getElementFromId(GUI_ID_XCOOR_EDITBOX, true)->setText(L"");
																root->getElementFromId(GUI_ID_YCOOR_EDITBOX, true)->setText(L"");

																break;
									}
									case GUI_ID_CIRCLE_BUTTON:
									{
																 cout << "Draw Circle" << endl;

																 std::string set("500 500-");
																 cout << set << endl;
																 char * cstr = new char[set.length() + 1];
																 strcpy(cstr, set.c_str());
																 RS232_cputs(cport_nr, cstr);


																 break;
									}
									case GUI_ID_RECTANGLE_BUTTON:
									{
																	cout << "Draw Square" << endl;

																	std::string set("600 600-");
																	cout << set << endl;
																	char * cstr = new char[set.length() + 1];
																	strcpy(cstr, set.c_str());
																	RS232_cputs(cport_nr, cstr);

																	break;
									}
									case GUI_ID_GAME_BUTTON:
									{
															   cout << "Game Mode" << endl;

															   std::string set("700 700-");
															   cout << set << endl;
															   char * cstr = new char[set.length() + 1];
															   strcpy(cstr, set.c_str());
															   RS232_cputs(cport_nr, cstr);

															   break;
									}

									default:
										return false;
									}
									break;
		}
		default:
			break;
		}
	}
	return false;
}