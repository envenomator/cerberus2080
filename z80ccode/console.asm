;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.8.0 #10562 (Linux)
;--------------------------------------------------------
	.module console
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _console
	.globl _con_init
	.globl _con_gotoxy
	.globl _con_cls
	.globl _putc
	.globl _puts
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
G$console$0_0$0==.
_console::
	.ds 2
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
	G$con_init$0$0	= .
	.globl	G$con_init$0$0
	C$console.c$5$0_0$4	= .
	.globl	C$console.c$5$0_0$4
;console.c:5: void con_init()
;	---------------------------------
; Function con_init
; ---------------------------------
_con_init::
	C$console.c$7$1_0$4	= .
	.globl	C$console.c$7$1_0$4
;console.c:7: console = videostart;
	ld	hl, #0xf800
	ld	(_console), hl
	C$console.c$8$1_0$4	= .
	.globl	C$console.c$8$1_0$4
;console.c:8: return;
	C$console.c$9$1_0$4	= .
	.globl	C$console.c$9$1_0$4
;console.c:9: }
	C$console.c$9$1_0$4	= .
	.globl	C$console.c$9$1_0$4
	XG$con_init$0$0	= .
	.globl	XG$con_init$0$0
	ret
	G$con_gotoxy$0$0	= .
	.globl	G$con_gotoxy$0$0
	C$console.c$11$1_0$6	= .
	.globl	C$console.c$11$1_0$6
;console.c:11: void con_gotoxy(unsigned char x, unsigned char y)
;	---------------------------------
; Function con_gotoxy
; ---------------------------------
_con_gotoxy::
	C$console.c$13$1_0$6	= .
	.globl	C$console.c$13$1_0$6
;console.c:13: console = videostart + (y*SCREENWIDTH) + x;
	ld	hl, #3+0
	add	hl, sp
	ld	c, (hl)
	ld	b, #0x00
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, hl
	add	hl, hl
	ld	c, l
	ld	a, h
	add	a, #0xf8
	ld	b, a
	ld	a, c
	ld	hl, #2
	add	hl, sp
	ld	iy, #_console
	add	a, (hl)
	ld	0 (iy), a
	ld	a, b
	adc	a, #0x00
	inc	iy
	ld	0 (iy), a
	C$console.c$14$1_0$6	= .
	.globl	C$console.c$14$1_0$6
;console.c:14: return;
	C$console.c$15$1_0$6	= .
	.globl	C$console.c$15$1_0$6
;console.c:15: }
	C$console.c$15$1_0$6	= .
	.globl	C$console.c$15$1_0$6
	XG$con_gotoxy$0$0	= .
	.globl	XG$con_gotoxy$0$0
	ret
	G$con_cls$0$0	= .
	.globl	G$con_cls$0$0
	C$console.c$17$1_0$7	= .
	.globl	C$console.c$17$1_0$7
;console.c:17: void con_cls()
;	---------------------------------
; Function con_cls
; ---------------------------------
_con_cls::
	C$console.c$19$2_0$8	= .
	.globl	C$console.c$19$2_0$8
;console.c:19: for(console = videostart; console < videoend; console++)
	ld	hl, #0xf800
	ld	(_console), hl
00103$:
	ld	iy, #_console
	ld	a, 0 (iy)
	sub	a, #0xb0
	ld	a, 1 (iy)
	sbc	a, #0xfc
	jr	NC,00101$
	C$console.c$21$3_0$9	= .
	.globl	C$console.c$21$3_0$9
;console.c:21: *console = ' ';
	ld	hl, (_console)
	ld	(hl), #0x20
	C$console.c$19$2_0$8	= .
	.globl	C$console.c$19$2_0$8
;console.c:19: for(console = videostart; console < videoend; console++)
	inc	0 (iy)
	jr	NZ,00103$
	inc	1 (iy)
	jr	00103$
00101$:
	C$console.c$23$1_0$7	= .
	.globl	C$console.c$23$1_0$7
;console.c:23: console = videostart;
	ld	hl, #0xf800
	ld	(_console), hl
	C$console.c$24$1_0$7	= .
	.globl	C$console.c$24$1_0$7
;console.c:24: return;
	C$console.c$25$1_0$7	= .
	.globl	C$console.c$25$1_0$7
;console.c:25: }
	C$console.c$25$1_0$7	= .
	.globl	C$console.c$25$1_0$7
	XG$con_cls$0$0	= .
	.globl	XG$con_cls$0$0
	ret
	G$putc$0$0	= .
	.globl	G$putc$0$0
	C$console.c$26$1_0$11	= .
	.globl	C$console.c$26$1_0$11
;console.c:26: void putc(char c)
;	---------------------------------
; Function putc
; ---------------------------------
_putc::
	C$console.c$28$1_0$11	= .
	.globl	C$console.c$28$1_0$11
;console.c:28: *console = c;
	ld	hl, (_console)
	ld	iy, #2
	add	iy, sp
	ld	a, 0 (iy)
	ld	(hl), a
	C$console.c$29$1_0$11	= .
	.globl	C$console.c$29$1_0$11
;console.c:29: console++;
	ld	iy, #_console
	inc	0 (iy)
	jr	NZ,00110$
	inc	1 (iy)
00110$:
	C$console.c$30$1_0$11	= .
	.globl	C$console.c$30$1_0$11
;console.c:30: if(console == videoend) 
	ld	a, 0 (iy)
	sub	a, #0xb0
	ret	NZ
	ld	a, 1 (iy)
	sub	a, #0xfc
	ret	NZ
	C$console.c$31$1_0$11	= .
	.globl	C$console.c$31$1_0$11
;console.c:31: console = videostart;
	ld	hl, #0xf800
	ld	(_console), hl
	C$console.c$32$1_0$11	= .
	.globl	C$console.c$32$1_0$11
;console.c:32: return;
	C$console.c$33$1_0$11	= .
	.globl	C$console.c$33$1_0$11
;console.c:33: }
	C$console.c$33$1_0$11	= .
	.globl	C$console.c$33$1_0$11
	XG$putc$0$0	= .
	.globl	XG$putc$0$0
	ret
	G$puts$0$0	= .
	.globl	G$puts$0$0
	C$console.c$35$1_0$13	= .
	.globl	C$console.c$35$1_0$13
;console.c:35: void puts(const char *s)
;	---------------------------------
; Function puts
; ---------------------------------
_puts::
	C$console.c$37$1_0$13	= .
	.globl	C$console.c$37$1_0$13
;console.c:37: while(*s)
	pop	de
	pop	bc
	push	bc
	push	de
00101$:
	ld	a, (bc)
	or	a, a
	ret	Z
	C$console.c$39$2_0$14	= .
	.globl	C$console.c$39$2_0$14
;console.c:39: putc(*s);
	push	bc
	push	af
	inc	sp
	call	_putc
	inc	sp
	pop	bc
	C$console.c$40$2_0$14	= .
	.globl	C$console.c$40$2_0$14
;console.c:40: s++;
	inc	bc
	C$console.c$42$1_0$13	= .
	.globl	C$console.c$42$1_0$13
;console.c:42: return;
	C$console.c$43$1_0$13	= .
	.globl	C$console.c$43$1_0$13
;console.c:43: }
	C$console.c$43$1_0$13	= .
	.globl	C$console.c$43$1_0$13
	XG$puts$0$0	= .
	.globl	XG$puts$0$0
	jr	00101$
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
