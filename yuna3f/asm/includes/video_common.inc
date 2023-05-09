
    ;==================================
    ; old routines
    ;==================================
    
    playVideoSub equ 0x8010A8D8
    
    ;==================================
    ; defines
    ;==================================
    
    subFontH equ 16
    subFontBytesPerRow equ 16
    
    videoSliceW equ 16
    videoSliceH equ 240
    videoSliceBytesPerRow equ (videoSliceW*3)
    
    ; one video frame = 4 60fps frames
    hwFramesPerVideoFrame equ 4
    
    ;==================================
    ; addresses
    ;==================================
    
;    mdecPixelBufferPtr equ 0x800EAD40
;    mdecGpuDstStrct equ 0x800EAD58
    
    ;==================================
    ; opcodes
    ;==================================
    
    subOp_terminator  equ 0x00
    subOp_endSub      equ 0x01
    subOp_swapSubBuf  equ 0x02
    subOp_waitUntil   equ 0x03
    subOp_queueSubA   equ 0x04
    subOp_queueSubB   equ 0x05
;    subOp_offsetTimer equ 0x04
    
;    subStringToken_linebreak    equ 0xFF
    subStringToken_linebreak    equ 0x0A

    ;==================================
    ; op macros
    ;==================================
    
    ; every event happens this many frames later than the time specified
    globalTimeOffset equ -8
    videoFps equ 59.94
    
    .macro genTime,time
;    .align 4
      .dh time+globalTimeOffset
;    .align 4
    .endmacro
    
    .macro genTimeMinSec,timeMin,timeSec
      ; 1 video frame = 4 real-time frames
;      genTime int(((float(timeMin) * 60.0 * 60.0) + (float(timeSec) * 60.0)) / 4)
      genTime int(((float(timeMin) * 60.0 * videoFps) + (float(timeSec) * videoFps)) / 1)
    .endmacro
    
    .macro genTimePairMinSec,timeMin1,timeSec1,timeMin2,timeSec2
      genTimeMinSec timeMin1,timeSec1
      genTimeMinSec timeMin2,timeSec2
    .endmacro
    
    
    
    .macro sub_terminator
;    .align 4
      .db subOp_terminator
;    .align 4
    .endmacro
    
    
    
    .macro sub_waitUntil,time
;    .align 4
      .db subOp_waitUntil
        genTime time
;    .align 4
    .endmacro
    
    .macro sub_waitUntilMinSec,timeMin,timeSec
      ; convert to frame value and use normal macro
      ; 1 video frame = 4 real-time frames
;      sub_waitUntil int(((float(timeMin) * 60.0 * 60.0) + (float(timeSec) * 60.0)) / 4)
      sub_waitUntil int(((float(timeMin) * 60.0 * videoFps) + (float(timeSec) * videoFps)) / 1)
    .endmacro
    
    
    
/*    .macro sub_queueSubATest,str
;    .align 4
      .db subOp_queueSubA
        .loadtable "table/yuna3_en.tbl"
        .string str
;    .align 4
    .endmacro
    
    .macro sub_queueSubBTest,str
;    .align 4
      .db subOp_queueSubB
        .loadtable "table/yuna3_en.tbl"
        .string str
;    .align 4
    .endmacro*/
    
    .macro sub_queueSubA,fileName
      .db subOp_queueSubA
        .incbin fileName
    .endmacro
    
    .macro sub_queueSubB,fileName
      .db subOp_queueSubB
        .incbin fileName
    .endmacro
    
    .macro sub_clearSubA
      .db subOp_queueSubA
        .asciiz ""
    .endmacro
    
    .macro sub_clearSubB
      .db subOp_queueSubB
        .asciiz ""
    .endmacro
    
    videoSubClearSecDelay equ 0.133
    
    .macro sub_clearSubAMinSec,timeMin,timeSec
      sub_waitUntilMinSec timeMin, (timeSec+videoSubClearSecDelay)
      sub_clearSubA
    .endmacro
    
    .macro sub_clearSubBMinSec,timeMin,timeSec
      sub_waitUntilMinSec timeMin, (timeSec+videoSubClearSecDelay)
      sub_clearSubB
    .endmacro
    
    .macro sub_clearSubABMinSec,timeMin,timeSec
      sub_waitUntilMinSec timeMin, (timeSec+videoSubClearSecDelay)
      sub_clearSubA
      sub_clearSubB
    .endmacro
    
    
    
    .macro sub_endSub
;    .align 4
      .db subOp_endSub
;    .align 4
    .endmacro
    
    
    
;    .macro sub_offsetTimer,time
;    .align 4
;      .dw subOp_offsetTimer
;      .dw time
;    .align 4
;    .endmacro
    
    videoSubtitleIndexEntrySize equ 20
    
    .macro makeVideoSubtitleIndexEntry,fileName,ptr
      .dw ptr
      .asciiz fileName
      .fill videoSubtitleIndexEntrySize-(strlen(fileName) + 5),0x00
    .endmacro

    ;==================================
    ; structs
    ;==================================
    
    ;===========
    ; prepped subtitle line buffer
    ;===========
    
    subDrawBuf_maxChars equ 128
    
    subDrawBuf_bytesPerCharEntry equ 8
      subDrawBuf_charEntry_id equ 0
      subDrawBuf_charEntry_x  equ 2
      subDrawBuf_charEntry_y  equ 4
      subDrawBuf_charEntry_w  equ 6
    
    ; ensure word alignment
    subDrawBuf_contentBufSize equ (((subDrawBuf_maxChars * subDrawBuf_bytesPerCharEntry) + 0x3) & ~0x3)
    subDrawBuf_contentBufNumWords equ (subDrawBuf_contentBufSize / 4)

    subDrawBuf_size           equ (8+subDrawBuf_contentBufSize)
      ; 4
      subDrawBuf_basePtr      equ 0
      ; 4
      subDrawBuf_currentPtr   equ 4
      ; subDrawBuf_contentBufSize
      subDrawBuf_contentBuf   equ 8
    
    .macro submakeInitSubDrawBuf,name
      ;===========
      ; a0 = dst
      ;===========
      name:
        ; a1 = base content pointer
        addiu $a1,$a0,subDrawBuf_contentBuf
        ; set base/current ptr to base
        sw $a1,subDrawBuf_basePtr($a0)
        sw $a1,subDrawBuf_currentPtr($a0)
        
        ; zero-fill content buf
        move $a2,$zero
        @@contentBufInitLoop:
          sw $zero,0($a1)
          addiu $a1,4
          
          addiu $a2,1
          bne $a2,subDrawBuf_contentBufNumWords,@@contentBufInitLoop
          nop
        
        jr $ra
        nop
    .endmacro
    ; HACK: because armips has no fucking clue
    .macro makeInitSubDrawBuf
      submakeInitSubDrawBuf initSubDrawBuf
    .endmacro
    
    ;===========
    ; subtitle block buffer
    ;===========
    
    subBlockBuf_maxLines equ 2
;    subBlockBuf_lineBufsSize equ subDrawBuf_size*subBlockBuf_maxLines

    ; after a fucking hour of trying to figure out why this wasn't resolving
    ; to the correct value, i eventually realized i had a vague memory of
    ; discovering on a previous project that armips uses simple name-based
    ; substitution when evaluating "variable" names in expressions,
    ; with the consequence that failing to enclose compound values in parentheses
    ; can lead to unexpected and unintuitive results.
    ; previously, subDrawBuf_size was the unparenthesized expression
    ; 8+subDrawBuf_contentBufSize.
    ; this meant that this line was evaluated as
    ; 12+(subBlockBuf_maxLines*8+subDrawBuf_contentBufSize) = 0x41C,
    ; rather than the expected
    ; 12+(subBlockBuf_maxLines*(8+subDrawBuf_contentBufSize)) = 0x81C.
    ; i write this in an effort to make myself remember in the future to never
    ; trust armips to get it right.
    subBlockBuf_size          equ (12+(subBlockBuf_maxLines*subDrawBuf_size))
      ; 4
      subBlockBuf_drawBufAPtr equ 0
      ; 4
      subBlockBuf_drawBufBPtr equ 4
      ; 4
      subBlockBuf_lineCount   equ 8
      ; subDrawBuf_size
      subBlockBuf_drawBufA    equ 12
      ; subDrawBuf_size
      subBlockBuf_drawBufB    equ (subBlockBuf_drawBufA+subDrawBuf_size)
    
    .macro submakeInitSubBlockBuf,name
      ;===========
      ; a0 = dst
      ;===========
      name:
        subiu $sp,8
        sw $ra,0($sp)
        sw $s0,4($sp)
        
          ; s0 = dst
          move $s0,$a0
          
          ; init line count
          sw $zero,subBlockBuf_lineCount($s0)
        
          ; init line A
          addiu $a0,$s0,subBlockBuf_drawBufA
          jal initSubDrawBuf
          sw $a0,subBlockBuf_drawBufAPtr($s0)
        
          ; init line B
          addiu $a0,$s0,subBlockBuf_drawBufB
          jal initSubDrawBuf
          sw $a0,subBlockBuf_drawBufBPtr($s0)
        
        lw $ra,0($sp)
        lw $s0,4($sp)
        jr $ra
        addiu $sp,8
    .endmacro
    .macro makeInitSubBlockBuf
      submakeInitSubBlockBuf initSubBlockBuf
    .endmacro

/*    .macro makeSubDrawBuf,index,label
      .align 4
      
      label:
        ; srcPtr
        .dw 0
        ; srcHash
        .dw 0
        
        ; width
        .dh 0
        
        ; flags
        .db 0
        
        ; index num
        .db index
        ; numSubLines
        .if index == textBoxBufStartIndex
          .db 2
        .else
          .db 0
        .endif
        
        ; subLines
        .if index == textBoxBufStartIndex
          .db textBoxBufStartIndex+1,textBoxBufStartIndex+2
        .else
          .fill 2,0
        .endif
        .fill 9,0
      
      .align 4
      
    .endmacro */
