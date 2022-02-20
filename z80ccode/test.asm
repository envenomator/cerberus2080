;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.8.0 #10562 (Linux)
;--------------------------------------------------------
	.module test
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _puts
	.globl _con_cls
	.globl _con_gotoxy
	.globl _con_init
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
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
	G$main$0$0	= .
	.globl	G$main$0$0
	C$test.c$3$0_0$4	= .
	.globl	C$test.c$3$0_0$4
;test.c:3: int main()
;	---------------------------------
; Function main
; ---------------------------------
_main::
	C$test.c$5$2_0$4	= .
	.globl	C$test.c$5$2_0$4
;test.c:5: const char *welcome = "Hello world!";
	C$test.c$8$1_0$4	= .
	.globl	C$test.c$8$1_0$4
;test.c:8: con_init();
	call	_con_init
	C$test.c$9$1_0$4	= .
	.globl	C$test.c$9$1_0$4
;test.c:9: con_gotoxy(10,10);
	ld	de, #0x0a0a
	push	de
	call	_con_gotoxy
	C$test.c$10$1_0$4	= .
	.globl	C$test.c$10$1_0$4
;test.c:10: puts(welcome);
	ld	hl, #___str_0
	ex	(sp),hl
	call	_puts
	pop	af
	C$test.c$11$1_0$4	= .
	.globl	C$test.c$11$1_0$4
;test.c:11: con_cls();
	call	_con_cls
	C$test.c$12$1_0$4	= .
	.globl	C$test.c$12$1_0$4
;test.c:12: while(1);
00102$:
	C$test.c$13$1_0$4	= .
	.globl	C$test.c$13$1_0$4
;test.c:13: }
	C$test.c$13$1_0$4	= .
	.globl	C$test.c$13$1_0$4
	XG$main$0$0	= .
	.globl	XG$main$0$0
	jr	00102$
Ftest$__str_0$0_0$0 == .
___str_0:
	.ascii "Hello world!"
	.db 0x00
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
