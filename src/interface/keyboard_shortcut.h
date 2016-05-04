#ifndef _INTERFACE_KEYBOARD_SHORTCUT_H_
#define _INTERFACE_KEYBOARD_SHORTCUT_H_

#define SHORTCUT_UNDEFINED 0xFFFF

void keyboard_shortcut_set(int key);
void keyboard_shortcut_handle(int key);
void keyboard_shortcut_handle_command(int shortcutIndex);
void keyboard_shortcut_format_string(char *buffer, uint16 shortcutKey);

#endif
