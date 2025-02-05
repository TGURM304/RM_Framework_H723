//
// Created by fish on 2025/1/20.
//

#include "app_terminal.h"

#include <cstring>
#include <map>
#include <string>

#include "bsp_buzzer.h"
#include "bsp_def.h"
#include "bsp_tim.h"
#include "bsp_uart.h"
#include "sys_signal.h"
#include "sys_task.h"
#include "app_conf.h"

const char about_text[] =
" ________  __            __       \r\n"
"/        |/  |          /  |      \r\n"
"$$$$$$$$/ $$/   _______ $$ |____  \r\n"
"$$ |__    /  | /       |$$      \\ \r\n"
"$$    |   $$ |/$$$$$$$/ $$$$$$$  |\r\n"
"$$$$$/    $$ |$$      \\ $$ |  $$ |\r\n"
"$$ |      $$ | $$$$$$  |$$ |  $$ |\r\n"
"$$ |      $$ |/     $$/ $$ |  $$ |\r\n"
"$$/       $$/ $$$$$$$/  $$/   $$/ \r\n"
"\r\n"
"Build: " __DATE__ " " __TIME__ " \r\n";

char tmp[1024];
std::string buf;
std::map <std::string, std::function <bool(std::vector<std::string>)>> cmd;
std::map <std::string, std::string> cmd_brief;

#define TERMINAL_CMD_MEM_SIZE 100
std::string cmd_mem[TERMINAL_CMD_MEM_SIZE];
int mem_ptr = 0, mem_cur_ptr = 0;

bool running = false, result = false, force_stop = false;
std::pair <std::function<bool(std::vector<std::string>)>, std::vector <std::string>> runtime;

void show_head() {
    bsp_uart_printf(TERMINAL_PORT, "%s%s@%s:%s%s%s$\e[00m ",
        TERMINAL_COLOR_GREEN,
        TERMINAL_USER_NAME, TERMINAL_PLATFORM_NAME,
        TERMINAL_COLOR_BLUE,
        "~",
        TERMINAL_COLOR_GREEN
    );
}

void fill_buf(const std::string &val) {
    for(int i = 0; i < buf.size(); i++) TERMINAL_SEND(KEY_LEFT, 3);
    TERMINAL_SEND(TERMINAL_CLEAR_BEHIND, 3);
    buf = val;
    TERMINAL_INFO_PRINTF("%s", buf.c_str());
}

static OS::Task terminal;

void terminal_task(void *args) {
    while(true) {
        if(!running) {
            OS::Task::Yield();
            continue;
        }
        result = runtime.first(runtime.second);
        if(force_stop) TERMINAL_INFO("\r\nStopped.\r\n"), force_stop = false;
        show_head();
        running = false;
    }
}

bool solve() {
    // bsp_uart_printf(TERMINAL_PORT, "Recv: [%d] %s\r\n", buf.size(), buf.c_str());
    if(buf.empty()) {
        show_head();
        return true;
    }
    std::vector <std::string> args;
    int p = 0;
    for(int i = 0; i < buf.length() and p < buf.length(); i++) {
        if(buf[i] == ' ') {
            args.emplace_back(buf.substr(p, i - p));
            p = i + 1;
            while(p < buf.length() and buf[p] == ' ') p ++;
        }
    }
    args.emplace_back(buf.substr(p));
    if(cmd.count(args[0])) {
        // 尽量不要阻塞执行任务
        // return cmd[args[0]](args);

        // 加入历史记录
        mem_ptr = 0;
        if(cmd_mem[(mem_cur_ptr - 1) % TERMINAL_CMD_MEM_SIZE] != buf) {
            cmd_mem[mem_cur_ptr] = buf;
            mem_cur_ptr = (mem_cur_ptr + 1) % TERMINAL_CMD_MEM_SIZE;
        }

        running = true, result = false;
        runtime = std::make_pair(cmd[args[0]], args);
        return true;
    }
    // Command Not Found
    TERMINAL_ERROR_PRINTF("command not found: %s\r\n", args[0].c_str());
    show_head();
    return false;
}

void stop_running_task() {
    if(running) {
        running = false;
        force_stop = true;
        OS::Signal::action(terminal, 0);
    }
}

void input(char c) {
    // Ctrl-C
    if(c == 3) {
        stop_running_task();
        return;
    }

    if(running) return;

    if(c == '\r' or c == '\n') {
        bsp_uart_printf(TERMINAL_PORT, "\r\n");
        while(!buf.empty() and buf.back() == ' ') buf.pop_back();
        solve();
        buf.clear();
        return;
    }

    if(c == 0x7f) {
        if(buf.empty()) return;
        bsp_uart_printf(TERMINAL_PORT, "\b \b");
        buf.pop_back();
        return;
    }

    // Complete Input
    if(c == '\t') {
        return;
    }

    if(('a' <= c and c <= 'z') or ('0' <= c and c <= '9') or c == ' ' or ('A' <= c and c <= 'Z')) {
        bsp_uart_send(TERMINAL_PORT, reinterpret_cast <uint8_t *> (&c), 1);
        buf.push_back(c);
    }

    // bsp_uart_printf(TERMINAL_PORT, "received: %d\r\n", (int) c);
}

void recv(bsp_uart_e e, uint8_t *s, uint16_t l) {
    if(l == 3 and s[0] == 27 and s[1] == 91) {
        if(s[2] == 65) {
            // Up
            mem_ptr = std::min(mem_ptr + 1, TERMINAL_CMD_MEM_SIZE);
            if(!cmd_mem[(mem_cur_ptr - mem_ptr + TERMINAL_CMD_MEM_SIZE) % TERMINAL_CMD_MEM_SIZE].empty()) {
                fill_buf(cmd_mem[(mem_cur_ptr - mem_ptr + TERMINAL_CMD_MEM_SIZE) % TERMINAL_CMD_MEM_SIZE]);
            } else {
                mem_ptr --;
            }
        }
        if(s[2] == 66) {
            // Down
            mem_ptr = std::max(mem_ptr - 1, -TERMINAL_CMD_MEM_SIZE);
            if(!cmd_mem[(mem_cur_ptr - mem_ptr + TERMINAL_CMD_MEM_SIZE) % TERMINAL_CMD_MEM_SIZE].empty()) {
                fill_buf(cmd_mem[(mem_cur_ptr - mem_ptr + TERMINAL_CMD_MEM_SIZE) % TERMINAL_CMD_MEM_SIZE]);
            } else {
                mem_ptr ++;
            }
        }
        return;
    }
    for(int i = 0; i < l; i++) input(s[i]);
}

void app_terminal_init() {
    bsp_uart_set_callback(TERMINAL_PORT, recv);
    app_terminal_register_cmd("help", "show commands",
        [](const std::vector<std::string>& args) -> bool {
            for(const auto& i : cmd) {
                if(cmd_brief.count(i.first))
                    TERMINAL_INFO_PRINTF("- %s (%s)\r\n", i.first.c_str(), cmd_brief[i.first].c_str());
                else
                    TERMINAL_INFO_PRINTF("- %s\r\n", i.first.c_str());
            }
            return true;
        }
    );
    app_terminal_register_cmd("clear", "clear the console",
        [](const std::vector<std::string>& args) -> bool {
            TERMINAL_INFO_PRINTF("%s", TERMINAL_CLEAR_ALL);
            return true;
        }
    );
    app_terminal_register_cmd("hello", "test command",
        [](const std::vector<std::string>& args) -> bool {
            if(args.size() == 1) {
                TERMINAL_INFO("usage: hello <name>\r\n");
                return true;
            }
            if(args.size() > 2) {
                TERMINAL_ERROR("fuck your stupid parameters\r\n");
                return false;
            }
            TERMINAL_INFO_PRINTF("hello, %s!\r\n", args[1].c_str());
            return true;
        }
    );
    app_terminal_register_cmd("reboot", "reboot system",
        [](const std::vector<std::string>& args) -> bool {
            TERMINAL_INFO("system reboot\r\n");
            bsp_sys_reset();
            return true;
        }
    );
    app_terminal_register_cmd("task", "get freertos task info",
        [](const std::vector<std::string>& args) -> bool {
            vTaskList(tmp);
            TERMINAL_INFO_PRINTF("%s", tmp);
            return true;
        }
    );
    app_terminal_register_cmd("about", "show about_text",
        [](const std::vector<std::string>& args) -> bool {
            TERMINAL_INFO_PRINTF("%s", about_text);
            return true;
        }
    );

    terminal.Create(terminal_task, static_cast <void *> (nullptr), "terminal", 512, OS::Task::MEDIUM);
}

void app_terminal_register_cmd(const std::string& name, const std::function <bool(std::vector<std::string>)>& func) {
    BSP_ASSERT(cmd.count(name) == 0);
    cmd[name] = func;
}

void app_terminal_register_cmd(const std::string& name, const std::string& brief, const std::function <bool(std::vector<std::string>)>& func) {
    BSP_ASSERT(cmd.count(name) == 0);
    cmd[name] = func;
    cmd_brief[name] = brief;
}

bool* app_terminal_running_flag() {
    return &running;
}