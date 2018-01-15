#include <fcgi_stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/dir.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

const char* css =
"#page{"
    "text-align:center"
"}"
    "#path{"
        "width:780px;"
        "text-align:left;"
        "margin:8px auto"
    "}"
        "#path a{"
            "color:#000;"
            "font-size:80%;"
            "padding:2px 9px;"
            "margin:3px 4px;"
            "background:#CCC;"
            "border-radius:3px"
        "}"
            "#path a:hover{"
                "color:#CCC;"
                "background:rgba(155, 155, 155, .2)"
            "}"
    "#ui a{"
        "display:inline-block;"
        "width:120px;"
        "height:120px;"
        "vertical-align:middle;"
        "padding:0 6px;"
        "position:relative"
    "}"
    "#ui a.dir{"
        "position:relative;"
        "line-height:8.3pt;"
        "font-size:7pt;"
        "font-weight:bold;"
        "background:transparent url(i/folder.png) no-repeat center center"
    "}"
        "#ui a.dir span{"
            "position:absolute;"
            "color:#FFF;"
            "top:8px;"
            "left:22px;"
            "width:47px;"
            "text-align:center;"
            "white-space:nowrap"
        "}"
        "#ui a img{"
            "background:transparent url(i/alpha.png) center center;"
            "max-height:96%;"
            "max-width:96%;"
            "width:auto;"
            "height:auto;"
            "position:absolute;"
            "top:0;"
            "bottom:0;"
            "left:0;"
            "right:0;"
            "margin:auto"
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

void print_room(const char* path)
{
    CURL *curl = curl_easy_init();
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

    char loc[2048] = CFG_DIR_GALLERY "/";
    strcat(loc, path);
    struct dirent **namelist = NULL;
    int n = scandir(loc, &namelist, 0, myalphasort);

    if (n < 0) {
        print_html_error("Unable to open the gallery directory.");
    } else {
        DIR *d = opendir(loc);

        /* Iterate through the top-level dir */
        if (d) {
            int total = 0;
            struct stat st;

            /* The path */
            printf("<div id=\"path\">");
            printf("<a href=\"eye\">/</a>");
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

                printf("<a href=\"eye?%s\">%s</a>", path_x, path_node);
            }
            printf("</div>");

            /* Files and directories */
            printf("<div id=\"ui\">");

            /* .. (for all except root) */
            if (strlen(path_esc)) {
                char up[2048];
                strcpy(up, path_esc_x);
                rtrim_slash(up);
                if (strlen(up))
                    printf("<a class=\"dir\" href=\"/eye?%s\">"
                        "<span>..</span></a>", up);
                else
                    printf("<a class=\"dir\" href=\"/eye\">"
                        "<span>..</span></a>");

                total++;
            }

            /* List the subdirectories first */
            for (i = 0; i < n ; i++) {
                /* Skip hidden and ./.. */
                if (IS_DOT(namelist[i]->d_name))
                    continue;

                fstatat(dirfd(d), namelist[i]->d_name, &st, 0);

                if (S_ISDIR(st.st_mode)) {
                    char safe_dir_name[escape_string(namelist[i]->d_name, NULL)];
                    escape_string(namelist[i]->d_name, safe_dir_name);
                    char safe_name_x[strlen(safe_dir_name) + 1];
                    strcpy(safe_name_x, safe_dir_name);
                    spaceplus(safe_name_x);

                    if (strlen(path_esc)) {
                        printf("<a class=\"dir\" href=\"/eye?%s/%s\">"
                            "<span>%s</span></a>",
                            path_esc_x, safe_name_x,
                            safe_dir_name);
                    } else {
                        printf("<a class=\"dir\" href=\"/eye?%s\">"
                            "<span>%s</span></a>",
                            safe_name_x,
                            safe_dir_name);
                    }

                    total++;
                }
            }

            /* Then list the image files */
            for (i = 0; i < n ; i++) {
                /* Skip hidden */
                if (IS_DOT(namelist[i]->d_name))
                    continue;

                fstatat(dirfd(d), namelist[i]->d_name, &st, 0);

                if (S_ISREG(st.st_mode)) {
                    char *image_name_esc = curl_easy_escape(curl, namelist[i]->d_name, 0);

                    if (strlen(path_esc)) {
                        char *path_esc_curl = curl_easy_escape(curl, path, 0);
                        printf("<a href=\"/"CFG_DIR_GALLERY"/%s/%s\">"
                            "<img src=\"tmb?"CFG_DIR_GALLERY"/%s/%s\" /></a>",
                            path_esc, namelist[i]->d_name,
                            path_esc_curl, image_name_esc
                        );
                        curl_free(path_esc_curl);
                    } else {
                        printf("<a href=\"/"CFG_DIR_GALLERY"/%s\">"
                            "<img src=\"tmb?"CFG_DIR_GALLERY"/%s\" /></a>",
                            image_name_esc,
                            image_name_esc
                        );
                    }

                    curl_free(image_name_esc);
                    total++;
                }
            }

            /* Fixers */
            for (; total % 6; total++) {
                printf("<a></a>");
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
            query[i] = '\0';
            break;
        }
        /* Prevent from possible overflow within strcat */
        if (i == 256) {
            query[i] = '\0';
            break;
        }
    }

    while (FCGI_Accept() >= 0) {
        print_html_header("PI×", css);
        print_room(query);
        print_html_footer();
    }

    return 0;
}
