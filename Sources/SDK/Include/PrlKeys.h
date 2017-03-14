/*
 * PrlKeys.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_API_KEYS_H__
#define __PARALLELS_API_KEYS_H__


/**
 * PS/2 scan codes symbolical names for the SDK purposes.
 * Note: scancode usage is deprecated, use PRL_KEY_* declared below
 */
#define PRL_SCAN_CODE_ESC                        {1}
#define PRL_SCAN_CODE_1                          {2}
#define PRL_SCAN_CODE_2                          {3}
#define PRL_SCAN_CODE_3                          {4}
#define PRL_SCAN_CODE_4                          {5}
#define PRL_SCAN_CODE_5                          {6}
#define PRL_SCAN_CODE_6                          {7}
#define PRL_SCAN_CODE_7                          {8}
#define PRL_SCAN_CODE_8                          {9}
#define PRL_SCAN_CODE_9                          {10}
#define PRL_SCAN_CODE_0                          {11}
#define PRL_SCAN_CODE_MINUS                      {12}
#define PRL_SCAN_CODE_PLUS                       {13}
#define PRL_SCAN_CODE_BACKSPACE                  {14}
#define PRL_SCAN_CODE_TAB                        {15}
#define PRL_SCAN_CODE_Q                          {16}
#define PRL_SCAN_CODE_W                          {17}
#define PRL_SCAN_CODE_E                          {18}
#define PRL_SCAN_CODE_R                          {19}
#define PRL_SCAN_CODE_T                          {20}
#define PRL_SCAN_CODE_Y                          {21}
#define PRL_SCAN_CODE_U                          {22}
#define PRL_SCAN_CODE_I                          {23}
#define PRL_SCAN_CODE_O                          {24}
#define PRL_SCAN_CODE_P                          {25}
#define PRL_SCAN_CODE_CBRACE_LEFT                {26}
#define PRL_SCAN_CODE_CBRACE_RIGHT               {27}
#define PRL_SCAN_CODE_ENTER                      {28}
#define PRL_SCAN_CODE_CTRL_LEFT                  {29}
#define PRL_SCAN_CODE_A                          {30}
#define PRL_SCAN_CODE_S                          {31}
#define PRL_SCAN_CODE_D                          {32}
#define PRL_SCAN_CODE_F                          {33}
#define PRL_SCAN_CODE_G                          {34}
#define PRL_SCAN_CODE_H                          {35}
#define PRL_SCAN_CODE_J                          {36}
#define PRL_SCAN_CODE_K                          {37}
#define PRL_SCAN_CODE_L                          {38}
#define PRL_SCAN_CODE_COLON                      {39}
#define PRL_SCAN_CODE_QUOTE                      {40}
#define PRL_SCAN_CODE_TILDA                      {41}
#define PRL_SCAN_CODE_SHIFT_LEFT                 {42}
#define PRL_SCAN_CODE_BACKSLASH                  {43}
#define PRL_SCAN_CODE_Z                          {44}
#define PRL_SCAN_CODE_X                          {45}
#define PRL_SCAN_CODE_C                          {46}
#define PRL_SCAN_CODE_V                          {47}
#define PRL_SCAN_CODE_B                          {48}
#define PRL_SCAN_CODE_N                          {49}
#define PRL_SCAN_CODE_M                          {50}
#define PRL_SCAN_CODE_LESS                       {51}
#define PRL_SCAN_CODE_GREATER                    {52}
#define PRL_SCAN_CODE_SLASH                      {53}
#define PRL_SCAN_CODE_SHIFT_RIGHT                {54}
#define PRL_SCAN_CODE_PAD_STAR                   {55}
#define PRL_SCAN_CODE_ALT_LEFT                   {56}
#define PRL_SCAN_CODE_SPACE                      {57}
#define PRL_SCAN_CODE_CAPS_LOCK                  {58}
#define PRL_SCAN_CODE_F1                         {59}
#define PRL_SCAN_CODE_F2                         {60}
#define PRL_SCAN_CODE_F3                         {61}
#define PRL_SCAN_CODE_F4                         {62}
#define PRL_SCAN_CODE_F5                         {63}
#define PRL_SCAN_CODE_F6                         {64}
#define PRL_SCAN_CODE_F7                         {65}
#define PRL_SCAN_CODE_F8                         {66}
#define PRL_SCAN_CODE_F9                         {67}
#define PRL_SCAN_CODE_F10                        {68}
#define PRL_SCAN_CODE_NUM_LOCK                   {69}
#define PRL_SCAN_CODE_SCROLL_LOCK                {70}
#define PRL_SCAN_CODE_PAD_7                      {71}
#define PRL_SCAN_CODE_PAD_8                      {72}
#define PRL_SCAN_CODE_PAD_9                      {73}
#define PRL_SCAN_CODE_PAD_MINUS                  {74}
#define PRL_SCAN_CODE_PAD_4                      {75}
#define PRL_SCAN_CODE_PAD_5                      {76}
#define PRL_SCAN_CODE_PAD_6                      {77}
#define PRL_SCAN_CODE_PAD_PLUS                   {78}
#define PRL_SCAN_CODE_PAD_1                      {79}
#define PRL_SCAN_CODE_PAD_2                      {80}
#define PRL_SCAN_CODE_PAD_3                      {81}
#define PRL_SCAN_CODE_PAD_0                      {82}
#define PRL_SCAN_CODE_PAD_DEL                    {83}
#define PRL_SCAN_CODE_PRINT_SCREEN               {224,42,224,55}
#define PRL_SCAN_CODE_EUROPE_1                   {43}
#define PRL_SCAN_CODE_LESS_GREATER               {86}
#define PRL_SCAN_CODE_F11                        {87}
#define PRL_SCAN_CODE_F12                        {88}
#define PRL_SCAN_CODE_HOME                       {224,71}
#define PRL_SCAN_CODE_UP                         {224,72}
#define PRL_SCAN_CODE_PAGE_UP                    {224,73}
#define PRL_SCAN_CODE_LEFT                       {224,75}
#define PRL_SCAN_CODE_RIGHT                      {224,77}
#define PRL_SCAN_CODE_END                        {224,79}
#define PRL_SCAN_CODE_DOWN                       {224,80}
#define PRL_SCAN_CODE_PAGE_DOWN                  {224,81}
#define PRL_SCAN_CODE_INSERT                     {224,82}
#define PRL_SCAN_CODE_DELETE                     {224,83}
#define PRL_SCAN_CODE_PAD_ENTER                  {224,28}
#define PRL_SCAN_CODE_CTRL_RIGHT                 {224,29}
#define PRL_SCAN_CODE_PAUSE_BREAK                {225,29,69,225,157,197}
#define PRL_SCAN_CODE_PAD_SLASH                  {224,53}
#define PRL_SCAN_CODE_ALT_RIGHT                  {224,56}
#define PRL_SCAN_CODE_CMD_LEFT                   {224,91}
#define PRL_SCAN_CODE_CMD_RIGHT                  {224,92}
#define PRL_SCAN_CODE_MENU                       {224,93}
#define PRL_SCAN_CODE_MEDIA_NEXT_TRACK           {224,25}
#define PRL_SCAN_CODE_MEDIA_PREV_TRACK           {224,16}
#define PRL_SCAN_CODE_MEDIA_STOP                 {224,36}
#define PRL_SCAN_CODE_MEDIA_PLAY_PAUSE           {224,34}
#define PRL_SCAN_CODE_MUTE                       {224,32}
#define PRL_SCAN_CODE_VOLUME_UP                  {224,48}
#define PRL_SCAN_CODE_VOLUME_DOWN                {224,46}
#define PRL_SCAN_CODE_MEDIA_SELECT               {224,109}
#define PRL_SCAN_CODE_APP_MAIL                   {224,108}
#define PRL_SCAN_CODE_APP_CALCULATOR             {224,33}
#define PRL_SCAN_CODE_APP_MY_COMPUTER            {224,107}
#define PRL_SCAN_CODE_WWW_SEARCH                 {224,101}
#define PRL_SCAN_CODE_WWW_HOME                   {224,50}
#define PRL_SCAN_CODE_WWW_BACK                   {224,106}
#define PRL_SCAN_CODE_WWW_FORWARD                {224,105}
#define PRL_SCAN_CODE_WWW_STOP                   {224,104}
#define PRL_SCAN_CODE_WWW_REFRESH                {224,103}
#define PRL_SCAN_CODE_WWW_FAVORITES              {224,102}
#define PRL_SCAN_CODE_EJECT                      {224,99}
#define PRL_SCAN_CODE_SYSTEM_POWER               {224,94}
#define PRL_SCAN_CODE_SYSTEM_SLEEP               {224,95}
#define PRL_SCAN_CODE_SYSTEM_WAKE                {224,99}
#define PRL_SCAN_CODE_BRAZILIAN_KEYPAD           {126}
#define PRL_SCAN_CODE_RO                         {115}
#define PRL_SCAN_CODE_HIRAGANA_KATAKANA          {112}
#define PRL_SCAN_CODE_YEN                        {125}
#define PRL_SCAN_CODE_HENKAN                     {121}
#define PRL_SCAN_CODE_MUHENKAN                   {123}
#define PRL_SCAN_CODE_PC9800_KEYPAD              {92}
#define PRL_SCAN_CODE_HANGUEL                    {242}
#define PRL_SCAN_CODE_HANJA                      {241}
#define PRL_SCAN_CODE_KATAKANA                   {120}
#define PRL_SCAN_CODE_HIRAGANA                   {119}
#define PRL_SCAN_CODE_ZENKAKU_HANKAKU            {118}
#define PRL_SCAN_CODE_F13                        {100}
#define PRL_SCAN_CODE_F14                        {101}
#define PRL_SCAN_CODE_F15                        {102}
#define PRL_SCAN_CODE_F16                        {103}
#define PRL_SCAN_CODE_F17                        {104}
#define PRL_SCAN_CODE_F18                        {105}
#define PRL_SCAN_CODE_F19                        {106}
#define PRL_SCAN_CODE_F20                        {107}
#define PRL_SCAN_CODE_F21                        {108}
#define PRL_SCAN_CODE_F22                        {109}
#define PRL_SCAN_CODE_F23                        {110}
#define PRL_SCAN_CODE_F24                        {118}
#define PRL_SCAN_CODE_PAD_EQUAL                  {89}
#define PRL_SCAN_CODE_BREAK                      {224,70,224,198}
#define PRL_SCAN_CODE_PRINT_SCREEN2              {224,55}
#define PRL_SCAN_CODE_SYSRQ                      {84}
#define PRL_SCAN_CODE_EURO                       {224,51}
#define PRL_SCAN_CODE_DOLLAR                     {224,52}


/**
 * Parallels virtual keycodes.
 */
typedef enum _PRL_KEY
{
	PRL_KEY_INVALID                          = 0,

	PRL_KEY_WILDCARD_ANY                     = 1,
	PRL_KEY_WILDCARD_KEYBOARD                = 2,
	PRL_KEY_WILDCARD_MOUSE                   = 3,
	PRL_KEY_WILDCARD_SHIFT                   = 4,
	PRL_KEY_WILDCARD_CTRL                    = 5,
	PRL_KEY_WILDCARD_ALT                     = 6,
	PRL_KEY_WILDCARD_WIN                     = 7,
	PRL_KEY_ESCAPE                           = 9,
	PRL_KEY_1                                = 10,
	PRL_KEY_2                                = 11,
	PRL_KEY_3                                = 12,
	PRL_KEY_4                                = 13,
	PRL_KEY_5                                = 14,
	PRL_KEY_6                                = 15,
	PRL_KEY_7                                = 16,
	PRL_KEY_8                                = 17,
	PRL_KEY_9                                = 18,
	PRL_KEY_0                                = 19,
	PRL_KEY_MINUS                            = 20,
	PRL_KEY_EQUAL                            = 21,
	PRL_KEY_BACKSPACE                        = 22,
	PRL_KEY_TAB                              = 23,
	PRL_KEY_Q                                = 24,
	PRL_KEY_W                                = 25,
	PRL_KEY_E                                = 26,
	PRL_KEY_R                                = 27,
	PRL_KEY_T                                = 28,
	PRL_KEY_Y                                = 29,
	PRL_KEY_U                                = 30,
	PRL_KEY_I                                = 31,
	PRL_KEY_O                                = 32,
	PRL_KEY_P                                = 33,
	PRL_KEY_LEFT_BRACKET                     = 34,
	PRL_KEY_RIGHT_BRACKET                    = 35,
	PRL_KEY_ENTER                            = 36,
	PRL_KEY_LEFT_CONTROL                     = 37,
	PRL_KEY_A                                = 38,
	PRL_KEY_S                                = 39,
	PRL_KEY_D                                = 40,
	PRL_KEY_F                                = 41,
	PRL_KEY_G                                = 42,
	PRL_KEY_H                                = 43,
	PRL_KEY_J                                = 44,
	PRL_KEY_K                                = 45,
	PRL_KEY_L                                = 46,
	PRL_KEY_SEMICOLON                        = 47,
	PRL_KEY_QUOTE                            = 48,
	PRL_KEY_TILDA                            = 49,
	PRL_KEY_LEFT_SHIFT                       = 50,
	PRL_KEY_BACKSLASH                        = 51,
	PRL_KEY_Z                                = 52,
	PRL_KEY_X                                = 53,
	PRL_KEY_C                                = 54,
	PRL_KEY_V                                = 55,
	PRL_KEY_B                                = 56,
	PRL_KEY_N                                = 57,
	PRL_KEY_M                                = 58,
	PRL_KEY_COMMA                            = 59,
	PRL_KEY_DOT                              = 60,
	PRL_KEY_SLASH                            = 61,
	PRL_KEY_RIGHT_SHIFT                      = 62,
	PRL_KEY_NP_STAR                          = 63,
	PRL_KEY_LEFT_ALT                         = 64,
	PRL_KEY_SPACE                            = 65,
	PRL_KEY_CAPS_LOCK                        = 66,
	PRL_KEY_F1                               = 67,
	PRL_KEY_F2                               = 68,
	PRL_KEY_F3                               = 69,
	PRL_KEY_F4                               = 70,
	PRL_KEY_F5                               = 71,
	PRL_KEY_F6                               = 72,
	PRL_KEY_F7                               = 73,
	PRL_KEY_F8                               = 74,
	PRL_KEY_F9                               = 75,
	PRL_KEY_F10                              = 76,
	PRL_KEY_NUM_LOCK                         = 77,
	PRL_KEY_SCROLL_LOCK                      = 78,
	PRL_KEY_NP_7                             = 79,
	PRL_KEY_NP_8                             = 80,
	PRL_KEY_NP_9                             = 81,
	PRL_KEY_NP_MINUS                         = 82,
	PRL_KEY_NP_4                             = 83,
	PRL_KEY_NP_5                             = 84,
	PRL_KEY_NP_6                             = 85,
	PRL_KEY_NP_PLUS                          = 86,
	PRL_KEY_NP_1                             = 87,
	PRL_KEY_NP_2                             = 88,
	PRL_KEY_NP_3                             = 89,
	PRL_KEY_NP_0                             = 90,
	PRL_KEY_NP_DELETE                        = 91,
	PRL_KEY_PRINT                            = 92,
	PRL_KEY_EUROPE_1                         = 93,
	PRL_KEY_EUROPE_2                         = 94,
	PRL_KEY_F11                              = 95,
	PRL_KEY_F12                              = 96,
	PRL_KEY_HOME                             = 97,
	PRL_KEY_UP                               = 98,
	PRL_KEY_PAGE_UP                          = 99,
	PRL_KEY_LEFT                             = 100,
	PRL_KEY_RIGHT                            = 102,
	PRL_KEY_END                              = 103,
	PRL_KEY_DOWN                             = 104,
	PRL_KEY_PAGE_DOWN                        = 105,
	PRL_KEY_INSERT                           = 106,
	PRL_KEY_DELETE                           = 107,
	PRL_KEY_NP_ENTER                         = 108,
	PRL_KEY_RIGHT_CONTROL                    = 109,
	PRL_KEY_PAUSE                            = 110,
	PRL_KEY_NP_SLASH                         = 112,
	PRL_KEY_RIGHT_ALT                        = 113,
	PRL_KEY_LEFT_WIN                         = 115,
	PRL_KEY_RIGHT_WIN                        = 116,
	PRL_KEY_MENU                             = 117,
	PRL_KEY_MEDIA_NEXT_TRACK                 = 118,
	PRL_KEY_MEDIA_PREV_TRACK                 = 119,
	PRL_KEY_MEDIA_STOP                       = 120,
	PRL_KEY_MEDIA_PLAY_PAUSE                 = 121,
	PRL_KEY_MUTE                             = 122,
	PRL_KEY_VOLUME_UP                        = 123,
	PRL_KEY_VOLUME_DOWN                      = 124,
	PRL_KEY_MEDIA_SELECT                     = 125,
	PRL_KEY_APP_MAIL                         = 126,
	PRL_KEY_APP_CALCULATOR                   = 127,
	PRL_KEY_APP_MY_COMPUTER                  = 128,
	PRL_KEY_WWW_SEARCH                       = 129,
	PRL_KEY_WWW_HOME                         = 130,
	PRL_KEY_WWW_BACK                         = 131,
	PRL_KEY_WWW_FORWARD                      = 132,
	PRL_KEY_WWW_STOP                         = 133,
	PRL_KEY_WWW_REFRESH                      = 134,
	PRL_KEY_WWW_FAVORITES                    = 135,
	PRL_KEY_EJECT                            = 136,
	PRL_KEY_SYSTEM_POWER                     = 137,
	PRL_KEY_SYSTEM_SLEEP                     = 138,
	PRL_KEY_SYSTEM_WAKE                      = 139,
	PRL_KEY_BRAZILIAN_KEYPAD                 = 140,
	PRL_KEY_RO                               = 141,
	PRL_KEY_HIRAGANA_KATAKANA                = 142,
	PRL_KEY_YEN                              = 143,
	PRL_KEY_HENKAN                           = 144,
	PRL_KEY_MUHENKAN                         = 145,
	PRL_KEY_PC9800_KEYPAD                    = 146,
	PRL_KEY_HANGUEL                          = 147,
	PRL_KEY_HANJA                            = 148,
	PRL_KEY_KATAKANA                         = 149,
	PRL_KEY_HIRAGANA                         = 150,
	PRL_KEY_ZENKAKU_HANKAKU                  = 151,
	PRL_KEY_F13                              = 152,
	PRL_KEY_F14                              = 153,
	PRL_KEY_F15                              = 154,
	PRL_KEY_F16                              = 155,
	PRL_KEY_F17                              = 156,
	PRL_KEY_F18                              = 157,
	PRL_KEY_F19                              = 158,
	PRL_KEY_F20                              = 159,
	PRL_KEY_F21                              = 160,
	PRL_KEY_F22                              = 161,
	PRL_KEY_F23                              = 162,
	PRL_KEY_F24                              = 163,
	PRL_KEY_NP_EQUAL                         = 164,
	PRL_KEY_BREAK                            = 165,
	PRL_KEY_PRINT_WITH_MODIFIER              = 166,
	PRL_KEY_SYSRQ                            = 167,
	PRL_KEY_FN                               = 168,
	PRL_KEY_EURO                             = 169,
	PRL_KEY_DOLLAR                           = 170,
	PRL_KEY_MON_BRIGHTNESS_DOWN              = 171,
	PRL_KEY_MON_BRIGHTNESS_UP                = 172,
	PRL_KEY_APP_EXPOSE                       = 173,
	PRL_KEY_APP_DASHBOARD                    = 174,
	PRL_KEY_KBD_BRIGHTNESS_DOWN              = 175,
	PRL_KEY_KBD_BRIGHTNESS_UP                = 176,
	PRL_KEY_MAC129                           = 177,
	PRL_KEY_LEFT_BUTTON                      = 178,
	PRL_KEY_MIDDLE_BUTTON                    = 179,
	PRL_KEY_RIGHT_BUTTON                     = 180,
	PRL_KEY_MOVE_UP_LEFT                     = 181,
	PRL_KEY_MOVE_UP                          = 182,
	PRL_KEY_MOVE_UP_RIGHT                    = 183,
	PRL_KEY_MOVE_LEFT                        = 184,
	PRL_KEY_MOVE_RIGHT                       = 185,
	PRL_KEY_MOVE_DOWN_LEFT                   = 186,
	PRL_KEY_MOVE_DOWN                        = 187,
	PRL_KEY_MOVE_DOWN_RIGHT                  = 188,
	PRL_KEY_WHEEL_UP                         = 189,
	PRL_KEY_WHEEL_DOWN                       = 190,
	PRL_KEY_WHEEL_LEFT                       = 191,
	PRL_KEY_WHEEL_RIGHT                      = 192,

	PRL_KEY_MAX                              = 193
} PRL_KEY;


#endif // __PARALLELS_API_KEYS_H__
//EOF
