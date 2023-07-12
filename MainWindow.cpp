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
#include <Alert.h>
#include <TimeZone.h>

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

bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

/**/

MainView::MainView()
	: BView("Big Clock",
			B_WILL_DRAW | B_PULSE_NEEDED | B_TRANSPARENT_BACKGROUND | B_FRAME_EVENTS,
			new BGroupLayout(B_VERTICAL))
{
	isReplicant = false;
	Initialize(be_plain_font->Size() * 10, be_plain_font->Size());
}

MainView::MainView(BMessage *data) : BView(data)
{
	isReplicant = true;
	SetViewColor(B_TRANSPARENT_COLOR);
	float bigSize = 0;
	float smallSize = 0;
	data->FindFloat("MainView::bigSize", &bigSize);
	data->FindFloat("MainView::smallSize", &smallSize);
	Initialize(bigSize, smallSize);
}

void
MainView::Initialize(float bigSize, float smallSize)
{
	bigFont = new BFont(be_plain_font);
	bigFont->SetSize(bigSize);
	smallFont = new BFont(be_plain_font);
	smallFont->SetSize(smallSize);

    lblTime = new BStringView("lblTime", "10:25");
	lblTime->SetAlignment(B_ALIGN_CENTER);
	lblTime->SetFont(bigFont);
	// Allow scaling sideways
	lblTime->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

    lblDate = new BStringView("lblDate", "Saturday 4 May, 2019");
	lblDate->SetAlignment(B_ALIGN_CENTER);
    // lblDate->SetFont(smallFont);
	lblDate->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	rplDragger = new BDragger(BRect(0,0,7,7), this, B_FOLLOW_NONE);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
		.AddGlue()
		.Add(lblTime)
		.Add(lblDate)
		.AddGlue()
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(rplDragger)
		.End()
	.End();

	// SetDrawingMode(B_OP_OVER);
	SetLabelColors();

	// tmrTicky = new BMessageRunner(BMessenger(this), new BMessage(M_Tick), 1000000);
	// if(tmrTicky->InitCheck() != B_OK) printf("tmrTicky didn't init properly!\n");
	Tick(); // Update the display immediately
}

MainView::~MainView()
{
	delete bigFont;
	delete smallFont;
}

/**/

status_t
MainView::Archive(BMessage* data, bool deep) const
{
	(void)deep; // unused-parameter

	// Don't archive the child views - we'll do it ourselves
	status_t status = BView::Archive(data, false);
	data->AddString("class", "MainView");
	data->AddString("add_on", appSignature);
	data->AddFloat("MainView::bigSize", bigFont->Size());
	data->AddFloat("MainView::smallSize", smallFont->Size());

	return status;
}

BArchivable*
MainView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "MainView")) return new MainView(data);
	return NULL;
}

/**/

void
MainView::AllAttached()
{
	BView::AllAttached();
	SetLabelColors();
}

void
MainView::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
		case M_Tick:
			Tick();
			break;
		case B_ABOUT_REQUESTED:
			ShowAbout();
			break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}

void
MainView::Pulse()
{
	this->Tick();
}

void
MainView::FrameResized(float width, float height)
{
	BView::FrameResized(width, height);
	SetTimeFont();
}

/**/

void
MainView::SetLabelColors()
{
	// the defaults are fine if we're in a window
	if(!isReplicant) return;

	// For replicants, set the transparency of the labels again,
	// as they are overrided on attach. Why.
	lblTime->SetViewColor(B_TRANSPARENT_COLOR);
	lblTime->SetDrawingMode(B_OP_OVER);
	lblDate->SetViewColor(B_TRANSPARENT_COLOR);
	lblDate->SetDrawingMode(B_OP_OVER);
}

void
MainView::SetTimeFont()
{
	const int test_font_size = 60;
	BFont testFont = BFont(be_plain_font);
	testFont.SetSize(test_font_size);

	BString formatted;
	BTimeFormat format;

	const time_t amTime = 11*(60*60) + 48*(60) + 26;
	format.Format(formatted, amTime, B_MEDIUM_TIME_FORMAT, new BTimeZone("UTC"));
	const float amWidth = testFont.StringWidth(formatted.String());

	const time_t pmTime = 23*(60*60) + 48*(60) + 26;
	format.Format(formatted, pmTime, B_MEDIUM_TIME_FORMAT, new BTimeZone("UTC"));
	const float pmWidth = testFont.StringWidth(formatted.String());

	const float finalWidth = std::max(amWidth, pmWidth) * 1.1 + 16;
	const float finalSize = std::max(
		test_font_size * (lblTime->Bounds().Width() / finalWidth),
		5.0f
	);

	bigFont->SetSize(finalSize);
	lblTime->SetFont(bigFont);

	// TODO: If the date is wider than the time, it would inf-loop increasing the size.
	// We need to not do that.
	// const float smallSize = std::max(finalSize / 5, be_plain_font->Size());
	// smallFont->SetSize(smallSize);
	// lblDate->SetFont(smallFont);
}

void
MainView::ShowAbout()
{
	BAlert *alert = new BAlert(
		"about",
		"BigClock v0.1.1a\n"
		"Copyright 2019-2023 Parnikkapore",
		"OK"
	);
	alert->Go();
}

void
MainView::Tick()
{
	time_t curTime = time(NULL);
	BString formatted;
	BTimeFormat fmTime;
	BDateFormat fmDate;

	// formatted = "";
	fmTime.Format(formatted, curTime, B_MEDIUM_TIME_FORMAT);
	lblTime->SetText(formatted);

	formatted = "";
	fmDate.Format(formatted, curTime, B_FULL_DATE_FORMAT);
	lblDate->SetText(formatted);
}
