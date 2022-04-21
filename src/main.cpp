#include <M5EPD.h>
#include <Ticker.h>
#include "./epdgui/epdgui.h"
#include "binaryttf.h"

const char *kTitles[20] = {
    "START", "RESET"};

enum
{
    KEY_START = 0,
    KEY_RESET,
};

#define DEFAULT_FONT_SIZE 200

#define TIMER_DURATION 900

EPDGUI_Button *btns[2];
M5EPD_Canvas canvas_result(&M5.EPD);
M5EPD_Canvas tomato_counter(&M5.EPD);

String timer_str;
int timer_seconds;
int completed_tomatoes;
int input_font_size = DEFAULT_FONT_SIZE;

Ticker countdownTimer;

void updateValue()
{
    canvas_result.fillCanvas(0);

    canvas_result.setTextSize(DEFAULT_FONT_SIZE);
    canvas_result.drawString(timer_str, 512, 272);

    canvas_result.pushCanvas(0, 256, UPDATE_MODE_DU);
}

void updateTomatoCounter()
{
    tomato_counter.fillCanvas(11);

    tomato_counter.setTextSize(64);
    tomato_counter.setTextColor(0,11);

    tomato_counter.drawString("Tomatoes: " + String(completed_tomatoes), 260, 60);

    tomato_counter.setTextSize(32);
    tomato_counter.drawString(String(M5.getBatteryRaw()/24) + "%", 520, 24);

    tomato_counter.pushCanvas(0, 0, UPDATE_MODE_NONE);
    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
}

void resetCompletedTomatoes()
{
    completed_tomatoes = 0;
    updateTomatoCounter();
}

void increaseCompletedTomatoes()
{
    completed_tomatoes++;
    updateTomatoCounter();
}

void setTimerStr()
{
    char buffer[6];

    int s = timer_seconds % 60;

    int m = ((timer_seconds - s)/60) % 60;
    sprintf(buffer, "%02d:%02d", m, s);
    timer_str = String(buffer);

}

void stopTimer()
{
    countdownTimer.detach();
    btns[KEY_START]->setLabel("START");
    btns[KEY_START]->Draw();
}

void resetTimer()
{
    stopTimer();
    timer_seconds = TIMER_DURATION;
    setTimerStr();
    updateValue();
}

void decreaseCountdownTimer()
{
    timer_seconds--;
    setTimerStr();
    updateValue();
    if (timer_seconds == 0) {
        stopTimer();
        increaseCompletedTomatoes();
    }
}

void startTimer()
{
    if (timer_seconds == 0) {
        resetTimer();
    }
    btns[KEY_START]->setLabel("STOP");
    btns[KEY_START]->Draw();
    countdownTimer.attach(1, decreaseCountdownTimer);
}

void onResetTimerPress(epdgui_args_vector_t &args)
{
    if (timer_seconds == TIMER_DURATION)
    {
        resetCompletedTomatoes();
    }
    resetTimer();
}

void onStartTimerPress(epdgui_args_vector_t &args)
{
    if (countdownTimer.active() == false)
    {
        startTimer();
    }
    else
    {
        stopTimer();
    }
}

void creatKeys()
{
    for (int i = 0; i < 2; i++)
    {
        int idx = i;
        btns[idx] = new EPDGUI_Button(kTitles[idx], 12 + i * 264, 828, 252, 120);
        btns[idx]->SetCustomString(kTitles[idx]);
        EPDGUI_AddObject(btns[idx]);
    }

    btns[KEY_START]->Bind(EPDGUI_Button::EVENT_RELEASED, onStartTimerPress);
    btns[KEY_RESET]->Bind(EPDGUI_Button::EVENT_RELEASED, onResetTimerPress);
}

void setup()
{
    disableCore0WDT();
    M5.begin(true, false, false, true, false);
    M5.TP.SetRotation(90);
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);

    canvas_result.createCanvas(516, 276);
    canvas_result.loadFont(binaryttf, sizeof(binaryttf));
    canvas_result.createRender(36, 14);
    for(int i = DEFAULT_FONT_SIZE; i >= 8; i-= 8)
    {
        canvas_result.createRender(i, 14);
        canvas_result.setTextSize(i);
        for (int i = 0; i < 10; i++)
        {
            canvas_result.preRender('0' + i);
        }
        canvas_result.preRender(':');
    }
    canvas_result.setTextDatum(BR_DATUM);

    tomato_counter.createCanvas(540, 120);
    tomato_counter.loadFont(binaryttf, sizeof(binaryttf));
    tomato_counter.createRender(36, 14);
    for(int i = DEFAULT_FONT_SIZE; i >= 8; i-= 8)
    {
        tomato_counter.createRender(i, 14);
        tomato_counter.setTextSize(i);
        for (int i = 0; i < 10; i++)
        {
            tomato_counter.preRender('0' + i);
        }
        tomato_counter.preRender(':');
    }
    tomato_counter.setTextDatum(MC_DATUM);

    creatKeys();
    resetTimer();
    resetCompletedTomatoes();
}

void loop()
{
    EPDGUI_Run();
}