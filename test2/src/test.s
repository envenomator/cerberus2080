    .setcpu "65C02"
    .code

temp    = $01

    ; init stack
    ldx $99
    txs

main:
    jsr GETIN
    jsr cleartiles
    bra main

GETIN:
    lda $0200  ; mail flag
    cmp #$01    ; character received?
    bne GETIN   ; blocked wait for character
    stz $0200  ; acknowledge receive
    lda $0201  ; receive the character from the mailbox slot
    rts

cleartiles:
    ; Fill the entire screen with empty tile (space)
    ; custom code for this example
    pha

    lda #$0
    sta temp            ; low byte to temp
    lda #$f8
    sta temp+1          ; high byte to temp

    ldx #$0
    pla
@outer:
    ;lda #$32            ; space character
    ldy #$0
@inner:
    sta (temp),y
    iny
    cpy #40
    bne @inner          ; next column
    
    pha     ; save value

    clc
    lda temp
    adc #40             ; next row
    sta temp
    bcc @nexttemp
    lda temp+1
    adc #$0             ; add the carry (1) to the high byte
    sta temp+1

@nexttemp:
    pla     ; recover value

    inx
    cpx #30

    bne @outer
    rts
