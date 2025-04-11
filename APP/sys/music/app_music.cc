//
// Created by fish on 2025/4/4.
//

#include "app_music.h"
#include "app_music_res.h"
#include "bsp_buzzer.h"

#include "sys_task.h"

bool playing = false;

app_music_note_t *playing_note_ptr = nullptr;
app_music_note_t *playing_note_end_ptr = nullptr;

OS::Task sys_music;
static void task(void *args) {
    if(!playing or playing_note_ptr == nullptr or playing_note_end_ptr == nullptr) {
        playing = false;
        bsp_buzzer_quiet();
        sys_music.Delete();
        return;
    }

    auto &p = playing_note_ptr;

    while(playing_note_ptr < playing_note_end_ptr) {
        if(p->freq == 0) {
            bsp_buzzer_quiet();
        } else {
            bsp_buzzer_alarm(p->freq, p->blank);
        }
        OS::Task::SleepMilliseconds(p->duration);
        playing_note_ptr ++;
    }

    playing = false;
    bsp_buzzer_quiet();
    sys_music.Delete();
}

void app_sys_music_play(app_sys_music_e e) {
#define SET(x) playing_note_ptr = x, playing_note_end_ptr = x + sizeof(x) / sizeof(app_music_note_t)
    switch(e) {
    case E_MUSIC_BOOT:
        SET(app_music_notes_boot);
        break;
    case E_MUSIC_YOU:
        SET(app_music_notes_you);
        break;
    default:
        return;
    }
#undef SET
    if(!playing) {
        playing = true;
        sys_music.Create(task, static_cast<void *>(nullptr), "music", 128, OS::Task::HIGH);
    }
}

void app_sys_music_stop() {
    playing = false;
    bsp_buzzer_quiet();
    sys_music.Delete();
}
