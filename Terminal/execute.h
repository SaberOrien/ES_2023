#ifndef EXECUTE_H
#define EXECUTE_H

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

void exeDeviceStatus();

#endif
