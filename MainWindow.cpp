#include <Application.h>
#include <Font.h>
#include <String.h>
#include <MessageRunner.h>
#include <DateFormat.h>
#include <TimeFormat.h>
#include <stdio.h>

#include <GroupLayout.h>
#include <LayoutBuilder.h>
#include <Dragger.h>
#include <StringView.h>

#include "MainWindow.h"

// Message codes
enum {
	M_Tick = 'ctik'
};

MainWindow::MainWindow(void)
	: BWindow(BRect(100,100,500,400), "Big Clock", B_TITLED_WINDOW,
			B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS)
{
	mainView = new MainView();
	SetPulseRate(1000000);
	SetLayout(new BGroupLayout(B_VERTICAL));

	AddChild(mainView);
}

MainView::MainView()
	: BView("Big Clock",
			B_WILL_DRAW | B_PULSE_NEEDED | B_TRANSPARENT_BACKGROUND,
			new BGroupLayout(B_VERTICAL))
{
	Initialize();
}

MainView::MainView(BMessage *data) : BView(data)
{
	SetViewColor(B_TRANSPARENT_COLOR);
	Initialize();
}

MainView::~MainView()
{
	delete bigFont;
	delete lblTime;
	delete lblDate;
}

void
MainView::Initialize()
{
	bigFont = new BFont(be_plain_font);
	bigFont->SetSize(bigFont->Size() * 10); // 10em

    lblTime = new BStringView{"lblTime", "10:25"};
	lblTime->SetAlignment(B_ALIGN_CENTER);
	lblTime->SetFont(bigFont);
	lblTime->SetViewColor(B_TRANSPARENT_COLOR);
	lblTime->SetDrawingMode(B_OP_OVER);
	// Allow scaling sideways
	lblTime->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

    lblDate = new BStringView{"lblDate", "Saturday 4 May, 2019"};
	lblDate->SetAlignment(B_ALIGN_CENTER);
	lblDate->SetViewColor(B_TRANSPARENT_COLOR);
	lblDate->SetDrawingMode(B_OP_ALPHA);
    // lblDate->SetFont(be_plain_font);
	lblDate->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	rplDragger = new BDragger{BRect(0,0,7,7), this, B_FOLLOW_NONE};

	BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
		.AddGlue()
		.Add(lblTime)
		.Add(lblDate)
		.AddGlue()
		.Add(rplDragger)
	.End();

	SetDrawingMode(B_OP_OVER);

	//tmrTicky = new BMessageRunner(BMessenger(this), new BMessage(M_Tick), 1000000);
	//if(tmrTicky->InitCheck() != B_OK) printf("tmrTicky didn't init properly!\n");
	Tick(); // Update the display immediately
}

void
MainView::AllAttached()
{
	BView::AllAttached();

	// Set the transparency of the labels again, as they are overrided on attach. Why.
	lblTime->SetViewColor(B_TRANSPARENT_COLOR);
	lblTime->SetDrawingMode(B_OP_ALPHA);
	lblDate->SetViewColor(B_TRANSPARENT_COLOR);
	lblDate->SetDrawingMode(B_OP_OVER);
}

status_t
MainView::Archive(BMessage* data, bool deep) const
{
	// Don't archive the child views - we'll do it ourselves
	status_t status = BView::Archive(data, false);
	data->AddString("class", "MainView");
	data->AddString("add_on", appSignature);

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
	time_t curTime = time(NULL);
	struct tm *loctime = localtime(&curTime);
	BString formatted;
	BTimeFormat fmTime;
	BDateFormat fmDate;

	//formatted = "";
	fmTime.Format(formatted, curTime, B_MEDIUM_TIME_FORMAT);
	lblTime->SetText(formatted);

	formatted = "";
	fmDate.Format(formatted, curTime, B_FULL_DATE_FORMAT);
	lblDate->SetText(formatted);
}

void
MainView::Pulse()
{
	this->Tick();
}

void
MainView::MessageReceived(BMessage *msg)
{
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
