#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Window.h>
#include <StringView.h>
#include <Dragger.h>

// Globals
#define appSignature "application/x-vnd.pnkp-HugeClock"

// Finally stuff that actually belongs here

class MainView : public BView
{
public:
						    MainView(BMessage* data);
						    MainView();
						    ~MainView();
		void	            MessageReceived(BMessage *msg);
		void			    Tick();
		void	            Pulse();
		
		status_t	        Archive(BMessage* data, bool deep) const;
		static BArchivable* Instantiate(BMessage* data);
			
private:
		BFont* 			    bigFont;
	    //BMessageRunner*   tmrTicky;
		BDragger*		    rplDragger;	
			
		BStringView*		lblTime;
		BStringView*		lblDate;
};

class MainWindow : public BWindow
{
public:
						MainWindow(void);
		MainView*		mainView;
		bool			QuitRequested(void);
			
private:
};

#endif
