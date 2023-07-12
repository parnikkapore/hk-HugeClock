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

        void                MessageReceived(BMessage *msg);
        void                AllAttached();
        void                FrameResized(float width, float height);
        void                Pulse();

        status_t            Archive(BMessage* data, bool deep) const;
        static BArchivable* Instantiate(BMessage* data);

private:
        BDragger*           rplDragger;
        BStringView*        lblTime;
        BStringView*        lblDate;

        BFont*              bigFont;
        BFont*              smallFont;
        // BMessageRunner*  tmrTicky;

        bool                isReplicant;

        void                Initialize(float bigSize, float smallSize);
        void                SetLabelColors();
        void                SetTimeFont();
        void                Tick();
        void                ShowAbout();
};

class MainWindow : public BWindow
{
public:
                        MainWindow(void);
        MainView*       mainView;
        bool            QuitRequested(void);

private:
};

#endif
