#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/dir.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

const char* css =
"#page{"
    "background:rgba(137,232,148,.1);"
    "padding:3px;"
"}"
    "#path{"
        "margin:3px;"
    "}"
        "#path a{"
            "color:#000;"
            "font-size:80%;"
            "padding:2px 9px;"
            "margin:3px 4px;"
            "background:#89E894;"
        "}"
            "#path a:hover{"
                "color:#89E894;"
                "background:transparent;"
            "}"
    "#list{"
        "width:100%;"
        "padding-bottom:100px;"
    "}"
        "#list a{"
            "color:#89E894;"
            "display:block;"
            "padding:0 4px;"
        "}"
        "#list a:hover{"
            "color:#000;"
            "background:#89E894;"
        "}"
;

#define MAX_DEPTH 8

typedef struct {
    char* start;
    int length;
} node;

int myalphasort(const struct dirent** e1, const struct dirent** e2)
{
    const char* a = (*e1)->d_name;
    const char* b = (*e2)->d_name;

    return strcasecmp(a, b);
}

void print_directory(const char* path)
{
    char path_esc[escape_string(path, NULL)];
    escape_string(path, path_esc);
    char path_esc_x[strlen(path_esc)+1];
    strcpy(path_esc_x, path_esc);
    spaceplus(path_esc_x);

    int i = 0;
    int ni = 0;
    int wl = 0;
    node nodes[MAX_DEPTH];
    node nodes_x[MAX_DEPTH];
    for (; ni < MAX_DEPTH && i < strlen(path_esc)+1; i++) {
        if (path_esc[i] == '/' || path_esc[i] == '\0') {
            if (wl) {
                nodes[ni].length = nodes_x[ni].length = wl;
                ni++;
                wl = 0;
            }
        } else {
            if (!wl) {
                nodes[ni].start = &path_esc[i];
                nodes_x[ni].start = &path_esc_x[i];
            }
            wl++;
        }
    }

    char loc[2048] = CFG_DIR_STORAGE "/";
    strcat(loc, path);
    struct dirent** namelist = NULL;
    int n = scandir(loc, &namelist, 0, myalphasort);

    if (n < 0) {
        print_html_error("Unable to open the specified directory.");
    } else {
        DIR *d = opendir(loc);

        /* Iterate through the top-level dir */
        if ( d ) {
            struct stat st;

            /* The path */
            printf("<div id=\"path\">");
            printf("<a href=\"dat\">/</a>");
            char path_x[2048];
            char path_nox[2048];
            char path_node[2048];
            char path_node_x[2048];
            for (int i = 0; i < ni; i++) {
                strcpy(path_node, nodes[i].start);
                path_node[nodes[i].length] = 0x00;

                strcpy(path_node_x, nodes_x[i].start);
                path_node_x[nodes_x[i].length] = 0x00;

                if (strlen(path_nox))
                    strcat(path_nox, "/");
                strcat(path_nox, path_node);

                if (strlen(path_x))
                    strcat(path_x, "/");
                strcat(path_x, path_node_x);

                printf("<a href=\"dat?%s\">%s</a>", path_x, path_node);
            }
            printf("</div>");

            /* File/directory list */
            printf("<div id=\"list\">");

            /* List subdirectories first */
            for (i = 0; i < n ; i++) {
                if (IS_DOT(namelist[i]->d_name)) continue; // Skip dots

                fstatat(dirfd(d), namelist[i]->d_name, &st, 0);

                if (S_ISDIR(st.st_mode)) {
                    char safe_name[escape_string(namelist[i]->d_name, NULL)];
                    escape_string(namelist[i]->d_name, safe_name);
                    char safe_name_x[strlen(safe_name) + 1];
                    strcpy(safe_name_x, safe_name);
                    spaceplus(safe_name_x);
                    if (strlen(path_x))
                        printf("<a class=\"d\" href=\"/dat?%s/%s\">□ %s</a>",
                            path_x, safe_name_x, safe_name);
                    else
                        printf("<a class=\"d\" href=\"/dat?%s\">□ %s</a>",
                            safe_name_x, safe_name);
                }
            }

            /* Then list the files */
            for (i = 0; i < n; i++) {
                if (IS_DOT(namelist[i]->d_name)) continue; // Skip dots

                fstatat(dirfd(d), namelist[i]->d_name, &st, 0);

                if (S_ISREG(st.st_mode)) {
                    char safe_name[escape_string(namelist[i]->d_name, NULL)];
                    escape_string(namelist[i]->d_name, safe_name);
                    if (strlen(path_nox))
                        printf("<a class=\"f\" "
                            "href=\"/"CFG_DIR_STORAGE"/%s/%s\">■ %s</a>",
                            path_nox, safe_name, safe_name);
                    else
                        printf("<a class=\"f\" "
                            "href=\"/"CFG_DIR_STORAGE"/%s\">■ %s</a>",
                            safe_name, safe_name);
                }
            }

            printf("</div>");

            closedir(d);
        }
    }
}

int main(const int argc, char **args)
{
    char* query = (getenv("QUERY_STRING")) ? getenv("QUERY_STRING")
                    : (argc > 1 ? args[1] : "");

    url_decode(query);

    /* Filter the input */
    int i = 0, l = strlen(query);
    for (; i < l; i++) {
        /* No .. */
        if (query[i] == '.' && query[i+1] == '.') {
            query[i] = 0x00;
            break;
        }
        /* Prevent from possible overflow within strcat */
        if (i == 256) {
            query[i] = 0x00;
            break;
        }
    }

    while (FCGI_Accept() >= 0) {
        print_html_header("73 74 6F 72 61 67 65 00", css);
        print_directory(query);
        print_html_footer();
    }

    return 0;
}
