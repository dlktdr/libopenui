/*
 * Copyright (C) OpenTX
 *
 * Source:
 *  https://github.com/opentx/libopenui
 *
 * This file is a part of libopenui library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#include "textedit.h"
#include "font.h"

#include "widgets/field_edit.h"

#if !defined(STR_EDIT)
#define STR_EDIT "Edit"
#endif

#if !defined(STR_CLEAR)
#define STR_CLEAR "Clear"
#endif

#include "keyboard_text.h"

#if defined(HARDWARE_KEYS)
#include "menu.h"
#endif

#if defined(CLIPBOARD)
#include "clipboard.h"
#endif

TextEdit::TextEdit(Window *parent, const rect_t &rect, char *value,
                   uint8_t length, LcdFlags windowFlags,
                   const char *_extra_chars) :
    FormField(parent, rect, windowFlags, 0,
              field_edit_create),
    value(value),
    length(length)
{
  extra_chars = (_extra_chars) ? _extra_chars : extra_chars_default;

  // properties
  lv_obj_set_scrollbar_mode(lvobj, LV_SCROLLBAR_MODE_OFF);
  lv_textarea_set_password_mode(lvobj, false);
  lv_textarea_set_one_line(lvobj, true);

  lv_textarea_set_placeholder_text(lvobj, "---");
  lv_textarea_set_max_length(lvobj, length);

  if (width() == 0) {
    lv_obj_set_width(lvobj, LV_DPI_DEF);
  }

  update();
}

void TextEdit::update()
{
  // value may not be null-terminated
  std::string txt(value, length);
  lv_textarea_set_text(lvobj, txt.c_str());  
}

void TextEdit::setCursorPos(int cursorPos)
{
  this->cursorPos = cursorPos;
  if (lvobj != nullptr)
    lv_textarea_set_cursor_pos(lvobj, cursorPos);
}

void TextEdit::trim()
{
  for (int i = length - 1; i >= 0; i--) {
    if (value[i] == ' ' || value[i] == '\0')
      value[i] = '\0';
    else
      break;
  }
}

void TextEdit::onEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (IS_VIRTUAL_KEY_EVENT(event)) {
    uint8_t c = event & 0xFF;
    if (c == LV_KEY_ENTER) {
      changeEnd();
    }
  }

// #if defined(HARDWARE_KEYS)
//   if (editMode) {
//     int c = value[cursorPos];
//     int v = c;

//     switch (event) {
//       case EVT_ROTARY_RIGHT:
//         for (int i = 0; i < ROTARY_ENCODER_SPEED(); i++) {
//           v = getNextChar(v);
//         }
//         break;

//       case EVT_ROTARY_LEFT:
//         for (int i = 0; i < ROTARY_ENCODER_SPEED(); i++) {
//           v = getPreviousChar(v);
//         }
//         break;

//       case EVT_KEY_BREAK(KEY_LEFT):
//         if (cursorPos > 0) {
//           setCursorPos(cursorPos - 1);
//         }
//         break;

//       case EVT_KEY_BREAK(KEY_RIGHT):
//         if (cursorPos < length - 1 && value[cursorPos + 1] != '\0') {
//           setCursorPos(cursorPos + 1);
//         }
//         break;

//       case EVT_KEY_BREAK(KEY_ENTER):
//         if (cursorPos < length - 1) {
//           if (value[cursorPos] == '\0') {
//             value[cursorPos] = ' ';
//             changed = true;
//           }
//           cursorPos++;
//           if (value[cursorPos] == '\0') {
//             value[cursorPos] = ' ';
//             changed = true;
//           }
//           update();
//           setCursorPos(cursorPos);
//         }
//         else {
//           changeEnd();
//           FormField::onEvent(event);
//         }
//         break;

//       case EVT_KEY_LONG(KEY_ENTER):
//       {
//         killEvents(event);
//         auto menu = new Menu(this);
//         menu->setTitle(STR_EDIT);
// #if defined(CLIPBOARD)
//         menu->addLine(STR_COPY, [=] {
//           clipboard.write((uint8_t *)value, length, Clipboard::CONTENT_TEXT);
//         });
//         if (clipboard.contentType == Clipboard::CONTENT_TEXT) {
//           menu->addLine(STR_PASTE, [=] {
//             clipboard.read((uint8_t *)value, length);
//             changeEnd(true);
//           });
//         }
// #endif
//         menu->addLine(STR_CLEAR, [=] {
//           memset(value, 0, length);
//           changeEnd(true);
//         });
//         break;
//       }

//       case EVT_KEY_BREAK(KEY_UP):
//         v = toggleCase(v);
//         break;

//       case EVT_KEY_LONG(KEY_LEFT):
//       case EVT_KEY_LONG(KEY_RIGHT):
//         v = toggleCase(v);
//         if (event == EVT_KEY_LONG(KEY_LEFT)) {
//           killEvents(KEY_LEFT);
//         }
//         break;

//       case EVT_KEY_BREAK(KEY_PGDN):
//         if (cursorPos < length) {
//           memmove(&value[cursorPos], &value[cursorPos + 1], length - cursorPos - 1);
//           value[length - 1] = '\0';
//           changed = true;
//           if (cursorPos > 0 && value[cursorPos] == '\0') {
//             cursorPos = cursorPos - 1;
//           }
//           update();
//           setCursorPos(cursorPos);
//         }
//         break;
//     }

//     if (c != v) {
//       // TRACE("value[%d] = %d", cursorPos, v);
//       value[cursorPos] = v;
//       update();
//       setCursorPos(cursorPos);
//       changed = true;
//     }
//   }
//   else {
//     FormField::onEvent(event);
//     setCursorPos(0);
//   }
// #endif
}

void TextEdit::onClicked()
{
  cursorPos = lv_textarea_get_cursor_pos(lvobj);
  TextKeyboard::show(this);
}

void TextEdit::onFocusLost()
{
  changeEnd();
  FormField::onFocusLost();
}
