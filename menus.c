#include "davlib.h"
#include <stdlib.h>

Theme *DAV_theme = &(Theme){
    .backgroundColor = BLACK,
    .panelColor = (Color){.r = 11, .g = 127, .b = 127, .a = 96},
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
void OnPushTitleColor(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->titleColor);
}
void OnPushPanelColor(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->panelColor);
}
void OnPushLabelColor(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->labelColor);
}
void OnPushLabelActive(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->labelActive);
}
void OnPushLabelHover(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->labelHover);
}
void OnPushValueColor(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->valueColor);
}
void OnPushValueActive(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->valueActive);
}
void OnPushValueHover(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->valueHover);
}
void OnPushBackgroundColor(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->backgroundColor);
}
void OnPushColorDim(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->colorDim);
}
void OnPushColorHover(Menu *menuptr) {
  SET_COLOR_ITEMS(menuptr, &DAV_theme->colorHover);
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
                                  .label = "Title Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushTitleColor,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Panel Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushPanelColor,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Label Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushLabelColor,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Label Active",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushLabelActive,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Label Hover",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushLabelHover,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Value Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushValueColor,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Value Active",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushValueActive,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Value Hover",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushValueHover,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Background Color",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushBackgroundColor,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Arrow Dim",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushColorDim,
                              },
                              &(MenuItem){
                                  .itemType = MENU_SUB,
                                  .label = "Arrow Hover",
                                  .menu = &DAV_colorMenuTemplate,
                                  .onPush = OnPushColorHover,
                              }};

Menu *DAV_themeMenu = &(Menu){
    .title = "Theme",
    .current = 0,
    .itemCount = sizeof(DAV_themeItems) / sizeof(DAV_themeItems[0]),
    .items = DAV_themeItems,
};
