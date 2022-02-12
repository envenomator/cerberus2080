// C include file for console.s functions
// 
extern void __fastcall__ con_init(); // needed before any console IO
extern void __fastcall__ con_cls();
extern void __fastcall__ con_puts(char *str);
extern void __fastcall__ con_putc(char);
extern void __fastcall__ con_gotoxy(unsigned char x, unsigned char y);
extern char __fastcall__ con_getc(); // blocked waiting
