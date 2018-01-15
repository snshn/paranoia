#pragma once

int strcasecmp(const char* f1, const char* f2);
char* strdup(const char* s);

#define IS_DOT(C) ((C[0]) == '.')

typedef struct {
  char original;
  char* safe;
} htmlspecialchar;

typedef struct {
    char* name;
    enum { MOVIE, BOOK, GAME, SHOW } type;
    char* link;
    unsigned short year;
} Masterpiece;

void print_html_header(const char* title, const char* style);
void print_html_footer();

void print_html_error(char* content);
void print_html_quote(char* text);
void print_html_404();

// c8 readable_size(unsigned int size);

void rtrim_ext(char* file_name);
void rtrim_slash(char* path);
int escape_string(const char* str, char* result);
void spaceplus(char* input);
void url_decode(char* s);

htmlspecialchar htmlchars[] = {
  { '"', "quot" },
  { '<', "lt" },
  { '>', "gt" },
  { '&', "amp" }
};
