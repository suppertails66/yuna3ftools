
;============================================================================
; OMAKE.BIN
; video player, and presumably stuff for bonus menu too
;============================================================================
  
  ; this executable doesn't use much memory, so we just occupy
  ; a big chunk of it with our new code and data
  omake_newCodeStartSection equ 0x8010FF80
  omake_newCodeEndSection   equ 0x80110000
  
.open "out/asm/OMAKE.BIN", 0x80108000

  ;===============================================
  ; MODIFICATION:
  ; videos always skippable with start
  ;===============================================
  
  .org 0x8010AD40
    li $v1,1
    nop
    nop

  ;===============================================
  ; MODIFICATION:
  ; do new video subtitle playback
  ;===============================================
    
  .org 0x8010B038
    j doNewVideoPlayback
    nop

  ;===============================================
  ; MODIFICATION:
  ; do new video subtitle updates
  ;===============================================

  .org 0x8010ABC4
    j doSubtitleUpdate
    nop
  
  .org 0x8010A4AC
    j doSubtitleDraw
    nop











/*;============================================================================
; NEW STUFF
;============================================================================
   
  .org omake_newCodeStartSection
  .area omake_newCodeEndSection-.,0xFF
  
    .align 4

    ;===============================================
    ; yuna walk ff
    ;===============================================
    
    
    
    
    

    ;============================================================================
    ; pad to end of sector
    ;============================================================================
    
    .align 0x800
      
  .endarea */
  
.close

  
