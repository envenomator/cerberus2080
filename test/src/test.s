    .setcpu "65C02"

    .code

main:
    lda #'A'
    sta $f800

end:
    jmp end
