#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

#include <json-c/json.h>

const char* css =
"#page{"
    "text-align:center;"
    "padding:0;"
    "font-size:67%;"
"}"
    "#page a{"
        "display:inline-block;"
        "background-color;"
        "padding:4px 9px;"
        "margin:15px;"
        "text-align:left;"
        "border-radius:4px;"
        "background:rgba(20,20,20,.7);"
    "}"
        "#page span{"
            "display:block;"
        "}"
        "#page span+span{"
            "color:#DDD;"
        "}"
;

static int json_array_sort_fn(const void *p1, const void *p2)
{
    (void)p1;
    (void)p2;
    return 1;
}

void print_links()
{
    int i, array_len;
    struct json_object *links_obj, *links_array, *links_array_obj,
                       *links_array_obj_url, *links_array_obj_info;

    links_obj = json_object_from_file(CFG_FILE_LINKS);

    json_object_object_get_ex(links_obj, "urls", &links_array);

    array_len = json_object_array_length(links_array);

    /* Flip the array */
    json_object_array_sort(links_array, json_array_sort_fn);

    for (i = 0; i < array_len; i++) {
        links_array_obj = json_object_array_get_idx(links_array, i);

        json_object_object_get_ex(links_array_obj, "url", &links_array_obj_url);
        json_object_object_get_ex(links_array_obj, "info", &links_array_obj_info);

        printf("<a href=\"%s\">",
            json_object_get_string(links_array_obj_url)
        );
        if (json_object_get_string_len(links_array_obj_info)) {
            printf("<span>%s</span>", json_object_get_string(links_array_obj_info));
        }
        printf("<span>%s</span>", json_object_get_string(links_array_obj_url));
        printf("</a>");
    }
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("NEш PAтн", css);
        print_links();
        print_html_footer();
    }

    return 0;
}
