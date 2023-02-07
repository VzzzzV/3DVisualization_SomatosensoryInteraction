#pragma once
#include "stdafx.h"
#include "QtVtk.h"
#include <QtWidgets/QApplication>
#include <vtkoutputwindow.h>
#include<windows.h>
#include<Leap.h>
int Pattern;
using namespace Leap;
DWORD _stdcall LEAP(LPVOID lpParameter);