#define DAVLIB_IMPLEMENTATION
#include "davlib.h"

#include <stdio.h>

void printItem(int colorID) {
  MenuItem *item = DAV_themeItems[colorID];
  Menu *menu = item->menu;
  Color *color = item->data;
  printf("%s count=%d (%d %d %d %d)\n", item->label, menu->itemCount, color->r,
         color->g, color->b, color->a);
  for (int i = 0; i < menu->itemCount; i++) {
    MenuItem *item = menu->items[i];
    printf("%s inc=%d max=%d min=%d val=%d\n", item->label, item->iinc,
           item->imax, item->imin, item->ivalue);
  }
  printf("\n");
}

int main() {
  InitializeMenus();
  for (int i = 1; i <= THEME_COLOR_HOVER; i++)
    printItem(i);
  // TestInitializeMenu(2, &DAV_theme->panelColor);
  // TestInitializeMenu(3, &DAV_theme->titleColor);
}
