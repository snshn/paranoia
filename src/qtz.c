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
    "padding:0;"
"}"
    "#page .source{"
        "display:block;"
        "opacity:.8;"
    "}"
;

static int json_array_sort_fn(const void *p1, const void *p2)
{
    (void)p1;
    (void)p2;
    return json_c_get_random_seed();
}

void print_quotes()
{
    struct json_object *quotes_obj, *quotes_array, *quotes_array_obj,
            *quotes_array_obj_text, *quotes_array_obj_author;
    int i, array_len;

    quotes_obj = json_object_from_file(CFG_FILE_QUOTES);
    json_object_object_get_ex(quotes_obj, "quotes", &quotes_array);

    array_len = json_object_array_length(quotes_array);

    /* Shuffle the array */
    json_object_array_sort(quotes_array, json_array_sort_fn);

    for ( i = 0; i < array_len; i++ ) {
        quotes_array_obj = json_object_array_get_idx(quotes_array, i);

        json_object_object_get_ex(quotes_array_obj, "text",
                                    &quotes_array_obj_text);
        json_object_object_get_ex(quotes_array_obj, "source",
                                    &quotes_array_obj_author);

        printf("<blockquote>%s",
            json_object_get_string(quotes_array_obj_text)
        );
        if (json_object_get_string_len(quotes_array_obj_author)) {
            printf("<span class=\"source\">%s</span>",
                   json_object_get_string(quotes_array_obj_author)
            );
        }
        printf("</blockquote>");
    }
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("S.M.A.R.T.", css);
        print_quotes();
        print_html_footer();
    }

    return 0;
}
