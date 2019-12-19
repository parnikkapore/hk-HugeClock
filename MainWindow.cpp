#include <Application.h>
#include <Font.h>
#include <String.h>
#include <MessageRunner.h>
#include <DateFormat.h>
#include <TimeFormat.h>
#include <stdio.h>

#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <Dragger.h>
#include <StringView.h>

#include "MainWindow.h"

// Message codes
enum {
	M_Tick = 'ctik'
};

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,500,400),"Big Clock",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS)
{	
	mainView = new MainView();
	SetPulseRate(1000000);
	SetLayout(new BGroupLayout(B_VERTICAL));
	
	AddChild(mainView);
	
	// Testing purposes
	/*
	BMessage* tstArch = new BMessage();
	mainView->Archive(tstArch, true);
	tstArch->PrintToStream();
	
	BArchivable* restorer = instantiate_object(tstArch);
	MainView* restored = NULL;
	if(restorer) restored = dynamic_cast<MainView*>(restorer);
	else		perror("Restore test failed?!: ");
	*/
}

MainView::MainView()
	:	BView("Big Clock", B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_PULSE_NEEDED, new BGroupLayout(B_HORIZONTAL))
{
	bigFont = new BFont(be_plain_font);
	bigFont->SetSize(bigFont->Size() * 10); // 10em
	
	lblTime.SetAlignment(B_ALIGN_CENTER);
	lblTime.SetFont(bigFont);
	lblTime.SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET)); // Allow scaling sideways
	
	lblDate.SetAlignment(B_ALIGN_CENTER);
  //lblDate.SetFont(be_plain_font);
	lblDate.SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	
	rplDragger = new BDragger{BRect(0,0,7,7), this, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM};
	
	AddChild(BGroupLayoutBuilder(B_VERTICAL, 10)
		.AddGlue()
		.Add(&lblTime)
		.Add(&lblDate)
		.AddGlue()
		.Add(rplDragger)
	);
	
	tmrTicky = new BMessageRunner(BMessenger(this), new BMessage(M_Tick), 1000000);
	if(tmrTicky->InitCheck() != B_OK) printf("tmrTicky didn't init properly!\n");
	Tick(); // Update the display immediately
}

MainView::MainView(BMessage *data)
	: BView(data)
{
	
}

status_t
MainView::Archive(BMessage* data, bool deep) const
{	
	status_t status = BView::Archive(data, true);
	data->AddString("class", "MainView");
	data->AddString("add_on", appSignature);
	
	printf("Archived: %d\n", status);
	return status;
}

BArchivable*
MainView::Instantiate(BMessage *data)
{
		return new MainView(data);
}

void
MainView::Tick()
{
	printf("Tick!\n");
	time_t curTime = time(NULL);
	struct tm *loctime = localtime(&curTime);
	BString formatted;
	BTimeFormat fmTime;
	BDateFormat fmDate;
	
	lblTime.SetText(BString() <<
		loctime->tm_hour << ":" <<
		loctime->tm_min << ":" <<
		loctime->tm_sec
	);
	
	//formatted = "";
	fmTime.Format(formatted, curTime, B_MEDIUM_TIME_FORMAT);
	lblTime.SetText(formatted);
	
	formatted = "";
	fmDate.Format(formatted, curTime, B_FULL_DATE_FORMAT);
	lblDate.SetText(formatted);
}

void
MainView::Pulse()
{
	printf("Bing - ");
	this->Tick();
}

void
MainView::MessageReceived(BMessage *msg)
{
	printf("msgr\n");
	switch (msg->what)
	{
		case M_Tick:
			Tick();
			break;
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
