#ifndef _CURSOR_XPM_H_
#define _CURSOR_XPM_H_

static char *cursor_xpm[] = {
// "columns rows colors chars-per-pixel"
"16 16 4 1",
" 	c None",
".	c #000000", // black outline
"+	c #FFFFFF", // white fill
"@	c #7F7F7F", // gray shadow 
// pixels
".               ",
"..              ",
".+.             ",
".++.            ",
".+++.           ",
".++++.          ",
".+++++.         ",
".++++++.        ",
".+++++++.       ",
".++++++++.      ",
".++.......      ",
".+.+.            ",
".. .+.           ",
"    .+.          ",
"     ..          ",
"                 "
};

static char *cursor_inverted_xpm[] = {
"16 16 4 1",
" 	c None",
".	c #FFFFFF", // white outline
"+	c #000000", // black fill
"@	c #9F9F9F", // light gray shadow

".               ",
"..              ",
".+.             ",
".++.            ",
".+++.           ",
".++++.          ",
".+++++.         ",
".++++++.        ",
".+++++++.       ",
".++++++++.      ",
".++.......      ",
".+.+.            ",
".. .+.           ",
"    .+.          ",
"     ..          ",
"                 "
};

static char *cursor_hand_xpm[] = {
"16 16 4 1",
" 	c None",
".	c #000000", // black outline
"+	c #FFFFFF", // white fill 
"@	c #7F7F7F", // gray shadow 

"      ...       ",
"     .+++.      ",
"     .+++.      ",
"     .+++.      ",
"     .+++.      ",
"     .+++. ..   ",
" ... .+++..+.   ",
".+@+..+++++.    ",
".+@++.+++++.    ",
" .+@++++++++.   ",
"  .+++++++++.   ",
"  .+++++++++.   ",
"   .++++++++.   ",
"   .++++++++.   ",
"    ........    ",
"                "
};

#endif /* _CURSOR_XPM_H_ */