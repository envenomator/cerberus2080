  .setcpu "65C02"

  .code

init:
  lda #<break
  sta $fffe
  lda #>break
  sta $ffff
  cli
  brk
loop:
  jmp loop

break:
  lda #'A'
  sta $f800
  rti
