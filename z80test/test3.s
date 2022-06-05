    OUTPUT "test.bin"
    DEVICE NONE

    .org $0205

main:
    call con_start

    ld hl, string
    call con_puts
    ld hl, string2
    call con_puts

loop:
    jp loop

con_putc:
    ; A - single character to print
    push hl
    ld hl, (con_pos)    ; load 16-bit pointer from position variable
    ld (hl), a      ; output character to (pos)
    inc hl          ; next position
    ld (con_pos), hl    ; store position
    pop hl
    ret

con_puts:
    ; HL - pointer to zero-terminated string
    ld a, (hl)
    cp 0
    ret z
    inc hl
    call con_putc
    jr con_puts

con_start:
    ; initialize console variable
    push hl
    push bc
    ld hl, con_pos      ; position variable
    ld bc, $f800    ; start of screen
    ld (hl), bc     ; store start of screen
    pop bc
    pop hl
    ret

string:
    db "Een Test, om te kijken",0
string2:
    db " of het werkt.",0
con_pos:
    dw 0
