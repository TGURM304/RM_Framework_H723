//
// Created by fish on 2025/4/4.
//

#pragma once

#include "app_music_def.h"

#define note_A   220
#define note_3A  110
#define note_5A  440
#define note_sA  233  //233.082
#define note_B   247  //246.942
#define note_3B  123  //123.471
#define note_5B  494  //493.883
#define note_C   262  //261.626
#define note_5C  523  //523.251
#define note_sC  277  //277.183
#define note_D   294  //293.665
#define note_sD  311  //311.127
#define note_5D  587  //587.33
#define note_3sD 156  //155.563
#define note_E   330  //329.629
#define note_3E  165  //164.814
#define note_F   349  //349.228
#define note_3F  175  //174.614
#define note_sF  370  //369.994
#define note_3sF 185  //184.997
#define note_G   392  //391.995
#define note_sG  415  //415.305
#define note_3G  196  //195.998
#define note_5sG 831  //830.609

inline app_music_note_t app_music_notes_you[] = {
#define blank 0.5
#define t 40
    // 前奏
    {note_5B, blank, 200},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5D, blank, 400},
    {note_G, blank, 200},
    {note_5C, blank, 200},
    {note_5C, blank, 200},
    {note_G, blank, 200},
    {note_5B, blank, 200},
    {note_5C, blank, 200},

    {note_5B, blank, 200},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5D, blank, 400},
    {note_G, blank, 200},
    {note_5C, blank, 200},
    {note_5C, blank, 200},
    {note_G, blank, 200},
    {note_5B, blank, 200},
    {note_5C, blank, 200},

    {note_5B, blank, 200},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5D, blank, 400},
    {note_G, blank, 200},
    {note_5C, blank, 200},
    {note_5C, blank, 200},
    {note_G, blank, 200},
    {note_5B, blank, 200},
    {note_5C, blank, 200},

    {note_5B, blank, 200},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5B, blank, 400},
    {note_G, blank, 200},
    {note_5D, blank, 400},
    {note_G, blank, 200},
    {note_5C, blank, 200},
    {note_5C, blank, 200},
    {note_G, blank, 200},
    {note_D, blank, 400},

    {note_E, blank, 1200},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 800},

    {note_5C, blank, 800},
    {note_5B, blank, 800},
    {note_E, blank, 800},
    {note_D, blank, 400},

    {note_E, blank, 1200},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5C, blank, 1600},

    {note_5B, blank, 400},
    {note_5D, blank, 800},
    {note_E, blank, 2000},

    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5B, blank, 800},

    {note_5C, blank, 800},
    {note_5B, blank, 800},
    {note_E, blank, 800},
    {note_D, blank, 400},

    {note_E, blank, 1200},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5B, blank, 800},

    {note_5C, blank, 800},
    {note_5D, blank, 2000},

    // 我一直追寻着你
    {0, blank, 800},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 400},

    // 你好像不远也不近
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 800},
    {note_C, blank, 1200},

    // 却总保持着距离
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_C, blank, 400},
    {note_D, blank, 400},

    // 我一直幻想着你
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 400},

    // 在我身边在我怀里
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5C, blank, 800},
    {note_C, blank, 1200},

    // 让我欢笑让我哭泣
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_B, blank, 400},
    {note_A, blank, 200},
    {note_3G, blank, 1000},

    // 你是我灵魂的旋律
    {0, blank, 200},
    {note_3G, blank, 200},
    {0, blank, t},
    {note_3G, blank, 200},
    {0, blank, t},
    {note_3G, blank, 200},
    {note_G, blank, 800},
    {note_E, blank, 600},
    {note_D, blank, 200},
    {note_C, blank, 400},
    {0, blank, t},
    {note_C, blank, 800},

    // 春日的细雨
    {0, blank, 200},
    {note_C, blank, 400},
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_A, blank, 1200},

    // 墓碑的雏菊
    {0, blank, 400},
    {note_A, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 400},
    {note_C, blank, 400},
    {note_D, blank, 1200},
    {0, blank, 400},

    // 我从来不会计算代价
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 1200},
    {note_E, blank, 400},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5B, blank, 800},
    {note_5C, blank, 1200},

    // 为了你可以纵身无底悬崖
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 800},
    {note_5A, blank, 800},
    {note_G, blank, 200},
    {note_5A, blank, 600},
    {note_G, blank, 200},
    {note_G, blank, 600},
    {note_G, blank, 800},
    {note_D, blank, 1600},

    // 像条狗更像一个笑话
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 1200},
    {note_E, blank, 400},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5B, blank, 800},
    {note_5C, blank, 1200},

    // 也许我很傻但我不会怕
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5B, blank, 800},
    {note_5A, blank, 1200},
    {0, blank, t},
    {note_5A, blank, 400},
    {note_G, blank, 200},
    {note_5A, blank, 600},
    {note_5C, blank, 800},
    {note_5D, blank, 1200},

    // 我愿意呀
    {0, blank, 400},
    {note_G, blank, 400},
    {note_5C, blank, 400},
    {note_5B, blank, 200},
    {note_5C, blank, 2400},

    {0, blank, 800},

    // 人们都追寻着你
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 400},

    // 都曾把你当作唯一
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 800},
    {note_C, blank, 1200},

    // 最后却无能为力
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_C, blank, 400},
    {note_D, blank, 400},

    // 人们都幻想着你
    {0, blank, 400},
    {note_E, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 400},

    // 幻想你依偎他怀里
    {0, blank, 400},
    {note_E, blank, 400},
    {0, blank, t},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5C, blank, 800},
    {note_C, blank, 1200},

    // 一朝拥有一劳永逸
    {0, blank, 400},
    {note_E, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_E, blank, 600},
    {note_D, blank, 400},
    {note_B, blank, 400},
    {note_A, blank, 200},
    {note_3G, blank, 1000},

    // 可是你不为谁守候
    {0, blank, 200},
    {note_3G, blank, 200},
    {0, blank, t},
    {note_3G, blank, 200},
    {0, blank, t},
    {note_3G, blank, 200},
    {note_G, blank, 800},
    {note_E, blank, 600},
    {note_D, blank, 200},
    {note_C, blank, 400},
    {0, blank, t},
    {note_C, blank, 800},

    // 不承诺永久
    {0, blank, 200},
    {note_C, blank, 400},
    {0, blank, t},
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_A, blank, 1200},

    // 不轻易停留
    {0, blank, 400},
    {note_A, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 400},
    {note_C, blank, 400},
    {note_D, blank, 2000},

    // 我知道只有不断出发
    {0, blank, 800},
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 1200},
    {note_E, blank, 400},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5B, blank, 800},
    {note_5C, blank, 1200},

    // 才能够紧随你纵情的步伐
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 800},
    {note_5A, blank, 1000},
    {note_G, blank, 200},
    {note_5A, blank, 400},
    {note_G, blank, 200},
    {0, blank, t},
    {note_G, blank, 600},
    {note_D, blank, 1600},

    // 就算是海角至天涯
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 800},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5D, blank, 800},
    {note_5C, blank, 1200},

    // 青丝变白发
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5C, blank, 800},
    {note_5A, blank, 1000},

    // 只等着你回答
    {note_G, blank, 200},
    {note_5A, blank, 400},
    {note_G, blank, 200},
    {note_5A, blank, 600},
    {note_5C, blank, 800},
    {note_5D, blank, 1200},

    // 我愿意呀
    {0, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_D, blank, 200},
    {note_C, blank, 2400},

    // 我从来不会计算代价
    {0, blank, 800},
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 1200},
    {note_E, blank, 400},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5B, blank, 800},
    {note_5C, blank, 1200},

    // 为了你可以纵身无底悬崖
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 800},
    {note_5A, blank, 1000},
    {note_G, blank, 200},
    {note_5A, blank, 400},
    {note_G, blank, 200},
    {note_G, blank, 600},
    {note_G, blank, 800},
    {note_D, blank, 1600},

    // 像条狗更像一个笑话
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 1200},
    {note_E, blank, 400},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5B, blank, 800},
    {note_5C, blank, 1200},

    // 也许我很傻但我不会怕
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5C, blank, 800},
    {note_5A, blank, 1200},
    {0, blank, t},
    {note_5A, blank, 400},
    {note_G, blank, 200},
    {note_5A, blank, 600},
    {note_5C, blank, 800},
    {note_5D, blank, 1200},

    // 我知道只有不断出发
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 1200},
    {note_E, blank, 400},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5B, blank, 800},
    {note_5C, blank, 1200},

    // 才能够紧随你纵情的步伐
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_G, blank, 800},
    {note_5A, blank, 1000},
    {note_G, blank, 200},
    {note_5A, blank, 400},
    {note_G, blank, 200},
    {note_G, blank, 600},
    {note_G, blank, 800},
    {note_D, blank, 1600},

    // 就算是海角至天涯
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 800},
    {0, blank, 200},
    {note_G, blank, 400},
    {note_E, blank, 200},
    {note_G, blank, 600},
    {note_5D, blank, 800},
    {note_5C, blank, 1200},

    // 青丝变白发
    {note_C, blank, 400},
    {note_D, blank, 400},
    {note_E, blank, 400},
    {note_5C, blank, 800},
    {note_5A, blank, 1000},

    // 只等着你回答
    {note_G, blank, 200},
    {note_5A, blank, 400},
    {note_G, blank, 200},
    {note_5A, blank, 600},
    {note_5C, blank, 800},
    {note_5D, blank, 1200},

    // 我愿意呀
    {0, blank, 400},
    {note_G, blank, 400},
    {note_5C, blank, 400},
    {note_5B, blank, 200},
    {note_5C, blank, 2400},

    // 尾
    {0, blank, 800},
    {note_E, blank, 800},
    {note_D, blank, 800},
    {note_C, blank, 800},
    {note_G, blank, 800},
    {note_C, blank, 800},
    {note_D, blank, 800},
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 800},
    {note_F, blank, 800},

    {note_E, blank, 800},
    {note_D, blank, 800},
    {note_C, blank, 800},
    {note_D, blank, 800},
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_E, blank, 800},
    {note_D, blank, 800},
    {note_C, blank, 800},
    {note_G, blank, 800},

    {note_E, blank, 800},
    {note_D, blank, 800},
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 800},
    {note_F, blank, 800},
    {note_E, blank, 800},
    {note_D, blank, 800},

    {note_E, blank, 800},
    {note_D, blank, 800},
    {note_E, blank, 800},
    {note_F, blank, 800},

    {note_E, blank, 800},
    {note_D, blank, 800},
    {note_C, blank, 800},
    {note_G, blank, 800},
    {note_C, blank, 800},
    {note_D, blank, 800},
    {note_E, blank, 800},
    {note_F, blank, 800},
    {note_G, blank, 800},
    {note_F, blank, 800},
    {note_E, blank, 800},
    {note_D, blank, 800}
#undef t
#undef blank
};