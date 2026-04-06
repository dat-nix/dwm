/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx = 3; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const int swallowfloating =
    0; /* 1 means swallow floating windows by default */
static const unsigned int gappih = 20; /* horiz inner gap between windows */
static const unsigned int gappiv = 10; /* vert inner gap between windows */
static const unsigned int gappoh =
    10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov =
    30; /* vert outer gap between windows and screen edge */
static const int smartgaps =
    0; /* 1 means no outer gap when there is only one window */
static const int showbar = 1;     /* 0 means no bar */
static const int topbar = 1;      /* 0 means bottom bar */
static const int horizpadbar = 2; /* horizontal padding for statusbar */
static const int vertpadbar = 0;  /* vertical padding for statusbar */

static const char *fonts[] = {
    "JetBrainsMono Nerd Font:size=11",
    "NotoColorEmoji:pixelsize=12:antialias=true:autohint=true"};
static const char dmenufont[] = "monospace:size=11";

/* Oxocarbon */
static const char col_bg[] = "#161616";
static const char col_bg_alt[] = "#262626";
static const char col_border[] = "#393939";
static const char col_fg[] = "#dde1e6";
static const char col_fg_dim[] = "#a8b0b8";
static const char col_blue[] = "#33b1ff";
static const char col_magenta[] = "#be95ff";
static const char col_red[] = "#ee5396";

static const char *colors[][3] = {
    /*               fg          bg          border      */
    [SchemeNorm] = {col_fg_dim, col_bg, col_border},
    [SchemeSel] = {col_fg, col_bg_alt, col_blue},
};

typedef struct {
  const char *name;
  const void *cmd;
} Sp;

static const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL};

static const char *spcmd2[] = {
    "st", "-n",    "spcalc", "-f", "JetBrainsMono Nerd Font:size=16",
    "-g", "50x20", "-e",     "bc", "-lq",
    NULL};

static const char *spcmd3[] = {
    "st",
    "-n",
    "spnote",
    "-g",
    "100x28",
    "-e",
    "sh",
    "-c",
    "mkdir -p ~/notes && cd ~/notes && printf 'Note name: ' && read name && [ "
    "-n \"$name\" ] && nvim \"$name.md\"",
    NULL};

static Sp scratchpads[] = {
    /* name      cmd    */
    {"spterm", spcmd1},
    {"spcalc", spcmd2},
    {"spnote", spcmd3},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     */
    /* class         instance      title           tags mask  isfloating
       isterminal  noswallow  monitor */
    {"Gimp", NULL, NULL, 0, 1, 0, 0, -1},
    {"Firefox", NULL, NULL, 1 << 8, 0, 0, 0, -1},

    /* generic st rule first */
    {"st-256color", NULL, NULL, 0, 0, 1, 0, -1},

    /* multiple scratchpads */
    {NULL, "spterm", NULL, SPTAG(0), 1, 1, 0, -1},
    {NULL, "spcalc", NULL, SPTAG(1), 1, 1, 0, -1},
    {NULL, "spnote", NULL, SPTAG(2), 1, 1, 0, -1},

    {NULL, NULL, "Event Tester", 0, 0, 0, 1, -1}, /* xev */
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

#include "fibonacci.c"
#include <X11/XF86keysym.h>
#define FORCE_VSPLIT                                                           \
  1 /* nrowgrid layout: force two clients to always split vertically */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile}, /* first entry is default */
    {"TTT", bstack},
    /* fibonacci layouts */
    {"[@]", spiral},
    {"[\\]", dwindle},

    {"[D]", deck},
    {"[M]", monocle},

    {"|M|", centeredmaster},         /* Master in middle, slaves on sides */
    {">M>", centeredfloatingmaster}, /* Same but master floats */

    {"><>", NULL}, /* floating */
    {NULL, NULL},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

#define STACKKEYS(MOD, ACTION)                                                 \
  {MOD, XK_j, ACTION##stack, {.i = INC(+1)}},                                  \
      {MOD, XK_k, ACTION##stack, {.i = INC(-1)}},                              \
      {MOD, XK_q, ACTION##stack, {.i = 0}},                                    \
      {MOD, XK_a, ACTION##stack, {.i = 1}},                                    \
      {MOD, XK_z, ACTION##stack, {.i = 2}},                                    \
      {MOD, XK_x, ACTION##stack, {.i = -1}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */

static const char *dmenucmd[] = {"dmenu_run", "-m",  dmenumon,   "-nb",
                                 col_bg,      "-nf", col_fg_dim, "-sb",
                                 col_bg_alt,  "-sf", col_blue,   NULL};

static const char *termcmd[] = {"st", NULL};

static const Key keys[] = {
    /* modifier                     key        function        argument */
    {MODKEY, XK_p, spawn, {.v = dmenucmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_b, togglebar, {0}},

    STACKKEYS(MODKEY, focus) STACKKEYS(MODKEY | ShiftMask, push)

        {MODKEY, XK_bracketright, incnmaster, {.i = +1}},
    {MODKEY, XK_bracketleft, incnmaster, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY | Mod1Mask, XK_z, incrgaps, {.i = +3}},
    {MODKEY | Mod1Mask, XK_x, incrgaps, {.i = -3}},
    {MODKEY | Mod1Mask, XK_a, defaultgaps, {0}},
    {MODKEY | Mod1Mask, XK_0, togglegaps, {0}},

    /* scratchpads */
    {MODKEY, XK_grave, togglescratch, {.ui = 0}}, /* terminal */
    {0, XK_F4, togglescratch, {.ui = 1}},         /* calculator */
    {0, XK_F9, togglescratch, {.ui = 2}},         /* notes */

    {MODKEY | ShiftMask, XK_l, spawn, SHCMD("~/.local/bin/powermenu")},

    /* chosen app shortcuts */
    {MODKEY, XK_w, spawn, SHCMD("firefox")},
    {MODKEY, XK_e, spawn, SHCMD("thunar")},
    {MODKEY, XK_c, spawn, SHCMD("code")},

    {MODKEY, XK_space, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY | ShiftMask, XK_c, killclient, {0}},

    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},             /* tile */
    {MODKEY | ShiftMask, XK_t, setlayout, {.v = &layouts[1]}}, /* bstack */
    {MODKEY, XK_y, setlayout, {.v = &layouts[2]}},             /* spiral */
    {MODKEY | ShiftMask, XK_y, setlayout, {.v = &layouts[3]}}, /* dwindle */
    {MODKEY, XK_u, setlayout, {.v = &layouts[4]}},             /* deck */
    {MODKEY | ShiftMask, XK_u, setlayout, {.v = &layouts[5]}}, /* monocle */
    {MODKEY, XK_i, setlayout, {.v = &layouts[6]}}, /* centeredmaster */
    {MODKEY | ShiftMask,
     XK_i,
     setlayout,
     {.v = &layouts[7]}}, /* centeredfloatingmaster */
    {MODKEY | ShiftMask, XK_f, setlayout, {.v = &layouts[8]}}, /* floating */

    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_f, togglefullscr, {0}},
    {MODKEY, XK_0, view, {.ui = ~0}},

    /* thinkpad extras */
    {0, XK_F8, spawn, SHCMD("~/.local/bin/toggle-airplane")},
    {0, XK_F12, spawn, SHCMD("~/.local/bin/dmenuunicode")},

    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8)

                {MODKEY | ShiftMask, XK_BackSpace, quit, {1}},

    {MODKEY, XK_s, togglesticky, {0}},
    {MODKEY, XK_g, shiftview, {.i = -1}},
    {MODKEY, XK_semicolon, shiftview, {.i = +1}},

    /* Volume (PipeWire + notify) */
    {0, XF86XK_AudioRaiseVolume, spawn,
     SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+ && dunstify -r 9992 -t "
           "1000 '🔊 Volume' \"$(wpctl get-volume @DEFAULT_AUDIO_SINK@ | awk "
           "'{print int($2*100)}')%\"")},

    {0, XF86XK_AudioLowerVolume, spawn,
     SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%- && dunstify -r 9992 -t "
           "1000 '🔉 Volume' \"$(wpctl get-volume @DEFAULT_AUDIO_SINK@ | awk "
           "'{print int($2*100)}')%\"")},

    {0, XF86XK_AudioMute, spawn,
     SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle && dunstify -r 9992 -t "
           "1000 '🔇 Volume' 'Mute toggled'")},

    /* Microphone + notify */
    {0, XF86XK_AudioMicMute, spawn,
     SHCMD("wpctl set-mute @DEFAULT_AUDIO_SOURCE@ toggle && dunstify -r 9993 "
           "-t 1000 '🎤 Mic' 'Toggle mic'")},

    /* Brightness (OSD) */
    {0, XF86XK_MonBrightnessUp, spawn,
     SHCMD("brightnessctl set +10% && dunstify -r 9991 -t 1000 '☀️ Brightness' "
           "\"$(brightnessctl info | grep -oP '\\(\\K[0-9]+(?=%)')%\"")},

    {0, XF86XK_MonBrightnessDown, spawn,
     SHCMD("brightnessctl set 10%- && dunstify -r 9991 -t 1000 '🌙 Brightness' "
           "\"$(brightnessctl info | grep -oP '\\(\\K[0-9]+(?=%)')%\"")},

    {MODKEY, XK_n, spawn, SHCMD("dunstctl history-pop")},
    {MODKEY | ShiftMask, XK_n, spawn, SHCMD("dunstctl close-all")},
    {MODKEY | ShiftMask, XK_r, spawn, SHCMD("~/.local/bin/recordmenu")},
    {0, XK_F7, spawn, SHCMD("~/.local/bin/monitor-menu")},
    {MODKEY, XK_o, spawn, SHCMD("~/.local/bin/webmenu")},
    {MODKEY, XK_m, spawn, SHCMD("~/.local/bin/usbmenu")},
    {MODKEY, XK_d, spawn, SHCMD("~/.local/bin/toggle-dnd")},

    {MODKEY, XK_v, spawn,
     SHCMD("greenclip print | grep . | dmenu -i -l 10 -p clipboard | xargs -r "
           "-d '\\n' -I '{}' greenclip print '{}'")},

    {0, XK_Print, spawn, SHCMD("flameshot gui -p ~/Pictures/Screenshots")},
    {ShiftMask, XK_Print, spawn,
     SHCMD("flameshot full -p ~/Pictures/Screenshots")},
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
