
;========================================
; basic stuff
;========================================

.psx

.fixloaddelay

;========================================
; defines
;========================================



;============================================================================
; SLPS_014.51
;============================================================================

  newExeLoadAddr equ 0x80010000
  
;.open "out/asm/SLPS_014.51", 0x80110000-0x800
.open "out/asm/SLPS_014.51", newExeLoadAddr-0x800
; size field in exe header
;.org 0x8000F800+0x1C
;  .word 0x1EE000

    .org 0x801AF468
    .area 0x105C,0xFF
      
      .incbin "out/filetable.bin"
      
    .endarea
  
.close

  
