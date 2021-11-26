#ifndef APP_H
#define APP_H

#include <Application.h>
#include "MainWindow.h"

class App : public BApplication
{
public:
	App(void);
	
private:
	MainWindow* mainwin;
};

#endif
