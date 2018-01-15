#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "lib/paranoia/paranoia.h"
#include "cfg.h"

const char* css = "";

const char* quotes[] = {
    "As the final words erupt from your mouth,\n"
    "who will remember your name?\n"
    "\n"
    " ― \"Disturbed\"",

    "Time is our only real currency\n"
    "\n"
    "\"Synchronicity\" (2015)",

    "Time is a funny thing. Time is a very peculiar item.\n"
    "You see, when you're young, when you're a kid, you got time, you got nothing but time.\n"
    "Throw away a couple of years here, couple of years there… it doesn't matter, you know.\n"
    "The older you get, you say: \"Jesus, how much I got?\". I got 35 summers left!\n"
    "Think about it: 35 Summers.\n"
    "\n"
    "\"Rumble Fish\" (1983)",

    "Remember when you was a kid and you would spend the whole year waiting\n"
    "for summer vacation and when it finally came it would fly by just like that?\n"
    "It's funny, Jimmy, life has a way of flying by faster than any old summer vacation.\n"
    "It really fucking does.\n"
    "\n"
    "\"Things to Do in Denver When You're Dead\" (1995)",

    "Today you're safe\n"
    "Tomorrow who knows\n"
    "Guarantees are fool's gold\n"
    "34 turns to 43\n"
    "See my mistakes\n"
    "Don't become me\n"
    "\n"
    " ― \"Curve\"",

    "I've seen things you people wouldn't believe.\n"
    "Attack ships on fire off the shoulder of Orion.\n"
    "I watched C-beams glitter in the dark near the Tänhauser Gate.\n"
    "All those… moments will be lost, in time, like tears… in rain.\n"
    "Time to die.\n"
    "\n"
    " ― Roy Batty, \"Blade Runner\" (1982)"
};

void become_famous()
{
    time_t now = time(NULL);

    srand(now);
    printf("<blockquote>%s</blockquote>\n",
            quotes[rand() % sizeof(quotes) / sizeof(quotes[0])]);

    struct tm tm;
    time_t epoch = 0;
    char *last_day = "2029-12-21 12:00:00";
    if (strptime(last_day, "%Y-%m-%d %H:%M:%S", &tm)) {
        epoch = mktime(&tm);
    }
    int days_left = (epoch - now) / 60 / 60 / 24;

    printf("<p>There%s %d day%s until I'm 40.</p>",
        days_left > 1 ? " are" : "'s",
        days_left,
        days_left > 1 ? "s" : ""
    );
}

int main()
{
    while (FCGI_Accept() >= 0) {
        print_html_header("~", css);
        become_famous();
        print_html_footer();
    }

    return 0;
}
