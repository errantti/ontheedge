
/*********************************
 *
 *		On the Edge
 * Copyright (2004) Azuris Productions
 * Jukka Tykkyläinen
 * http://koti.mbnet.fi/jtykky/stuff/azuris
 *
 * IMenuHandler - interface class for menus
 *
 *********************************/

#ifndef __IMENU_H__
#define __IMENU_H__

#include "IMenuManager.h"

class IMenuHandler : public IEventReceiver
{
public:
	IMenuHandler(MENU_ID id, IMenuManager *mgr) : m_manager(mgr), m_ID(id)
	{
		m_environment = CGUIEnvironment::GetGUIEnvironment();
	}

	virtual ~IMenuHandler() {}

	// InitializeMenu
	// Loads the menu controls to the gui environment
	virtual bool InitializeMenu() = 0;
	// removes the menu controls from the gui environment
	virtual void OnClose() = 0;

	// handler for messages for this menu
	virtual bool OnEvent(SEvent event) = 0;

	void SetWindowHandle(CGUIWindow *window) {m_window = window;}

	MENU_ID GetID() {return m_ID;}

	void SendCloseMessage()
	{
		if(m_window == 0)
			return;
		SEvent e;
		e.type = EW_GUI_EVENT;
		e.guiEvent.type = EWGUI_ELEMENT_DESTROYED;
		m_window->OnEvent(e);
	}

protected:
	CGUIEnvironment *m_environment;
	IMenuManager *m_manager;

	CGUIWindow *m_window;

	MENU_ID m_ID;
};


#endif // #ifndef __IMENU_H__