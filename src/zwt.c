#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

#include <json-c/json.h>

const char* css =
"p{"
    "color:#FFF"
"}"
;

void print_zwitter()
{
    struct json_object *zwitts_obj, *zwitts_array, *zwitts_array_obj;
    int i, array_len;

    zwitts_obj = json_object_from_file(CFG_FILE_ZWITTS);
    if ( zwitts_obj == NULL ) {
        print_html_error("Error while opening the zwitter database file.");
    }

    json_object_object_get_ex(zwitts_obj, "zwitts", &zwitts_array);

    array_len = json_object_array_length(zwitts_array);

    for ( i = 0; i < array_len; i++ ) {
        zwitts_array_obj = json_object_array_get_idx(zwitts_array, i);
        printf("<p>%s</p>", json_object_get_string(zwitts_array_obj));
    }
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("Captain's log", css);
        print_zwitter();
        print_html_footer();
    }

    return 0;
}
