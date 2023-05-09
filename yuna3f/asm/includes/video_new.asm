
    ;========================================
    ; 
    ;========================================
    
    ; s0 = pointer to video name
    doNewVideoPlayback:
      jal doVideoSubtitleSetup
      move $a0,$s0
      
      ; make up work
      jal playVideoSub
      move $a0,$s0
      
      ; do playback cleanup
      jal doVideoSubtitleTeardown
      nop
      
      ; make up work
      j 0x8010B040
      nop

    ;========================================
    ; 
    ;========================================
    
    ; a0 = target video name
    doVideoSubtitleSetup:
      subiu $sp,12
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
        
        ; s0 = target video name
        move $s0,$a0
        
        ; set up subtitle block buffers
        
        li $a0,subBlockBuf_size
        jal allocHeap
        nop
        sw $v0,subtitleBlockBufAPtr
        jal initSubBlockBuf
        move $a0,$v0
        
        li $a0,subBlockBuf_size
        jal allocHeap
        nop
        sw $v0,subtitleBlockBufBPtr
        jal initSubBlockBuf
        move $a0,$v0
        
        ; set up subtitles if enabled
        jal getConfigSetting
        li $a0,subtitlesOptionIndex
        sw $zero,currentSubtitlePtr
        bne $v0,$zero,@@subtitlePointerSet
        nop
          ; the programmers thoughtfully assumed case-insensitivity for filenames
          ; and used whatever case they felt like on a given day when referring to them,
          ; so we copy and capitalize the input string so we can check against it
          
          ; a0/a1 = dst
          la $a0,videoCapitalizedNameBuffer
          move $a1,$a0
          @@capitalizeLoop:
            lbu $v0,0($s0)
            addiu $s0,1
            
            blt $v0,0x61,@@notLowerCase
            nop
            bgt $v0,0x7A,@@notLowerCase
            nop
              addiu $v0,-0x20
            @@notLowerCase:
            
            beq $v0,$zero,@@capitalizeLoopDone
            sb $v0,0($a0)
            
            j @@capitalizeLoop
            addiu $a0,1
          @@capitalizeLoopDone:
          
          ; s0 = capitalized name string
          move $s0,$a1
          
          ; use video name to decide initial subtitle pointer
  ;        la $a0,testSubtitles
  ;        sw $a0,currentSubtitlePtr
          sw $zero,currentSubtitlePtr
          la $s1,videoSubtitleIndex
          @@indexSearchLoop:
            ; copy pointer
            lw $v0,0($s1)
            nop
            sw.u $v0,currentSubtitlePtr
            ; null pointer == end of index
            beq $v0,$zero,@@indexSearchDone
            sw.l $v0,currentSubtitlePtr
            
            ; check if this is actually the target entry
            move $a0,$s0
            jal strcmp
            addiu $a1,$s1,4
            
            ; loop if not target entry
            bne $v0,$zero,@@indexSearchLoop
            addiu $s1,videoSubtitleIndexEntrySize
          @@indexSearchDone:
        @@subtitlePointerSet:
        
        ; reset timer
        sw $zero,subtitleTimer
        
        ; save old font index
        la $a0,currentFontIndex
        lw $v0,0($a0)
        la $a1,video_oldFontIndex
        sw $v0,0($a1)
        ; use subtitle font
        li $v0,fontIndex_subtitle
        sw $v0,0($a0)
      
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      jr $ra
      addiu $sp,12
    
    videoCapitalizedNameBuffer:
      .fill 16,0x00
    
    ;========================================
    ; 
    ;========================================
    
    doVideoSubtitleTeardown:
      subiu $sp,4
      sw $ra,0($sp)
;      sw $s0,4($sp)
        
        ; free subtitle blocks
        
        lw $a0,subtitleBlockBufBPtr
        jal freeHeap
        nop
        sw $zero,subtitleBlockBufBPtr
        
        lw $a0,subtitleBlockBufAPtr
        jal freeHeap
        nop
        sw $zero,subtitleBlockBufAPtr
        
        ; restore old font index
        lw $v0,video_oldFontIndex
        la $a0,currentFontIndex
        sw $v0,0($a0)
      
      lw $ra,0($sp)
;      lw $s0,4($sp)
      nop
      jr $ra
      addiu $sp,4

    ;========================================
    ; doSubtitleUpdate
    ;========================================
    
    doSubtitleUpdate:
      ; make up work
;      jal 0x800D924C
;      addiu $a1, $zero, 0x0003
      jal 0x8018B7E8
      nop
      
      ; reset block buffers for next video frame
      
      lw $a0,subtitleBlockBufAPtr
      jal resetBlockBuffer
      nop
      
      lw $a0,subtitleBlockBufBPtr
      jal resetBlockBuffer
      nop
      
      ; reset pixel pos
      sw $zero,videoSlicePixelX
      
      ; run subtitle interpreter frame update
      jal runSubtitleInterpreter
      nop
      
      ; advance subtitle timer
      la $a0,subtitleTimer
      lw $v0,0($a0)
      nop
;      addiu $v0,1
      addiu $v0,hwFramesPerVideoFrame
      sw $v0,0($a0)
    
      ; make up work
      j 0x8010ABCC
      nop
    
  ;============================================================================
  ; creation of required struct functions
  ; via macros from common_sub
  ;============================================================================
    
    makeInitSubDrawBuf
    makeInitSubBlockBuf
    
  ;============================================================================
  ; subtitle functions
  ;============================================================================
  
    ;========================================
    ; render a subtitle string to a block
    ; buffer
    ;
    ; a0 = src string ptr
    ; a1 = dst blockbuf ptr
    ; a2 = y-position centerpoint
    ; returns pointer past end of src string
    ;========================================
    
    prepSubtitle:
      subiu $sp,20
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
      sw $s3,16($sp)
      
        ; s0 = src
        move $s0,$a0
        ; s1 = dst
        move $s1,$a1
        ; s2 = line count
        move $s2,$zero
        ; s3 = y-center
        move $s3,$a2
        ; s3 = old font index
        ; FIXME: handle elsewhere
;        la $v0,currentFontIndex
;        ; set font index to subtitle font
;        lw $s3,0($v0)
;        li $v1,fontIndex_subtitle
;        sw $v1,0($v0)
        
        ;=====
        ; write content
        ;=====
        
        @@lineRenderLoop:
          ; a1 = get pointer to line buffer for current line
          sll $v0,$s2,2
          addu $a1,$s1,$v0
          lw $a1,subBlockBuf_drawBufAPtr($a1)
          ; render line
          jal prepSubtitleLine
          ; a0 = pointer to current pos in src string
          move $a0,$s0
          
          ; increment line count
          addiu $s2,1
          
          ; check line terminator
          lbu $a0,0($v0)
          addiu $v0,1
          
          ; if linebreak, continue;
          ; otherwise, assume it's a terminator and we're done
          beq $a0,subStringToken_linebreak,@@lineRenderLoop
          move $s0,$v0
        
        ;=====
        ; apply Y-centering offset based on line count
        ;=====
        
        ; make sure lines exist
        beq $s2,$zero,@@done
        nop
        
        ; a0 = centering offset
        li $a0,(subFontH / 2)
        mult $a0,$s2
;        lw $a1,subtitleBlockBufA_baseY
;        move $a1,$s3
        mflo $a0
        ; a0 = initial y-position
;        subu $a0,$a1,$a0
        subu $a0,$s3,$a0
        
        ; a2 = line counter
        move $a2,$zero
        
        @@lineCenterLoop:
          ; a1 = line pointer
          sll $v0,$a2,2
          addu $a1,$s1,$v0
          lw $a1,subBlockBuf_drawBufAPtr($a1)
          nop
          addiu $a1,subDrawBuf_contentBuf
          
          @@charCenterLoop:
            ; v0 = fetch next char id
            lhu $v0,subDrawBuf_charEntry_id($a1)
            nop
            
            ; done if id == 0 (terminator)
            beq $v0,$zero,@@charCenterLoopDone
            nop
            
            ; otherwise, apply offset to Y
;            lh $v0,subDrawBuf_charEntry_y($a1)
;            nop
;            addu $v0,$a0
;            sh $v0,subDrawBuf_charEntry_y($a1)
            sh $a0,subDrawBuf_charEntry_y($a1)
            
            j @@charCenterLoop
            addiu $a1,subDrawBuf_bytesPerCharEntry
          @@charCenterLoopDone:
          
          ; check if we just did the last line
          addiu $a2,1
          bne $a2,$s2,@@lineCenterLoop
          ; move to next line's y-pos
          addiu $a0,subFontH
        
      @@done:
      ; restore font index
;      sw $s3,currentFontIndex
      ; write line count
      sw $s2,subBlockBuf_lineCount($s1)
      ; return pointer past end of src string
      move $v0,$s0
      
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
      lw $s3,16($sp)
      jr $ra
      addiu $sp,20
  
    ;========================================
    ; render a subtitle line to a line buffer
    ;
    ; a0 = src string ptr
    ; a1 = dst drawbuf ptr
    ; returns pointer to "line terminator"
    ; in src string, which will be either
    ; a linebreak or the string terminator
    ;========================================
    
    prepSubtitleLine:
      subiu $sp,32
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
      sw $s3,16($sp)
      sw $s4,20($sp)
      sw $s5,24($sp)
      sw $s6,28($sp)
      
        ; s0 = src
        move $s0,$a0
        ; s1 = dst
        addiu $s1,$a1,subDrawBuf_contentBuf
        ; s2 = x-pos/width
        move $s2,$zero
        
        ;=====
        ; do initial positioning
        ;=====
        
        ; s3 = last char ID (for kerning)
        move $s3,$zero
        
        ; s6 = width table pointer
;        lw $a3,extraVideoRsrcPtr
;        li $v0,fontPack_widthOffset
;        addu $s6,$a3,$v0
        ; a1 = kerning table pointer
;        li $v0,fontPack_kerningOffset
;        addu $a1,$a3,$v0
        
        ; s6 = width table pointer
        lw $v0,currentFontIndex
        li $v1,fontWidthTable
        sll $v0,2
        addu $v1,$v0
        lw $s6,0($v1)
        nop
        
        ; s4 = dst
        move $s4,$s1
        
        @@charLoop:
          ; s5 = fetch next char ID
          lbu $s5,0($s0)
          ; write placeholder terminator sequence to dst
          sh $zero,subDrawBuf_charEntry_id($s4)
          
          ; if terminator or linebreak, done
          beq $s5,$zero,@@charLoopDone
          nop
          beq $s5,subStringToken_linebreak,@@charLoopDone
          nop
          
          ; write char id
          sh $s5,subDrawBuf_charEntry_id($s4)
          
          ; advance src
          ; (do not advance if terminator found -- caller needs to
          ; be able to check terminator type)
          addiu $s0,1
          
          ;=====
          ; apply kerning
          ;=====
          
/*          ; do nothing if last char was null
          beq $s3,$zero,@@noKerning
          ; a2 = last char id * chars per row (256)
          sll $a2,$s3,8
            ; add current char id
            addu $a2,$s5
            ; a3 = pointer to kerning matrix entry
            addu $a3,$a1,$a2
            ; v1 = kerning amount
            lb $v1,0($a3)
            nop
            ; apply to x-pos
            addu $s2,$v1
          @@noKerning:*/
          
          ; skip kerning if last char was space or null
          beq $s3,$zero,@@noKerning
          move $a0,$s5
          beq $s3,char_space,@@noKerning
          move $a1,$s3
            jal getCharKerning
            nop
            
            ; apply kerning amount to x-pos
            addu $s2,$v0
          @@noKerning:
          
          ; set previous char = current
          move $s3,$s5
          
          ;=====
          ; set x
          ;=====
          
          ; write x-pos (y will be set after all lines are rendered)
          sh $s2,subDrawBuf_charEntry_x($s4)
          
          ;=====
          ; apply width
          ;=====
          
          ; a3 = pointer to width table entry
          addu $a3,$s6,$s5
          addiu $a3,-charRangeStart
          ; v1 = width
          lbu $v1,0($a3)
;          nop
          ; write to buffer
          ; HACK: if a space, do not advance dst.
          ; this will result in the entry that was just generated being
          ; overwritten by the next one or the terminator, but the updated
          ; x-position being preserved (there is no point wasting time
          ; rendering empty characters in a time-critical situation)
          beq $s5,char_space,@@noDstAdvance
          sh $v1,subDrawBuf_charEntry_w($s4)
            ; advance dst
            addiu $s4,subDrawBuf_bytesPerCharEntry
          @@noDstAdvance:
          
          ; loop
          j @@charLoop
          ; advance dst
;          addiu $s4,subDrawBuf_bytesPerCharEntry
          ; apply to x-pos
          addu $s2,$v1
          
        @@charLoopDone:
        
        ;=====
        ; apply X-centering offset based on total width of line
        ;=====
        
        ; a0 = x-offset = (screenW - lineW) / 2
        lw $a0,subtitleTotalW
        nop
        subu $a0,$s2
        srl $a0,1
        
        ; s4 = src/dst
        move $s4,$s1
        
        @@xCenterLoop:
          ; v0 = char id
          lhu $v0,subDrawBuf_charEntry_id($s4)
          ; v1 = base x-pos
          lhu $v1,subDrawBuf_charEntry_x($s4)
          ; done if char null
          beq $v0,$zero,@@xCenterLoopDone
          ; add x-offset
          addu $v1,$a0
          
          ; write back updated x
          sh $v1,subDrawBuf_charEntry_x($s4)
          
          ; loop
          j @@xCenterLoop
          ; advance src/dst
          addiu $s4,subDrawBuf_bytesPerCharEntry
        @@xCenterLoopDone:
        
      @@done:
      ; return pointer to terminator of src string
      move $v0,$s0
      
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
      lw $s3,16($sp)
      lw $s4,20($sp)
      lw $s5,24($sp)
      lw $s6,28($sp)
      jr $ra
      addiu $sp,32
  
    ;========================================
    ; 
    ;========================================
    
/*    resetBlockBuffers:
      subiu $sp,4
      sw $ra,0($sp)
      
        lw $a0,subtitleBlockBufAPtr
        jal resetBlockBuffer
        nop
        
        ; FIXME
;        lw $a0,subtitleBlockBufBPtr
;        jal resetBlockBuffer
;        nop 
      
      lw $ra,0($sp)
      nop
      jr $ra
      addiu $sp,4 */
  
    ;========================================
    ; reset a block buffer's lines for
    ; new frame display
    ;
    ; a0 = block buffer pointer
    ;========================================
    
    resetBlockBuffer:
      ; reset each line
    
      ; a0 = src
      addiu $a0,subBlockBuf_drawBufAPtr
      ; a1 = counter
      li $a1,0
      @@loop:
        ; a2 = src
        sll $a2,$a1,2
        addu $a2,$a0
        lw $a2,0($a2)
        nop
        
        ; a3 = base ptr
        lw $a3,subDrawBuf_basePtr($a2)
        nop
        ; set current ptr to base
        sw $a3,subDrawBuf_currentPtr($a2)
        
        addiu $a1,1
        bne $a1,subBlockBuf_maxLines,@@loop
        nop
      
      jr $ra
      nop
  
    ;========================================
    ; executed from interrupt when MDECout
    ; for current video slice completed
    ;========================================
    
    doSubtitleDraw:
      ; finish getting pointer to pixel data
      lui $at, 0x8011
      addu $at, $at, $v0
      lw $a1, 0xC268($at)
      nop
      sw $a1,mdecPixelBufferPtr
      
      ; draw both block buffers
      
      lw $a0,subtitleBlockBufAPtr
      jal drawBlockSlice
      nop
      
      lw $a0,subtitleBlockBufBPtr
      jal drawBlockSlice
      nop
      
      ; update slice x-pos
      la $a0,videoSlicePixelX
      lw $a1,0($a0)
      nop
      addiu $a1,videoSliceW
      sw $a1,0($a0)
      
      ; make up work
      sll $v0, $s0, 2
      lui $at, 0x8011
      j 0x8010A4B4
      addu $at, $at, $v0
  
    ;========================================
    ; a0 = block pointer
    ;========================================
    
    drawBlockSlice:
      subiu $sp,16
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
    
        ; s0 = src
        addiu $s0,$a0,subBlockBuf_drawBufAPtr
        ; s1 = counter
        li $s1,0
        @@loop:
          ; draw only as many lines as exist
          lw $v0,subBlockBuf_lineCount($s0)
          ; s2 = src
          sll $s2,$s1,2
          beq $s1,$v0,@@done
          addu $s2,$s0
          lw $s2,0($s2)
          nop
          
          ; a0 = current ptr
          lw $a0,subDrawBuf_currentPtr($s2)
          ; draw line
          jal drawLineSlice
          nop
          
          ; FIXME: update from returned pointer
;          sw $v0,subDrawBuf_currentPtr($s2)
          
          ; a3 = base ptr
  ;        lw $a3,subDrawBuf_basePtr($s2)
  ;        nop
  ;        ; set current ptr to base
  ;        sw $a3,subDrawBuf_currentPtr($s2)
          
          addiu $s1,1
          bne $s1,subBlockBuf_maxLines,@@loop
          nop
      
      @@done:
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
      jr $ra
      addiu $sp,16
  
    ;========================================
    ; a0 = line current pointer
    ; returns v0 = new line current pointer
    ;========================================
    
    drawLineSlice:
      subiu $sp,16
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
;      sw $s3,16($sp)
;      sw $s4,20($sp)
;      sw $s5,24($sp)
;      sw $s6,28($sp)
      
        ; s1 = current slice X
        ; get current gpu word x
        lw $s1,videoSlicePixelX
        ; s0 = srcptr
        move $s0,$a0
        ; s2 = end of current slice
        addiu $s2,$s1,videoSliceW
        
        ; HACK:
        ; t6 = dstSliceBase = slice base row putpos
        ; (assumed same for all chars)
        lhu $t6,subDrawBuf_charEntry_y($s0)
        lw $a0,mdecPixelBufferPtr
        ; multiply by 48 (bytes per 16px row)
        ; t5 = y*16
        sll $t5,$t6,4
        ; t6 = y*32
        sll $t6,5
        ; add t5 to get y*48
        addu $t6,$t5
        addu $t6,$a0
        
        ; t5 = fontBasePtr
;        lw $t5,extraVideoRsrcPtr
;        nop
;        addiu $t5,fontPack_fontOffset
        la $t5,font_subtitle
        
        @@charDrawLoop:
          ; t0 = charId
          lhu $t0,subDrawBuf_charEntry_id($s0)
          ; t1 = charX
          lhu $t1,subDrawBuf_charEntry_x($s0)
          
          ; if id == 0, done
          beq $t0,$zero,@@done
          nop
          ; if X >= end of slice, done
          bge $t1,$s2,@@done
          nop
          
          ; fetch unpacked char data to buffer
;          jal fetchCharData
;          move $a0,$t0
;          ; t2 = charPtr
;          la $t2,nextCharBuf
          
          ; a2 = copyW = charW
          lhu $a2,subDrawBuf_charEntry_w($s0)
          nop
          
          ; t2 = charPtr
          ; HACK:
          ; char id *= 256 (16x16 font, 8bpp, 256 bytes per char)
          addiu $t2,$t0,-charRangeStart
          sll $t2,$t2,8
          ; add fontBasePtr
          addu $t2,$t5
;          move $t2,$t2
          
          ; t4 = (charX + charW)
          addu $t4,$t1,$a2
          ; if < current slice X, skip
          blt $t4,$s1,@@loopDone
          nop
          
          ; t3 = leftMargin = (charX - currentSliceX)
          subu $t3,$t1,$s1
          ; if < 0
          bgez $t3,@@leftMarginNotNegative
          nop
            ; offset charPtr and copyW by this amount,
            ; then set to 0
            subu $t2,$t3
            addu $a2,$t3
            move $t3,$zero
          @@leftMarginNotNegative:
          ; t3 = dstBasePtr = dstSliceBase + (leftMargin * 3)
          sll $v0,$t3,1
          addu $a3,$v0,$t3
          addu $t3,$a3,$t6
          
          ; t4 = rightMargin = (nextSliceX - (charX + charW))
          ; (nextSliceX - (charX + charW))
          subu $t4,$s2,$t4
          ; if < 0
          bgez $t4,@@rightMarginNotNegative
          nop
            ; offset copyW by this amount
            addu $a2,$t4
          @@rightMarginNotNegative:
          ; t4 = copyW
          move $t4,$a2
          
          ; copyW = charW
          ; leftMargin = (charX - currentSliceX)
          ;   - if < 0, offset charPtr and copyW by this amount,
          ;             then set to 0
          ; rightMargin = (nextSliceX - (charX + charW))
          ;   - if < 0, offset copyW by this amount
          
          ; copy char rows.
          ; at this point:
          ; - t2 = charBasePtr
          ; - t3 = dstBasePtr
          ; - t4 = copyW
          
          ; a3 = counter
          move $a3,$zero
          @@rowCopyLoop:
            ; src
            move $t1,$t2
            ; dst
            move $t0,$t3
            ; width (in pixels)
            move $a2,$t4
            
            @@byteCopyLoop:
              ; v0 = fetch from src
              lbu $v0,0($t1)
              addiu $t1,1
              
              ; if zero, skip (transparent)
              beq $v0,$zero,@@transparent
              subiu $a2,1
                ; convert from raw 4-bit index to 8-bit grayscale value
;                sll $v1,$v0,4
;                or $v0,$v1
                ; write 24-bit output
                sb $v0,0($t0)
                sb $v0,1($t0)
                sb $v0,2($t0)
              @@transparent:
              
              bgt $a2,$zero,@@byteCopyLoop
              addiu $t0,3
            @@byteCopyLoopDone:
            
            addiu $a3,1
            beq $a3,subFontH,@@rowCopyLoopDone
            nop
            
            ; advance src/dst a row
            addiu $t2,subFontBytesPerRow
            j @@rowCopyLoop
            addiu $t3,videoSliceBytesPerRow
          @@rowCopyLoopDone:
          
          ; loop
          @@loopDone:
          j @@charDrawLoop
      @@doneAndIncrement:
          addiu $s0,subDrawBuf_bytesPerCharEntry
      @@done:
      ; return new lineptr
      move $v0,$s0
      
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
;      lw $s3,16($sp)
;      lw $s4,20($sp)
;      lw $s5,24($sp)
;      lw $s6,28($sp)
      jr $ra
      addiu $sp,16
    
  ;============================================================================
  ; subtitle interpreter
  ;============================================================================

    ;========================================
    ; memory
    ;========================================
    
    currentVideoId:
      .dw 0
    
    currentSubtitlePtr:
      .dw 0
    
    subtitleTimer:
      .dw 0
    
    subtitleBlockBufAPtr:
      .dw 0
    subtitleBlockBufBPtr:
      .dw 0
    
    subtitleBlockBufA_baseY:
      .dw 208
    subtitleBlockBufB_baseY:
      .dw 32
    subtitleTotalW:
      .dw 320
    
    videoSlicePixelX:
      .dw 0
    
    mdecPixelBufferPtr:
      .dw 0
    
    video_oldFontIndex:
      .dw 0

    ;========================================
    ; handle next frame
    ;========================================
      
    runSubtitleInterpreter_stackSize equ 8
    
    runSubtitleInterpreter:
      subiu $sp,runSubtitleInterpreter_stackSize
      sw $ra,0($sp)
      sw $s0,4($sp)
;        sw $s1,8($sp)
        
        ; s0 = pointer to srcptr
        la $s0,currentSubtitlePtr
        
        @@interpreterLoop:
          ; a0 = srcptr
          lw $a0,0($s0)
;          li $a1,~0x3
          nop
          
          ; do nothing if srcptr null
          beq $a0,$zero,@@done
          ; enforce op alignment by rounding up to next word boundary
;          addiu $a0,3
;          and $a0,$a1
          nop
          
          ; a1 = fetch opcode from src
          lbu $a1,0($a0)
          li $a2,subOp_terminator
          ; increment src
;            addi $a0,1
          ; save updated srcptr
;            sw $a0,0($s0)
          
          ; do nothing if terminator
          beq $a1,$a2,@@done
          la $a2,subOp_handlerTable
          
          ; otherwise, call op handler from table
          sll $a1,2
          addu $a1,$a2
          lw $v0,0($a1)
          nop
          jalr $v0
          nop
          
          ; handler returns 0 if we should continue the interpreter loop
          ; or nonzero otherwise
          beq $v0,$zero,@@interpreterLoop
          nop
      
      @@done:
      
      lw $ra,0($sp)
      lw $s0,4($sp)
;        lw $s1,8($sp)
      jr $ra
      addiu $sp,runSubtitleInterpreter_stackSize

      ;==================================
      ; op handler call table
      ;==================================
      
      subOp_handlerTable:
        ; 00: subOp_terminator (dummy)
        .dw 0
        ; 01: subOp_endSub
        .dw 0
        ; 02: subOp_swapSubBuf
        .dw 0
        ; 03: subOp_waitUntil
        .dw subOp_waitUntil_handler
        ; 04: subOp_queueSubA
        .dw subOp_queueSubA_handler
        ; 05: subOp_queueSubB
        .dw subOp_queueSubB_handler

    ;==================================
    ; op handlers
    ;
    ; a0 = value of currentSubtitlePtr
    ; 
    ; handlers must:
    ; - update currentSubtitlePtr in memory
    ;   if needed
    ; - return 0 if the interpreter
    ;   should continue running or
    ;   nonzero if not
    ;==================================
      
      subOp_waitUntil_handler:
        ; a1 = target counter value
;        lw $a1,4($a0)
        lbu $v0,2($a0)
        lbu $a1,1($a0)
        sll $v0,8
        or $a1,$v0
        ; a2 = current counter value
        lw $a2,subtitleTimer
        li $v0,1
        
        ; if target value not reached, do nothing and return nonzero
        ; to break interpreter loop
        blt $a2,$a1,@@done
        nop
          
          ; target value reached or exceeded:
          ; advance srcptr
          addiu $a0,3
          sw $a0,currentSubtitlePtr
          ; return 0 = continue interpreter loop
          li $v0,0
        
        @@done:
        jr $ra
        nop
      
      subOp_queueSubA_handler:
        subiu $sp,8
        sw $ra,0($sp)
        sw $s0,4($sp)
        
          ; s0 = srcptr
;          move $s0,$a0
          
          ; a0 = src
          ; (skip opcode to get string pointer)
          addiu $a0,1
          
          ; a1 = dst
          lw $a1,subtitleBlockBufAPtr
          ; a2 = y-center
          lw $a2,subtitleBlockBufA_baseY
          jal prepSubtitle
          nop
          
          ; update srcptr from return value
          sw $v0,currentSubtitlePtr
        
        @@done:
        ; return 0 = continue interpreter loop
        li $v0,0
        
        lw $ra,0($sp)
        lw $s0,4($sp)
        jr $ra
        addiu $sp,8
      
      subOp_queueSubB_handler:
        subiu $sp,8
        sw $ra,0($sp)
        sw $s0,4($sp)
          
          ; a0 = src
          ; (skip opcode to get string pointer)
          addiu $a0,1
          
          ; a1 = dst
          lw $a1,subtitleBlockBufBPtr
          ; a2 = y-center
          lw $a2,subtitleBlockBufB_baseY
          jal prepSubtitle
          nop
          
          ; update srcptr from return value
          sw $v0,currentSubtitlePtr
        
        @@done:
        ; return 0 = continue interpreter loop
        li $v0,0
        
        lw $ra,0($sp)
        lw $s0,4($sp)
        jr $ra
        addiu $sp,8
    
  ;============================================================================
  ; subtitles
  ;============================================================================
    
    .align 4
    videoSubtitleIndex:
      makeVideoSubtitleIndexEntry "AYAKO.STR", videoSub_ayako
      makeVideoSubtitleIndexEntry "EF.STR", videoSub_ef
      makeVideoSubtitleIndexEntry "EPILOGUE.STR", videoSub_epilogue
      makeVideoSubtitleIndexEntry "ERIKA.STR", videoSub_erika
      makeVideoSubtitleIndexEntry "MEIKUI.STR", videoSub_meikui
      makeVideoSubtitleIndexEntry "OPENING.STR", videoSub_opening
      makeVideoSubtitleIndexEntry "PROLOG.STR", videoSub_prolog
      makeVideoSubtitleIndexEntry "PRINCESS.STR", videoSub_princess
      makeVideoSubtitleIndexEntry "SANSIMAI.STR", videoSub_sansimai
      makeVideoSubtitleIndexEntry "TENKA.STR", videoSub_tenka
      makeVideoSubtitleIndexEntry "TENSEI.STR", videoSub_tensei
      makeVideoSubtitleIndexEntry "YELLINE.STR", videoSub_yelline
      
      ; index terminator
      .dw 0x00000000
    
    ;==================================
    ; test
    ;==================================
      
;      testSubtitles:
/*        sub_waitUntilMinSec 0, 1.000
        
        sub_queueSubA "test string\nline 2"
        
        sub_waitUntilMinSec 0, 2.000
        
        sub_queueSubA "another, longer test string\nand this is line 2 of it"
        
        sub_waitUntilMinSec 0, 3.000
        
        sub_queueSubA "an even longer test string than that one\nand this is the second line, which is also longer"
        
        sub_waitUntilMinSec 0, 4.000
        
        sub_queueSubA "shorter one-line string"
        sub_queueSubB "sub buffer b test"
        
        sub_waitUntilMinSec 0, 5.000
        
        sub_queueSubA ""
        sub_queueSubB ""*/
        
;        sub_waitUntilMinSec 0, 1.000
;        sub_queueSubA "out/script/video/AYAKO-0.bin"
        
        ; done
;        sub_terminator
    
    ;==================================
    ; AYAKO.STR
    ;==================================
      
      videoSub_ayako:
        
        sub_waitUntilMinSec 0, 7.692
        sub_queueSubA "out/script/video/AYAKO-0.bin"
;        sub_clearSubAMinSec 0, 8.627
        sub_clearSubAMinSec 0, 8.692
        
        sub_waitUntilMinSec 0, 18.215
        sub_queueSubA "out/script/video/AYAKO-1.bin"
        sub_clearSubAMinSec 0, 19.399
        
        sub_waitUntilMinSec 0, 20.870
        sub_queueSubA "out/script/video/AYAKO-2.bin"
        sub_clearSubAMinSec 0, (22.418-0.400)
        
        sub_waitUntilMinSec 0, 23.059
        sub_queueSubA "out/script/video/AYAKO-3.bin"
        
        sub_waitUntilMinSec 0, 27.561
        sub_queueSubA "out/script/video/AYAKO-4.bin"
        sub_clearSubAMinSec 0, 29.142
        
        sub_waitUntilMinSec 0, 30.215
        sub_queueSubA "out/script/video/AYAKO-5.bin"
        sub_clearSubAMinSec 0, 31.215
        
        sub_waitUntilMinSec 0, 34.760
        sub_queueSubA "out/script/video/AYAKO-6.bin"
        
        sub_waitUntilMinSec 0, 37.094
        sub_queueSubA "out/script/video/AYAKO-7.bin"
        sub_clearSubAMinSec 0, 38.653
        
        ; done
        sub_terminator
    
    ;==================================
    ; EF.STR
    ;==================================
      
      videoSub_ef:
        
        sub_waitUntilMinSec 0, 16.458
        sub_queueSubA "out/script/video/EF-0.bin"
        
        sub_waitUntilMinSec 0, 18.260
        sub_queueSubA "out/script/video/EF-1.bin"
;        sub_clearSubAMinSec 0, 19.709
        sub_clearSubAMinSec 0, 19.260
        
        sub_waitUntilMinSec 0, 35.481
        sub_queueSubA "out/script/video/EF-2.bin"
        
        sub_waitUntilMinSec 0, 40.329
        sub_queueSubA "out/script/video/EF-3.bin"
        sub_clearSubAMinSec 0, 42.431
        
        sub_waitUntilMinSec 0, 44.756
        sub_queueSubA "out/script/video/EF-4.bin"
        sub_clearSubAMinSec 0, 46.466
        
        ; done
        sub_terminator
    
    ;==================================
    ; ERIKA.STR
    ;==================================
      
      videoSub_erika:
        
        sub_waitUntilMinSec 0, 15.600
        sub_queueSubA "out/script/video/ERIKA-0.bin"
        sub_clearSubAMinSec 0, 17.779
        
        sub_waitUntilMinSec 0, 18.368
        sub_queueSubA "out/script/video/ERIKA-2.bin"
        
        sub_waitUntilMinSec 0, 20.353
        sub_queueSubA "out/script/video/ERIKA-3.bin"
        sub_clearSubAMinSec 0, 22.258
        
        sub_waitUntilMinSec 0, 23.511
        sub_queueSubA "out/script/video/ERIKA-5.bin"
        
        sub_waitUntilMinSec 0, 25.626
        sub_queueSubA "out/script/video/ERIKA-6.bin"
        
        sub_waitUntilMinSec 0, 28.105
        sub_queueSubA "out/script/video/ERIKA-7.bin"
;        sub_clearSubAMinSec 0, 29.347
        sub_clearSubAMinSec 0, 29.105
        
        sub_waitUntilMinSec 0, 30.395
        sub_queueSubA "out/script/video/ERIKA-9.bin"
        
        sub_waitUntilMinSec 0, 32.784
        sub_queueSubA "out/script/video/ERIKA-10.bin"
        
        sub_waitUntilMinSec 0, 34.016
        sub_queueSubA "out/script/video/ERIKA-11.bin"
        sub_clearSubAMinSec 0, 35.432
        
        sub_waitUntilMinSec 0, 37.158
        sub_queueSubA "out/script/video/ERIKA-13.bin"
        sub_clearSubAMinSec 0, 39.684
        
        ; done
        sub_terminator
    
    ;==================================
    ; EPILOGUE.STR
    ;==================================
    
      videoSub_epilogue:
        
        ; "sister!"
        sub_waitUntilMinSec 0, 11.205
        sub_queueSubA "out/script/video/EPILOGUE-0.bin"
        sub_clearSubAMinSec 0, 12.205
        
        ; "yui-hua!"
        sub_waitUntilMinSec 0, 15.690
        sub_queueSubA "out/script/video/EPILOGUE-1.bin"
        sub_clearSubAMinSec 0, 16.826
        
        ; "the power of darkness"
        sub_waitUntilMinSec 0, 39.477
        sub_queueSubA "out/script/video/EPILOGUE-2.bin"
        
        ; "th-this is"
        sub_waitUntilMinSec 0, 42.416
        sub_queueSubA "out/script/video/EPILOGUE-3.bin"
        
        ; "the power of light"
        sub_waitUntilMinSec 0, 43.993
        sub_queueSubA "out/script/video/EPILOGUE-4.bin"
        sub_clearSubAMinSec 0, (45.739-0.400)
        
        ; "yui-hua"
        sub_waitUntilMinSec 0, 53.160
        sub_queueSubA "out/script/video/EPILOGUE-5.bin"
        sub_clearSubAMinSec 0, 54.476
        
        ; "sister"
        sub_waitUntilMinSec 0, 56.464
        sub_queueSubA "out/script/video/EPILOGUE-6.bin"
;        sub_clearSubAMinSec 0, 57.515
        
        ; "so you still call"
        sub_waitUntilMinSec 0, 57.821
        sub_queueSubA "out/script/video/EPILOGUE-7.bin"
        sub_clearSubAMinSec 1, 0.678
        
        ; "sister"
        sub_waitUntilMinSec 1, 1.178
        sub_queueSubA "out/script/video/EPILOGUE-8.bin"
        sub_clearSubAMinSec 1, 2.178
        
        ; "i longed after you"
        sub_waitUntilMinSec 1, 4.211
        sub_queueSubA "out/script/video/EPILOGUE-9.bin"
        
        ; "you, who were"
        sub_waitUntilMinSec 1, 7.144
        sub_queueSubA "out/script/video/EPILOGUE-10.bin"
        sub_clearSubAMinSec 1, 10.195
        
        ; "but before i knew it"
        sub_waitUntilMinSec 1, 10.867
        sub_queueSubA "out/script/video/EPILOGUE-11.bin"
        
        ; "the responsibility is"
        sub_waitUntilMinSec 1, 15.538
        sub_queueSubA "out/script/video/EPILOGUE-12.bin"
        sub_clearSubAMinSec 1, 20.627
        
        ; "i'm sorry"
        sub_waitUntilMinSec 1, 21.440
        sub_queueSubA "out/script/video/EPILOGUE-13.bin"
        sub_clearSubAMinSec 1, (22.653-0.200)
        
        ; "yuna kagurazaka"
;        sub_waitUntilMinSec 1, (25.362-0.100)
        sub_waitUntilMinSec 1, (25.362-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-14.bin"
        sub_clearSubAMinSec 1, 27.058
        
        ; "thank you"
;        sub_waitUntilMinSec 1, (28.271-0.100)
        sub_waitUntilMinSec 1, (28.271-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-15.bin"
        sub_clearSubAMinSec 1, 29.520
        
        ; "this is the only"
        sub_waitUntilMinSec 1, 31.852
        sub_queueSubA "out/script/video/EPILOGUE-16.bin"
        sub_clearSubAMinSec 1, 35.633
        
        ; "take care of"
        sub_waitUntilMinSec 1, 36.552
        sub_queueSubA "out/script/video/EPILOGUE-17.bin"
        sub_clearSubAMinSec 1, 38.201
        
        ; "everyone"
        sub_waitUntilMinSec 1, 59.480
        sub_queueSubA "out/script/video/EPILOGUE-18.bin"
        sub_clearSubAMinSec 2, 0.858
        
        ; "people's hearts are"
        sub_waitUntilMinSec 2, 2.189
        sub_queueSubA "out/script/video/EPILOGUE-19.bin"
        
        ; "but they hold a"
        sub_waitUntilMinSec 2, 4.238
        sub_queueSubA "out/script/video/EPILOGUE-20.bin"
        
        ; "in every person's heart"
;        sub_waitUntilMinSec 2, (9.227-0.050)
        sub_waitUntilMinSec 2, (9.227-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-21.bin"
        
        ; "and so, too, does"
;        sub_waitUntilMinSec 2, (12.297-0.050)
        sub_waitUntilMinSec 2, (12.297-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-22.bin"
;        sub_clearSubAMinSec 2, 14.319
        
        ; "it's warm"
        sub_waitUntilMinSec 2, 14.798
        sub_queueSubA "out/script/video/EPILOGUE-23.bin"
        sub_clearSubAMinSec 2, 16.374
        
        ; "thank you"
;        sub_waitUntilMinSec 2, (17.782-0.050)
        sub_waitUntilMinSec 2, (17.782-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-24.bin"
        
        ; "for believing in"
        sub_waitUntilMinSec 2, 19.567
        sub_queueSubA "out/script/video/EPILOGUE-25.bin"
        sub_clearSubAMinSec 2, 22.358
        
        ; "so you're leaving"
;        sub_waitUntilMinSec 2, (23.524-0.050)
        sub_waitUntilMinSec 2, (23.524-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-26.bin"
        
        ; "will you come again"
;        sub_waitUntilMinSec 2, (25.326-0.050)
        sub_waitUntilMinSec 2, (25.326-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-27.bin"
        
        ; "yep! we'll"
;        sub_waitUntilMinSec 2, (27.311-0.050)
        sub_waitUntilMinSec 2, (27.311-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-33.bin"
        
        ; "miss yui-hua, you come and"
;        sub_waitUntilMinSec 2, (29.319-0.050)
        sub_waitUntilMinSec 2, (29.319-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-34.bin"
        
        ; "i'll show you around"
        sub_waitUntilMinSec 2, 31.340
        sub_queueSubA "out/script/video/EPILOGUE-35.bin"
        
        ; "yes"
;        sub_waitUntilMinSec 2, (33.530-0.050)
        sub_waitUntilMinSec 2, (33.530-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-36.bin"
        
        ; "the hamburger and ice cream"
        sub_waitUntilMinSec 2, 34.402
        sub_queueSubA "out/script/video/EPILOGUE-37.bin"
        
        ; "hey, that's nothing but"
        sub_waitUntilMinSec 2, 38.310
        sub_queueSubA "out/script/video/EPILOGUE-38.bin"
        sub_clearSubAMinSec 2, (40.574-0.100)
        
        ; "i'll be looking forward"
;        sub_waitUntilMinSec 2, (42.512-0.200)
        sub_waitUntilMinSec 2, (42.512-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-40.bin"
        sub_clearSubAMinSec 2, 45.039
        
        ; "see you, yuna"
;        sub_waitUntilMinSec 2, (45.851-0.150)
        sub_waitUntilMinSec 2, (45.851-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-41.bin"
        sub_clearSubAMinSec 2, 47.212
        
        ; "polylina"
;        sub_waitUntilMinSec 2, (48.502-0.100)
        sub_waitUntilMinSec 2, (48.502-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-42.bin"
        sub_clearSubAMinSec 2, (50.610-0.300)
        
        ; "well, shall we"
;        sub_waitUntilMinSec 2, (51.175-0.200)
        sub_waitUntilMinSec 2, (51.175-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-43.bin"
        
        ; "to earth"
;        sub_waitUntilMinSec 2, (53.225-0.100)
        sub_waitUntilMinSec 2, (53.225-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-44.bin"
        sub_clearSubAMinSec 2, 54.232
        
        ; "okay"
;        sub_waitUntilMinSec 2, (55.074-0.200)
        sub_waitUntilMinSec 2, (55.074-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-45.bin"
        
        ; "okay"
;        sub_waitUntilMinSec 2, (56.105-0.200)
        sub_waitUntilMinSec 2, (56.105-0.000)
        sub_queueSubA "out/script/video/EPILOGUE-46.bin"
        sub_clearSubAMinSec 2, 57.707
        
        ; done
        sub_terminator
    
    ;==================================
    ; MEIKUI.STR
    ;==================================
      
      videoSub_meikui:
        
        sub_waitUntilMinSec 0, 22.789
        sub_queueSubA "out/script/video/MEIKUI-1.bin"
        sub_clearSubAMinSec 0, 23.789
        
        sub_waitUntilMinSec 0, 24.720
        sub_queueSubA "out/script/video/MEIKUI-2.bin"
        
        sub_waitUntilMinSec 0, 25.912
        sub_queueSubA "out/script/video/MEIKUI-3.bin"
        sub_clearSubAMinSec 0, (27.607-0.200)
        
        sub_waitUntilMinSec 0, 29.262
        sub_queueSubA "out/script/video/MEIKUI-4.bin"
        
        sub_waitUntilMinSec 0, 30.445
        sub_queueSubA "out/script/video/MEIKUI-5.bin"
        
        sub_waitUntilMinSec 0, 33.056
        sub_queueSubA "out/script/video/MEIKUI-6.bin"
        sub_clearSubAMinSec 0, 34.056
        
        sub_waitUntilMinSec 0, 39.691
        sub_queueSubA "out/script/video/MEIKUI-8.bin"
        sub_clearSubAMinSec 0, 41.095
        
        sub_waitUntilMinSec 0, 43.962
        sub_queueSubA "out/script/video/MEIKUI-10.bin"
        
        sub_waitUntilMinSec 0, 45.293
        sub_queueSubA "out/script/video/MEIKUI-11.bin"
        sub_clearSubAMinSec 0, 46.495
        
        sub_waitUntilMinSec 0, 48.987
;        sub_waitUntilMinSec 0, 49.766
        sub_queueSubA "out/script/video/MEIKUI-12.bin"
        sub_clearSubAMinSec 0, 51.411
        
        sub_waitUntilMinSec 0, 51.776
        sub_queueSubA "out/script/video/MEIKUI-14.bin"
        sub_clearSubAMinSec 0, 53.677
        
        sub_waitUntilMinSec 0, 54.170
;        sub_waitUntilMinSec 0, 54.958
        sub_queueSubA "out/script/video/MEIKUI-15.bin"
        sub_clearSubAMinSec 0, 56.653
        
        sub_waitUntilMinSec 0, 59.182
        sub_queueSubA "out/script/video/MEIKUI-18.bin"
        sub_clearSubAMinSec 1, (1.943-0.400)
        
        sub_waitUntilMinSec 1, 2.347
;        sub_waitUntilMinSec 1, 3.786
        sub_queueSubA "out/script/video/MEIKUI-19.bin"
        sub_clearSubAMinSec 1, (5.707-0.200)
        
        ; done
        sub_terminator
    
    ;==================================
    ; OPENING.STR
    ;==================================
      
      videoSub_opening:
        
        ; "hohoemi wa kurayami no naka de"
        sub_waitUntilMinSec 0, 7.292
        sub_queueSubB "out/script/video/OPENING-5.bin"
        sub_queueSubA "out/script/video/OPENING-6.bin"
        
        ; "yuuki o terasu"
        sub_waitUntilMinSec 0, 11.134
        sub_queueSubB "out/script/video/OPENING-7.bin"
        sub_queueSubA "out/script/video/OPENING-8.bin"
        
        ; "hohoemi wa kanashimi o keshite"
        sub_waitUntilMinSec 0, 15.035
        sub_queueSubB "out/script/video/OPENING-9.bin"
        sub_queueSubA "out/script/video/OPENING-10.bin"
        
        ; "nanairo no niji o"
        sub_waitUntilMinSec 0, 18.913
        sub_queueSubB "out/script/video/OPENING-11.bin"
        sub_queueSubA "out/script/video/OPENING-12.bin"
        sub_clearSubAMinSec 0, (25.457-1.000)
        
        ; "yuna"
        sub_queueSubB "out/script/video/OPENING-1.bin"
        sub_clearSubBMinSec 0, 27.832
        
        ; "legend of the galaxy fraulein"
;        sub_waitUntilMinSec 0, 32.670
        sub_waitUntilMinSec 0, (31.043-0.100)
        sub_queueSubB "out/script/video/OPENING-0.bin"
;        sub_clearSubBMinSec 0, 34.907
        
        ; "tatoeba yozora o"
        sub_waitUntilMinSec 0, 35.390
        sub_queueSubB "out/script/video/OPENING-13.bin"
        sub_queueSubA "out/script/video/OPENING-14.bin"
        sub_clearSubABMinSec 0, (41.582+0.168)
        
        ; "chiisana hoshikuzu"
;        sub_waitUntilMinSec 0, 43.132
        sub_waitUntilMinSec 0, (43.132+0.068)
        sub_queueSubB "out/script/video/OPENING-15.bin"
        sub_queueSubA "out/script/video/OPENING-16.bin"
        sub_clearSubABMinSec 0, 49.444
        
        ; "ima sugu uketomete iku wa"
;        sub_waitUntilMinSec 0, 50.599
        sub_waitUntilMinSec 0, 50.635
;        sub_waitUntilMinSec 0, 50.414
        sub_queueSubB "out/script/video/OPENING-17.bin"
        sub_queueSubA "out/script/video/OPENING-18.bin"
        
        ; "namida ni kawaranai"
;        sub_waitUntilMinSec 0, 54.285
        sub_waitUntilMinSec 0, 54.503
        sub_queueSubB "out/script/video/OPENING-19.bin"
        sub_queueSubA "out/script/video/OPENING-20.bin"
        
        ; "kono mama miushinatte"
        sub_waitUntilMinSec 0, 58.376
;        sub_waitUntilMinSec 0, 58.163
        sub_queueSubB "out/script/video/OPENING-21.bin"
        sub_queueSubA "out/script/video/OPENING-22.bin"
;        sub_clearSubABMinSec 1, 5.906
        sub_clearSubABMinSec 1, 5.756
        
        ; "watashi nara itsumo koko ni iru"
;        sub_waitUntilMinSec 1, (7.308-0.100)
        sub_waitUntilMinSec 1, (7.308-0.000)
        sub_queueSubB "out/script/video/OPENING-23.bin"
        sub_queueSubA "out/script/video/OPENING-24.bin"
        
        ; "ichiban chikaku de"
        sub_waitUntilMinSec 1, 11.193
        sub_queueSubB "out/script/video/OPENING-25.bin"
        sub_queueSubA "out/script/video/OPENING-26.bin"
        
        ; "watashi ni wa minnna wakatteru"
        sub_waitUntilMinSec 1, 15.079
        sub_queueSubB "out/script/video/OPENING-27.bin"
        sub_queueSubA "out/script/video/OPENING-28.bin"
        
        ; "anata no egao o matteru"
;        sub_waitUntilMinSec 1, (18.965-0.100)
        sub_waitUntilMinSec 1, (18.965-0.000)
        sub_queueSubB "out/script/video/OPENING-29.bin"
        sub_queueSubA "out/script/video/OPENING-30.bin"
        
        ; "hohoemi wa kurayami no naka de"
;        sub_waitUntilMinSec 1, (22.844-0.100)
        sub_waitUntilMinSec 1, (22.844-0.000)
        sub_queueSubB "out/script/video/OPENING-31.bin"
        sub_queueSubA "out/script/video/OPENING-32.bin"
        
        ; "yuuki o terasu hikari"
;        sub_waitUntilMinSec 1, (26.700-0.100)
        sub_waitUntilMinSec 1, (26.700-0.000)
        sub_queueSubB "out/script/video/OPENING-33.bin"
        sub_queueSubA "out/script/video/OPENING-34.bin"
        
        ; "hohoemi wa kanashimi o keshite"
;        sub_waitUntilMinSec 1, (30.588-0.100)
        sub_waitUntilMinSec 1, (30.588-0.000)
        sub_queueSubB "out/script/video/OPENING-35.bin"
        sub_queueSubA "out/script/video/OPENING-36.bin"
        
        ; "nanairo niji o"
;        sub_waitUntilMinSec 1, (34.446-0.100)
        sub_waitUntilMinSec 1, (34.446-0.000)
        sub_queueSubB "out/script/video/OPENING-37.bin"
        sub_queueSubA "out/script/video/OPENING-38.bin"
        sub_clearSubABMinSec 1, (40.762-0.300)
        
        ; done
        sub_terminator
    
    ;==================================
    ; PROLOG.STR
    ;==================================
      
      videoSub_prolog:
        
        ; "i am revived"
        sub_waitUntilMinSec 0, 8.103
        sub_queueSubA "out/script/video/PROLOG-0.bin"
        sub_clearSubAMinSec 0, 10.143
        
        ; "i bring destruction"
        sub_waitUntilMinSec 0, 20.269
        sub_queueSubA "out/script/video/PROLOG-1.bin"
        sub_clearSubAMinSec 0, 24.036
        
        ; "all living beings"
        sub_waitUntilMinSec 0, 25.787
        sub_queueSubA "out/script/video/PROLOG-2.bin"
        sub_clearSubAMinSec 0, 30.561
        
        ; "i am the"
        sub_waitUntilMinSec 0, 32.774
        sub_queueSubA "out/script/video/PROLOG-3.bin"
        
        ; "the omniscient"
        sub_waitUntilMinSec 0, 35.384
        sub_queueSubA "out/script/video/PROLOG-4.bin"
        sub_clearSubAMinSec 0, 38.176
        
        ; "this is earth"
        sub_waitUntilMinSec 0, 42.682
        sub_queueSubA "out/script/video/PROLOG-5.bin"
        sub_clearSubAMinSec 0, 44.515
        
        ; done
        sub_terminator
    
    ;==================================
    ; PRINCESS.STR
    ;==================================
      
      videoSub_princess:
        
        sub_waitUntilMinSec 0, 6.010
        sub_queueSubA "out/script/video/PRINCESS-0.bin"
        sub_clearSubAMinSec 0, (9.231-0.200)
        
        sub_waitUntilMinSec 0, 10.494
        sub_queueSubA "out/script/video/PRINCESS-1.bin"
        sub_clearSubAMinSec 0, 11.494
        
        sub_waitUntilMinSec 0, 13.530
        sub_queueSubA "out/script/video/PRINCESS-2.bin"
        sub_clearSubAMinSec 0, (14.897-0.100)
        
        sub_waitUntilMinSec 0, 15.738
        sub_queueSubA "out/script/video/PRINCESS-3.bin"
        sub_clearSubAMinSec 0, 18.325
        
        sub_waitUntilMinSec 0, 19.227
        sub_queueSubA "out/script/video/PRINCESS-4.bin"
        sub_clearSubAMinSec 0, 20.842
        
        sub_waitUntilMinSec 0, 21.484
        sub_queueSubA "out/script/video/PRINCESS-6.bin"
        
        sub_waitUntilMinSec 0, 24.714
        sub_queueSubA "out/script/video/PRINCESS-7.bin"
        sub_clearSubAMinSec 0, 26.032
        
        ; done
        sub_terminator
    
    ;==================================
    ; SANSIMAI.STR
    ;==================================
      
      videoSub_sansimai:
        
        sub_waitUntilMinSec 0, 12.344
        sub_queueSubA "out/script/video/SANSIMAI-0.bin"
;        sub_clearSubAMinSec 0, 13.378
        sub_clearSubAMinSec 0, 13.344
        
        sub_waitUntilMinSec 0, 14.217
        sub_queueSubA "out/script/video/SANSIMAI-1.bin"
        sub_clearSubAMinSec 0, (16.459-0.100)
        
        sub_waitUntilMinSec 0, 17.487
        sub_queueSubA "out/script/video/SANSIMAI-2.bin"
        
        sub_waitUntilMinSec 0, 19.361
        sub_queueSubA "out/script/video/SANSIMAI-3.bin"
        
        sub_waitUntilMinSec 0, 21.429
        sub_queueSubA "out/script/video/SANSIMAI-4.bin"
        
        sub_waitUntilMinSec 0, 23.042
        sub_queueSubA "out/script/video/SANSIMAI-5.bin"
        sub_clearSubAMinSec 0, (24.580-0.100)
        
        sub_waitUntilMinSec 0, 27.606
        sub_queueSubA "out/script/video/SANSIMAI-6.bin"
        
        sub_waitUntilMinSec 0, 30.811
        sub_queueSubA "out/script/video/SANSIMAI-7.bin"
        sub_clearSubAMinSec 0, 32.630
        
        sub_waitUntilMinSec 0, 44.005
        sub_queueSubA "out/script/video/SANSIMAI-8.bin"
        
        sub_waitUntilMinSec 0, 45.765
        sub_queueSubA "out/script/video/SANSIMAI-9.bin"
        sub_clearSubAMinSec 0, 47.329
        
        sub_waitUntilMinSec 0, 48.656
        sub_queueSubA "out/script/video/SANSIMAI-11.bin"
        sub_clearSubAMinSec 0, 50.323
        
        sub_waitUntilMinSec 0, 52.067
        sub_queueSubA "out/script/video/SANSIMAI-12.bin"
        sub_clearSubAMinSec 0, 53.067
        
        sub_waitUntilMinSec 0, 56.893
        sub_queueSubA "out/script/video/SANSIMAI-13.bin"
        sub_clearSubAMinSec 0, (59.024-0.200)
        
        sub_waitUntilMinSec 1, 6.419
        sub_queueSubA "out/script/video/SANSIMAI-14.bin"
        sub_clearSubAMinSec 1, 7.719
        
        ; done
        sub_terminator
    
    ;==================================
    ; TENSEI.STR
    ;==================================
      
      videoSub_tenka:
        
        sub_waitUntilMinSec 0, 11.890
        sub_queueSubA "out/script/video/TENKA-0.bin"
        sub_clearSubAMinSec 0, 12.890
        
        ; done
        sub_terminator
    
    ;==================================
    ; TENSEI.STR
    ;==================================
      
      videoSub_tensei:
        
        ; "i am revived"
        sub_waitUntilMinSec 0, 19.820
        sub_queueSubA "out/script/video/TENSEI-0.bin"
        sub_clearSubAMinSec 0, 21.839
        
        ; done
        sub_terminator
    
    ;==================================
    ; YELLINE.STR
    ;==================================
      
      videoSub_yelline:
        
        sub_waitUntilMinSec 0, 0.428
        sub_queueSubA "out/script/video/YELLINE-0.bin"
        
        sub_waitUntilMinSec 0, 2.319
        sub_queueSubA "out/script/video/YELLINE-1.bin"
        sub_clearSubAMinSec 0, 4.268
        
        ; done
        sub_terminator
    