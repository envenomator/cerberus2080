    .setcpu "65C02"

VIDSTART = $F800
SCREENWIDTH  = 40
SCREENHEIGHT = 30

    .exportzp xpos,ypos,conptr
    .export  _con_putc, _con_puts, _con_getc
    .export  _con_init, _con_cls, _con_gotoxy
    .globalzp sp
    .import popa,popax,incsp2,decsp2

    .zeropage
xpos:    .res 1
ypos:    .res 1
conptr:  .res 2
strptr:  .res 2

    .code
   
.proc _con_puts: near
    ; C __fastcall__ A/X ptr to print
    sta strptr
    stx strptr+1
    jsr con_print
    rts
.endproc

.proc _con_init: near
_con_init:
    ; C COMPLIANT
    ; initializes the console variables
    ; reset to X,Y = 0,0
    pha
    phx
    phy
    stz xpos
    stz ypos
    lda #<VIDSTART
    sta conptr
    lda #>VIDSTART
    sta conptr+1
    ply
    plx
    pla
    rts
.endproc

.proc _con_cls: near
_con_cls:
    ; C COMPLIANT
    ; Fill the entire screen with empty tile (space)
    ; and reset console to 0,0
    pha
    phx
    phy
    jsr _con_init

    ldx #$0
@outer:
    lda #' '            ; space character
    ldy #$0
@inner:
    sta (conptr),y
    iny
    cpy #SCREENWIDTH
    bne @inner          ; next column
    clc
    lda conptr
    adc #SCREENWIDTH             ; next row
    sta conptr
    bcc @next
    lda conptr+1
    adc #$0             ; add the carry (1) to the high byte
    sta conptr+1
@next:
    inx
    cpx #SCREENHEIGHT
    bne @outer
    
    jsr _con_init
    ply
    plx
    pla
    rts
.endproc

.proc _con_gotoxy
    ; C compliant code
    pha         ; Y argument
    ldy #0
    lda (sp),y  ; X argument from the stack - unsigned char
    tax         ; X argument in register x
    ply
    jsr con_gotoxy
    jsr popa    ; stack cleanup
    rts
.endproc

.proc _con_putc
    ; C compliant code - fastcalled
    ; A register contains character to put
    jsr con_printchar
    rts
.endproc

con_gotox:
    ; Assembly only call
    pha
    phx
    phy
    ldy ypos
    jsr _con_gotoxy
    ply
    plx
    pla
    rts

con_gotoxy:
    ; Assembly only call
    ; input .x == x position
    ; input .y == y position
    pha
    phx
    phy
    cpx #SCREENWIDTH
    bcs @done           ; >= to WIDTH, set carry and exit
    cpy #SCREENHEIGHT
    bcs @done           ; >= to HEIGHT, set carry and exit
    stx xpos
    sty ypos

    lda #SCREENWIDTH
    sta conptr      ; FAC1 == conptr(low), FAC2 = ypos. FAC1 gets clobbered to final low byte of result
    ; multiply ypos * SCREENWIDTH, store in conptr
@mul8:
    lda #$00
    ldx #$08
    clc
@m0:
    bcc @m1
    clc
    adc ypos
@m1:
    ror
    ror conptr
    dex
    bpl @m0
    sta conptr+1
    ; result now in conptr / conptr+1
    ; Add both video start address (F800) and xpos to conptr.
    ; As xpos <40, we can use the low byte immediately
    clc
    lda conptr
    adc xpos
    sta conptr
    lda conptr+1
    adc #$F8
    sta conptr+1
@done:
    ply
    plx
    pla
    rts

_con_getc:
    ; C COMPLIANT
    lda $0200           ; mail flag
    cmp #$01            ; character received?
    bne _con_getc       ; blocked wait for character
    stz $0200  ; acknowledge receive
    lda $0201  ; receive the character from the mailbox slot
    ldx #$0    ; 16-bit promotion of high-byte as return value to C
    rts

con_print:
    ; Assembly only call
    ; prints zero-terminated string pointed to by strptr in zeropage
    pha
    phx
    phy

    ldy #0
@loop:
    lda (strptr),y
    beq @done
    jsr con_printchar 
    iny
    bra @loop
@done:
    ply
    plx
    pla
    rts

con_printchar:
    ; Assembly only call
    ; prints character from A to the current X,Y coordinate in zeropage
    ; X,Y is always a previously checked valid coordinate
    pha
    phx
    phy

    cmp #$d ; CR
    beq @CRLF
    cmp #$a ; LF
    beq @CRLF

    ; print normally
    ldy #0
    sta (conptr),y
    ; update position and check validity
    ; wrap around at end of screen to 0,0
    ; X = X + 1
    lda xpos
    cmp #SCREENWIDTH-1
    beq @CRLF
    clc
    adc #1
    sta xpos
    bra @nextptr
@CRLF:
    lda #SCREENWIDTH
    sec
    sbc xpos            ; move down SCREENWIDTH - xpos characters
    clc
    adc conptr          ; add to low byte of pointer
    sta conptr
    lda conptr+1
    adc #0
    sta conptr+1        ; add to high byte of pointer

    ; now reset x and check y next
    stz xpos
    lda ypos
    cmp #SCREENHEIGHT-1
    bne @nextrow
    ; return to 0,0
    jsr _con_init
    bra @done
@nextrow:
    inc ypos
    bra @done
@nextptr:
    lda conptr
    clc
    adc #1
    sta conptr
    bcc @done
    lda conptr+1
    adc #0
    sta conptr+1
@done:
    ply
    plx
    pla
    rts
