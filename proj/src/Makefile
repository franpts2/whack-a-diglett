# name of the program (Minix service)
PROG=proj

# Add search paths for source files in subdirectories
.PATH: ${.CURDIR}/controllers
.PATH: ${.CURDIR}/controllers/timer
.PATH: ${.CURDIR}/controllers/kbdmouse
.PATH: ${.CURDIR}/controllers/video
.PATH: ${.CURDIR}/game
.PATH: ${.CURDIR}/game/modes
.PATH: ${.CURDIR}/game/modes/playing-modes
.PATH: ${.CURDIR}/game/cursor
.PATH: ${.CURDIR}/game/sprites
.PATH: ${.CURDIR}/utils
.PATH: ${.CURDIR}/fonts

# List all your source files (just the filenames, not the paths)
SRCS = main.c timer.c keyboard.c kbc.c aux.c utils.c video.c game.c menu.c testfont.c cursor.c sprite.c playing.c playing_kbd.c playing_mouse.c choose_mode.c animated_sprite.c pause.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies (for LCOM projects)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>