
;============================================================================
; BMS096.BIN
; multiplayer mode resources
;============================================================================
  
.open "out/asm/BMS096.BIN", 0x80108000

  ;===============================================
  ; MODIFICATION:
  ; new multiplayer string 0
  ;===============================================
  
  .org 0x8010A100
    j useNewMultiplayerString_0
    nop

  ;===============================================
  ; MODIFICATION:
  ; new multiplayer string 1
  ;===============================================
  
  .org 0x8010BA84
    j useNewMultiplayerString_1
    nop

  ;===============================================
  ; MODIFICATION:
  ; new multiplayer string 2
  ;===============================================
  
  .org 0x8010BFE4
    j useNewMultiplayerString_2
    nop

  ;===============================================
  ; MODIFICATION:
  ; new multiplayer string 3
  ;===============================================

  .org 0x8010B9B8
    j useNewMultiplayerString_3_0
    nop

  .org 0x8010BBE0
    j useNewMultiplayerString_3_1
    nop
  
  .org 0x8010BC54
    j useNewMultiplayerString_3_2
    nop
  
  .org 0x8010BE54
    j useNewMultiplayerString_3_3
    nop







  
.close

  
