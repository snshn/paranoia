#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "lib/paranoia/paranoia.h"
#include "lib/soldout/markdown.h"
#include "lib/soldout/renderers.h"
#include "cfg.h"

#include <json-c/json.h>

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

char* css =
"#page img{"
    "margin:3px"
"}"
"#page>a{"
    "display:inline-block;"
    "color:#FFF;"
    "font-size:9pt;"
    "text-decoration:none;"
    "padding:4px;"
    "margin:4px;"
    "background:#2E2E2E;"
    "border-radius:4px;"
    "opacity:0.6"
"}"
"#page>a:hover{"
    "opacity:1"
"}"
"#page article{"
    "color:#FFF;"
    "padding:16px;"
    "margin:8px 0;"
    "background:#000;"
"}"
;

void print_articles(const char* current)
{
    struct json_object *zwitts_obj, *zwitts_array, *zwitts_array_obj;
    int i, array_len;

    zwitts_obj = json_object_from_file(CFG_FILE_PAPERS);
    if ( zwitts_obj == NULL ) {
        print_html_error("Error while opening the blog database file.");
    }

    json_object_object_get_ex(zwitts_obj, "articles", &zwitts_array);

    array_len = json_object_array_length(zwitts_array);

    for ( i = 0; i < array_len; i++ ) {
        zwitts_array_obj = json_object_array_get_idx(zwitts_array, i);

        const char* article_name = json_object_get_string(zwitts_array_obj);

        if (
            (!i && !strlen(current))
            ||
            (strcmp(current, article_name) == 0)
        ) {
            // Latest/selected article
            char path[500] = CFG_DIR_PAPERS "/";
            strcat(path, article_name);
            strcat(path, ".md");
            struct buf *ib, *ob;
            size_t ret;
            FILE *in = fopen(path, "rb");
            const struct mkd_renderer *hrndr;
            const struct mkd_renderer **prndr;

            /* default options: strict markdown input, HTML output */
            hrndr = &mkd_html;
            prndr = &hrndr;

            /* reading everything */
            ib = bufnew(READ_UNIT);
            bufgrow(ib, READ_UNIT);
            while ((ret = fread(ib->data + ib->size, 1,
                    ib->asize - ib->size, in)) > 0) {
                ib->size += ret;
                bufgrow(ib, ib->size + READ_UNIT);
            }

            fclose(in);

            /* performing markdown parsing */
            ob = bufnew(OUTPUT_UNIT);
            markdown(ob, ib, *prndr);

            /* output the result */
            printf("<article>%s</article>", ob->data);

            /* cleanup */
            bufrelease(ib);
            bufrelease(ob);
        } else {
            // Link buttons before/after the current article
            char article_name_url[strlen(article_name)];
            strcpy(article_name_url, article_name);
            spaceplus(article_name_url);
            printf("<a href=\"/idx?%s\">%s</a>",
                article_name_url, article_name);
        }
    }
}

int main(const int argc, char **args)
{
    char *query = (getenv("QUERY_STRING")) ? getenv("QUERY_STRING")
                    : (argc > 1 ? args[1] : "");

    url_decode(query);

    while (FCGI_Accept() >= 0) {
        print_html_header("~", css);
        print_articles(query);
        print_html_footer();
    }

    return 0;
}
