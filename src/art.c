#include <fcgi_stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

#include <json-c/json.h>
#include <json-c/random_seed.h>

const char* css =
"#page{"
    "text-align:center"
"}"
    "#page a{"
        "display:inline-block;"
        "position:relative;"
        "margin:11px;"
        "width:90px;"
        "height:120px;"
        "background:transparent center center no-repeat;"
        "background-size:cover;"
        "opacity:.8;"
        "transition:opacity 666ms"
    "}"
    "#page a[style]{"
        "background-color:#555"
    "}"
    "#page a:hover{"
        "opacity:1;"
        "transition:opacity 999ms"
    "}"
;

static int json_array_sort_fn(const void *p1, const void *p2)
{
    (void)p1;
    (void)p2;
    return json_c_get_random_seed();
}

void print_movies()
{
    CURL *curl = curl_easy_init();
    struct json_object *art_obj, *art_array, *art_array_obj, *art_array_obj_hide,
            *art_array_obj_name, *art_array_obj_link, *art_array_obj_year;
    int i = 0, array_len = 0;

    art_obj = json_object_from_file(CFG_FILE_ART);
    json_object_object_get_ex(art_obj, "masterpieces", &art_array);

    array_len = json_object_array_length(art_array);

    /* Shuffle the array */
    json_object_array_sort(art_array, json_array_sort_fn);

    int shown = 0;

    for (; i < array_len; i++) {
        art_array_obj = json_object_array_get_idx(art_array, i);

        json_object_object_get_ex(art_array_obj, "hide", &art_array_obj_hide);
        if (json_object_get_boolean(art_array_obj_hide)) continue;
        json_object_object_get_ex(art_array_obj, "name", &art_array_obj_name);
        json_object_object_get_ex(art_array_obj, "link", &art_array_obj_link);
        json_object_object_get_ex(art_array_obj, "year", &art_array_obj_year);

        const char *name = json_object_get_string(art_array_obj_name);
        char *name_esc = curl_easy_escape(curl, name, 0);

        printf("<a href=\"%s\" title=\"%s (%d)\" "
                    "style=\"background-image:url('tmb?%s/%s.png')\">"
                "</a>",
                json_object_get_string(art_array_obj_link),
                name,// TODO safe_name
                json_object_get_int(art_array_obj_year),
                CFG_DIR_ART,
                name_esc
        );

        curl_free(name_esc);
        shown++;
    }

    /* Fixers */
    for (; shown % 7; shown++) {
        printf("<a></a>");
    }
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("I've seen films", css);
        print_movies();
        print_html_footer();
    }

    return 0;
}


/*

8MM.png
Beyond the Law.png
Chaplin.png
Cypher.jpg
.Event Horizon.jpg
Gunhed.jpg
Identity.jpg
Impostor.jpg
Leon.jpg
Metropia.jpg
Modern Times.png
Puzzlehead.png
The Big Lebowski.jpg
The Reader.png

/Action/
Rampage.jpg
Riddick.jpg
Silent Trigger.png
The Shooter.png

/B-Grade/
Army of Darkness.png
Bloodsuckers.jpg
Crime Zone.jpg
Frankenhooker.jpg
Gene Generation.jpg
Hobo With a Shotgun.png
I Come In Peace.jpg
Meet the Feebles.png
Nirvana.jpg
Poultrygeist Night of the Chicken Dead.png
The Breed.jpg
Velocity Trap.jpg

/TV-Shows/
OZ.png
.Sealab 2021.png

*/
