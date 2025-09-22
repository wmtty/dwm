/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 2; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const unsigned int gappih = 4;   /* horiz inner gap between windows */
static const unsigned int gappiv = 4;   /* vert inner gap between windows */
static const unsigned int gappoh =
    4; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov =
    4; /* vert outer gap between windows and screen edge */
static int smartgaps =
    0; /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning =
    0; /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor
          X */
static const unsigned int systrayonleft =
    0; /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2; /* systray spacing */
static const int systraypinningfailfirst =
    1; /* 1: if pinning fails, display systray on the first monitor, False:
          display systray on the last monitor*/
static const int showsystray = 1; /* 0 means no systray */
static const int vertpad            = 2;       /* vertical padding of bar */
static const int sidepad            = 2;       /* horizontal padding of bar */
static const int showbar = 1;     /* 0 means no bar */
static const int topbar = 1;      /* 0 means bottom bar */
static const char *fonts[] = {
    /* "JetBrainsMono Nerd Font Propo:size=9:style=Medium"}; */
    "ComicShannsMono Nerd Font Propo:size=10:style=Medium"};
static const char dmenufont[] = "JetBrainsMono Nerd Font:size=9:style=Medium";
#include "/home/ak/.config/suckless/dwm/themes/nord.h" /* Importing Theme */
static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {col_gray3, col_gray1, col_gray2},
    [SchemeSel] = {col_gray1, col_cyan, col_cyan},
};

typedef struct {
    const char *name;
    const void *cmd;
} Sp;

const char *spcmd1[] = {"alacritty",
                        "--class",
                        "sterm",
                        "-o",
                        "window.dimensions.columns=150",
                        "-o",
                        "window.dimensions.lines=38",
                        NULL};
const char *spcmd2[] = {"alacritty",
                        "--class",
                        "srmpc",
                        "-o",
                        "window.dimensions.columns=150",
                        "-o",
                        "window.dimensions.lines=38",
                        "-e",
                        "rmpc",
                        NULL};
const char *spcmd3[] = {"thorium",
                        "--class=sgpt",
                        "--name=sgpt",
                        "--app=http://chatgpt.com",
                        "--new-window",
                        "--user-data-dir=$HOME/.local/thorium-profiles/gpt",
                        NULL};

const char *spcmd4[] = {"emacsclient", "-c", "-a", "emacs", NULL};

const char *spcmd5[] = {"thorium",
                        "--class=schess",
                        "--name=schess",
                        "--app=http://chess.com",
                        "--new-window",
                        "--user-data-dir=$HOME/.local/thorium-profiles/chess",
                        NULL};

static Sp scratchpads[] = {
    /* name          cmd  */
    {"sterm", spcmd1},   {"srmpc", spcmd2},  {"ChatGPT", spcmd3},
    {"stemacs", spcmd4}, {"schess", spcmd5},
};

static const char *tagsel[][2] = {
   /*   fg         bg    */
  { col_gray2, col_gray1 }, /* norm */
  { col_gray1, col_cyan2  }, /* sel */
  { col_cyan2,  col_gray1 }, /* occ but not sel */
  { col_cyan,  col_gray3 }, /* has pinned tag */
};
/* tagging */
static const char *tags[] = {"󰣨", "",  "", "", "",
                             "",  "󰭹", "", ""};
static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 0, -1},
    {"Firefox", NULL, NULL, 1 << 8, 0, -1},
    {NULL, "sterm", NULL, SPTAG(0), 1, -1},
    {NULL, "srmpc", NULL, SPTAG(1), 1, -1},
    {"sgpt", "chatgpt.com", NULL, SPTAG(2), 1, -1},
    {"schess", "chess.com", NULL, SPTAG(2), 1, -1},
    {"Blueman-manager", NULL, NULL, 0, 1, -1},
    {"mpv", NULL, NULL, 0, 0, -1},
    {"iwgtk", "iwgtk", NULL, 0, 1, -1},
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */
static const int refreshrate =
    120; /* refresh rate (per second) for client move/resize */

#define FORCE_VSPLIT                                                           \
    1 /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile}, /* first entry is default */
    {"[M]", monocle},
    {"[@]", spiral},
    {"[\\]", dwindle},
    {"H[]", deck},
    {"TTT", bstack},
    {"===", bstackhoriz},
    {"HHH", grid},
    {"###", nrowgrid},
    {"---", horizgrid},
    {":::", gaplessgrid},
    {"|M|", centeredmaster},
    {">M>", centeredfloatingmaster},
    {"><>", NULL}, /* no layout function means floating behavior */
    {NULL, NULL},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
    {MODKEY, KEY, view, {.ui = 1 << TAG}},                                     \
        {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},             \
        {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                      \
        {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
    {                                                                          \
        .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                   \
    }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */

static const char *dmenucmd[] = {
    "dmenu_run", "-m",  dmenumon,  "-fn", dmenufont, "-nb",
    col_gray1,   "-nf", col_gray3, "-sb", col_cyan,  "-sf",
    col_gray1,   "-c",  "-l",      "20",  NULL};

static const char *termcmd[] = {"alacritty", NULL};

/* Commands */
// Brightness Commands
static const char *brupcmd[] = {"light", "-A", "1", NULL};
static const char *brdowncmd[] = {"light", "-U", "1", NULL};
// Volume Commands
static const char *volume[3][5] = {
    {"wpctl", "set-volume", "@DEFAULT_SINK@", "0.05+", NULL},
    {"wpctl", "set-volume", "@DEFAULT_SINK@", "0.05-", NULL},
    {"wpctl", "set-mute", "@DEFAULT_SINK@", "toggle", NULL},
};
// float to tile, max to tile with same key
void toggle_tile_max(const Arg *arg) {
    if (selmon->lt[selmon->sellt]->arrange == &monocle)
        setlayout(&((Arg){.v = &layouts[0]})); // layouts[0] = tile
    else
        setlayout(&((Arg){.v = &layouts[1]})); // layouts[2] = monocle
}

void toggle_tile_float(const Arg *arg) {
    if (selmon->sel && selmon->sel->isfloating) {
        // Agar selected window floating hai -> tile layout
        setlayout(&((Arg){.v = &layouts[0]})); // layouts[0] = tile
        if (selmon->sel)
            selmon->sel->isfloating = 0;
        arrange(selmon);
    } else {
        // Agar tiled hai -> floating toggle
        togglefloating(NULL);
    }
}
//--------------------------------

#include "shiftview.c"
static const Key keys[] = {
    /* modifier                     key        function        argument */
    {MODKEY, XK_e, spawn, {.v = dmenucmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_h, setcfact, {.f = +0.25}},
    {MODKEY | ShiftMask, XK_l, setcfact, {.f = -0.25}},
    {MODKEY | ShiftMask, XK_o, setcfact, {.f = 0.00}},
    {MODKEY | ShiftMask, XK_Return, zoom, {0}},
    // {MODKEY | Mod4Mask, XK_u, incrgaps, {.i = +1}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_u, incrgaps, {.i = -1}},
    // {MODKEY | Mod4Mask, XK_i, incrigaps, {.i = +1}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_i, incrigaps, {.i = -1}},
    // {MODKEY | Mod4Mask, XK_o, incrogaps, {.i = +1}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_o, incrogaps, {.i = -1}},
    // {MODKEY | Mod4Mask, XK_6, incrihgaps, {.i = +1}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_6, incrihgaps, {.i = -1}},
    // {MODKEY | Mod4Mask, XK_7, incrivgaps, {.i = +1}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_7, incrivgaps, {.i = -1}},
    // {MODKEY | Mod4Mask, XK_8, incrohgaps, {.i = +1}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_8, incrohgaps, {.i = -1}},
    // {MODKEY | Mod4Mask, XK_9, incrovgaps, {.i = +1}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_9, incrovgaps, {.i = -1}},
    // {MODKEY | Mod4Mask, XK_0, togglegaps, {0}},
    // {MODKEY | Mod4Mask | ShiftMask, XK_0, defaultgaps, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY, XK_q, killclient, {0}},
    // &layouts[0]} },
    {MODKEY, XK_f, toggle_tile_float, {0}},
    {MODKEY, XK_m, toggle_tile_max, {0}},
    { MODKEY|ShiftMask,                       XK_f,      setlayout,      {.v = &layouts[13]} },
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    {MODKEY, XK_t, togglescratch, {.ui = 0}},
    {MODKEY, XK_p, togglescratch, {.ui = 1}},
    {MODKEY, XK_c, togglescratch, {.ui = 2}},
    /* {MODKEY, XK_n, togglescratch, {.ui = 3}}, */
    {MODKEY, XK_o, togglescratch, {.ui = 4}},
    {MODKEY, XK_bracketleft, shiftview, {.i = -1}},
    {MODKEY, XK_bracketright, shiftview, {.i = +1}},
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8){MODKEY | ControlMask, XK_q, quit, {0}},
    {MODKEY | ControlMask | ShiftMask, XK_r, quit, {1}},

    // volume control
    {0, XF86XK_AudioRaiseVolume, spawn, {.v = volume[0]}},
    {0, XF86XK_AudioLowerVolume, spawn, {.v = volume[1]}},
    {0, XF86XK_AudioMute, spawn, {.v = volume[2]}},
    // brightness
    {0, XF86XK_MonBrightnessUp, spawn, {.v = brupcmd}},
    {0, XF86XK_MonBrightnessDown, spawn, {.v = brdowncmd}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
