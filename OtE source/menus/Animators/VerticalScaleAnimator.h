// On the Edge
// 
// Copyright © 2004-2021 Jukka Tykkyläinen
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef __CVERTICALSCALEANIMATOR_H__
#define __CVERTICALSCALEANIMATOR_H__

#include "..\\..\\Edgewalker\\GUI\\GUIWindow.h"

/**
 * Animates a window by scaling it vertically from a line to 
 * complete window, uses override-rendering that doesn't
 * render window's children
 */
class CVerticalScaleAnimator : public IGUIWindowAnimationSequence
{
public:
	CVerticalScaleAnimator(CGUIWindow *window, bool bFadeIn, float duration,
		bool bEnableControlsAfterAnimation);

	virtual bool OverridesRender() {return true;}

	virtual void Render();
	virtual void Update(float frameTime);

protected:
	float m_timeFromStart;

	CTexture *m_background;
};

#endif 