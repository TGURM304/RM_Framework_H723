//
// Created by fish on 2025/4/4.
//

#pragma once

#include "app_music_def.h"

// 定义低音
#define A1  131
#define A2  147
#define A3  165
#define A4  175
#define A5  196
#define A6  220
#define A7  247

// 定义中音
#define B1  262
#define B2  296
#define B3  330
#define B4  349
#define B5  392
#define B6  440
#define B7  494

// 定义高音
#define C1  523
#define C2  587
#define C3  659
#define C4  698
#define C4p 741
#define C5  784
#define C6  880
#define C7  988

// 定义高二度
#define D1  1047
#define D2  1175
#define D3  1319
#define D4  1397
#define D5  1568
#define D6  1760
#define D7  1976

// 定义节拍
#define OneBeat   200
#define HalfBeat  100

inline app_music_note_t app_music_notes_boot[] = {
    // {D7, 0.5, 125}, {0, 0, 50}, {D7, 0.5, 125},
    {0, 0, 2},
    {B1, 0.1, HalfBeat * 2},
    {0, 0, 3},
    {C3, 0.1, HalfBeat * 2},
    {0, 0, 3},
    {D5, 0.1, HalfBeat * 2},
    {0, 0, 15},
    {D1, 0.05, HalfBeat * 3},
};