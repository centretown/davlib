#include "davlib.h"
#include "raylib.h"
#include <assert.h>

extern Rectangle arrowRects[ARROW_COUNT];
bool IsTop();
void ResetArrows();

void DrawItemValue(DrawLayout *layout, Theme *theme, MenuItem *item,
                   Rectangle rect, int fontSize, Color color) {
  if (item->onDraw) {
    item->onDraw(item, rect, fontSize, color);
    return;
  }

  switch (item->itemType) {
  case MENU_SUB: {
    DrawText(item->label, rect.x, rect.y, fontSize, color);
    break;
  }
  case MENU_CHOICE: {
    const char *choice = item->choices[item->choiceCurrent];
    if (item->choices) {
      DrawText(choice, rect.x, rect.y, fontSize, color);
    }
    break;
  }
  case MENU_FLOAT:
    DrawText(TextFormat("%0.2f", item->fvalue), rect.x, rect.y, fontSize,
             color);
    break;
  case MENU_INT:
    DrawText(TextFormat("%d", item->ivalue), rect.x, rect.y, fontSize, color);
    break;
  case MENU_DISPLAY:
    if (item->onDisplay) {
      item->onDisplay( // item, item->rect);
          item, theme,
          (Rectangle){
              .x = item->rect.x,
              .y = item->rect.y,
              .width = layout->panel.width - 2 * layout->padding,
              .height = item->rect.height,
          });
    }
    break;
  }
}

void DrawPanelHeading(DrawLayout *layout, Theme *theme, Vector2 point) {
  if (!IsTop()) {
    bool isHovered = PointInRect(point, arrowRects[ARROW_BACK]);
    DrawTextureEx(theme->outArrow,
                  (Vector2){arrowRects[ARROW_BACK].x, arrowRects[ARROW_BACK].y},
                  0.0f, layout->scale,
                  (isHovered) ? theme->colorHover : theme->colorDim);
  }

  float fontSize = theme->fontSize;
  DrawText(layout->menu->title, layout->titleX, layout->titleY, fontSize,
           theme->titleColor);
  int y = layout->titleY + (float)(theme->padding) + fontSize;
  DrawLine(layout->baseX, y, layout->baseX + layout->panel.width, y,
           theme->titleColor);
}

void DrawArrows(DrawLayout *layout, MenuItem *item, Theme *theme,
                Vector2 point) {
  if (item->itemType == MENU_DISPLAY) {
    return;
  }
  Color hover = theme->colorHover;
  Color dim = theme->colorDim;
  bool leftHovered = PointInRect(point, arrowRects[ARROW_LEFT]);
  bool rightHovered = PointInRect(point, arrowRects[ARROW_RIGHT]);
  float scale = layout->scale;
  if (item->itemType == MENU_SUB) {
    DrawTextureEx(theme->rightArrow,
                  (Vector2){arrowRects[ARROW_LEFT].x, arrowRects[ARROW_LEFT].y},
                  0.0f, scale, (leftHovered) ? hover : dim);
    DrawTextureEx(
        theme->leftArrow,
        (Vector2){arrowRects[ARROW_RIGHT].x, arrowRects[ARROW_RIGHT].y}, 0.0f,
        scale, (rightHovered) ? hover : dim);
  } else {
    DrawTextureEx(theme->leftArrow,
                  (Vector2){arrowRects[ARROW_LEFT].x, arrowRects[ARROW_LEFT].y},
                  0.0f, scale, (leftHovered) ? hover : dim);
    DrawTextureEx(
        theme->rightArrow,
        (Vector2){arrowRects[ARROW_RIGHT].x, arrowRects[ARROW_RIGHT].y}, 0.0f,
        scale, (rightHovered) ? hover : dim);
  }
}

static float roundnessFactor = 0.075f;
static int roundnessSegments = 8;

void DrawMenu(Theme *theme, Vector2 position, Vector2 point) {
  assert(theme);
  DrawLayout layout;
  LayoutMenu(&layout, theme, position, point);
  Menu *menu = layout.menu;
  float fontSize = theme->fontSize;

  DrawRectangleRounded(layout.panel, layout.roundness * roundnessFactor,
                       roundnessSegments, theme->panelColor);
  DrawRectangleRoundedLines(layout.panel, layout.roundness * roundnessFactor,
                            roundnessSegments, theme->titleColor);
  DrawPanelHeading(&layout, theme, point);

  for (int itemIndex = 0; itemIndex < menu->itemCount; itemIndex++) {
    MenuItem *item = layout.menu->items[itemIndex];

    DrawText(item->label, item->rect.x, item->rect.y, fontSize,
             (itemIndex == menu->current)     ? theme->labelActive
             : (itemIndex == menu->hoverItem) ? theme->labelHover
                                              : theme->labelColor);

    Color color = theme->valueColor;
    if (itemIndex == menu->current) {
      DrawArrows(&layout, item, theme, point);
      color = theme->valueActive;
    } else if (itemIndex == menu->hoverItem) {
      color = theme->valueHover;
    }

    DrawItemValue(&layout, theme, item,
                  (Rectangle){.x = item->rect.x + layout.columnWidth,
                              .y = item->rect.y,
                              .width = layout.columnWidth,
                              .height = fontSize},
                  fontSize, color);
  }
}

void LayoutMenu(DrawLayout *layout, Theme *theme, Vector2 position,
                Vector2 point) {
  Menu *menu = CurrentMenu();
  // ResetArrows();

  float fontSize = theme->fontSize;
  layout->menu = menu;
  layout->scale = fontSize / 40.0f;
  layout->padding = theme->padding;
  layout->lineHeight = fontSize + fontSize / 2;

  layout->baseX = position.x;
  layout->baseY = position.y;

  Rectangle panel =
      (Rectangle){.x = position.x,
                  .y = position.y,
                  .width = (2 * theme->valueColumn * theme->fontSize) +
                           (2 * theme->padding) + (2 * theme->fontSize),
                  .height = (layout->menu->itemCount + 1) * layout->lineHeight +
                            3 * theme->padding};

  // a little trickery to get a consistent roundness see DrawRectangleRounded
  // in raylib/rshapes.c
  layout->roundness = (panel.width > panel.height)
                          ? (panel.width / panel.height)
                          : (panel.height / panel.width);
  layout->panel = panel;
  arrowRects[ARROW_BACK] = (Rectangle){.x = layout->baseX + layout->padding,
                                       .y = layout->baseY + layout->padding,
                                       .width = theme->outArrow.width,
                                       .height = theme->outArrow.height};

  float titleLength = (float)MeasureText(menu->title, fontSize);
  layout->titleX = layout->panel.width / 2 - titleLength / 2;
  layout->titleY = layout->baseY + theme->padding;
  layout->lineY = layout->titleY + (float)(theme->padding) + fontSize;

  // float row = layout->lineY; // + layout->lineHeight;
  float row = layout->baseY + layout->lineHeight + theme->padding + fontSize;
  float columnWidth = theme->valueColumn * fontSize;
  layout->columnWidth = columnWidth;

  for (int itemIndex = 0; itemIndex < menu->itemCount;
       itemIndex++, row += layout->lineHeight) {

    MenuItem *item = menu->items[itemIndex];
    item->rect.x = layout->baseX + fontSize;
    item->rect.y = row;
    item->rect.height = fontSize;
    item->rect.width = 2.0f * columnWidth;
  }

  MenuItem *item = menu->items[menu->current];
  float arrowWidth = theme->leftArrow.width * layout->scale;
  float arrowHeight = theme->leftArrow.height * layout->scale;
  float x = item->rect.x + columnWidth;

  layout->input = (Rectangle){
      .x = x,
      .y = item->rect.y,
      .width = columnWidth,
      .height = fontSize,
  };

  float y = item->rect.y - (arrowHeight - fontSize) / 2;
  arrowRects[ARROW_LEFT] = (Rectangle){
      .x = x - arrowWidth - theme->padding,
      .y = y,
      .width = arrowWidth,
      .height = arrowHeight,
  };
  arrowRects[ARROW_RIGHT] = (Rectangle){
      .x = x + columnWidth + theme->padding,
      .y = y,
      .width = arrowWidth,
      .height = arrowHeight,
  };
}
