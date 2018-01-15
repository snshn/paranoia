#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

#include <json-c/json.h>

const char* css =
"#page{"
    "width:420px;"
    "padding:10em 0"
"}"
    ".song{"
        "color:#BBB;"
        "white-space:nowrap"
    "}"
        ".song span{"
            "color:#FFF"
        "}"
;

void print_tunes()
{
    int i, array_len;
    struct json_object *music_object, *music_array, *music_array_tune_object,
                       *music_array_tune_name, *music_array_tune_artist;

    music_object = json_object_from_file(CFG_FILE_TUNES);

    json_object_object_get_ex(music_object, "tunes", &music_array);

    array_len = json_object_array_length(music_array);

    for (i = 0; i < array_len; i++) {
        music_array_tune_object = json_object_array_get_idx(music_array, i);

        json_object_object_get_ex(music_array_tune_object, "name", &music_array_tune_name);
        json_object_object_get_ex(music_array_tune_object, "artist", &music_array_tune_artist);

        printf("<div class=\"song\">♪ "
                    "<a href=\"https://youtube.com/results?search_query=%s %s\" rel=\"noreferrer\">"
                        "<span class=\"name\">%s</span> by <span class=\"artist\">%s</span>"
                    "</a>"
                "</div>",
                json_object_get_string(music_array_tune_artist),
                json_object_get_string(music_array_tune_name),
                json_object_get_string(music_array_tune_name),
                json_object_get_string(music_array_tune_artist)
        );
    }
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("♪", css);
        print_tunes();
        print_html_footer();
    }

    return 0;
}
