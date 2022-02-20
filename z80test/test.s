    OUTPUT "test.bin"
    DEVICE NONE

    .org $0205

main:
    ld a, (string) 
    ld ($f800),a

loop:
    jp loop

string:
    db "test",0
