    .setcpu "65C02"

VIDSTART = $F800
SCREENWIDTH  = 40
SCREENHEIGHT = 30

    .exportzp xpos,ypos,conptr

    .zeropage
;xpos     = $04
;ypos     = $05
;conptr   = $06
;strptr   = $08
xpos:    .res 1
ypos:    .res 1
conptr:  .res 2
strptr:  .res 2

    .code
    
main:
    ; init stack
    ldx #$ff
    txs

    ; console init
    jsr con_init

    ; print string

    lda #'x'
    jsr con_printchar
    ldy #0
    ldx #39
    jsr con_gotoxy
    jsr con_printchar
    ldy #29
    jsr con_gotoxy
    jsr con_printchar
    ldx #0
    jsr con_gotoxy
    jsr con_printchar
    ldx #19
    ldy #14
    jsr con_gotoxy
    jsr con_printchar

end:
    jmp end

con_init:
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

con_cls:
    ; Fill the entire screen with empty tile (space)
    ; and reset console to 0,0
    pha
    phx
    phy
    jsr con_init

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
    
    jsr con_init
    ply
    plx
    pla
    rts

con_gotox:
    pha
    phx
    phy
    ldy ypos
    jsr con_gotoxy
    ply
    plx
    pla
    rts

con_gotoxy:
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

con_print:
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
    jsr con_init
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

    .data
hello:
    ;.asciiz "Hello world!, dit is een voorbeeld van een string die duidelijk te lang is voor een 40-character display. Ik ben benieuwd hoeveel regels dit in beslag gaat nemen op het nieuwe scherm. We zullen zien."
    ;.byte "Hello world!", $0d, $0d, "Test",0
    ;.byte "0",$0d,"40",$0d,"80",$0d,"120",$0d,"160",$0d,"200",$0d,"01234567890123456",$0d,"Test",0
    ;.byte "0",$0d,"40",$0d,"80",$0d,"120",$0d,"160",$0d,"200",$0d,"012345678901234567890",$0d,"Test",0
    .asciiz "Hello world!"
crlf:
    .byte $0d,0
