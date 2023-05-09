
;============================================================================
; LONGMAP.BIN
;============================================================================
  
  ; this executable doesn't use much memory, so we just occupy
  ; a big chunk of it with our new code and data
  longmap_newCodeStartSection equ 0x8010FF80
  longmap_newCodeEndSection   equ 0x80110000
  
.open "out/asm/LONGMAP.BIN", 0x80108000

  ;===============================================
  ; MODIFICATION:
  ; yuna walk ff check
  ;===============================================
  
  ; this doubles the walk speed when fast-forwarding
/*  .org 0x8010C8F8
    j doYunaWalkFfCheck
    nop*/
  
  ; ...buf you know, why not just skip the wait entirely
  .org 0x8010C784
    j saveYunaWalkDstX
    sll $v0, $v0, 16
  
  .org 0x8010C7B0
    j saveYunaWalkDstY
    sll $v0, $v0, 16
  
/*  .org 0x8010C7F4
    j saveYunaWalkDstXY
    nop*/
  
  .org 0x8010C8D8
    j doYunaWalkFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; use new chapter number string
  ;===============================================
  
  .org 0x8010A85C
    j doNewWorldMapChapterNum
    sb $v0, 0x0000($v1)

  ;===============================================
  ; MODIFICATION:
  ; we have no need to place the "e" string after
  ; the location name, so skip the complicated
  ; string calculations and just put it at a
  ; hardcoded position in front of it
  ;===============================================
  
  ; NOTE: this skips setting s3 to a value needed for correctly
  ; generating padding later on in this function.
  ; but we don't print that padding anymore, so it doesn't matter.
;  .org 0x8010B994
;    li $a0,100
;    j 0x8010B9C0
;    nop
  
  .org 0x8010B994
    j 0x8010B9AC
    nop
  
  .org 0x8010B9AC
    li $a0,100
    j 0x8010B9C0
    nop

  ;===============================================
  ; MODIFICATION:
  ; don't print padding string on transfer window
  ;===============================================
  
  .org 0x8010BA9C
    nop
    nop

  ;===============================================
  ; MODIFICATION:
  ; use printConsecutiveStrings instead of
  ; cumbersome string copy for showing fanart
  ; messages
  ;===============================================
  
  .org 0x8010CF8C
    ; s0 = artist name string
    lw $a1, 0xFE48($at)
    nop
    move $s0,$a1
    jal 0x801890AC
    addiu $a0, $zero, 0x0063
    
    j 0x8010CFE0
    nop
  
  .org 0x8010D054
    j printFanartMessage
    nop









;============================================================================
; NEW STUFF
;============================================================================
   
  .org longmap_newCodeStartSection
  .area longmap_newCodeEndSection-.,0xFF
  
    .align 4

    ;===============================================
    ; yuna walk ff
    ;===============================================
    
    ; v0/v1 = yuna x/y
    ; a0/a1 = yuna xvel/yvel
    ; s0 = current "step counter"
    ; s1 = target value for "step counter"
/*    doYunaWalkFfCheck:
      ; ensure we have at least 2 "steps" left to walk
      subu $t0,$s1,$s0
      blt $t0,2,@@done
      nop
      
      subiu $sp,16
;      sw $ra,0($sp)
      sw $v0,0($sp)
      sw $v1,4($sp)
      sw $a0,8($sp)
      sw $a1,12($sp)
        jal checkIfFfButtonPressed
        nop
        move $t0,$v0
;      lw $ra,0($sp)
      lw $v0,0($sp)
      lw $v1,4($sp)
      lw $a0,8($sp)
      lw $a1,12($sp)
      addiu $sp,16
      
      beq $t0,$zero,@@done
      nop
        ; double xvel/yvel
        sll $a0,1
        sll $a1,1
        ; increment step counter
        ; (it'll get incremented again later for a total of 2)
        addiu $s0,1
      @@done:
      ; make up work
      addu $v0,$a0
      j 0x8010C900
      addu $v1,$a1 */
    
    saveYunaWalkDstX:
      sw $v0,nextYunaWalkDstX
      ; make up work
      j 0x8010C790
      subu $v0, $v0, $a1
    
    saveYunaWalkDstY:
      sw $v0,nextYunaWalkDstY
      ; make up work
      j 0x8010C7B8
      subu $v0, $v0, $v1
    
/*    saveYunaWalkDstXY:
      la $a0,0x8010F888
      ; fetch raw 
      lw $a1,0($a0)
      lw $a0,4($a0)
      sw $a1,nextYunaWalkDstX
      sw $a0,nextYunaWalkDstY
      
      
      ; make up work
      lui $a2, 0x8011
      j 0x8010C7FC
      lw $a2, 0xF888($a2) */
      
    nextYunaWalkDstX:
      .dw 0
    nextYunaWalkDstY:
      .dw 0
    
    doYunaWalkFfCheck:
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      nop
      
        ; v0/v1 = new target x/y
        lw $v0,nextYunaWalkDstX
        lw $v1,nextYunaWalkDstY
        
        ; set current counter value to target to prevent further "steps"
        j 0x8010C900
        move $s0,$s1
      
      @@done:
      ; make up work
      lui $v0, 0x8011
      j 0x8010C8E0
      lw $v0, 0xF880($v0)
      
      
    
    
    
    
    

    ;============================================================================
    ; pad to end of sector
    ;============================================================================
    
    .align 0x800
      
  .endarea
  
.close

  
