#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "../../cfg.h"
#include "paranoia.h"

void print_html_header(const char *title, const char *style)
{
    unsigned char i = 0;

    printf("Content-Type: text/html\n\n"
        "<!doctype html>"
        "<html lang=\"en\">"
        "<meta charset=\"utf-8\"/>"
        "<meta name=\"referrer\" content=\"no-referrer\"/>"
        "<title>%s → %s</title>"
        "<link rel=\"icon\" href=\"favicon.ico\"/>"
        "<link rel=\"stylesheet\" href=\"css/ssc.css\"/>"
        "<meta name=\"description\" "
            "content=\"Digital soulbox lost in time\"/>"
        "<style>%s</style>"
        "<header>"
            "<a id=\"top\" href=\"/\" title=\"GOTO START\"></a>",
        CFG_TITLE, title, style
    );

    /* Navigation */
    printf("<nav>");
    for ( i = 0; i < sizeof(menu_items) / sizeof(MenuItem); i++ ) {
        const MenuItem *menu_item = &menu_items[i];
        printf("<a href=\"/%s\" title=\"%s\">%s</a>",
                menu_item->link, menu_item->title, menu_item->name);
    }
    printf("</nav>");
    /* /Navigation */

    printf(
        "</header>"
        "<div id=\"page\">"
    );
}

void print_html_footer()
{
    printf(
        "</div>"
        "<footer>"
            "<span>"
                "Protected by Deckard, Blade Runner. 26354.\n"
                "Everything is filed, monitored, checked and cleared.\n"
                "\n"
                "Have a better one."
            "</span>"
        "</footer>"
        "</html>\n"
    );
}

void print_html_error(char* content)
{
    printf("<div class=\"message error\">%s</div>", content);
}

void print_html_quote(char* text)
{
    printf("<blockquote>%s</blockquote>", text);
}

void print_html_404()
{
    printf("<div class=\"not-found\">404<b>_</b></div>");
}

/*
char* readable_size(u32 size)
{
    $sizes = array(" Bytes", "KB", "MB"); //, " GB", " TB", " PB", " EB", " ZB", " YB");
    if (!$size)
    {
        return('n/a');
    }
    else
    { // TODO make it return 234.3 instead of 234
        return (round($size/pow(1024, ($i = floor(log($size, 1024)))), $i > 1 ? 2 : 0) . " " . $sizes[$i]);
    }
    return "";
}
*/

/* Remove the file extension */
void rtrim_ext(char* file_name) {
    unsigned short length = strlen(file_name);
    unsigned short point_index = length;
    unsigned short i = length;

    for (; i > 0; i--) {
        if (file_name[i] == '.') {
            point_index = i;
            break;
        }
    }

    file_name[point_index] = '\0';
}

/* Go one level up */
void rtrim_slash(char* path) {
    unsigned short length = strlen(path);
    unsigned short point_index = length;
    unsigned short i = length;

    for (; i > 0; i--) {
        if (path[i] == '/') {
            point_index = i;

            if (i < length)
                break;
        }
    }

    // There was only one element in the path
    if (point_index == length) {
        point_index = 0;
    }

    path[point_index] = '\0';
}

int escape_string(const char* str, char* result)
{
    unsigned short i = 0, h = 0, r = 0;
    unsigned short hlen = sizeof(htmlchars) / sizeof(htmlspecialchar);
    unsigned short count = strlen(str);
    unsigned short ptr_size = count;

    if (result == NULL) { /* void pointer estimates the result length */
        for (; i < count; i++) {
            for (h = 0; h < hlen; h++) {
                if (str[i] == htmlchars[h].original)
                    ptr_size += strlen(htmlchars[h].safe) + 1; // +1 for & and ;
            }
        }

        return ptr_size + 1; // +1 for the NULL-byte
    } else {
        for (; i < count; i++, r++) {
            result[r] = str[i];
            for (h = 0; h < hlen; h++) {
                if (str[i] == htmlchars[h].original) {
                    result[r] = '\0'; // prepare the string for concatenation
                    strcat(result, "&");
                    strcat(result, htmlchars[h].safe);
                    strcat(result, ";");
                    r += strlen(htmlchars[h].safe) + 1;
                }
            }
        }

        result[r] = '\0';

        return r;
    }
}

inline int is_hex(int x) {
    return (x > '/' && x < ':') || (x > '`' && x < 'g') || (x > '@' && x < 'G');
}

/* "/file?a+b" -> "/file?a b" */
void url_decode(char* encoded_url)
{
    for (int i = 0, ilen = strlen(encoded_url); i < ilen; i++) {
        if (encoded_url[i] == '+') {
            encoded_url[i] = ' ';
        }
    }
}

/* ' ' -> '+' */
void spaceplus(char* input) {
    int i = 0, ilen = strlen(input);

    for ( ; i < ilen; i++ ) {
        if (input[i] == ' ') {
            input[i] = '+';
        }
    }
}
