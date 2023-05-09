
;============================================================================
; ALBUM.BIN
; card album overlay
;============================================================================
  
.open "out/asm/ALBUM.BIN", 0x80108000

  ;===============================================
  ; MODIFICATION:
  ; new card label concatenation
  ;===============================================
  
  .org 0x8010A4D0
    j setUpAlbumMemCardLabel
    nop
  
  ;===============================================
  ; MODIFICATION:
  ; resize and reposition main menu
  ;===============================================
  
  albumMainMenuWidthChange equ -16
  
  ; window x-pos
  .org 0x8010A1D8
    addiu $a2, $zero, 0x0050-(albumMainMenuWidthChange)
  
  ; window width
  .org 0x8010A1FC
    addiu $a1, $zero, 0x0060+(albumMainMenuWidthChange)

  ; text x-pos
  .org 0x8010A234
    addiu $a0, $zero, 0x0058-(albumMainMenuWidthChange)
  .org 0x8010A25C
    addiu $a0, $zero, 0x0058-(albumMainMenuWidthChange)
  .org 0x8010A288
    addiu $a0, $zero, 0x0058-(albumMainMenuWidthChange)






  
.close

  
