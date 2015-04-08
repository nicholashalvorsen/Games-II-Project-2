
              // file is included in more than one place
#define WIN32_LEAN_AND_MEAN
#ifndef MENU_H                 // Prevent multiple definitions if this 
#define MENU_H 

class Menu;

#include "constants.h"
#include <string>
#include <sstream>
#include <vector>
#include "d3dApp.h"

namespace menuNS
{ }

// inherits from Entity class
class Menu 
{
private:
   ID3D10Device* md3dDevice;
   int selectedItem;
   float offset;
   int opacity;
   std::vector<Menu*> children;
   Menu* parent;
   Menu* rootMenu;
   static Menu* activeMenu;
   std::string menuHeading;
   std::vector<std::string> menuItems;
   D3DXVECTOR2 menuAnchor;
   int verticalOffset;
   int linePtr;
   D3DXCOLOR highlightColor ;
   D3DXCOLOR normalColor;
   bool upDepressedLastFrame;
   bool downDepressedLastFrame;
   static int menuState;
   static int lastMenuState;
   std::string heading;
   ID3DX10Font* mFont;

   float shift;
   int MODDER;

   bool BACK_WAS_PRESSED;
   bool ENTER_WAS_PRESSED;
   bool UP_WAS_PRESSED;
   bool DOWN_WAS_PRESSED;

   bool movingMenu;


public:
    // constructor
    Menu();
	void initialize(ID3D10Device *device, Menu* p);
	void initialize(ID3D10Device *device, Menu* p, std::string h);
	void update();
	int getSelectedItem() {return selectedItem;}
	void displayMenu(float frametime, int a=-1, int w=-1);
	void setMenuHeading(std::string s) { menuHeading = s; }
	std::vector<Menu*> getChildren() { return children; }
	void setChildren(std::vector<Menu*> v) { children = v; }
	std::vector<std::string> getMenuItems() { return menuItems; }
	void setMenuItems(std::vector<std::string> v);
	Menu* getActiveMenu() { return activeMenu; }
	float getOffset() { return offset; }
	void setOffset(float o) { offset = o; }
	void setOpacity(int o) { opacity = o; }
	int getMenuState() { return menuState; }
	int getAnchorX() { return menuAnchor.x; }
	int getAnchorY() { return menuAnchor.y; }
	void setMovingMenu(bool m) { movingMenu = m; }
	
};

#endif

