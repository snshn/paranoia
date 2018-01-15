#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

#include <json-c/json.h>

const char* css =
""
;

void print_commands()
{
    int i, array_len;
    struct json_object *comm_obj, *comm_array, *comm_array_obj;

    comm_obj = json_object_from_file(CFG_FILE_COMM);
    if ( comm_obj == NULL ) {
        print_html_error("Error while opening the cli database file.");
    }

    json_object_object_get_ex(comm_obj, "commands", &comm_array);

    array_len = json_object_array_length(comm_array);

    for ( i = 0; i < array_len; i++ ) {
        comm_array_obj = json_object_array_get_idx(comm_array, i);
        printf("<code>%s</code>", json_object_get_string(comm_array_obj));
    }
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("C0mm& cеИ+R", css);
        print_commands();
        print_html_footer();
    }

    return 0;
}
