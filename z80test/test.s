    OUTPUT "test.bin"
    DEVICE NONE

    .org $0205

    ld hl, string
    ld bc, $f800
main:
    ld a, (hl) 
    cp 0
    jp z, loop

    ld (bc),a
    inc hl
    inc bc
    jr main

loop:
    jp loop

string:
    db "Een test, om te kijken!",0
