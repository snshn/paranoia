#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

#include <json-c/json.h>
#include <json-c/random_seed.h>

const char* css =
"#page{"
    "text-align:center;"
"}"
"#page img{"
    "width:200px;"
    "height:150px;"
"}"
"#page div{"
    "text-align:left;"
    "display:inline-block;"
    "position:relative;"
    "margin:10px;"
"}"
    "#page img{"
        "background-image:url(i/static.gif);"
        "background-size:cover;"
    "}"
    ".price{"
        "color:#FFF;"
        "background:#C00;"
        "padding:2px 3px;"
        "float:right;"
        "border-radius:2px;"
        "position:absolute;"
        "top:0;"
        "right:0;"
        "cursor:default;"
    "}"
    ".name{"
        "display:block;"
        "font-size:75%;"
    "}"
;

static int json_array_sort_fn(const void *p1, const void *p2)
{
    (void)p1;
    (void)p2;
    return json_c_get_random_seed();
}

void waste_life()
{
    struct json_object *art_obj, *art_array, *art_array_obj,
            *art_array_obj_name, *art_array_obj_about, *art_array_obj_price,
            *art_array_obj_price_comment, *art_array_obj_image;
    int i = 0, array_len = 0;

    art_obj = json_object_from_file(CFG_FILE_TVS);
    json_object_object_get_ex(art_obj, "tubes", &art_array);

    array_len = json_object_array_length(art_array);

    /* Shuffle the array */
    json_object_array_sort(art_array, json_array_sort_fn);

    for (; i < array_len; i++) {
        art_array_obj = json_object_array_get_idx(art_array, i);

        json_object_object_get_ex(art_array_obj, "name", &art_array_obj_name);
        json_object_object_get_ex(art_array_obj, "about", &art_array_obj_about);
        json_object_object_get_ex(art_array_obj, "image", &art_array_obj_image);
        json_object_object_get_ex(art_array_obj, "price", &art_array_obj_price);
        json_object_object_get_ex(art_array_obj, "price_comment", &art_array_obj_price_comment);

        printf("<div title=\"%s\">"
                    "<img src=\"%s/%s\" />"
                    "<span class=\"price\" title=\"%s\">$%.2f</span>"
                    "<span class=\"name\">%s</span>"
                "</div>",
                json_object_get_string_len(art_array_obj_about) ?
                    json_object_get_string(art_array_obj_about) :
                    "",
                CFG_DIR_TVS, json_object_get_string(art_array_obj_image),
                json_object_get_string_len(art_array_obj_price_comment) ?
                    json_object_get_string(art_array_obj_price_comment) :
                    "",
                json_object_get_double(art_array_obj_price),
                json_object_get_string(art_array_obj_name)
        );
    }

    /* Fixers */
    for (; i % 3; i++) {
        printf("<div></div>");
    }
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("SALE! SALE!", css);
        waste_life();
        print_html_footer();
    }

    return 0;
}
