/* -*- mode: c++ -*-
 * Leidokos-Python -- Wraps Kaleidoscope modules' c++
 *    code to be available in Python programs.
 * Copyright (C) 2017 noseglasses <shinynoseglasses@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Leidokos-Python.h"
#include "KPV_Exception.hpp"

#include "key_defs.h"

// Kaleidoscope defines modifier functions that causes problems due to 
// missing casts
//
#ifdef KEY_FLAGS
#undef KEY_FLAGS
constexpr uint8_t KEY_FLAGS = B00000000;
#endif

#ifdef CTRL_HELD
#undef CTRL_HELD
constexpr uint8_t CTRL_HELD = B00000001;
#endif

#ifdef LALT_HELD
#undef LALT_HELD
constexpr uint8_t LALT_HELD = B00000010;
#endif

#ifdef RALT_HELD
#undef RALT_HELD
constexpr uint8_t RALT_HELD = B00000100;
#endif

#ifdef SHIFT_HELD
#undef SHIFT_HELD
constexpr uint8_t SHIFT_HELD = B00001000;
#endif

#ifdef GUI_HELD
#undef GUI_HELD
constexpr uint8_t GUI_HELD = B00010000;
#endif

#ifdef SYNTHETIC
#undef SYNTHETIC
constexpr uint8_t SYNTHETIC = B01000000;
#endif

#ifdef RESERVED
#undef RESERVED
constexpr uint8_t RESERVED = B10000000;
#endif

#ifdef LCTRL
#undef LCTRL
constexpr Key LCTRL(const Key &k) { 
   return (Key) { k.keyCode, static_cast<uint8_t>(k.flags | CTRL_HELD) };
}
#endif

#ifdef LALT
#undef LALT
constexpr Key LALT(const Key &k) { 
   return (Key) { k.keyCode, static_cast<uint8_t>(k.flags | LALT_HELD) };
}
#endif

#ifdef RALT
#undef RALT
constexpr Key RALT(const Key &k) { 
   return (Key) { k.keyCode, static_cast<uint8_t>(k.flags | RALT_HELD) };
}
#endif

#ifdef LSHIFT
#undef LSHIFT
constexpr Key LSHIFT(const Key &k) { 
   return (Key) { k.keyCode, static_cast<uint8_t>(k.flags | SHIFT_HELD) };
}
#endif

#ifdef LGUI
#undef LGUI
constexpr Key LGUI(const Key &k) { 
   return (Key) { k.keyCode, static_cast<uint8_t>(k.flags | GUI_HELD) };
}
#endif

#define FOR_ALL_HID_MODIFIERS(FUNC) \
FUNC(LEFT_CONTROL) \
FUNC(LEFT_SHIFT) \
FUNC(LEFT_ALT) \
FUNC(LEFT_GUI) \
FUNC(RIGHT_CONTROL) \
FUNC(RIGHT_SHIFT) \
FUNC(RIGHT_ALT) \
FUNC(RIGHT_GUI)

#define FOR_ALL_HELD_MODIFIERS(FUNC) \
   FUNC(KEY_FLAGS) \
   FUNC(CTRL_HELD) \
   FUNC(LALT_HELD) \
   FUNC(RALT_HELD) \
   FUNC(SHIFT_HELD) \
   FUNC(GUI_HELD) \
   FUNC(SYNTHETIC) \
   FUNC(RESERVED)

#define FOR_ALL_MODIFIERS(FUNC) \
   FUNC(LCTRL) \
   FUNC(LALT) \
   FUNC(RALT) \
   FUNC(LSHIFT) \
   FUNC(LGUI)
   
#define FOR_ALL_KEY_TYPE_FLAGS(FUNC) \
   FUNC(IS_SYSCTL) \
   FUNC(IS_CONSUMER) \
   FUNC(SWITCH_TO_KEYMAP) \
   FUNC(IS_INTERNAL) \
   
#define FOR_ALL_HID_TYPES(FUNC) \
   FUNC(HID_TYPE_LC) \
   FUNC(HID_TYPE_NARY) \
   FUNC(HID_TYPE_OOC) \
   FUNC(HID_TYPE_OSC) \
   FUNC(HID_TYPE_RTC) \
   FUNC(HID_TYPE_SEL)
   
namespace leidokos {
namespace python {
      
#define DEFINE_MODIFIER_FUNCTIONS(MOD) \
   static Key_ m_##MOD(const Key_ &key) { return MOD(key); }
   
FOR_ALL_MODIFIERS(DEFINE_MODIFIER_FUNCTIONS)

#define DEFINE_HELD_MODIFIER_FUNCTIONS(MOD) \
   static uint8_t hm_##MOD() { return MOD; }
   
FOR_ALL_HELD_MODIFIERS(DEFINE_HELD_MODIFIER_FUNCTIONS)

#define DEFINE_KEY_TYPE_FLAGS(FLAG) \
   static uint8_t flag_##FLAG() { return FLAG; }
   
FOR_ALL_KEY_TYPE_FLAGS(DEFINE_KEY_TYPE_FLAGS)

#define DEFINE_HID_TYPE_FLAG(FLAG) \
   static uint8_t flag_##FLAG() { return FLAG; }
   
FOR_ALL_HID_TYPES(DEFINE_HID_TYPE_FLAG)

#define FOR_ALL_SPECIAL_KEYS(FUNC) \
   FUNC(NoKey) \
   FUNC(skip) \
   FUNC(Transparent)
   
#define DEFINE_KEY_FUNCTIONS(KEY) \
   static Key_ k_##KEY() { return Key_##KEY; }

FOR_ALL_SPECIAL_KEYS(DEFINE_KEY_FUNCTIONS)

#define FOR_ALL_LIGHT_KEYS(FUNC) \
   FUNC(BacklightDown) \
   FUNC(BacklightUp) \
   FUNC(RFN2) \
   FUNC(LFN2)
   
FOR_ALL_LIGHT_KEYS(DEFINE_KEY_FUNCTIONS)

static const char *modifierKeycodeToName(uint8_t keyCode) {
   
   switch(keyCode) {
      
#define DEFINE_MOD_CASE(MOD) \
      case HID_KEYBOARD_##MOD: \
         return #MOD; \
         break;
      
      FOR_ALL_HID_MODIFIERS(DEFINE_MOD_CASE)
   }
   
   KP_EXCEPTION("modifierKeycodeToName: Unknown modifier key " << unsigned(keyCode))

   return "";
}


static const char *modifierKeyToName(const Key &key) {
   return modifierKeycodeToName(key.keyCode);
}

static boost::shared_ptr<Key> makeKeyRaw(uint16_t raw) {
   boost::shared_ptr<Key> tmp( new Key );
   tmp->raw = raw;
   return tmp;
}

static boost::shared_ptr<Key> makeKey_KeyCodeAndFlags(uint8_t keyCode, uint8_t flags) {
   return boost::shared_ptr<Key>( new Key{ {.keyCode = keyCode, .flags = flags }} );
}

static void exportPython() {
   
   using namespace boost::python;
   
   class_<Key_>("Key",
      "Provides functionality to deal with and to represent keys.",
      boost::python::init< >()
   )
      .def("__init__", boost::python::make_constructor(&makeKey_KeyCodeAndFlags),                      
           "Initializes the key.\n\n"
           "Args:\n"
           "   keyCode (uint8_t): The key code.\n"
           "   flags (uint8_t): The flags value."
       )
      .def("__init__", boost::python::make_constructor(&makeKeyRaw),
           "Initializes the key.\n\n"
           "Args:\n"
           "   raw (uint16_t): The raw value of the key."
       )
      .def(self == uint16_t())
      .def(self == Key_())
//       .def(self = uint16_t())
      .def(self != Key_())
      .def(self >= uint16_t())
      .def(self <= uint16_t())
      .def(self > uint16_t())
      .def(self < uint16_t())
      .def(self >= Key_())
      .def(self <= Key_())
      .def(self > Key_())
      .def(self < Key_())
      
      .def_readwrite("keyCode", &Key_::keyCode,
         "The key code"
       )
      
      .def("getKeyCode", make_getter(&Key_::keyCode),
         "Retreives the key code.\n\n"
         "Returns:\n"
         "   uint8_t: The key code."
      )
      .def("setKeyCode", make_setter(&Key_::keyCode),
         "Sets the key code.\n\n"
         "Args:\n"
         "   keyCode (uint8_t): The key code."
      )
      
      .def_readwrite("flags", &Key_::flags,
         "The flags value"
       )
      .def("getFlags", make_getter(&Key_::flags),
         "Retreives the flags.\n\n"
         "Returns:\n"
         "   uint8_t: The flags."
      )
      .def("setFlags", make_setter(&Key_::flags),
         "Sets the flags.\n\n"
         "Args:\n"
         "   flags (uint8_t): The flags."
      )
      
      .def_readwrite("raw", &Key_::raw,
         "The raw value"
       )
      .def("getRaw", make_getter(&Key_::raw),
         "Retreives the raw data.\n\n"
         "Returns:\n"
         "   uint16_t: The raw  data."
      )
      .def("setRaw", make_setter(&Key_::raw),
         "Sets the raw data.\n\n"
         "Args:\n"
         "   raw (uint16_t): The raw data."
      )
       ;
     ;
         
   #define REGISTER_MODIFIER_FUNCTIONS(MOD) \
      boost::python::def("add" #MOD, &leidokos::python::m_##MOD, \
         "Adds modifier \"" #MOD "\" to a key.\n\n" \
         "Returns:\n" \
         "   Key: The key with modifier \"" #MOD "\" enabled.");
      
   FOR_ALL_MODIFIERS(REGISTER_MODIFIER_FUNCTIONS)
      
   #define REGISTER_HELD_MODIFIER_FUNCTIONS(MOD) \
      boost::python::def(#MOD, &leidokos::python::hm_##MOD, \
         "Returns the modifier identifier \"" #MOD "\".\n\n" \
         "Returns:\n" \
         "   int: The key modifier identifier \"" #MOD "\"." \
      );
      
   FOR_ALL_HELD_MODIFIERS(REGISTER_HELD_MODIFIER_FUNCTIONS)
      
   #define REGISTER_KEY_TYPE_FLAGS(FLAG) \
      boost::python::def(#FLAG, &leidokos::python::flag_##FLAG, \
         "Returns the flag \"" #FLAG "\".\n\n" \
         "Returns:\n" \
         "   int: The flag identifier \"" #FLAG "\"." \
      );
      
   FOR_ALL_KEY_TYPE_FLAGS(REGISTER_KEY_TYPE_FLAGS)
      
   #define REGISTER_HID_TYPE_FLAG(FLAG) \
      boost::python::def(#FLAG, &leidokos::python::flag_##FLAG, \
         "Returns the flag \"" #FLAG "\".\n\n" \
         "Returns:\n" \
         "   int: The flag identifier \"" #FLAG "\"." \
      );
      
   FOR_ALL_HID_TYPES(REGISTER_HID_TYPE_FLAG)
         
   #define EXPORT_KEY_FUNCTIONS(KEY) \
      boost::python::def("key"#KEY, &leidokos::python::k_##KEY, \
         "Returns the key \"" #KEY "\".\n\n" \
         "Returns:\n" \
         "   Key: The key \"" #KEY "\"." \
      );
      
   FOR_ALL_LIGHT_KEYS(EXPORT_KEY_FUNCTIONS)
      
   FOR_ALL_SPECIAL_KEYS(EXPORT_KEY_FUNCTIONS)
   
   boost::python::def("modifierKeyToName", &modifierKeyToName,
         "Maps a modifier key to its associated name.\n\n"
         "Args:\n"
         "   modifier (Key): The modifier ID to map.\n\n"
         "Returns:\n"
         "   string: The modifier name.");
   
   boost::python::def("modifierKeycodeToName", &modifierKeycodeToName,
         "Maps a modifier key code to its associated name.\n\n"
         "Args:\n"
         "   modifier (uint8_t): The modifier ID to map.\n\n"
         "Returns:\n"
         "   string: The modifier name.");
}
      
LEIDOKOS_PYTHON_EXPORT(&exportPython, nullptr)

} // namespace python
} // namespace leidokos
