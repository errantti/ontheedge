// On the Edge
// 
// Copyright � 2004-2021 Jukka Tykkyl�inen
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
#ifndef __IINPUTRECEIVER_H__
#define __IINPUTRECEIVER_H__

class IInputReceiver
{
public:
	IInputReceiver(bool bActivateInput = true);
	virtual ~IInputReceiver();

	void ActivateInputReceiver();
	void CloseInputReceiver();

	virtual void OnInputActivation() {m_bReceivingInput = true;}
	virtual void OnInputDeActivation() {m_bReceivingInput = false;}

	virtual void ProcessInput() = 0;

protected:
	bool m_bReceivingInput;

private:
	bool m_bInputOpen;
};

#endif // #ifndef __IINPUTRECEIVER_H__