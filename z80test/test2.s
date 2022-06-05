    OUTPUT "test.bin"
    DEVICE NONE

    .org $0205

    ld hl, string
    ld de, $f800
    ld b, 8
main:
    ld a, (hl) 
    or $20
    ld (de), a
    inc hl
    inc de
    djnz main

loop:
    jp loop

string:
    db "Een Test, om te kijken!",0
