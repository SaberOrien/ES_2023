#ifndef __EXECUTE
#define __EXECUTE

typedef enum {OFF, ON} updateLed;
void setLed(updateLed state);
void exeSetLed();
void exeClearLed();
void exeBlinkLed();
void exeStatusLed();
int chooseLed();
void exeInvertLed();

void exeDispNum();
void updateDisplay();
void exeDispRead();
void exeReadButton();
int chooseButton();
void exePullupEn();
void exePullupDis();
void updatePUPDR(int enabled);
void exeDeviceStatus();

#endif
