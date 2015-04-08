
#include "menu.h"

Menu* Menu::activeMenu;
int Menu::menuState;
int Menu::lastMenuState;

// http://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

Menu::Menu()
{
	MODDER = 100;
	shift = 0;
	selectedItem = -1;	//nothing return
}

void Menu::initialize(ID3D10Device* device, Menu* p)
{
	BACK_WAS_PRESSED = false;
	ENTER_WAS_PRESSED = false;
	UP_WAS_PRESSED = false;
	DOWN_WAS_PRESSED = false;

	md3dDevice = device;
	
	if(p == NULL) {
		rootMenu = this;
		parent = this;
	}
	else {
		rootMenu = p->rootMenu;
		parent = p;
	}

	movingMenu = false;
	activeMenu = rootMenu;
	offset = 0;
	opacity = 255;
	highlightColor = RED;
	normalColor = WHITE;
	menuAnchor = D3DXVECTOR2(270,250);
	verticalOffset = 30;
	linePtr = 0;
	menuState = -1;
	lastMenuState = 0;
	selectedItem = -1;
	md3dDevice = device;

	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 24;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy_s(fontDesc.FaceName, L"Consolas");

	D3DX10CreateFontIndirect(md3dDevice, &fontDesc, &mFont);

	upDepressedLastFrame = false;
	downDepressedLastFrame = false;
}

void Menu::initialize(ID3D10Device* device, Menu* p, std::string h)
{
	// Single layer menu
	md3dDevice = device;
	heading = h;

	rootMenu = NULL;
	parent = NULL;

	movingMenu = false;

	activeMenu = this;
	offset = 0;
	opacity = 255;
	highlightColor = RED;
	normalColor = WHITE;
	menuAnchor = D3DXVECTOR2(270,10);
	verticalOffset = 30;
	linePtr = 0;
	menuState = 0;
	lastMenuState = 0;
	selectedItem = -1;
	
	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 24;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy_s(fontDesc.FaceName, L"Consolas");

	D3DX10CreateFontIndirect(md3dDevice, &fontDesc, &mFont);

	upDepressedLastFrame = false;
	downDepressedLastFrame = false;
}

void Menu::update()
{
	menuState=-1;
	if (GetAsyncKeyState(VK_UP) && !UP_WAS_PRESSED)
	{
		UP_WAS_PRESSED = true;
		if(menuAnchor != D3DXVECTOR2(270,250)) {
			shift += verticalOffset;
		}
		linePtr--;
	} else if(!GetAsyncKeyState(VK_UP)) {
		UP_WAS_PRESSED = false;
	}

	if (GetAsyncKeyState(VK_DOWN) && !DOWN_WAS_PRESSED)
	{
		DOWN_WAS_PRESSED = true;
		linePtr++;
		shift -= verticalOffset;
	} else if(!GetAsyncKeyState(VK_DOWN)) {
		DOWN_WAS_PRESSED = false;
	}


	if (linePtr > (int)(menuItems.size()-1)) {
		linePtr = 0;
		if(movingMenu) {
			menuAnchor = D3DXVECTOR2(270,250);
		}
		shift = 0;
	}
	if (linePtr < 0) {
		linePtr = menuItems.size()-1;
		if(menuItems.size()>0)
			if(movingMenu) {
				menuAnchor = D3DXVECTOR2(270,250+(-verticalOffset*linePtr));
			}
	}

	if (GetAsyncKeyState(VK_RETURN) && !ENTER_WAS_PRESSED) {
		ENTER_WAS_PRESSED = true;
		if(children[linePtr] != NULL) {
			selectedItem = linePtr;
			activeMenu = children[linePtr];
			activeMenu->setOffset(MENU_OFFSET);	// set it forward and move it back
			menuState = (lastMenuState-(lastMenuState % MODDER)+linePtr+1)*MODDER;
		} else if(!GetAsyncKeyState(VK_RETURN)) {
			menuState = lastMenuState-(lastMenuState % MODDER) + linePtr+1;
		}
		lastMenuState = menuState;
	} else {
		ENTER_WAS_PRESSED = false;
	}

	if (GetAsyncKeyState(VK_BACK) && !BACK_WAS_PRESSED) {
		BACK_WAS_PRESSED = true;
		if(activeMenu != parent && parent != NULL) {
			activeMenu = parent;
			selectedItem = -1;
			setOpacity(255);
			menuState = lastMenuState/MODDER;
			lastMenuState = menuState;
		}
	} else if(!GetAsyncKeyState(VK_BACK)) {
		BACK_WAS_PRESSED = false;
	}
}


void Menu::displayMenu(float frametime, int w, int a)
{	
	if(shift < 0.0f) {
		shift+=300.0f*frametime;
		if(movingMenu) {
			menuAnchor.y-=300.0f*frametime;
		}
	}
	if(shift > 0.0f) {
		shift-=300.0f*frametime;
		if(movingMenu) {
			menuAnchor.y+=300.0f*frametime;
		}
	}

	D3DXCOLOR c1 = highlightColor;
	D3DXCOLOR c2 = normalColor;
	if(this != activeMenu) {	// FADE IT BRO
		highlightColor = D3DXCOLOR(opacity/255.0,1,1,1);
		normalColor = D3DXCOLOR(opacity/255.0,1,1,1);
	}

	//menuHeadingFont->print(this->menuHeading, menuAnchor.x+this->getOffset(), menuAnchor.y);
	RECT R = {menuAnchor.x+this->getOffset(), menuAnchor.y, 0, 0};
	std::wstring stemp = s2ws(this->menuHeading);
	LPCWSTR result = stemp.c_str();
	mFont->DrawText(0, result, -1, &R, DT_NOCLIP, highlightColor);

	for(int i=0;i<this->menuItems.size();++i) {
		if (linePtr==i && this == activeMenu) {	// Only highlight the active menu
			//menuItemFontHighlight->print(this->menuItems[i], activeMenu->menuAnchor.x+this->getOffset(), activeMenu->menuAnchor.y+verticalOffset*(i+1));
			auto top = activeMenu->menuAnchor.y+verticalOffset*(i+1);
			RECT R = {activeMenu->menuAnchor.x+this->getOffset(), top, 0, 0};
			std::wstring stemp = s2ws(this->menuItems[i]);
			LPCWSTR result = stemp.c_str();
			mFont->DrawText(0, result, -1, &R, DT_NOCLIP, highlightColor);
		}
		else {
			//menuItemFont->print(this->menuItems[i], activeMenu->menuAnchor.x+this->getOffset(), activeMenu->menuAnchor.y+verticalOffset*(i+1));
			RECT R = {activeMenu->menuAnchor.x+this->getOffset(), activeMenu->menuAnchor.y+verticalOffset*(i+1), 0, 0};
			std::wstring stemp = s2ws(this->menuItems[i]);
			LPCWSTR result = stemp.c_str();
			mFont->DrawText(0, result, -1, &R, DT_NOCLIP, normalColor);
		}
	}

	if(this == activeMenu) {	// Constantly move to 0
		if(activeMenu->getOffset() > 0)
			activeMenu->setOffset(activeMenu->getOffset()-300.0f*frametime);
		if(activeMenu->getOffset() < 0)
			activeMenu->setOffset(activeMenu->getOffset()+300.0f*frametime);
	}

	if(parent != NULL && parent != this) {	// If it's not the root menu
		parent->setOffset(getOffset()-MENU_OFFSET);	// All relative to the menu above it

		float op = 255+parent->getOffset();
		if(op<25) op = 25; else if (op>255) op = 255;
		parent->setOpacity(op);
		parent->displayMenu(frametime);
	}

	highlightColor = c1;
	normalColor = c2;

}

void Menu::setMenuItems(std::vector<std::string> v) {
	menuItems = v;
	children.resize(v.size());
	for(int i=0;i<children.size();++i) {
		children[i] = NULL;
	}
}
