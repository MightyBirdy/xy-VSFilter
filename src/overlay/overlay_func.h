#pragma once

#ifdef FROM_OVERLAY
typedef ULONGLONG REFERENCE_TIME;
#endif

#include "../../include/SubRenderIntf.h"

void overlay_send_frame(ISubRenderFrame *sub_frame);
void trigger_paint();

DWORD WINAPI init_overlay(LPVOID lpParam);
void clear_screen();

extern FILE *mylog;

typedef struct {
	ULONGLONG id;
	POINT position;
	SIZE size;
	LPCVOID pixels;
	int pitch;
} tsubdata;

extern ISubRenderFrame *frame;
extern std::vector<tsubdata> subs;
extern RECT clipRect;
