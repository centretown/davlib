#include "davlib.h"
#include "raylib.h"
#include <stdlib.h>

Theme *DAV_theme = &(Theme){
    .backgroundColor = BLACK,
    .panelColor = (Color){.r = 0, .g = 12, .b = 15, .a = 240},
    .titleColor = (Color){.r = 255, .g = 200, .b = 0, .a = 255},
    .labelColor = (Color){.r = 0, .g = 97, .b = 87, .a = 255},
    .labelHover = (Color){.r = 0, .g = 157, .b = 142, .a = 255},
    .labelActive = (Color){.r = 0, .g = 247, .b = 227, .a = 255},
    .valueColor = (Color){.r = 137, .g = 127, .b = 0, .a = 255},
    .valueHover = (Color){.r = 177, .g = 167, .b = 0, .a = 255},
    .valueActive = (Color){.r = 217, .g = 207, .b = 0, .a = 255},
    .colorDim = (Color){.r = 127, .g = 127, .b = 0, .a = 255},
    .colorHover = (Color){.r = 255, .g = 255, .b = 0, .a = 255},
    .fontSize = 24,
    .padding = 24,
    .valueColumn = 13.0f,
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

void OnSetVector3X(Menu *menuptr) {
  MENU_ITEM_FLOAT(menuptr);
  Vector3 *vector3 = menu->data;
  vector3->x = value;
}

void OnSetVector3Y(Menu *menuptr) {
  MENU_ITEM_FLOAT(menuptr);
  Vector3 *vector3 = menu->data;
  vector3->y = value;
}

void OnSetVector3Z(Menu *menuptr) {
  MENU_ITEM_FLOAT(menuptr);
  Vector3 *vector3 = menu->data;
  vector3->z = value;
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
void SetVector3Items(Menu *menu) {
  assert(menu->data);
  Vector3 *vector3 = menu->data;
  MenuItem **items = menu->items;
  items[VECTOR_X]->fvalue = vector3->x;
  items[VECTOR_Y]->fvalue = vector3->y;
  items[VECTOR_Z]->fvalue = vector3->z;
}

void SetColorItems(Menu *menu) {
  assert(menu->data);
  Color *color = menu->data;
  MenuItem **items = menu->items;
  items[COLOR_ITEM_DISPLAY]->data = color;
  items[COLOR_ITEM_RED]->ivalue = color->r;
  items[COLOR_ITEM_GREEN]->ivalue = color->g;
  items[COLOR_ITEM_BLUE]->ivalue = color->b;
  items[COLOR_ITEM_ALPHA]->ivalue = color->a;
}

#include <assert.h>
void OnDrawDisplayColor(MenuItem *item, Theme *theme, Rectangle rect) {
  Color *color = item->data;
  if (item->itemType == MENU_DISPLAY) {
    DrawRectangleRounded(rect, .75f, 16, *color);
  }
}

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

MenuItem *DAV_vector3Items[] = {
    &(MenuItem){
        .itemType = MENU_FLOAT,
        .label = "X Axis",
        .fvalue = 0,
        .fmin = 0.0f,
        .fmax = 1.0f,
        .finc = .05f,
        .onChoose = OnSetVector3X,
    },
    &(MenuItem){
        .itemType = MENU_FLOAT,
        .label = "Y Axis",
        .fvalue = 0,
        .fmin = 0.0f,
        .fmax = 1.0f,
        .finc = .05f,
        .onChoose = OnSetVector3Y,
    },
    &(MenuItem){
        .itemType = MENU_FLOAT,
        .label = "Z Axis",
        .fvalue = 0,
        .fmin = 0.0f,
        .fmax = 1.0f,
        .finc = .05f,
        .onChoose = OnSetVector3Z,
    },
};

Menu DAV_vector3MenuTemplate = (Menu){
    .title = "Vector",
    .current = 0,
    .itemCount = sizeof(DAV_vector3Items) / sizeof(DAV_vector3Items[0]),
    .items = DAV_vector3Items,
};

MenuItem *DAV_colorItems[] = {
    &(MenuItem){
        .itemType = MENU_DISPLAY,
        .onDisplay = OnDrawDisplayColor,
    },
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
    .current = 1,
    .itemCount = sizeof(DAV_colorItems) / sizeof(DAV_colorItems[0]),
    .items = DAV_colorItems,
};

MenuItem *DAV_cubeItems[] = {
    &(MenuItem){
        .itemType = MENU_SUB,
        .label = "Position",
        .menu = &DAV_vector3MenuTemplate,
        .onPush = SetVector3Items,
    },
    &(MenuItem){
        .itemType = MENU_SUB,
        .label = "Size",
        .menu = &DAV_vector3MenuTemplate,
        .onPush = SetVector3Items,
    },
    &(MenuItem){
        .itemType = MENU_SUB,
        .label = "Color",
        .menu = &DAV_colorMenuTemplate,
        .onDraw = OnDrawColorBox,
        .onPush = SetColorItems,
    },
};

Menu *DAV_cubeMenu = &(Menu){
    .title = "Theme",
    .current = 0,
    .itemCount = sizeof(DAV_cubeItems) / sizeof(DAV_cubeItems[0]),
    .items = DAV_cubeItems,
};

MenuItem *DAV_themeItems[] = {
    // comment here for formatting
    &(MenuItem){
        .itemType = MENU_CHOICE,
        .label = "Font Size",
        .choices =
            (char *[]){
                "24",
                "28",
                "32",
                "36",
                "40",
                "44",
                "48",
                "56",
                "16",
                "20",
            },
        .choiceCount = 10,
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

// void InitializeCubeMenu(CubeShape *cube) { MenuItem **items = DAV_cubeItems;
// }

void InitializeThemeMenu(int colorID, Color *color) {
  MenuItem *item = DAV_themeItems[colorID];
  item->data = color;
}

void InitializeMenus() {
  InitializeThemeMenu(THEME_BACKGROUND_COLOR, &DAV_theme->backgroundColor);
  InitializeThemeMenu(THEME_PANEL_COLOR, &DAV_theme->panelColor);
  InitializeThemeMenu(THEME_TITLE_COLOR, &DAV_theme->titleColor);
  InitializeThemeMenu(THEME_LABEL_COLOR, &DAV_theme->labelColor);
  InitializeThemeMenu(THEME_VALUE_COLOR, &DAV_theme->valueColor);
  InitializeThemeMenu(THEME_LABEL_HOVER, &DAV_theme->labelHover);
  InitializeThemeMenu(THEME_VALUE_HOVER, &DAV_theme->valueHover);
  InitializeThemeMenu(THEME_LABEL_ACTIVE, &DAV_theme->labelActive);
  InitializeThemeMenu(THEME_VALUE_ACTIVE, &DAV_theme->valueActive);
  InitializeThemeMenu(THEME_COLOR_DIM, &DAV_theme->colorDim);
  InitializeThemeMenu(THEME_COLOR_HOVER, &DAV_theme->colorHover);
}
