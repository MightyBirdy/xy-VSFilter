#include "stdafx.h"
#include "overlay_func.h"
#include <assert.h>

BOOL init = false;

void overlay_send_frame(ISubRenderFrame *sub_frame)
{
	if (!init)
	{
		CreateThread(0, NULL, init_overlay, NULL, NULL, NULL);
		init = true;
	}

	if (!sub_frame)
	{
		clear_screen();
		return;
	}

	RECT outputRect;
	sub_frame->GetClipRect(&clipRect);
	sub_frame->GetOutputRect(&outputRect);

	frame = sub_frame;
	int count = 0;
	sub_frame->GetBitmapCount(&count);	

	for (int i = 0; i < count; i++)
	{
		tsubdata x;
		sub_frame->GetBitmap(i, &x.id, &x.position, &x.size, &x.pixels, &x.pitch);

		subs.push_back(x);
	}
	trigger_paint();
}
