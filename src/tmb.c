#include <fcgi_stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

#include "cfg.h"
#include "lib/md5/md5.h"
#include "lib/thumb/thumb.h"

#define MD5_DIGEST_LENGTH 32

char * supported_formats[] = { "jpg", "png" };

int main(int ArgCount, char* Args[])
{
    CURL *curl = curl_easy_init();
    /* Decode the GET query string */
    char* query = (getenv("QUERY_STRING")) ? getenv("QUERY_STRING")
                    : (ArgCount > 1 ? Args[1] : "");

    char *query_esc = curl_easy_unescape(curl, query, 0, 0);

    while(FCGI_Accept() >= 0)
    {
        FILE *inFile = fopen(query_esc, "rb");

        if (!inFile) {
            printf("Location: /i/pixel.png" "\n\n");
            curl_free(query_esc);
            return -1;
        }

        /* Calculate the md5 hash of the input file */
        int i = 0, bytes_read = 0;
        char data[1024];
        MD5_CTX mdContext;
        char hash[MD5_DIGEST_LENGTH + 1] = "";

        MD5_Init(&mdContext);
        while ( (bytes_read = fread(data, 1, 1024, inFile)) ) {
            MD5_Update(&mdContext, data, bytes_read);
        }
        MD5_Final(&mdContext);

        fclose(inFile);

        for (i = 0; i < 16; i++) {
            char buf[2];
            sprintf(buf, "%02x", mdContext.digest[i]);
            strcat(hash, buf);
        }

        /* Attempt to lacate a previously created thumbnail file */
        DIR *d = opendir(CFG_DIR_THUMBS);

        if (!d) {
            fprintf(stderr, "Creating " CFG_DIR_THUMBS "/" "\n");
            mkdir(CFG_DIR_THUMBS, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            closedir(d);
            d = opendir(CFG_DIR_THUMBS);
        }

        if (d) {
            struct dirent *dir;
            int format_count = sizeof(supported_formats) / sizeof(supported_formats[0]);

            /* Attempt to locate a pre-generated thumbnail for the given image */
            while ( (dir = readdir(d)) ) {
                for (int i = 0; i < format_count; i++) {
                    char hash_ext[strlen(hash) + 4 + 1];
                    strcpy(hash_ext, hash);
                    strcat(hash_ext, ".");
                    strcat(hash_ext, supported_formats[i]);
                    if (strcmp(dir->d_name, hash_ext) == 0) {
                        printf("Location: " CFG_DIR_THUMBS "/%s" "\n\n", hash_ext);
                        closedir(d);
                        curl_free(query_esc);
                        return 0;
                    }
                }
            }
            closedir(d);

            /* Create a new thumbnail and redirect the user to it afterwards */
            char tpath[MD5_DIGEST_LENGTH + strlen(CFG_DIR_THUMBS) + 1 + 4 + 1];
            strcpy(tpath, CFG_DIR_THUMBS);
            strcat(tpath, "/");
            strcat(tpath, hash);

            if (thumb_create(query_esc, CFG_THUMB_SIZE, tpath) == 0) {
                printf("Location: %s" "\n\n", tpath);
                curl_free(query_esc);
                return 0;
            }
        } else {
            fprintf(stderr, "Unable to access " CFG_DIR_THUMBS "/" "\n");
        }

        /* If all else fails, redirect to the source */
        printf("Location: %s" "\n\n", query_esc);
        curl_free(query_esc);
        return -1;
    }

    curl_free(query_esc);
    return -1;
}
