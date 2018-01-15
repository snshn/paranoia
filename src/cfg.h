#pragma once

/* Website name */
#define CFG_TITLE "Пагапо|а 2.O"

/* Relative paths for various directories */
#define CFG_DIR_DB      "db"
#define CFG_DIR_STORAGE "data"
#define CFG_DIR_PAPERS  CFG_DIR_DB"/txt"
#define CFG_DIR_ART     CFG_DIR_STORAGE"/pictures/.artworks"
#define CFG_DIR_GALLERY CFG_DIR_STORAGE"/pictures"
#define CFG_DIR_TVS     "i/tv"
#define CFG_DIR_THUMBS  "t"
#define CFG_THUMB_SIZE  200

/* Name of files within the database directory */
#define CFG_FILE_ART    CFG_DIR_DB"/art.json"    // /art
#define CFG_FILE_COMM   CFG_DIR_DB"/cli.json"    // /zwt
#define CFG_FILE_LINKS  CFG_DIR_DB"/links.json"  // /www
#define CFG_FILE_PAPERS CFG_DIR_DB"/papers.json" // /idx
#define CFG_FILE_QUOTES CFG_DIR_DB"/quotes.json" // /qtz
#define CFG_FILE_TUNES  CFG_DIR_DB"/tunes.json"  // /uzx
#define CFG_FILE_TVS    CFG_DIR_DB"/tvs.json"    // /tvs
#define CFG_FILE_ZWITTS CFG_DIR_DB"/zwitts.json" // /zwt

/* Navigation menu item struct */
typedef struct {
    const char *link;
    const char *name;
    const char *title;
} MenuItem;

/* Navigation menu items */
MenuItem menu_items[] = {
    {
        .link = "idx",
        .name = "Id×",
        .title = "№1"
    },
    {
        .link = "dat",
        .name = "D.ATA",
        .title = "73 74 6F 72 61 67 65"
    },
    {
        .link = "qtz",
        .name = "Qo⊕s",
        .title = "WARNING! Do not enter. Makes smarter!"
    },
    {
        .link = "eye",
        .name = "Pi××",
        .title = "\\/!züAL aЯ+z 6_||erY"
    },
    {
        .link = "uzx",
        .name = "μZx",
        .title = "Music is the soundtrack to our lives"
    },
    {
        .link = "tvs",
        .name = "+v-$нøρ",
        .title = "%$ale!!%"
    },
    {
        .link = "www",
        .name = "paтн",
        .title = "URL Rehab"
    },
    {
        .link = "art",
        .name = "©-nema",
        .title = "I've seen films"
    },
    {
        .link = "zwt",
        .name = "Zш¡++Ег",
        .title = "Captain's log"
    },
    {
        .link = "cmd",
        .name = "cMD",
        .title = "root@paranoia:/bin#"
    },
    {
        .link = "inf",
        .name = "Ii!|⌉l",
        .title = "Me, Myself, and I"
    },
};

/*
('newz', 'ИЕШZ', 'Bræking NeШz', b'0', 3, b'0'),
('tools', '†∞lz', '╓╦╦─', b'0', 8, b'1'),
('freunde', 'Fгeпzy', 'Fellow Fish', b'0', 9, b'1'),
('fanmail', 'Koптакт', 'Love Letters', b'0', 13, b'1');
*/
