#include "davlib.h"
#include "raylib.h"
#include <stdlib.h>

Theme *DAV_theme = &(Theme){
    .backgroundColor = BLACK,
    .panelColor = (Color){.r = 11, .g = 23, .b = 15, .a = 240},
    .titleColor = (Color){.r = 0x00, .g = 0xf0, .b = 0x30, .a = 0xcf},
    .labelColor = (Color){.r = 0x00, .g = 0x89, .b = 0x79, .a = 0xcf},
    .valueColor = (Color){.r = 0x7b, .g = 0x4f, .b = 0x13, .a = 0xcf},
    .labelHover = (Color){.r = 0x89, .g = 0x89, .b = 0x79, .a = 0xef},
    .valueHover = (Color){.r = 0x7b, .g = 0x4f, .b = 0x13, .a = 0xef},
    .labelActive = (Color){.r = 0x00, .g = 0x89, .b = 0x79, .a = 0xff},
    .valueActive = (Color){.r = 0xcb, .g = 0x7f, .b = 0x13, .a = 0xff},
    .colorDim = (Color){.r = 127, .g = 127, .b = 0, .a = 255},
    .colorHover = (Color){.r = 255, .g = 255, .b = 0, .a = 255},
    .fontSize = 24,
    .padding = 24,
    .valueColumn = 15.0f,
};

typedef enum ThemeColorID {
  THEME_COLOR_NONE,
  THEME_BACKGROUND_COLOR,
  THEME_PANEL_COLOR,
  THEME_TITLE_COLOR,
  THEME_LABEL_COLOR,
  THEME_LABEL_ACTIVE,
  THEME_LABEL_HOVER,
  THEME_VALUE_COLOR,
  THEME_VALUE_ACTIVE,
  THEME_VALUE_HOVER,
  THEME_COLOR_DIM,
  THEME_COLOR_HOVER,
} ThemeColorID;

// Theme Handlers
void OnChooseFont(Menu *menuptr) {
  MENU_ITEM_CHOICE(menuptr);
  DAV_theme->fontSize = atoi(item->choices[choiceCurrent]);
}
void OnSetColorRed(Menu *menuptr) {
  MENU_ITEM_INT(menuptr);
  Color *color = menu->data;
  color->r = value;
}
void OnSetColorGreen(Menu *menuptr) {
  MENU_ITEM_INT(menuptr);
  Color *color = menu->data;
  color->g = value;
}
void OnSetColorBlue(Menu *menuptr) {
  MENU_ITEM_INT(menuptr);
  Color *color = menu->data;
  color->b = value;
}
void OnSetColorAlpha(Menu *menuptr) {
  MENU_ITEM_INT(menuptr);
  Color *color = menu->data;
  color->a = value;
}

#include <assert.h>
void SetColorItems(Menu *menu) {
  assert(menu->data);
  Color *color = menu->data;
  MenuItem **items = menu->items;
  items[0]->ivalue = color->r;
  items[1]->ivalue = color->g;
  items[2]->ivalue = color->b;
  items[3]->ivalue = color->a;
}

#include <assert.h>
void OnDrawColorBox(struct MenuItem *item, Rectangle rect, int fontSize,
                    Color color) {
  if (item->itemType == MENU_SUB) {
    assert(item->menu);
    assert(item->data);
    Color *c = item->data;
    color = *c;
    // color = *(Color *)(item->menu->data);
  }
  DrawRectangleRounded(rect, .75f, 8, color);
}

MenuItem *DAV_colorItems[] = {
    &(MenuItem){
        .itemType = MENU_INT,
        .label = "Red",
        .ivalue = 0,
        .imin = 0,
        .imax = 256,
        .iinc = 4,
        .onChoose = OnSetColorRed,
    },
    &(MenuItem){
        .itemType = MENU_INT,
        .label = "Green",
        .ivalue = 0,
        .imin = 0,
        .imax = 256,
        .iinc = 4,
        .onChoose = OnSetColorGreen,
    },
    &(MenuItem){
        .itemType = MENU_INT,
        .label = "Blue",
        .ivalue = 0,
        .imin = 0,
        .imax = 256,
        .iinc = 4,
        .onChoose = OnSetColorBlue,
    },
    &(MenuItem){
        .itemType = MENU_INT,
        .label = "Alpha",
        .ivalue = 0,
        .imin = 0,
        .imax = 256,
        .iinc = 4,
        .onChoose = OnSetColorAlpha,
    },
};

Menu DAV_colorMenuTemplate = (Menu){
    .title = "Colors",
    .current = 0,
    .itemCount = sizeof(DAV_colorItems) / sizeof(DAV_colorItems[0]),
    .items = DAV_colorItems,
};

Menu CloneColorMenuTemplate() { return DAV_colorMenuTemplate; }

MenuItem *DAV_themeItems[] = {&(MenuItem){
                                  .itemType = MENU_CHOICE,
                                  .label = "Font Size",
                                  .choices =
                                      (char *[]){
                                          "24",
                                          "32",
                                          "40",
                                          "48",
                                          "56",
                                          "20",
                                      },
                                  .choiceCount = 6,
                                  .onChoose = OnChooseFont,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Background Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Panel Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Title Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Label Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Label Active",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Label Hover",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Value Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Value Active",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Value Hover",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Arrow Dim",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Arrow Hover",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onDraw = OnDrawColorBox,
                                  .onPush = SetColorItems,
                              }};

Menu *DAV_themeMenu = &(Menu){
    .title = "Theme",
    .current = 0,
    .itemCount = sizeof(DAV_themeItems) / sizeof(DAV_themeItems[0]),
    .items = DAV_themeItems,
};

void InitializeMenu(int colorID, Color *color) {
  MenuItem *item = DAV_themeItems[colorID];
  item->data = color;
}

void InitializeMenus() {
  InitializeMenu(THEME_BACKGROUND_COLOR, &DAV_theme->backgroundColor);
  InitializeMenu(THEME_PANEL_COLOR, &DAV_theme->panelColor);
  InitializeMenu(THEME_TITLE_COLOR, &DAV_theme->titleColor);
  InitializeMenu(THEME_LABEL_COLOR, &DAV_theme->labelColor);
  InitializeMenu(THEME_VALUE_COLOR, &DAV_theme->valueColor);
  InitializeMenu(THEME_LABEL_HOVER, &DAV_theme->labelHover);
  InitializeMenu(THEME_VALUE_HOVER, &DAV_theme->valueHover);
  InitializeMenu(THEME_LABEL_ACTIVE, &DAV_theme->labelActive);
  InitializeMenu(THEME_VALUE_ACTIVE, &DAV_theme->valueActive);
  InitializeMenu(THEME_COLOR_DIM, &DAV_theme->colorDim);
  InitializeMenu(THEME_COLOR_HOVER, &DAV_theme->colorHover);
}
