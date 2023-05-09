
;========================================
; basic stuff
;========================================

.psx

.fixloaddelay

;=======================
; build settings
;=======================

  asm_debugOn equ 0

;=======================
; include auto-generated index files
;=======================



;========================================
; old functions
;========================================
  
  ; a0 = entry to insert after
  ; a1 = new entry
;  linkIntoDisplayList equ 0x8003FD50

  printConsecutiveStrings equ 0x80120C7C
  triggerXaPlayback equ 0x8012B094
  stopStreamedSound equ 0x8012B4D4
  redrawObj equ 0x8012C280
  initHeap equ 0x8012DF7C
  allocHeap equ 0x8012E1E8
  freeHeap equ 0x8012E218
  freeRenderSlotSet equ 0x8013E69C
  loadingWait equ 0x80132914
  printPopUpDialogue equ 0x80134FE8
  getConfigSetting equ 0x80135578
  hideObjAndNext equ 0x801356E4
  getPressedButtons equ 0x80135CD8
  getTriggeredButtons equ 0x80135D10
  checkIfConfirmButtonTriggered equ 0x80135DE4
  printTwoLineSysMessage equ 0x801370C4
  doPostCharPrintWait equ 0x80139C38
  printText equ 0x80139DA0
  sendToVramViaDispSlot equ 0x8013AA00
  findVramFreeSpaceForData equ 0x8013BF70
  sendGrpToFreeVramArea equ 0x8013C5D8
  setUpDispSlotParams equ 0x8013DAE8
  setUpWindowPos equ 0x80180288
  setUpWindowWidth equ 0x80180594
  setUpWindowHeight equ 0x80180698
  memcpy equ 0x8018AA4C
  memset equ 0x8018AA80
  
  strcmp equ 0x8018AB98
  strcpy equ 0x8018ABFC
  StoreImage equ 0x8018DD30
  waitForGpuQueue equ 0x8018DA6C
  sectorNumToBcdMsf equ 0x8019655C
  
  waitFrame equ 0x80189454

;========================================
; old memory
;========================================
  
;  nameScreenNameEntryBuffer equ 0x8012C624

  charDecodingTableArray equ 0x801B2A78

  rawButtonsPressed equ 0x801CD688
  rawButtonsPressedLastFrame equ 0x801CD6CC

  ; nonzero if in multiplayer mode?
  multiplayerFlag equ 0x801CC644
  multiplayerFlag_gp equ 0x46C
  ; id of controller currently receiving input in multiplayer:
  ; 1 = player 1, 2 = player 2
  ; (seems to just remain 1 for single player, as you'd expect)
  multiplayerCurrentControllerId equ 0x801CC646
  multiplayerCurrentControllerId_gp equ 0x46E
  
  currentDispSlot equ 0x801CCB58
  currentDispSlot_gp equ 0x980
  
  baseXaPlaybackSector equ 0x801CD114
  baseXaPlaybackSector_gp equ 0xF3C
  
  endXaPlaybackSector equ 0x801CD118
  endXaPlaybackSector_gp equ 0xF40
  
  currentXaPlaybackSector equ 0x801CD11C
  currentXaPlaybackSector_gp equ 0xF44
  
  printCbcFlag equ 0x801CD410
  
  optionCursor_gp equ 0xFC0
  optionStereo_gp equ 0xFC2
  optionVoice_gp equ 0xFC4
  
  currentMenuType equ 0x801CD248
  currentMenuType_gp equ 0x1070
  
  dispSlotSpecArray equ 0x801D25B2
  dispSlotVramDstArray equ 0x801F62D8

;========================================
; defines
;========================================

  ;======
  ; basic
  ;======
    
;  opCreditsCharsPerString equ 40
  debugFlag equ 0x801CD444
  debugFlagGp equ 0x126C
  
  button_l2 equ 0x0001
  button_r2 equ 0x0002
  button_l1 equ 0x0004
  button_r1 equ 0x0008
  button_triangle equ 0x0010
  button_circle equ 0x0020
  button_x equ 0x0040
  button_square equ 0x0080
  button_select equ 0x0100
  button_start equ 0x0800
  button_up equ 0x1000
  button_right equ 0x2000
  button_down equ 0x4000
  button_left equ 0x8000
  
  ; fast-forward button
  ffButton equ button_r2

  ;======
  ; heap
  ;======

  heapStart equ 0x80010000
  
  ; removal of the old font and character conversion tables
  ; frees up large amounts of memory, so we can reduce the size
  ; of the heap and have more static memory
  ; (strictly speaking, we don't need to adjust this value,
  ; but it should make any errors more immediately obvious)
  ; YUNA3.FON, YUNA3.TBL
  removedHeapFileSize equ 0x29E00+0x2E00
  oldHeapSize equ 0xF8000
  oldHeapEnd equ heapStart+oldHeapSize
  heapSize equ oldHeapSize-removedHeapFileSize
  heapEnd equ heapStart+heapSize
  
  newStaticMemBlockStart equ heapEnd
  newStaticMemBlockEnd equ oldHeapEnd

  ;======
  ; font and printing
  ;======
  
  printBufW equ 16
  printBufH equ 16
  printBufSize equ printBufW*printBufH
  printBufSendW equ 16
;  printBufSendH equ 12
  
  printBufSendH_std equ 12
  printBufSendH_narrow equ 12
  printBufSendH_subtitle equ 16
  
  charBufW equ 16
  charBufH equ 16
  charBufSize equ charBufW*charBufH
  
  fontCharW equ 12
  fontCharH equ 12
  fontCharSize equ (fontCharW*fontCharH)/2
  
  ; all text is drawn this many pixels lower to compensate
  ; for the lower height of the new font.
  ; old glyphs are 15(?) pixels high, new ones are 10
  fontExtraYOffset equ 2
  dialogueExtraYOffset equ -2
  
  printBufUninitColor equ 0xFF
  
  charRangeStart equ 0x10
  dteRangeStart equ 0x80
  
  opcode_absoluteStringRedirect equ 0x01
  opcode_tabledStringRedirect equ 0x02
  opcode_sSubWait equ 0x03
  opcode_sSubOff equ 0x06
  opcode_sSubVramOverride equ 0x07
  opcode_linebreak equ 0x0A
  char_digitSpace equ 0x1C
  char_space equ 0x20
  char_digitBase equ 0x30
  
  oldDigitSpacing equ 13
  newDigitSpacing equ 7
  
  linebreakHeight equ 12
;  linebreak4LineHeight equ 12
  
  ; characters are always squeezed together by this many pixels
  kerningBaseOffset equ -1
  kerningIndex_noKerningMarker equ 0xFFFF
  kerningRepeatLoopOffset equ 2
  
  ; number of entries at the start of the global string table that are skipped
  ; and omitted from the table index
  skippedInitialGlobalStringTableEntryCount equ 257
  
  fontIndex_std      equ 0
  fontIndex_narrow   equ 1
  fontIndex_subtitle equ 2
  
  fontDefaultKerning_std equ 0
  fontDefaultKerning_narrow equ -1
  fontDefaultKerning_subtitle equ -2
  
  fontGlyphW_std equ 12
  fontGlyphW_narrow equ 12
  fontGlyphW_subtitle equ 16
  
  fontGlyphH_std equ 12
  fontGlyphH_narrow equ 12
  fontGlyphH_subtitle equ 16
  
  ; if charDecodingTable num is this,
  ; use subtitle decoding table
  charDecodingTable_subtitleCode equ 0x100
  
  ; subtitles are sent to this index of the first of the 8-bit palette sets
  subtitleTargetSubPaletteBase equ 0xF0
  
  ; if this many or more print buffer flushes occur in a frame,
  ; force a frame wait.
  ; this was originally added to try to deal with reported issues on duckstation
  ; which ultimately turned out to be the result of inaccuracy in the default
  ; "recompiler" cpu execution mode. so, i've removed it.
;  maxPrintBufferSendsPerFrame equ 32

  ;======
  ; text positioning
  ;======
  
  statusTechDescWindowExtraWidth equ 0x3A
  
  ; the original game forces the unit action window to always be a certain
  ; minimum width even if all the strings shown in it are shorter, but imo
  ; it looks kind of awkward, so i'm dropping that
;  newMinUnitActionWindowW equ 7*14
  newMinUnitActionWindowW equ 1

  ;======
  ; streamed subtitles
  ;======
  
  ; lower = longer delay before events occur
;  sSub_xaTimingOffset equ -6

;========================================
; addtional include defines
;========================================
    
  .include "asm/includes/video_common.inc"

;============================================================================
; SLPS_014.51
;============================================================================

  newExeLoadAddr equ 0x80010000
  newExeEndAddr equ 0x801FF800
  
  newCodeStartSection equ 0x801FF000
  newCodeEndSection   equ 0x801FF800
  
;.open "out/asm/SLPS_014.51", 0x80110000-0x800
.open "out/asm/SLPS_014.51", newExeLoadAddr-0x800
; load address in exe header
.org newExeLoadAddr-0x800+0x18
  .word newExeLoadAddr
; size field in exe header
.org newExeLoadAddr-0x800+0x1C
  .word newExeEndAddr-newExeLoadAddr

  ;===============================================
  ; DEBUG:
  ; enable debug mode
  ;===============================================
  
  .if asm_debugOn
    ; after hudson logo, write v0 (which is currently 1) to debug flag
    ; instead of clearing it
    .org 0x80118090
    
      ; ???
      ; if on, starting a new game goes directly to the ending movie.
      ; however, it's checked many more times throughout gameplay...
      ; having it on during battle seems to just end up freezing the game, though
      ; (missing debug data?)
      sw $zero, 0x1268($gp)
  ;    sw $v0, 0x1268($gp)
      
      ; enables "scenario select" debug menu when starting new game
  ;    sw $zero, debugFlagGp($gp)
      sw $v0, debugFlagGp($gp)
      
      ; ? never checked, but written several times?
      sw $zero, 0x1248($gp)
  ;    sw $v0, 0x1248($gp)
  .endif
  
;    .org 0x80118094
;    .area 0x8001BEF4-.,0xFF
      
      
      
;    .endarea

  ;===============================================
  ; MODIFICATION:
  ; reduce heap size in proportion to memory
  ; freed from removed files
  ;===============================================

  .org 0x801186F4
    li.u $a1,heapSize
    jal initHeap
    li.l $a1,heapSize

  ;===============================================
  ; MODIFICATION:
  ; adv scrolling fast-forward
  ;===============================================
   
   advScroll_currentDistance_gp equ 0x08CE
   advScroll_totalDistance_gp equ 0x11C0
   advScroll_pixelsPerIter_gp equ 0x11E4
  
/*  .org 0x80133324
    j doAdvScrollFfDown
    nop
  
  .org 0x8013337C
    j doAdvScrollFfUp
    nop
  
  .org 0x801333D8
    j doAdvScrollFfRight
    nop
  
  .org 0x80133424
    j doAdvScrollFfLeft
    nop*/
  
  ; simpler: just scroll the entire distance at once if fast-forward pressed
  .org 0x801332C8
    
    j doAdvScrollFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; ff button skips char-by-char text printing
  ;===============================================
  
  ; include our fast-forward button when checking for buttons that
  ; skip text waiting period
  
  .org 0x80139C6C
    andi $s0, $v0, 0x00A0|ffButton
    addiu $v0, $zero, 0x00A0|ffButton
  
  .org 0x80139CD4
    andi $v0, $v0, 0x00A0|ffButton
  
  ; allow skip to happen before first character prints
;  .org 0x80139E84
;    j doCbcPrintStartFfCheck
;    addiu $s1, $s1, 0x000D

  ;===============================================
  ; MODIFICATION:
  ; ff button skips button wait prompt
  ;===============================================
  
  .org 0x8012F90C
    j doAdvButtonPromptFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; ff button stops streamed sound clips if we
  ; are waiting on them to finish
  ;===============================================
  
  .org 0x8012B47C
    j doStreamedSoundWaitFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; don't start playing streamed sound clips
  ; if ff button on
  ;===============================================
  
  .org 0x8012AF04
    j doStreamedSoundStartFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; flag specific abilities whose animations can't
  ; be skipped because they have hacks in the
  ; animation code that will cause it to fail
  ; if not run (e.g. teleport)
  ;===============================================
  
  .org 0x8016E960
    j doAbilitySkipBlacklistCheck
    sw $s3, 0x0014($sp)

  ;===============================================
  ; MODIFICATION:
  ; pre-attack prep
  ;===============================================
  
  .org 0x8016F284
    j doPreAttackFfPrep
    sw $v1, 0x003C($sp)

  ;===============================================
  ; MODIFICATION:
  ; post-attack cleanup
  ;===============================================
  
  .org 0x8016F4A4
    j doPostAttackFfCleanup
    nop

  ;===============================================
  ; MODIFICATION:
  ; do not allow abilities that load extra sprites
  ; under certain circumstances to be skipped;
  ; this leads to memory leaks.
  ; specifically, this affects ako+mako combo attacks,
  ; and possibly other similar moves.
  ;===============================================
  
  .org 0x8016F0DC
    j doSpecialAttackNoSkipCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; reset special attack noskip flag at start of function
  ; that uses it
  ;===============================================
    
  .org 0x8016E9E4
    j resetSpecialAttackNoSkipCheck
    addiu $sp, $sp, 0xFF30

  ;===============================================
  ; MODIFICATION:
  ; skip attack animation if ff button on
  ;===============================================
  
;  .org 0x80172B78
;    j doAttackAnimFfCheck
;    nop
  
  .org 0x80172410
    j doAttackAnimFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; no extra effects like "lasers" from attacks
  ; if ff button on
  ;===============================================
  
  .org 0x80173778
    j attackLaserStuffFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; don't do extra effects for enemy reaction
  ; if attack anim previously skipped with ff button
  ; (causes issues with e.g. mirage cannon)
  ;===============================================
  
  .org 0x80177C1C
    j enemyReactionExtraAnimFfCheck
    sw $t0, 0x0014($sp)

  ;===============================================
  ; MODIFICATION:
  ; don't do counterattack anim if ff button on
  ;===============================================
  
  ; TODO: we may not be skipping as many steps here as possible.
  ; but it's hard to tell which parts of this function actually
  ; need to run for it to work properly, so i've erred
  ; on the side of caution.
  ; also seems to have some display issues with duplicated sprites...
  ; hmm
;  .org 0x8017EA60
;    j doCounterattackAnimFfCheck
;    nop
  
;  .org 0x8017EA4C
;    j doCounterattackAnimFfCheck
;    sw $v0, 0x0038($v1)
  
  .org 0x8017E7D4
    j doCounterattackAnimFfCheck
    sh $v1, 0x0026($sp)

  ;===============================================
  ; MODIFICATION:
  ; skip hp drain anim if ff button on
  ;===============================================
  
  .org 0x80185628
    j doHpBarDrainAnimFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; skip hp restore anim if ff button on
  ;===============================================
  
  .org 0x801856CC
    j doHpBarRestoreAnimFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; show hp bar for less time after attack
  ; if ff button on
  ;===============================================
  
  .org 0x80185754
    j doPostHpDrainAnimFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; skip ep bar drain anim if ff button on
  ;===============================================
  
  .org 0x80185C20
    j doEpBarDrainAnimFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; skip ep bar restore anim if ff button on
  ;===============================================
  
  .org 0x80185CC4
    j doEpBarRestoreAnimFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; show ep bar for less time after attack
  ; if ff button on
  ;===============================================
  
  .org 0x80185D4C
    j doPostEpDrainAnimFfCheck
    nop

  ;===============================================
  ; MODIFICATION:
  ; skip healing(?) anim if ff button on
  ;===============================================
  
  .org 0x8017CB90
    j doHealingAnimFfCheck
    sw $s1, 0x003C($sp)
    bltz $v0, 0x8017CBA8
    ; have to remove this -- ra is now written in the hack handler
;    sw $ra, 0x0040($sp)
    nop

  ;===============================================
  ; MODIFICATION:
  ; skip ep restore buff anim if ff button on
  ;===============================================
  
  .org 0x8017C538
    j doEpRestoreAnimFfCheck
    sw $s1, 0x003C($sp)
    bltz $v0, 0x8017C550
    ; have to remove this -- ra is now written in the hack handler
;    sw $ra, 0x0040($sp)
    nop

  ;===============================================
  ; MODIFICATION:
  ; skip buff(?) anim if ff button on
  ;===============================================
  
  .org 0x8017C95C
    j doBuffAnimFfCheck
    sw $s0, 0x0038($sp)

  ;===============================================
  ; MODIFICATION:
  ; skip status effect heal anim if ff button on
  ;===============================================
  
  .org 0x8017CD38
    j doStatusEffectHealAnimFfCheck
    sw $s1, 0x003C($sp)
    bltz $v0, 0x8017CD50
;    sw $ra, 0x0040($sp)
    nop

  ;===============================================
  ; MODIFICATION:
  ; skip debuff(?) anim if ff button on
  ;===============================================
  
  .org 0x8017CEEC
    j doDebuffAnimFfCheck
    sw $s0, 0x0038($sp)

  ;===============================================
  ; MODIFICATION:
  ; explosion skip if ff button on
  ;===============================================
  
  .org 0x80179498
    j doExplosionFfCheck
    sw $v0, 0x0010($sp)

  ;===============================================
  ; MODIFICATION:
  ; skip death blinkout anim if ff button on
  ;===============================================
  
  .org 0x8017968C
    j doDeathBlinkoutFfCheck
    addiu $s0, $s0, 0x0001

  ;===============================================
  ; MODIFICATION:
  ; don't do counterattack extras (e.g. lasers) anim if ff button on
  ;===============================================
  
/*  .org 0x8017EAC8
    j doCounterattackExtrasAnimFfCheck
    nop */

  ;===============================================
  ; MODIFICATION:
  ; skip status effect anims if ff button on
  ;===============================================
  
  ; sleep
  .org 0x80178520
    jal doStatusEffect1FfCheck
  ; greater sleep
  .org 0x80178554
    jal doStatusEffect2FfCheck
  ; confusion
  .org 0x80178588
    jal doStatusEffect3FfCheck
  ; greater confusion
  .org 0x801785BC
    jal doStatusEffect4FfCheck
  ; poison
  .org 0x801785F0
    jal doStatusEffect5FfCheck
  ; tech lock
  .org 0x80178624
    jal doStatusEffect6FfCheck
  ; paralyze
  .org 0x80178658
    jal doStatusEffect7FfCheck
  ; no items
  .org 0x8017868C
    jal doStatusEffect8FfCheck

  ;===============================================
  ; MODIFICATION:
  ; ff button can skip mission objective screen
  ;===============================================
  
  .org 0x8012C7BC
    j doMissionObjectiveFfCheck
    nop

  ;===============================================
  ; MODIFICATION (HACK):
  ; quick fix for game's sloppy input handling
  ; causing buttons to be misdetected as triggered
  ; because the game doesn't check them frequently
  ; enough
  ;===============================================
  
  ; moving unit with "move" command
  .org 0x8014DEF8
    jal doButtonTriggerFix
    nop

  ;===============================================
  ; MODIFICATION:
  ; fix up the horribly botched SS1 image
  ; decompression routine
  ;===============================================
  
    ;=========
    ; decompress halfwords directly to little endian
    ; so they don't have to be converted in a second
    ; pass after the fact
    ;=========
    
    ; repeat runs
    
    .org 0x8013A6B0
;    sb $v0, 0x0000($v1)
    sb $v0, 0x0001($v1)
    
    .org 0x8013A6BC
;    sb $v0, 0x0001($v1)
    sb $v0, 0x0000($v1)
    
    ; absolute halfwords
    
    .org 0x8013A728
;    sb $v1, 0x0000($v0)
;    sb $a0, 0x0001($v0)
    sb $v1, 0x0001($v0)
    sb $a0, 0x0000($v0)
    
    ; don't do endianness swap
    
    .org 0x8013A754
    j 0x8013A774
    nop
  
    ;=========
    ; copy repeat runs all at once instead of doing a
    ; an unnecessary exponentially growing copy loop.
    ; 
    ; the original code assumes memcpy may not be a simple
    ; byte-by-byte copy (as C makes no such guarantee).
    ; since we happen to know that this memcpy is, in fact,
    ; a simple byte-by-byte copy implementation, we can speed
    ; up decompression by just passing memcpy the full amount
    ; to copy from the get-go.
    ;=========
    
    .org 0x8013A6E8
    ; copySize = leftToCopy
    move $s0,$s1

  ;===============================================
  ; MODIFICATION:
  ; permanently cache the index to TALK.PIC
  ; (character portrait pack) so we don't have to
  ; read it from disc every single time the
  ; character portrait changes.
  ; this is only a patch on the disc seek time incurred
  ; from hopping between the voice file block and
  ; the portraits, but it's something.
  ;===============================================
  
    ;=========
    ; allocate permanent space on heap for index at startup
    ;=========
    
    .org 0x8012CD5C
      j allocForTalkPicIndexCache
      nop
  
    ;=========
    ; use cache memory when accessing TALK.PIC
    ;=========
    
    .org 0x80132A5C
      j doTalkPicCacheNeededCheck
      nop
  
    ;=========
    ; cache index on first access to TALK.PIC
    ;=========
    
    .org 0x80132AB0
      j doTalkPicCacheLoadNeededCheck
      addu $fp, $v0, $zero
  
    ;=========
    ; make sure cache doesn't get freed
    ;=========
    
    .org 0x80132B50
      jal freeIfNotTalkPic
    
    .org 0x80132B84
      jal freeIfNotTalkPic
    
    .org 0x80132BA4
      jal freeIfNotTalkPic

  ;===============================================
  ; MODIFICATION:
  ; 
  ;===============================================
  
    ;=========
    ; 
    ;=========
    
    .org 0x80131F9C
      j doLookingUpTalkPicCheck
      sw $s0, 0x0018($sp)
  
    ;=========
    ; 
    ;=========
    
    .org 0x80132044
      j doLookingUpTalkPicSubstitutionCheck
      nop
  
  ;===============================================
  ; MODIFICATION:
  ; skip level up animations if skip button
  ; pressed
  ;===============================================
    
    .org 0x80123750
      jal doLevelUpAnimSkipCheck
      nop
  
  ;===============================================
  ; MODIFICATION:
  ; skip level up animations if skip button
  ; pressed (nonstandard, e.g. earning exp at end
  ; of spaceship dock invasion)
  ;===============================================
    
    .org 0x80119348
      jal doSpecialLevelUpAnimSkipCheck
      nop
  
  ;===============================================
  ; MODIFICATION:
  ; skip "defense" animation if skip button
  ; pressed
  ;===============================================
  
    .org 0x8017D928
      j doDefenseAnimSkipCheck
      nop
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      

  ;===============================================
  ; MODIFICATION:
  ; text speed
  ;===============================================
    
    ; TODO: there is code that overwrites this default, so there is probably
    ; more work to be done here
    
    .org 0x801CCABA
;      .dh 0x0007
      .dh 0x0002

  ;===============================================
  ; MODIFICATION:
  ; replace printText
  ;===============================================
    
    .org 0x80139DE4
      ; don't increment src
      nop
    
    .org 0x80139DF0
      ; a0 = x
      ; a1 = y
      ; a2 = string pointer
      ; a3 = color modifier
      move $a0,$s1
      move $a1,$s2
      move $a2,$s0
      jal newPrintText
      move $a3,$s5
      
      j 0x80139EA0
      nop

  ;===============================================
  ; MODIFICATION:
  ; when printing via printStdDialogue, do not
  ; skip first byte of string if < 0x80
  ;===============================================
    
    .org 0x80134F0C
      j 0x80134F24
      nop

  ;===============================================
  ; MODIFICATION:
  ; do not skip first byte of pop-up messages
  ; (e.g. item acquisition) if it's < 0x80
  ;===============================================
  
    .org 0x80135074
;      addiu $s1, $s1, 0x0001
      nop

  ;===============================================
  ; MODIFICATION:
  ; use new de-optimized atk/def labels on status
  ; screen
  ;===============================================
    
    .org 0x80182E74
      lui $s0,hi(newStr_statusScreenAtk)
      addiu $s0,lo(newStr_statusScreenAtk)
    
    .org 0x80182F14
      lui $s0,hi(newStr_statusScreenDef)
      addiu $s0,lo(newStr_statusScreenDef)
    
    ; stupid fucking programmer. "oh i won't bother making a new string,
    ; i can just add +2 to the string the first time and then subtract 2 off
    ; to get what i want. surely this doesn't pose a maintainability problem
    ; at all. everyone knows you have to save every single byte possible
    ; at all times because you might need them later. surely all the lessons
    ; i learned programming pc-engine games are perfectly applicable to this
    ; new 32-bit environment. not that i did a very good job saving space on
    ; that either with my brilliant scripting language which encodes the size of
    ; the parameters into every single command, and which i am still reusing
    ; TO THIS VERY DAY, but you know."
    
    .org 0x80182EA8
      j doStatusScreenAtkLoadPatch
      addiu $a1, $zero, 0x0068
    
    .org 0x80182F48
      j doStatusScreenDefLoadPatch
      addiu $a1, $zero, 0x0068

  ;===============================================
  ; MODIFICATION:
  ; adjust printNumber for new text encoding
  ;===============================================
    
    ; default char = digit base (zero)
    .org 0x80181114
      addiu $t6, $zero, char_digitBase
    
    ; write only one byte for default char
    .org 0x80181180
;      sb $t5, 0x0021($a1)
      nop
    
    ; read/write back previously written byte for default char
    ; when converting to digit
    .org 0x801811B0
;      lbu $v0, 0x0021($a1)
      lbu $v0, 0x0020($a1)
    .org 0x801811BC
;      sb $v0, 0x0021($a1)
      sb $v0, 0x0020($a1)
    
    ; dst += 1 (not 2) when generating intial string
    .org 0x801811C8
;      addiu $a1, $a1, 0x0002
      addiu $a1, $a1, 0x0001
    
    ; put terminator at buffer+8, not buffer+16, to account for halved size
    .org 0x801811CC
;      sb $zero, 0x0040($sp)
      sb $zero, 0x0038($sp)
    
    ; new space char to overwrite leading zeroes with
    .org 0x801811E0
      addiu $t4, $zero, char_digitSpace
    
    ; overwrite only one byte when generating leading spaces
    .org 0x8018120C
;      sb $t3, 0x0021($a0)
      nop
    
    ; dst += 1 (not 2) when generating leading spaces
    .org 0x8018122C
      addiu $a0, $a0, 0x0001
    
    ; do not multiply by 2 when offsetting to target pos
    .org 0x80181260
;      sll $a2, $a2, 1
      nop
    
    ; offset x-position by newDigitSpacing per unprinted digit instead of 13
    .org 0x80181244
/*      ; *2
      sll $a0, $v0, 1
      ; *3
      addu $a0, $a0, $v0
      ; *12
      sll $a0, $a0, 2
      ; *13
      addu $a0, $a0, $v0*/
      ; *2
      sll $a0, $v0, 1
      ; *3
      addu $a0, $a0, $v0
      ; *6
      sll $a0, $a0, 1
      ; *7
      addu $a0, $a0, $v0
;      nop
    
    ; offset output x-position rightward so that the output is right-aligned
    ; to the same position as the original
    .org 0x80181264
      j doNumberPrintExtraRightOffset
      addu $a0, $t8, $a0
;      nop

  ;===============================================
  ; MODIFICATION:
  ; change dimensions of status screen technique
  ; description window to allow for more text
  ;===============================================
  
    .org 0x80183500
      ; box height in lines
      addiu $s1, $zero, 0x0003
  
    .org 0x8018350C
      ; additional box x-offset
      addiu $a2, $s4, 0x0046
  
    .org 0x80183514
      ; additional box y-offset
      addiu $a3, $s5, 0x0015
  
    .org 0x8018355C
      ; box width / 2
      ; (defined in terms of a bidirectional extent from a centerpoint)
;      addiu $a1, $zero, 0x0044
      ; this width matches that of the one-line window used for the
      ; technique descriptions in the action select menu
      addiu $a1, $zero, 0x0044+statusTechDescWindowExtraWidth
  
    ; x-offset of text from base point
    .org 0x801836F8
      addiu $s1, $s4, 0x000C-statusTechDescWindowExtraWidth
    .org 0x8018370C
      addiu $s1, $s4, 0x000C-statusTechDescWindowExtraWidth
    .org 0x80183744
      addiu $s1, $s4, 0x000C-statusTechDescWindowExtraWidth
    ; keep range label centered
    .org 0x801837E8
;      addu $a0, $s1, $zero
      addiu $a0, $s1, statusTechDescWindowExtraWidth
    ; for standard attack
    .org 0x801835D8
      addiu $s3, $s4, 0x000C-statusTechDescWindowExtraWidth

  ;===============================================
  ; MODIFICATION:
  ; change dimensions of action select technique
  ; description window to allow for more text
  ;===============================================
  
    ; note that the code here is shared among all generic vertically-scrolling
    ; selector menus such as attack, tech, and battle items,
    ; so care has to be taken to only affect the tech menu
  
    .org 0x801880DC
      j modifyActionSelTechDescWinPos
      ori $a3, $a3, 0x0001
    
/*    .org 0x80188100
      ; box height / 4?
;      addiu $a1, $zero, 0x0003
      addiu $a1, $zero, 0x000C*/
    
    .org 0x80188100
      j modifyActionSelTechDescWinDimensions
      nop

  ;===============================================
  ; MODIFICATION:
  ; use new strings for technique descriptions
  ;===============================================
  
    .include "out/script/asm/techdesc_overwrite.inc"

  ;===============================================
  ; MODIFICATION:
  ; only display one string for technique
  ; descriptions in status menu
  ;===============================================
  
    .org 0x801837E0
;      jal printText
      nop

  ;===============================================
  ; TODO:
  ; only display one string for regular attack
  ; descriptions in status menu?
  ;===============================================

  ;===============================================
  ; MODIFICATION:
  ; only display one string for technique
  ; descriptions in action select menu
  ;===============================================
  
    .org 0x80188998
      ; a2 = first string pointer
      lw $a2,0x0000($v1)
      ; the original game does a bunch of manipulations here
      ; to merge the individual strings for each line of the description
      ; into a single one-line buffer for it to print.
      ; we don't have to do any of it now.
      j 0x80188ACC
      nop

  ;===============================================
  ; MODIFICATION:
  ; correctly compute width of attack menu for
  ; new text characteristics
  ;===============================================
    
    ; get actual pixel width instead of raw byte count
    .org 0x80187F70
;      jal strlen
      jal getStringWidth
      nop
    
    ; skip byte length -> monospace pixel width conversion
    .org 0x80187FA0
      j 0x80187FC0
      ; extra width beyond string width
;      addiu $s1,8
      addiu $s1,8

  ;===============================================
  ; MODIFICATION:
  ; correctly compute width of unit name + action
  ; window shown when a unit performs an action
  ;===============================================
  
    unitActionTextExtraXOffset equ 4
  
    .org 0x8016C248
      jal getStringWidth
      nop
  
    .org 0x8016C26C
      jal getStringWidth
      addu $s0, $v0, $zero
    
    ; change from raw byte length to pixel width for determining
    ; minimum window size
    .org 0x8016C280
      slti $v0, $s0, newMinUnitActionWindowW
    .org 0x8016C288
      slti $v0, $s0, newMinUnitActionWindowW
    .org 0x8016C294
      addiu $s0, $zero, newMinUnitActionWindowW
    
    ; skip byte -> pixel width conversion
    .org 0x8016C2A8
      j 0x8016C2C8
      ; extra width?
      addiu $a2,$s0,0x10+unitActionTextExtraXOffset
    
    ; text x-position
    ; (original is flush with left border, which looks ugly)
    .org 0x8016C2D8
      addiu $a0, $zero, 0x000E+unitActionTextExtraXOffset
    .org 0x8016C2F8
      addiu $a0, $zero, 0x000E+unitActionTextExtraXOffset

  ;===============================================
  ; MODIFICATION:
  ; correctly compute width of item name window
  ; shown when using an item
  ;===============================================
  
    ; get raw pixel width instead of byte length
    .org 0x8016C3D0
      jal getStringWidth
      nop
    
    ; don't do normal byte length -> string width conversion
    .org 0x8016C3E0
      ; window x
      addiu $a0, $zero, 0x0008
      ; window y
      addiu $a1, $zero, 0x0008
      move $a2,$s0
      ; extra width of window compared to raw string length
      j 0x8016C408
      addiu $a2,0x0010+unitActionTextExtraXOffset
    
    ; position text further right in window so it's not flush against the left side
    .org 0x8016C418
      ; x
      addiu $a0, $zero, 0x000E+unitActionTextExtraXOffset
      ; y
      addiu $a1, $zero, 0x0010

  ;===============================================
  ; MODIFICATION:
  ; fix printConsecutiveStrings to work with
  ; new text system
  ;===============================================
  
  .org 0x80120CE4
    j doNewPrintConsecutiveStrings
    nop

  ;===============================================
  ; MODIFICATION:
  ; fix acquisition messages for non-random
  ; items hidden on map for new text system
  ;===============================================
  
  ; for whatever insane reason, hidden items found on the map
  ; use a completely different method for preparing their
  ; "item acquired" message compared to random items dropped
  ; by enemies, which involves allocating heap memory instead
  ; of simply using the stack. there's no reason whatsoever
  ; to do this, so we just modify the code to use the "regular"
  ; method instead.
  
  .org 0x80167020
    ; load delay
    nop
    ; first string (item name)
    move $a0,$a1
    ; second string ("acquired")
    la $a1,0x801156B0
    jal printConsecutiveStrings
    nop
    j 0x80167074
    nop

  ;===============================================
  ; MODIFICATION:
  ; use new printFixedPosString
  ;===============================================
    
    .org 0x8013A1AC
      j newPrintFixedPosString
      nop

  ;===============================================
  ; MODIFICATION:
  ; do not apply newExtraYOffset to text which
  ; appears in the standard dialogue box,
  ; so it will fit properly with four lines
  ;===============================================
    
    ; printStdDialogue
    .org 0x80134F5C
      jal printTextWithDialogueYOffset
    
    ; printPopUpDialogue
    .org 0x801350B0
      jal printTextWithDialogueYOffset

  ;===============================================
  ; MODIFICATION:
  ; fix asciiToSjis for new text system
  ;===============================================
    
    ; our new text system is ascii-compatible, so just copy the input
    ; string instead of doing any conversions
    .org 0x80123B5C
      ; don't advance src
      nop
      ; set up call to strcpy
      ; swap src/dst regs
      move $v0,$a0
      move $a0,$a1
      ; jump rather than call, because this function doesn't save regs
      j strcpy
      move $a1,$v0

  ;===============================================
  ; MODIFICATION:
  ; check for subtitle auto-start codes when
  ; preparing to play a streamed sound
  ;===============================================
  
    .org 0x8012AF54
      j doStreamedSubtitleQueueCheck
      nop

  ;===============================================
  ; MODIFICATION:
  ; start queued streamed subtitles
  ;===============================================
    
    .org 0x8012B320
      j doStreamedSubtitleStartCheck
      nop

  ;===============================================
  ; MODIFICATION:
  ; update streamed subtitles
  ;===============================================
    
    .org 0x80189460
      j doStreamedSubtitleWaitFrameUpdate
      sw $s0, 0x0010($sp)

  ;===============================================
  ; MODIFICATION:
  ; stop streamed subtitles when
  ; stopStreamedSound called
  ;===============================================
  
    .org 0x8012B52C
      j doStopStreamedSoundSubtitleStop
      nop

  ;===============================================
  ; MODIFICATION:
  ; continue rendering even when screen is fully
  ; faded out if subtitles are being displayed
  ;===============================================
  
    .org 0x801894EC
      j doFadeSubtitleRenderingCheck
      nop
  
  ;===============================================
  ; MODIFICATION:
  ; use new unit name abbreviation logic
  ;===============================================
    
    ;=====
    ; disable old, hardcoded abbreviation system
    ;=====
    
    ; no special handling for yuri's name
    .org 0x80184788
      j 0x801847C4
      nop
    
    ; everyone else
    .org 0x801847CC
      move $v0,$v1
    
    ; no special handling for element fairy (desert planet mission)
    .org 0x80184888
      j 0x801848F4
      addu $a0, $zero, $zero
    
    ; no special handling for copy robots
    .org 0x80184818
      j 0x80184880
      nop
    
    ;=====
    ; use new abbreviation string table
    ;=====
    
    .org 0x801847C4
      j checkForNewUnitNameAbbrev
      nop
  
  ;===============================================
  ; MODIFICATION:
  ; correctly generate polylina snack messages
  ; for new text encoding
  ;===============================================
    
    .org 0x80189858
      j queuePolylinaSnackString
;      sw $s0, 0x0010($sp)
      nop
    
    .org 0x80189974
      j doPolylinaSnackFirstDigit
      nop
    
    .org 0x8018998C
      j doPolylinaSnackSecondDigit
      nop
  
  ;===============================================
  ; MODIFICATION:
  ; adjust width of top-level "information" menu
  ; (status/items)
  ;===============================================
    
    .org 0x80183A78
      ; window width
;      addiu $a1, $zero, 0x003C
;      addiu $a1, $zero,28
      addiu $a1, $zero,34
  
  ;===============================================
  ; MODIFICATION:
  ; new 8x8-coded strings for battle interface
  ; text
  ;===============================================
    
    .org 0x801B2F40
      ; 00: lesser sleep
;      .db 0xBE,0xBB,0xBC,0xBD,0x00,0x00,0x00,0x00
      .db 0x1B,0x1C,0x1D,0x1E,0x1F,0x00,0x00,0x00
      ; 01: greater sleep
      .db 0x2B,0x2C,0x2D,0x2E,0x2F,0x00,0x00,0x00
      ; 02: lesser confusion
      .db 0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x00
      ; 03: greater confusion
      .db 0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0x00
      ; 04: paralysis
      .db 0x5B,0x5C,0x5D,0x5E,0x5F,0x00,0x00,0x00
      ; 05: tech seal
      .db 0x6B,0x6C,0x6D,0x6E,0x6F,0x00,0x00,0x00
      ; 06: item seal
      .db 0x7B,0x7C,0x7D,0x7E,0x7F,0x00,0x00,0x00
      ; 07: atk +150%
      .db 0xEB,0xEC,0xA0,0xA1,0xA2,0xA3,0x00,0x00
      ; 08: atk +200%
      .db 0xEB,0xEC,0xA0,0xA4,0xA5,0xA3,0x00,0x00
      ; 09: atk rising
;      .db 0xEB,0xEC,0xA0,0x4B,0x4C,0x00,0x00,0x00
;      .db 0xEB,0xEC,0xD0,0xD1,0x00,0x00,0x00,0x00
      .db 0xEB,0xEC,0xA0,0xD4,0xD5,0xD6,0x00,0x00
      ; 0A: def +150%
      .db 0xED,0xEE,0xA0,0xA1,0xA2,0xA3,0x00,0x00
      ; 0B: def +200%
      .db 0xED,0xEE,0xA0,0xA4,0xA5,0xA3,0x00,0x00
      ; 0C: def rising
;      .db 0xED,0xEE,0xA0,0x4B,0x4C,0x00,0x00,0x00
;      .db 0xED,0xEE,0xD0,0xD1,0x00,0x00,0x00,0x00
      .db 0xED,0xEE,0xA0,0xD4,0xD5,0xD6,0x00,0x00
      ; 0D: move up
      .db 0xFB,0xFC,0xA0,0x4B,0x4C,0x00,0x00,0x00
      ; 0E: move down
      .db 0xFB,0xFC,0xAC,0x4D,0x4E,0x4F,0x00,0x00
      ; 0F: atk -75%
      .db 0xEB,0xEC,0xAC,0xAD,0xAE,0xA3,0x00,0x00
      ; 10: atk -50%
      .db 0xEB,0xEC,0xAC,0xAF,0xA2,0xA3,0x00,0x00
      ; 11: atk falling
;      .db 0xEB,0xEC,0xAC,0x4D,0x4E,0x4F,0x00,0x00
;      .db 0xEB,0xEC,0xD2,0xD3,0x00,0x00,0x00,0x00
      .db 0xEB,0xEC,0xAC,0xD7,0xD8,0xD9,0x00,0x00
      ; 12: def -75%
      .db 0xED,0xEE,0xAC,0xAD,0xAE,0xA3,0x00,0x00
      ; 13: def -50%
      .db 0xED,0xEE,0xAC,0xAF,0xA2,0xA3,0x00,0x00
      ; 14: def falling
;      .db 0xED,0xEE,0xD2,0xD3,0x00,0x00,0x00,0x00
      .db 0xED,0xEE,0xAC,0xD7,0xD8,0xD9,0x00,0x00
      ; 15: stamina rising
;      .db 0x3B,0x3C,0x3D,0x3E,0xA0,0x4B,0x4C,0x00
;      .db 0xA6,0xA7,0xA8,0xA9,0xA0,0x00,0x00,0x00
;      .db 0xA6,0xA7,0xA8,0xA9,0xD0,0xD1,0x00,0x00
      .db 0xA6,0xA7,0xA0,0xD4,0xD5,0xD6,0x00,0x00
      ; 16: stamina falling
      ; (not enough room for "down" text at end)
;      .db 0xA6,0xA7,0xA8,0xA9,0xAC,0x00,0x00,0x00
;      .db 0xA6,0xA7,0xA8,0xA9,0xD2,0xD3,0x00,0x00
      .db 0xA6,0xA7,0xAC,0xD7,0xD8,0xD9,0x00,0x00
      ; 17: 0%
      .db 0x80,0xC2,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 18: +5%
      .db 0xA0,0xAF,0xC5,0xC4,0x00,0x00,0x00,0x00
      ; 19: +10%
      .db 0xA0,0xC6,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 1A: -5%
      .db 0xAC,0xAF,0xC5,0xC4,0x00,0x00,0x00,0x00
      ; 1B: -10%
      .db 0xAC,0xC6,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 1C: -10%
      .db 0xAC,0xC6,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 1D: "can't move" label for impassable terrain
      .db 0x9B,0x9C,0x9D,0x9E,0x9F,0x00,0x00,0x00
;      .db 0x3F,0x3F,0x9B,0x9C,0x9D,0x9E,0x9F,0x00
      ; 1E: 1
      .db 0xCD,0x00,0x00,0x00,0x00,0x00,0x00,0x00
      ; 1F: 2
      .db 0xCE,0x00,0x00,0x00,0x00,0x00,0x00,0x00
      ; 20: crank in
      .db 0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x00
    
/*
      ; 00: lesser sleep
      .db 0xBE,0xBB,0xBC,0xBD,0x00,0x00,0x00,0x00
      ; 01: greater sleep
      .db 0xBA,0xBB,0xBC,0xBD,0x00,0x00,0x00,0x00
      ; 02: lesser confusion
      .db 0xBE,0xBF,0xC0,0xC1,0xC0,0x00,0x00,0x00
      ; 03: greater confusion
      .db 0xBA,0xBF,0xC0,0xC1,0xC0,0x00,0x00,0x00
      ; 04: paralysis
      .db 0x95,0x96,0x97,0x98,0x00,0x00,0x00,0x00
      ; 05: tech seal
      .db 0x9A,0x9B,0x97,0x98,0x00,0x00,0x00,0x00
      ; 06: item seal
      .db 0x9C,0x9D,0x9E,0x97,0x98,0x00,0x00,0x00
      ; 07: atk +150%
      .db 0x9F,0xA0,0xA1,0xA2,0xA3,0x00,0x00,0x00
      ; 08: atk +200%
      .db 0x9F,0xA0,0xA4,0xA5,0xA3,0x00,0x00,0x00
      ; 09: atk +up
      .db 0x9F,0xA0,0xA6,0xA7,0xA8,0xA9,0x00,0x00
      ; 0A: def +150%
      .db 0xAA,0xA0,0xA1,0xA2,0xA3,0x00,0x00,0x00
      ; 0B: def +200%
      .db 0xAA,0xA0,0xA4,0xA5,0xA3,0x00,0x00,0x00
      ; 0C: def +up
      .db 0xAA,0xA0,0xA6,0xA7,0xA8,0xA9,0x00,0x00
      ; 0D: move up
      .db 0xAB,0xA0,0xA6,0xA7,0xA8,0x00,0x00,0x00
      ; 0E: move down
      .db 0xAB,0xAC,0xB0,0xB1,0xB2,0x00,0x00,0x00
      ; 0F: atk -75%
      .db 0x9F,0xAC,0xAD,0xAE,0xA3,0x00,0x00,0x00
      ; 10: atk -50%
      .db 0x9F,0xAC,0xAF,0xA2,0xA3,0x00,0x00,0x00
      ; 11: atk -down
      .db 0x9F,0xAC,0xB0,0xB1,0xB2,0xB3,0x00,0x00
      ; 12: def -75%
      .db 0xAA,0xAC,0xAD,0xAE,0xA3,0x00,0x00,0x00
      ; 13: def -50%
      .db 0xAA,0xAC,0xAF,0xA2,0xA3,0x00,0x00,0x00
      ; 14: def -down
      .db 0xAA,0xAC,0xB0,0xB1,0xB2,0xB3,0x00,0x00
      ; 15: stamina +up
      .db 0xB4,0xB5,0xA0,0xB6,0xB7,0xB8,0xB9,0x00
      ; 16: stamina -down
      .db 0xB4,0xB5,0xAC,0xB0,0xB1,0xB2,0xB3,0x00
      ; 17: 0%
      .db 0x80,0xC2,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 18: +5%
      .db 0xA0,0xAF,0xC5,0xC4,0x00,0x00,0x00,0x00
      ; 19: +10%
      .db 0xA0,0xC6,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 1A: -5%
      .db 0xAC,0xAF,0xC5,0xC4,0x00,0x00,0x00,0x00
      ; 1B: -10%
      .db 0xAC,0xC6,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 1C: -10%
      .db 0xAC,0xC6,0xC3,0xC4,0x00,0x00,0x00,0x00
      ; 1D: "can't move" label for impassable terrain
      .db 0x97,0x98,0x00,0x00,0x00,0x00,0x00,0x00
      ; 1E: 1
      .db 0xCD,0x00,0x00,0x00,0x00,0x00,0x00,0x00
      ; 1F: 2
      .db 0xCE,0x00,0x00,0x00,0x00,0x00,0x00,0x00
      ; 20: crank in
      .db 0xDA,0xDB,0xDC,0xDA,0xDD,0xDC,0x00,0x00*/
  
  ;===============================================
  ; MODIFICATION:
  ; new positioning for "impassable" label on
  ; terrain that can't be entered
  ;===============================================
    
    .org 0x80186250
      ; x-pos
;      addiu $a1, $zero, 0x0028
      addiu $a1, $zero, 0x0008
  
  ;===============================================
  ; MODIFICATION:
  ; offset digits of battle evaluation statistics
  ;===============================================
    
    battleEvalDigitXOffset equ 16
    
    .org 0x80124B2C
      addiu $a2, $zero, 0x00A4+battleEvalDigitXOffset
    .org 0x80124B4C
      addiu $a2, $zero, 0x00AC+battleEvalDigitXOffset
    .org 0x80124B80
      addiu $a2, $zero, 0x00AC+battleEvalDigitXOffset
    .org 0x80124BF4
      addiu $a2, $zero, 0x00B4+battleEvalDigitXOffset
    
    .org 0x80124C28
      addiu $a2, $zero, 0x00A4+battleEvalDigitXOffset
    .org 0x80124C4C
      addiu $a2, $zero, 0x00AC+battleEvalDigitXOffset
    .org 0x80124C6C
      addiu $a2, $zero, 0x00B4+battleEvalDigitXOffset
    .org 0x80124C94
      addiu $a2, $zero, 0x00AC+battleEvalDigitXOffset
    .org 0x80124CF0
      addiu $a2, $zero, 0x00B4+battleEvalDigitXOffset
    .org 0x80124D08
      addiu $a2, $zero, 0x00B4+battleEvalDigitXOffset
    .org 0x80124D30
      addiu $a2, $zero, 0x00B4+battleEvalDigitXOffset
  
  ;===============================================
  ; MODIFICATION:
  ; right-align labels on character list
  ;===============================================
    
    charList_labelOffsetLv equ 14
    charList_labelOffsetMhp equ 16
    charList_labelOffsetMep equ 16
    charList_labelOffsetHp equ 12
    charList_labelOffsetEp equ 12
    charList_labelOffsetAtk equ 4
    charList_labelOffsetTatk equ 12
    charList_labelOffsetDef equ 4
    charList_labelOffsetTdef equ 10
    charList_labelOffsetMov equ 2
    charList_labelOffsetSpd equ 17
    
    ; "LV"
    .org 0x8018156C
      addiu $a0, $zero, 0x008F+charList_labelOffsetLv
    
    ; "MHP"
    .org 0x801815B8
      addiu $a0, $zero, 0x00C3+charList_labelOffsetMhp
    
    ; "MEP"
    .org 0x801815E0
      addiu $a0, $zero, 0x0104+charList_labelOffsetMep
    
    ; "HP"
    .org 0x801815E4
      addiu $a0, $zero, 0x00D0+charList_labelOffsetHp
    
    ; "EP"
    .org 0x8018160C
      addiu $a0, $zero, 0x0111+charList_labelOffsetEp
    
    ; "ATK"
    .org 0x80181610
      addiu $a0, $zero, 0x00D0+charList_labelOffsetAtk
    
    ; "TATK"
    .org 0x80181638
     addiu $a0, $zero, 0x0104+charList_labelOffsetTatk
    
    ; "DEF"
    .org 0x8018163C
      addiu $a0, $zero, 0x00D0+charList_labelOffsetDef
    
    ; "TDEF"
    .org 0x80181664
     addiu $a0, $zero, 0x0104+charList_labelOffsetTdef
    
    ; "MOV"
    .org 0x80181668
      addiu $a0, $zero, 0x00D0+charList_labelOffsetMov
    
    ; "SPD"
    .org 0x80181684
     addiu $a0, $zero, 0x0104+charList_labelOffsetSpd
  
  ;===============================================
  ; MODIFICATION:
  ; adjust position of "LV" in status screen
  ; technique list
  ;===============================================
    
    techList_lvOffset equ 14
    
    ; "LV"
    .org 0x80183044
      addiu $a0, $zero, 0x00C8+techList_lvOffset
  
  ;===============================================
  ; MODIFICATION:
  ; adjust position of location string for save
  ; files
  ;===============================================
    
    saveFileLocation_extraXOffset equ -1
    saveFileLocation_extraYOffset equ -1
    
    ; x-offset
    .org 0x80136B4C
      addiu $a0, $zero, (0x0070+saveFileLocation_extraXOffset)
    
    ; additional y-offset
    .org 0x80136B58
      addiu $a1, $a1, (0x0012+saveFileLocation_extraYOffset)
  
  ;===============================================
  ; MODIFICATION:
  ; new options on options menu
  ;===============================================
  
    numNewOptionItems equ 2
    
    subtitlesOptionIndex equ 3
    ffModeOptionIndex equ 4
    
    ; when redrawing state of selected options, offset target obj
    ; indices to correspond to the new options added in OPT.CSA's TAN file
    .org 0x80135738
      addiu $s0, $s0, 0x0004+numNewOptionItems
    .org 0x80135770
      addiu $s0, $s0, 0x0006+numNewOptionItems
    .org 0x801357A0
      addiu $s0, $s0, 0x0008+numNewOptionItems

    ; account for new items when wrapping around top/bottom of list
    ; up pressed
    .org 0x80135910
      addiu $v0, $zero, 0x0003+numNewOptionItems
    ; down pressed
    .org 0x80135984
      slti $v0, $v0, 0x0004+numNewOptionItems
    
  ;===============================================
  ; MODIFICATION:
  ; draw new option states on options menu
  ;===============================================
  
    .org 0x801357B8
      j drawExtraOptionStates
      nop
    
  ;===============================================
  ; MODIFICATION:
  ; toggle new options on options menu
  ;===============================================
    
    .org 0x801359CC
      j checkForExtraOptionsToggle
      nop
    
  ;===============================================
  ; MODIFICATION:
  ; check new options in getConfigSetting
  ;===============================================
    
    .org 0x80135578
      j getConfigSetting_checkNewOptions
      nop
    
  ;===============================================
  ; MODIFICATION:
  ; check new options in toggleConfigSetting
  ;===============================================
  
    ; NOTE: there is no need to modify writeConfigSetting,
    ; as it's only used to reset the options if the menu is cancelled.
    ; since the halfwords are entirely copied and restored as-is,
    ; the new options will be restored along with the voice settings
    ; in this case.
  
    .org 0x801355E8
      j toggleConfigSetting_checkNewOptions
      nop
    
  ;===============================================
  ; MODIFICATION:
  ; directly copy option halfwords when preparing
  ; copy to restore from if options menu cancelled
  ;===============================================
    
    ; the original game uses calls to getConfigSetting here,
    ; which would be fine except we modified it to exclude the new options
    ; as needed.
    ; to work around this, we copy the full values directly.
    
    .org 0x8013582C
      lhu $a0,optionCursor_gp($gp)
      lhu $a1,optionVoice_gp($gp)
      lhu $v0,optionStereo_gp($gp)
      sh $a0,0x0FA4($gp)
      sh $a1,0x0FA8($gp)
;      sh $a2,0x0FA6($gp)
      j 0x8013584C
      nop
    
  ;===============================================
  ; MODIFICATION:
  ; do not ignore calls to writeConfigSetting
  ; whose value is >= 2
  ; (needed for above copy modification to work)
  ;===============================================
  
    .org 0x80135B3C
      nop
    
  ;===============================================
  ; MODIFICATION:
  ; directly copy options to save file instead of
  ; going through getConfigSettings
  ;===============================================
    
    .org 0x80136F28
      lhu $v0,optionCursor_gp($gp)
    .org 0x80136F34
      lhu $v0,optionStereo_gp($gp)
    .org 0x80136F40
      lhu $v0,optionVoice_gp($gp)
    
  ;===============================================
  ; MODIFICATION:
  ; exclude R2 from the buttons that contribute
  ; to button repeat conditions
  ;===============================================
    
    ; the original game uses very simple logic to decide whether the conditions
    ; for automated button repeats have occurred (e.g. holding right in the
    ; battle action select menu will cause it to repeatedly scroll
    ; through the action buttons). it's only checked that *any* button is pressed
    ; rather than any particular one, which causes unintuitive behavior
    ; when multiple buttons are held. of course, since R2 is now being used
    ; as a fast-forward, it's natural to hold it for long periods,
    ; which unfortunately also means there will be a "random" delay of
    ; 0-9 frames before button presses are handled.
    ; as a hack to work around this, R2 is simply ignored for the purpose
    ; of deciding whether button repeats should occur. this is mostly fine because
    ; it's not rarely used for repeating actions.
    
    .org 0x80186E28
      j doExtraButtonRepeatLogic
      nop
    
  ;===============================================
  ; MODIFICATION:
  ; make sure R2 is correctly repeated in
  ; situations where it needs to be
  ;===============================================
    
    ; character status select menu
    .org 0x80182400
      jal updateButtonRepeatWithR2
    
  ;===============================================
  ; MODIFICATION:
  ; reset printBufferSendsSinceLastFrameWait in
  ; waitFrame
  ;===============================================
    
;    .org 0x80189460
;      j doExtraWaitFrameBegin
;      nop
    
    
    
    
    
    
    
    
    
;============================================================================
; NEW STUFF
;============================================================================

  .org newStaticMemBlockStart
  .area newStaticMemBlockEnd-.,0xFF
  
    .align 4

    ;===============================================
    ; 
    ;===============================================
    
;    printTextWithoutExtraYOffset:
;      addiu $a1,-fontExtraYOffset
;      j printText
;      nop
    
    printTextWithDialogueYOffset:
      addiu $a1,dialogueExtraYOffset
      j printText
      nop
      
    ;===============================================
    ; 
    ;===============================================
    
    ; - read next input
    ; - if a command, handle as normal
    ; - else (if a char)
    ;   - if buffer not initialized or full, initialize new buffer
    ;   - prep next character data by converting to 8bpp and looking up
    ;     width and kerning.
    ;   - determine how much of next character will fit in current buffer.
    ;     calculation is (currentDispSlotX + nextCharKerning + nextCharW).
    ;   - write "left" part of character and flush buffer to vram
    ;     (or if we want to optimize, flag flush as needed and only send when forced to,
    ;     though this will complicate character-by-character printing).
    ;   - if "right" part of character exists, initialize new buffer,
    ;     write remaining part of character to it, and flush/flag for flush
    
    ; a0 = x
    ; a1 = y
    ; a2 = string pointer
    ; a3 = color modifier
    newPrintText:
      subiu $sp,8
      sw $ra,0($sp)
      sw $s0,4($sp)
;      sw $s1,8($sp)
;      sw $s2,12($sp)
;      sw $s3,16($sp)
;      sw $s4,20($sp)
        ; s0 = x
;        move $s0,$a0
        ; s1 = y
;        move $s1,$a1
        ; s2 = string pointer
;        move $s2,$a2
        ; s2 = color modifier
;        move $s2,$a3
;        sw $a2,printState_srcPtr
        ; s3 = base x
;        move $s3,$a0
        
        ; HACK: the new font is not as tall as the original one.
        ; offset the y-position downward to compensate.
        addiu $a1,fontExtraYOffset
        
        ; save params
        sw $a0,printState_currentTargetX
        sw $a0,printState_baseTargetX
        sw $a1,printState_currentTargetY
        sw $a2,printState_srcPtr
        sw $a3,printState_colorMod
        sw $zero,printState_onLastCharOfLine
        li $v0,1
        sw $v0,printState_currentlyPrinting
        
        ; ensure that the color indices corresponding to the "uninitialized"
        ; color of the print buffer are set to transparent
        ; TODO: if this works, it this means we can remove the print conversion
        ; buffer and save having to copy and convert the content
        jal loadPrintUninitPal
        nop
        
        jal resetPrintState
        nop
        
        ; mark target display slot as needing init
        li $v0,-1
        sw $v0,printState_targetDispSlot
        
        @@printLoop:
          ; fetch next input
          jal fetchNextPrintByte
          nop
          
          ; done if terminator
          beq $v0,$zero,@@done
          nop
          
          ; check if char or op
          bge $v0,charRangeStart,@@isChar
          nop
          @@isOp:
            bne $v0,opcode_linebreak,@@notLinebreak
            nop
              ; reset x
;              move $s0,$s3
              lw $a0,printState_baseTargetX
              la $a1,printState_currentTargetY
              lw $v0,0($a1)
              sw $a0,printState_currentTargetX
              ; add height to Y
              addiu $v0,linebreakHeight
              ; reset print state to start new buffer
              jal resetPrintState
              sw $v0,0($a1)
              
              j @@opDone
              nop
            @@notLinebreak:
            
            bne $v0,opcode_absoluteStringRedirect,@@notAbsRedirect
            nop
              jal doAbsoluteStrRedirect
              nop
              j @@opDone
              nop
            @@notAbsRedirect:
            
            bne $v0,opcode_tabledStringRedirect,@@notTableRedirect
            nop
              jal doTabledStrRedirect
              nop
              j @@opDone
              nop
            @@notTableRedirect:
            
            @@opDone:
            j @@printLoop
            nop
          @@isChar:
            ; update current/last char fields
            li $a0,printState_currentChar
            lbu $v1,0($a0)
            sb $v0,0($a0)
            ; never set lastChar to space to allow for inter-word kerning
;            beq $v1,char_space,@@lastCharWasSpace
;            nop
              sb $v1,printState_lastChar
;            @@lastCharWasSpace:
            
            ; look up target char's bitmap and width data
            jal fetchCharData
            move $a0,$v0
            
            ; init print buffer if needed
            ; TODO: can this go after kerning checks?
            lw $v0,printState_currentBufX
            nop
            blt $v0,printBufW,@@noBufferPreInitNeeded
            nop
              ; adjust buffer x by width of buffer
              addiu $v0,-printBufW
              sw $v0,printState_currentBufX
              
              ; create new buffer
;              move $a0,$s0
              jal startNewPrintBuf
              nop
;              move $a1,$s1
              
              ; target x (for next buffer, if any) += buffer width
;              addiu $s0,printBufSendW
            @@noBufferPreInitNeeded:
            
            ; apply kerning if applicable.
            ; kerning may cause printState_currentBufX to underflow;
            ; in this case, we need to decrement printState_targetDispSlot
            ; and call swapPrintBufs.
            lbu $a1,printState_lastChar
            lbu $a0,printState_currentChar
            ; skip kerning if last char was space or null
            beq $a1,$zero,@@kerningDone
            nop
            beq $a1,char_space,@@kerningDone
            nop
              jal getCharKerning
              nop
              
              ; add kerning to printState_currentBufX
              la $a0,printState_currentBufX
              lw $a1,0($a0)
              nop
              addu $a1,$v0
              ; if result >= 0, no additional action required
              bge $a1,$zero,@@kerningDone
              sw $a1,0($a0)
              @@printBufUnderflow:
                ; we moved far enough back that we now need to return
                ; to the previous print buffer.
                ; first, add printBufW to underflowed result to get new position
                addiu $a1,printBufW
                sw $a1,0($a0)
                
                lw $v1,printState_fixedPosModeOn
                nop
                bne $v1,$zero,@@handleFixedPosUnderflow
                nop
                @@handleStandardUnderflow:
                  ; decrement target display slot
                  la $a0,printState_targetDispSlot
                  lw $a1,0($a0)
                  nop
                  addiu $a1,-1
                  sw $a1,0($a0)
                  j @@underflowHandled
                  nop
                @@handleFixedPosUnderflow:
                  ; buffer dst x -= 16
;                  la $v1,printState_fixedPosCurrentBufferXOffset
                  la $v1,printState_currentTargetX
                  lw $v0,0($v1)
                  nop
                  addiu $v0,-printBufSendW
                  sw $v0,0($v1)
                  
                  li $v0,1
                  sw $v0,printState_fixedPosBufferRewound
                @@underflowHandled:
                
                ; swap previous and current print buffers;
                ; they will be swapped back when the most recent buffer
                ; is returned to
                jal swapPrintBufs
                nop
            @@kerningDone:
            
            ; compute size of left transfer
            ; calculation is (currentDispSlotX + nextCharKerning + nextCharW).
;            lw $a0,printState_currentBufX
            la $v1,printState_currentBufX
            ; a0 = current buffer pos
            lw $a0,0($v1)
            ; a1 = width of next char
            lw $a1,nextCharW
            li $v0,printBufW
            ; a2 = next buffer pos after applying character width
            addu $a2,$a0,$a1
            ; if final pos <= buffer width, char fits entirely within
            ; current buffer, and only one transfer is needed
            ; update current buffer pos
            ble $a2,$v0,@@noLeftTransferWidthLimit
            sw $a2,0($v1)
              ; limit width of left transfer to available area
              ; (printBufW - initialBufX)
              subu $a1,$v0,$a0
            @@noLeftTransferWidthLimit:
            
            ; a2 = width
            move $a2,$a1
            ; a1 = dst x-offset from buffer start (= current buffer pos)
            move $a1,$a0
            ; a0 = src
            li $a0,nextCharBuf
            jal composeToPrintBuf
            nop
            
/*            ; HACK: if buffer not filled,
            ; and not in fixed-pos mode,
            ; and not on last character,
            ; do not flush buffer
;            lw $a0,printState_fixedPosModeOn
;            lw $a0,printBufferSendsSinceLastFrameWait
            lw $a1,printState_onLastCharOfLine
            lw $v0,printCbcFlag
            lw $v1,printState_currentBufX
;            bne $a0,$zero,@@noFlushSkip
;            nop
            bne $a1,$zero,@@noFlushSkip
            nop
            bne $v0,$zero,@@noFlushSkip
            nop
;            bge $a0,maxPrintBufferSendsPerFrame-2,@@noFlushSkip
;            nop
            bge $v1,printBufW,@@noFlushSkip
            nop
              j @@noRightTransfer
              nop
            @@noFlushSkip:*/
            
            ; send updated buffer to vram
            jal flushPrintBuf
            nop
            
            ; check if right transfer needed
            la $v1,printState_currentBufX
            ; a0 = new buffer pos
            lw $a0,0($v1)
            li $v0,printBufW
            ble $a0,$v0,@@noRightTransfer
            nop
              
              ; adjust buffer x by width of buffer
              addiu $a0,-printBufW
              sw $a0,0($v1)
              
              ; create new buffer
              jal startNewPrintBuf
              nop
            
              ; a2 = width
              lw $a2,printState_currentBufX
              ; a0 = src = nextCharBuf + (nextCharW - thisTransferWidth)
              lw $v1,nextCharW
              li $v0,nextCharBuf
              subu $a0,$v1,$a2
              addu $a0,$v0
              ; a1 = dst x-offset from buffer start
              jal composeToPrintBuf
              li $a1,0
              
/*              ; send updated buffer to vram
              ; HACK: do not flush if not in cbc mode and not on last character
;              lw $a0,printState_fixedPosModeOn
;              lw $a0,printBufferSendsSinceLastFrameWait
              lw $a1,printState_onLastCharOfLine
              lw $v0,printCbcFlag
;              bne $a0,$zero,@@noSecondFlushSkip
;              nop
              bne $a1,$zero,@@noSecondFlushSkip
              nop
;              bge $a0,maxPrintBufferSendsPerFrame-1,@@noSecondFlushSkip
;              nop
              bne $v0,$zero,@@noSecondFlushSkip
              nop
                j @@noRightTransfer
                nop
              @@noSecondFlushSkip:*/
              jal flushPrintBuf
              nop
            @@noRightTransfer:
            
            ; if number of buffer flushes >= max per frame, force a frame wait
/*            lw $v1,printBufferSendsSinceLastFrameWait
            nop
            blt $v1,maxPrintBufferSendsPerFrame,@@printMaxNotReached
            nop
              ; wait for pending transfers
;              jal waitForGpuQueue
;              addu $a0, $zero, $zero
              
              ; printBufferSendsSinceLastFrameWait will be reset by this call
              jal waitFrame
              nop
            @@printMaxNotReached:*/
            
            lw $v0,printState_fixedPosModeOn
            nop
            bne $v0,$zero,@@charWaitDone
            nop
              ; do normal character-by-character print wait (if applicable)
  ;            jal doPostCharPrintWait
  ;            nop
              
              jal checkIfFfButtonPressed
              nop
              
              beq $v0,$zero,@@noWaitSkip
              la $v0,printCbcFlag
                ; if ff on, clear cbc print flag to instantly print
                ; remaining text
                sw $zero,0($v0)
              @@noWaitSkip:
              ; make up work
              jal doPostCharPrintWait
              nop
            @@charWaitDone:
            
            j @@printLoop
            nop
      
      @@done:
      
      sw $zero,printState_currentlyPrinting
      
      lw $ra,0($sp)
      lw $s0,4($sp)
;      lw $s1,8($sp)
;      lw $s2,12($sp)
;      lw $s3,16($sp)
;      lw $s4,20($sp)
      jr $ra
      addiu $sp,8
    
    newPrintFixedPosString_stackSize equ 24
;    maxFixedPosStringWidth equ 84
    maxFixedPosStringWidth equ 88
    
    ; a0 = display slot num
    ; a1 = x-offset from base pos
    ; a2 = y-offset from base pos
    ; a3 = string pointer
    ; sp+0x10 = color modifier
    newPrintFixedPosString:
      subiu $sp,newPrintFixedPosString_stackSize
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
      sw $s3,16($sp)
      sw $s4,20($sp)
        
        ; check if string is empty (probably not needed for our new system,
        ; but original game does this)
        lbu $v0,0($a3)
        nop
        beq $v0,$zero,@@done
        nop
        
        ; HACK: don't apply fontExtraYOffset for this
        ; (it's automatically applied in newPrintText, so we make up
        ; the difference here)
        addiu $a2,-fontExtraYOffset
        
        ; save params
        move $s0,$a0
        move $s1,$a1
        move $s2,$a2
        move $s3,$a3
        ; save current font
        lw $s4,currentFontIndex
;        lw $s4,0x10+newPrintFixedPosString_stackSize($sp)

        ; determine whether string will fit within allowed bounds,
        ; and use narrow font if it will not
        jal getStringWidth
        move $a0,$s3
        
        ble $v0,maxFixedPosStringWidth,@@willFit
        li $v0,fontIndex_narrow
          sw $v0,currentFontIndex
        @@willFit:
        
;        move $s0,$a0
        li $v0,1
        sw $v0,printState_fixedPosModeOn
;        li $v0,-printBufSendW
;        sw $v0,printState_fixedPosCurrentBufferXOffset
;        sw $v0,printState_currentTargetX
        sw $zero,printState_fixedPosBufferRewound
        sw $s0,printState_fixedPosDispSlot
          
;          move $a0,$a1
          ; offset initial x-pos by negative width of buffer;
          ; due to the way buffer initialization works, it will be
          ; offset back to the initial value before printing occurs
          addiu $a0,$s1,-printBufSendW
          move $a1,$s2
          move $a2,$s3
          lw $a3,0x10+newPrintFixedPosString_stackSize($sp)
          
          jal newPrintText
          nop
        
        sw $zero,printState_fixedPosModeOn
        sw $s4,currentFontIndex
        
      @@done:
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
      lw $s3,16($sp)
      lw $s4,20($sp)
      jr $ra
      addiu $sp,newPrintFixedPosString_stackSize
    
    doAbsoluteStrRedirect:
      subiu $sp,8
      sw $ra,0($sp)
      sw $s0,4($sp)
      
        ; a0 = fetch 16-bit id
        jal fetchNextPrintByteRaw
        nop
        jal fetchNextPrintByteRaw
        move $s0,$v0
        sll $v0,8
        
        ; get pointer to corresponding entry in global string table
        jal getGlobalStringTableEntry
        or $a0,$s0,$v0
        
        ; a0 = pointer from table
        ; (only low 3 bytes of pointer are stored)
        lbu $a1,2($v0)
        lbu $a2,1($v0)
        sll $a1,16
        lbu $a3,0($v0)
        sll $a2,8
        li $a0,0x80000000
        or $a0,$a1
        or $a0,$a2
        or $a0,$a3
        
        ; set script pos to pointer
        sw $a0,printState_srcPtr
      lw $ra,0($sp)
      lw $s0,4($sp)
      jr $ra
      addiu $sp,8
    
    doTabledStrRedirect:
      subiu $sp,8
      sw $ra,0($sp)
      sw $s0,4($sp)
        ; a0 = fetch 16-bit id
        jal fetchNextPrintByteRaw
        nop
        jal fetchNextPrintByteRaw
        move $s0,$v0
        sll $v0,8
        
        ; get pointer to corresponding entry in global string table
        jal getGlobalStringTableEntry
        or $a0,$s0,$v0
        
        ; set script pos to pointer
        sw $v0,printState_srcPtr
      lw $ra,0($sp)
      lw $s0,4($sp)
      jr $ra
      addiu $sp,8
    
    resetPrintState:
      ; set current buffer x to max width so that next transfer
      ; will result in new buffer
      li $v0,printBufW
      sw $v0,printState_currentBufX
      ; set last char to null
      sb $zero,printState_currentChar
      sb $zero,printState_lastChar
      ; reset dte state (shouldn't be necessary)
      sb $zero,printState_dteState
      jr $ra
      nop
    
    clearPrintBuf:
      la $a0,printBuf
      li $a1,(printBufSize/4)
      li $v0,(printBufUninitColor|(printBufUninitColor<<8)|(printBufUninitColor<<16)|(printBufUninitColor<<24))
      @@loop:
        sw $v0,0($a0)
        subiu $a1,1
        bne $a1,$zero,@@loop
        addiu $a0,4
      jr $ra
      nop
    
    swapPrintBufs:
      la $a0,printBuf
      li $a1,(printBufSize/4)
      la $a2,lastPrintBuf
      @@loop:
        lw $v0,0($a0)
        lw $v1,0($a2)
        sw $v0,0($a2)
        sw $v1,0($a0)
        subiu $a1,1
        addiu $a0,4
        bne $a1,$zero,@@loop
        addiu $a2,4
      jr $ra
      nop
    
    startNewPrintBuf:
      subiu $sp,4
      sw $ra,0($sp)
;      sw $s0,4($sp)
;      sw $s1,8($sp)
        ; s0 = target x
;        move $s0,$a0
        ; s1 = target y
;        move $s1,$a1
        
        lw $v0,printState_fixedPosModeOn
        nop
        bne $v0,$zero,@@doFixedPosRewindCheck
        nop
          ; check if rewind occurred (next slot != previous slot + 1).
          ; if so, no slot initialization is needed.
          ; if target slot is < 0, field is uninitialized and new slot is needed.
          lw $a1,printState_targetDispSlot
          lh $a0,currentDispSlot_gp($gp)
          blt $a1,$zero,@@needNewDispSlot
          addiu $v0,$a1,1
          bne $v0,$a0,@@rewound
          nop
          @@needNewDispSlot:
            ; claim display slot
            ; increment next slot num
            ; TODO: is it okay to increment this now?
            ; do old functions care?
            addiu $v0,$a0,1
            sh $v0,currentDispSlot_gp($gp)
            ; save current buffer's slot num
            sw $a0,printState_targetDispSlot
            nop
            
            ; send (empty) graphic to vram to initialize slot
            ; TODO: wastes time
    ;        la $a1,printBuf
    ;        li $a2,printBufSendW
    ;        jal sendGrpToFreeVramArea
    ;        li $a3,printBufSendH
            ; set up slot, but don't send yet
            ; TODO: we assume this operation succeeds, and it should,
            ; but we should technically be checking if the returned value
            ; is negative to indicate an error occurred (not enough space?)
;            li $a1,printBufSendW
            ; a2 = send height for current font
            lw $v0,currentFontIndex
            la $v1,printBufSendHTable
            sll $v0,2
            addu $v0,$v1
            lw $a2,0($v0)
            jal findVramFreeSpaceForData
            li $a1,printBufSendW
            
            ; HACK: if printing subtitles and sSub_VramOverrideOn is on,
            ; we don't use the vram position the game gave us
            ; and instead go to a fixed position specified by the override.
            ; the allocated position is saved so we can still tell the game
            ; to free it later.
            lw $v0,currentFontIndex
            lw $v1,sSub_VramOverrideOn
            bne $v0,fontIndex_subtitle,@@noSubtitleVramOverride
            lw $a0,sSub_VramOverrideX
              beq $v1,$zero,@@noSubtitleVramOverride
              lw $a1,sSub_VramOverrideY
                lw $a2,printState_targetDispSlot
                la $a3,dispSlotVramDstArray
                sll $t0,$a2,3
                ; a3 = pointer to vram position array entry
                addu $a3,$t0
                ; v0/v1 = original x/y
                lhu $v0,0($a3)
                lhu $v1,2($a3)
                ; save new x/y
                sh $a0,0($a3)
                sh $a1,2($a3)
                
                ; save original x/y
                ; t0 = target index in save array
                lw $t0,sSub_currentStringStartSlot
                la $t1,sSub_VramOverrideOldXyArray
                subu $t0,$a2,$t0
                ; t0 = pointer to target pos
                sll $t0,2
                addu $t0,$t1
                ; save x/y
                sh $v0,0($t0)
                sh $v1,2($t0)
                
                ; advance to next x/y pos within texpage
                addiu $v0,$a0,(printBufSendW/2)
                ; if end of texpage reached, wrap
                andi $v1,$v0,0x7F
                bne $v1,$zero,@@noRowChange
                li $t0,0xFFFFFF80
                  and $v0,$a0,$t0
                  addiu $a1,printBufSendH_subtitle
                @@noRowChange:
                sw $v0,sSub_VramOverrideX
                sw $a1,sSub_VramOverrideY
            @@noSubtitleVramOverride:
            
            subiu $sp,0x1C
              ; a0/a1 = slot num
              lw $a0,printState_targetDispSlot
              ; a2 = x
  ;            move $a2,$s0
              lw $a2,printState_currentTargetX
              move $a1,$a0
              ; a3 = y
  ;            move $a3,$s1
              lw $a3,printState_currentTargetY
            
              ; sp+0x10 = 1
              li $v0,1
              sw $v0,0x10($sp)
              ; sp+0x14/0x18 = 0
              sw $zero,0x14($sp)
              jal setUpDispSlotParams
              sw $zero,0x18($sp)
            addiu $sp,0x1C
            
            ; set up 6b render struct
            ; a0 = dst
            lw $a0,printState_targetDispSlot
            ; a1 = priority
            ; normal priority is 0x50, but if printing subtitles,
            ; set priority to max so they'll go in front of everything
            ; (particularly, screen fades have a priority of 0x3C
            ; and we need to appear in front of those)
            lw $v0,currentFontIndex
            li $a1,0x50
            bne $v0,fontIndex_subtitle,@@noSubtitlePriority
            nop
              li $a1,0x01
            @@noSubtitlePriority:
            jal 0x8013E9DC
            nop
            
            ; copy old print buf for use if rewind occurs
            ; dst
            la $a0,lastPrintBuf
            ; src
            la $a1,printBuf
            ; size
            jal memcpy
            li $a2,printBufSize
            
            ; clear buf
  ;          sw $zero,printState_currentBufX
            jal clearPrintBuf
            nop
            
            ; increment target X for buffer
            la $a0,printState_currentTargetX
            lw $v0,0($a0)
            nop
            addiu $v0,printBufSendW
            sw $v0,0($a0)
            
            j @@rewindCheckDone
            nop
          @@rewound:
            ; set new slot num to old + 1
            sw $v0,printState_targetDispSlot
            ; swap print buffers, restoring old one to active status
            jal swapPrintBufs
            nop
            j @@rewindCheckDone
            nop
        @@doFixedPosRewindCheck:
          la $v1,printState_fixedPosBufferRewound
          lw $v0,0($v1)
          nop
          bne $v0,$zero,@@fixedPosRewound
          sw $zero,0($v1)
            
            ; copy old print buf for use if rewind occurs
            ; dst
            la $a0,lastPrintBuf
            ; src
            la $a1,printBuf
            ; size
            jal memcpy
            li $a2,printBufSize
            
            ; clear buf
  ;          sw $zero,printState_currentBufX
            jal clearPrintBuf
            nop
            
            ; increment target X for buffer
            la $a0,printState_currentTargetX
            lw $v0,0($a0)
            nop
            addiu $v0,printBufSendW
            sw $v0,0($a0)
            
            j @@rewindCheckDone
            nop
          @@fixedPosRewound:
            ; swap print buffers, restoring old one to active status
            jal swapPrintBufs
            nop
            
            ; increment target X for buffer
            la $a0,printState_currentTargetX
            lw $v0,0($a0)
            nop
            addiu $v0,printBufSendW
            sw $v0,0($a0)
            
        @@rewindCheckDone:
        
      lw $ra,0($sp)
;      lw $s0,4($sp)
;      lw $s1,8($sp)
      addiu $sp,4
      jr $ra
      nop
    
    ; a0 = src (row separation assumed to be charBufW bytes)
    ; a1 = dst x-offset in buffer
    ; a2 = width
    composeToPrintBuf:
      lw $t4,printState_colorMod
      ; t3 = start of decoding array, offset by color mod * 16
      li $t3,charDecodingTableArray
      sll $v0,$t4,4
      addu $t3,$v0
      
      ; ...unless color mod is special marker for subtitle decoding,
      ; in which case use that instead
      bne $t4,charDecodingTable_subtitleCode,@@notSubtitleDec
      nop
        li $t3,subtitleCharDecodingTable
      @@notSubtitleDec:
      
      ; a1 = dst
      li $v0,printBuf
      addu $a1,$v0
      ; a3 = height counter
      li $a3,charBufH
      @@copyYLoop:
        ; v1 = width counter
        move $v1,$a2
        ; v0 = src
        move $v0,$a0
        ; t0 = dst
        move $t0,$a1
        @@copyXLoop:
          ; t1 = next from src
          lbu $t1,0($v0)
          ; t2 = next from dst
          lbu $t2,0($t0)
          
          ; t1 = decoded src
          addu $t1,$t3
          lbu $t1,0($t1)
          
          ; ++src
          addiu $v0,1
          ; if next from dst != 0, do not overwrite
;          bne $t2,$zero,@@noSend
;          nop
          
          beq $t4,charDecodingTable_subtitleCode,@@subtitleCompose
          nop
          bne $t4,0xF,@@normalCompose
          nop
          @@invertedCompose:
            ; do not overwrite an initialized lighter shade with a darker one
            beq $t2,printBufUninitColor,@@send
            nop
            bge $t2,$t1,@@send
            nop
;            @@invertedSend:
;              sb $t1,0($t0)
            j @@noSend
            nop
          @@subtitleCompose:
            ; always overwrite unless src = zero or uninit
            beq $t1,printBufUninitColor,@@noSend
            nop
            beq $t1,$zero,@@noSend
            nop
            j @@send
            nop
          @@normalCompose:
            ; do not overwrite an initialized darker shade with a lighter one
            ; (this is obviously not how actual blending would work,
            ; but i suspect it will look better than doing the actual
            ; calculations would)
            beq $t2,printBufUninitColor,@@send
            nop
            bge $t2,$t1,@@noSend
            nop
            @@send:
              sb $t1,0($t0)
          @@noSend:
          ; --counter
          addiu $v1,-1
          ; ++dst
          bne $v1,$zero,@@copyXLoop
          addiu $t0,1
        
        ; move to next row in src
        addiu $a0,charBufW
        ; --counter
        addiu $a3,-1
        ; move to next row in dst
        bne $a3,$zero,@@copyYLoop
        addiu $a1,printBufW
      
      jr $ra
      nop
    
    subtitleCharDecodingTable:
      ; index 0 = transparent
;      .db subtitleTargetSubPaletteBase+0x00
      .db 0x00
      .db subtitleTargetSubPaletteBase+0x01
      .db subtitleTargetSubPaletteBase+0x02
      .db subtitleTargetSubPaletteBase+0x03
      .db subtitleTargetSubPaletteBase+0x04
      .db subtitleTargetSubPaletteBase+0x05
      .db subtitleTargetSubPaletteBase+0x06
      .db subtitleTargetSubPaletteBase+0x07
      .db subtitleTargetSubPaletteBase+0x08
      .db subtitleTargetSubPaletteBase+0x09
      .db subtitleTargetSubPaletteBase+0x0A
      .db subtitleTargetSubPaletteBase+0x0B
      .db subtitleTargetSubPaletteBase+0x0C
      .db subtitleTargetSubPaletteBase+0x0D
      .db subtitleTargetSubPaletteBase+0x0E
      ; final index is reserved for uninitialized buffer transparency,
      ; so remap it to the next lowest index
      ; (matching behavior of normal font, which does this for other reasons)
;      .db subtitleTargetSubPaletteBase+0x0F
      .db subtitleTargetSubPaletteBase+0x0E
    .align 4
    
    ; set convertedPrintBuf to contents of printBuf,
    ; but with uninitialized pixels set to zero
    convertPrintBuf:
      la $a0,printBuf
      li $a1,printBufSize
      la $a2,convertedPrintBuf
      @@loop:
        lbu $v0,0($a0)
        subiu $a1,1
        ; if uninitialized, set to zero
;        bne $v0,printBufUninitColor,@@initialized
;        nop
;          move $v0,$zero
        @@initialized:
        sb $v0,0($a2)
        addiu $a0,1
        bne $a1,$zero,@@loop
        addiu $a2,1
      jr $ra
      nop
    
    ; send print buffer to VRAM
    flushPrintBuf:
      subiu $sp,4
      sw $ra,0($sp)
        jal convertPrintBuf
        nop
        
        lw $v0,printState_fixedPosModeOn
        nop
        bne $v0,$zero,@@doFixedPosFlush
        nop
          lw $a0,printState_targetDispSlot
  ;        la $a1,printBuf
          la $a1,convertedPrintBuf
          jal sendToVramViaDispSlot
          nop
          
          j @@flushDone
          nop
        @@doFixedPosFlush:
          ; v0 = pointer to positioning info from target slot
          lw $v0,printState_fixedPosDispSlot
          li $v1,dispSlotVramDstArray
          sll $v0,3
          addu $v0,$v1
          
          ; set up and call StoreImage with converted buffer contents
          subiu $sp,8
            ; x
            lhu $a0,0($v0)
            lw $a1,printState_currentTargetX
            nop
            ; divide by 2 to get halfword position
            srl $a1,1
            addu $a0,$a1
            sh $a0,0($sp)
            
            ; y 
            lhu $a0,2($v0)
            lw $a1,printState_currentTargetY
            nop
            addu $a0,$a1
            sh $a0,2($sp)
            
            ; h
            ; HACK: this is only used with the standard font,
            ; so we can just use a hardcoded value
            li $a0,printBufSendH_std
            sh $a0,6($sp)
            
            ; w (halfword)
;            li $a0,printBufSendW/2
            lw $a0,printState_currentBufX
            nop
            ; if buffer not entirely full (or more),
            ; only send applicable portion
            blt $a0,printBufSendW,@@notFullBufferSend
            addiu $a0,1
              ; skip format conversion and just use the existing buffer,
              ; as it's already the correct width
              la $a1,convertedPrintBuf
              li $a0,printBufSendW/2
              j @@fixedPosSendReady
              sh $a0,4($sp)
            @@notFullBufferSend:
            srl $a0,1
            
            ; ensure width is nonzero
            ; (should always be the case)
            beq $a0,$zero,@@fixedPosSendDone
            sh $a0,4($sp)
            
            ; convert buffer to output width
            la $a2,convertedPrintBuf
            la $a3,fixedPosOutPrintBuf
            ; t0 = y counter
            li $t0,printBufH
            @@fixedPosConvertLoopY:
              ; t1 = x counter
              move $t1,$a0
              ; t2 = src
              move $t2,$a2
              @@fixedPosConvertLoopX:
                ; copy halfword from src to dst
                lhu $v0,0($t2)
                ; src += 2
                addiu $t2,2
                sh $v0,0($a3)
                
                addiu $t1,-1
                ; dst += 2
                bne $t1,$zero,@@fixedPosConvertLoopX
                addiu $a3,2
              
              addiu $t0,-1
              ; src += row size
              bne $t0,$zero,@@fixedPosConvertLoopY
              addiu $a2,printBufW
            
            ; send to vram
            ; TODO: any need to do queue wait afterwards?
            ; shouldn't be, since original game doesn't
            la $a1,fixedPosOutPrintBuf
            
            @@fixedPosSendReady:
            jal StoreImage
            addiu $a0,$sp,0
            
            ; ???
;            jal waitForGpuQueue
;            addu $a0, $zero, $zero
            
          @@fixedPosSendDone:
          addiu $sp,8
        @@flushDone:
      
      ; increment count of flushes since last waitFrame
/*      la $a0,printBufferSendsSinceLastFrameWait
      lw $v1,0($a0)
      nop
      addiu $v1,1
      sw $v1,0($a0)*/
      
      lw $ra,0($sp)
;      lw $s0,4($sp)
      addiu $sp,4
      jr $ra
      nop
    
    ; returns next byte in src stream (parsing dte sequences)
    fetchNextPrintByte:
      lw $a0,printState_srcPtr
      nop
      lbu $v0,0($a0)
      nop
      blt $v0,dteRangeStart,@@notDte
      nop
      
      @@dte:
        subiu $v0,dteRangeStart
        sll $v0,1
        lbu $a1,printState_dteState
        nop
        beq $a1,$zero,@@notSecondDteChar
        ; set dte flag
        li $a1,1
          ; clear dte flag
          move $a1,$zero
          ; target second char of dictionary pair
          addiu $v0,1
          ; ++srcPtr (to advance past dte code)
          addiu $a0,1
        @@notSecondDteChar:
        sb $a1,printState_dteState
        la $a1,dteTable
        addu $v0,$a1
        j @@done
        lbu $v0,0($v0)
        
      @@notDte:
        ; ++srcPtr
        addiu $a0,1
      
      @@done:
      ; do end-of-line checks
      sw $zero,printState_onLastCharOfLine
      ; do not do end-of-line checks if a redirect op
      beq $v0,opcode_absoluteStringRedirect,@@nextNotLineEnd
      nop
      beq $v0,opcode_tabledStringRedirect,@@nextNotLineEnd
      nop
      ; peek at next byte and flag that we are on last character of line
      ; if terminator or linebreak
      lbu $a1,0($a0)
      nop
      beq $a1,$zero,@@nextIsLineEnd
      nop
      bne $a1,opcode_linebreak,@@nextNotLineEnd
      nop
      @@nextIsLineEnd:
        li $a1,1
        sw $a1,printState_onLastCharOfLine
      @@nextNotLineEnd:
      sw $a0,printState_srcPtr
      jr $ra
      nop
    
    ; fetch next print byte (not parsing dte sequences)
    fetchNextPrintByteRaw:
      lw $a0,printState_srcPtr
      nop
      lbu $v0,0($a0)
      ; ++srcPtr
      addiu $a0,1
      sw $a0,printState_srcPtr
      jr $ra
      nop
    
    loadPrintUninitPal:
      subiu $sp,4
      sw $ra,0($sp)
        la $a0,printUninitImgStoreSpec1
;        la.u $a1,printUninitImg
        la.u $a1,printSubtitleAndUninitImg
        jal StoreImage
;        la.l $a1,printUninitImg
        la.l $a1,printSubtitleAndUninitImg
        
        la $a0,printUninitImgStoreSpec2
        la.u $a1,printUninitImg
        jal StoreImage
        la.l $a1,printUninitImg
      lw $ra,0($sp)
      addiu $sp,4
      jr $ra
      nop
      
      .align 4
      printUninitImg:
;        .dh 0xDF7A
        .dh 0x0000
        .dh 0x0000
      
      .align 4
      printSubtitleAndUninitImg:
        .incbin "out/rsrc_raw/pal/subtitle_clipped.pal"
        ; uninit transparency
        .dh 0x0000
      
      .align 4
      printUninitImgStoreSpec1:
        ; x
;        .dh 0x00FF
;        .dh printBufUninitColor
        ; HACK, relies on printBufUninitColor being subtitleTargetSubPaletteBase+15
        .dh subtitleTargetSubPaletteBase
        ; y
        .dh 0x01E0
        ; w
;        .dh 1
        .dh 16
        ; h
        .dh 1
        
      .align 4
      printUninitImgStoreSpec2:
        ; x
;        .dh 0x00FF
        .dh printBufUninitColor
        ; y
        .dh 0x01E8
        ; w
        .dh 1
        ; h
        .dh 2
    
    ; array
    .align 4
      printBuf:
        .fill printBufSize,printBufUninitColor
      lastPrintBuf:
        .fill printBufSize,printBufUninitColor
      ; used for flushing buffer -- uninitialized pixels are set to 0
      convertedPrintBuf:
        .fill printBufSize,0x00
      ; fixed buffer is copied here with width reduced as needed for actual
      ; size of area begin targeted.
      ; this is needed to avoid issues where the 16px buffer granularity
      ; would result in wasted space on the right edge of the target vram
      ; area (e.g. the game has an 84px space, but sending the full 16px when we're
      ; already at 80px+ would overflow past the right edge)
      fixedPosOutPrintBuf:
        .fill printBufSize,0x00
    
    ; word
    .align 4
      printState_srcPtr:
        .dw 0
      printState_currentBufX:
        .dw 0
      printState_targetDispSlot:
        .dw 0
      printState_currentTargetX:
        .dw 0
      printState_currentTargetY:
        .dw 0
      printState_baseTargetX:
        .dw 0
      printState_colorMod:
        .dw 0
      printState_onLastCharOfLine:
        .dw 0
      printState_currentlyPrinting:
        .dw 0
      
      printState_fixedPosModeOn:
        .dw 0
;      printState_fixedPosCurrentBufferXOffset:
;        .dw 0
      printState_fixedPosDispSlot:
        .dw 0
      printState_fixedPosBufferRewound:
        .dw 0
    
    ; byte
    .align 4
    printState_charStateWord:
      printState_currentChar:
        .db 0
      printState_lastChar:
        .db 0
      printState_dteState:
        .db 0
      printState_charWordFiller:
        .db 0
    
    .align 4
      dteTable:
        .incbin "out/script/script_dictionary.bin"
  
  ; a0 = index
  ; returns v0 = pointer to content
  getGlobalStringTableEntry:
    addiu $a0,-skippedInitialGlobalStringTableEntryCount
    
    sll $a0,1
    la $a1,globalStringTable
    addu $a0,$a1
    lhu $a0,0($a0)
    nop
    jr $ra
    addu $v0,$a0,$a1
    
    .align 4
    globalStringTable:
      .incbin "out/script/globalstringtable.bin"
    
    .align 4
  
  ; a0 = char codepoint
  fetchCharData:
    subiu $sp,20
    sw $ra,0($sp)
    sw $s0,4($sp)
    sw $s1,8($sp)
    sw $s2,12($sp)
    sw $s3,16($sp)
;    fontCharSize
;    subiu $a0,charRangeStart
      ; s0 = codepoint
      move $s0,$a0
      ; s1 = target font index * 4
      lw $s1,currentFontIndex
      nop
      sll $s1,2
      
      ; convert codepoint to raw font index
      subiu $s0,charRangeStart
      
;      li $v0,fontCharSize
;      mult $s0,$v0
;      mflo $a0
      
      ; look up glyph w/h of font (s2/s3)
      la $a0,fontGlyhphWTable
      addu $a0,$s1
      lw $s2,0($a0)
      la $a0,fontGlyhphHTable
      addu $a0,$s1
      lw $s3,0($a0)
      nop
      
      ; multiply glyph w/h to get font char size
      mult $s2,$s3
      mflo $a2
      ; divide by 2 to get input glyph byte size
      srl $a2,1
      ; multiply by codepoint to get offset
      mult $s0,$a2
      ; a3 = offset
      mflo $a3
      
      ; a0 = pointer to current font's char data
      li $a1,fontTable
      addu $a1,$s1
      lw $a1,0($a1)
      nop
      addu $a0,$a3,$a1
      
      ; HACK: subtitle font must be stored as 8bpp instead of 4bpp.
      ; (it's used for rendering subtitles over the fmvs, with only
      ; a very small amount of time available for the whole operation,
      ; and every bit of speed matters.)
      ; so, if using that font, we instead do an odd downconversion
      ; from 8bpp to 8bpp-with-raw-4bpp-indices to match the format
      ; of the other fonts.
      bne $s1,(fontIndex_subtitle*4),@@notSubtitleFont
      nop
        ; a0 = src (8bpp)
        ; (we previously assumed the needed offset was half the actual amount,
        ; and make it up here)
        addu $a0,$a3
        ; a1 = dst (8bpp)
        la $a1,nextCharBuf
        ; counter
        ; (HACK: we know both the subtitle characters and the char buffer are 16x16,
        ; so we can just copy byte-by-byte)
        li $a2,(fontGlyphW_subtitle*fontGlyphH_subtitle)
        @@subtitleCharCopyLoop:
          lbu $v0,0($a0)
          addiu $a0,1
          ; reduce to high nybble in low position only
          srl $v0,4
          sb $v0,0($a1)
          
          addiu $a2,-1
          bne $a2,$zero,@@subtitleCharCopyLoop
          addiu $a1,1
        j @@charCopyDone
        nop
      @@notSubtitleFont:
      
      ; copy character data to buffer
      ; a0 = src (4bpp)
      ; a1 = dst (8bpp)
      la $a1,nextCharBuf
      ; a2 = counter for output rows
;      li $a2,fontCharH
      move $a2,$s3
      ; t0 = output end-of-line advance width (charBufW-fontCharW)
      li $t0,charBufW
      subu $t0,$s2
      @@charCopyYLoop:
        ; a3 = counter for 4bpp input pixel columns
;        li $a3,(fontCharW/2)
        move $a3,$s2
        srl $a3,1
        @@charCopyXLoop:
          lbu $v0,0($a0)
          addiu $a0,1
          ; v1 = second pixel
          andi $v1,$v0,0xF
          ; v0 = first pixel
          srl $v0,4
          
          sb $v0,0($a1)
          sb $v1,1($a1)
          
          ; --counter
          subiu $a3,1
          bne $a3,$zero,@@charCopyXLoop
          addiu $a1,2
        
        ; --counter
        subiu $a2,1
        ; skip any unused columns on right side of output buffer
        bne $a2,$zero,@@charCopyYLoop
;        addiu $a1,(charBufW-fontCharW)
        addu $a1,$t0
      
      @@charCopyDone:
      
      ; look up width
      li $a1,fontWidthTable
      addu $a1,$s1
      lw $a1,0($a1)
      nop
      addu $a1,$s0
      lbu $v0,0($a1)
      nop
      sw $v0,nextCharW
    
    lw $ra,0($sp)
    lw $s0,4($sp)
    lw $s1,8($sp)
    lw $s2,12($sp)
    lw $s3,16($sp)
    jr $ra
    addiu $sp,20
    
    .align 4
      nextCharBuf:
        .fill charBufSize,0x00
    .align 4
      nextCharW:
        .dw 0
    
    .align 4
      ; index of currently selected font
      currentFontIndex:
        .dw fontIndex_std
      
    .align 4
      fontTable:
        .dw font_std
        .dw font_narrow
        .dw font_subtitle
      
      fontWidthTable:
        .dw fontWidth_std
        .dw fontWidth_narrow
        .dw fontWidth_subtitle
      
      fontKerningIndexTable:
        .dw fontKerningIndex_std
        .dw fontKerningIndex_narrow
        .dw fontKerningIndex_subtitle
      
      fontKerningDataTable:
        .dw fontKerningData_std
        .dw fontKerningData_narrow
        .dw fontKerningData_subtitle
      
      fontDefaultKerningTable:
        .dw fontDefaultKerning_std
        .dw fontDefaultKerning_narrow
        .dw fontDefaultKerning_subtitle
      
      fontGlyhphWTable:
        .dw fontGlyphW_std
        .dw fontGlyphW_narrow
        .dw fontGlyphW_subtitle
      
      fontGlyhphHTable:
        .dw fontGlyphH_std
        .dw fontGlyphH_narrow
        .dw fontGlyphH_subtitle
      
      printBufSendHTable:
        .dw printBufSendH_std
        .dw printBufSendH_narrow
        .dw printBufSendH_subtitle
    
    .align 4
    font_std:
      .incbin "out/font/default/font.bin"
    .align 4
    font_narrow:
      .incbin "out/font/narrow/font.bin"
    .align 4
    font_subtitle:
;      .incbin "out/font/subtitle/font.bin"
      .incbin "out/font/subtitle_8bpp/font.bin"
    
    .align 4
    fontWidth_std:
      .incbin "out/font/default/width.bin"
    .align 4
    fontWidth_narrow:
      .incbin "out/font/narrow/width.bin"
    .align 4
    fontWidth_subtitle:
      .incbin "out/font/subtitle/width.bin"
    
    .align 4
    fontKerningIndex_std:
      .incbin "out/font/default/kerning_index.bin"
    .align 4
    fontKerningIndex_narrow:
      .incbin "out/font/narrow/kerning_index.bin"
    .align 4
    fontKerningIndex_subtitle:
      .incbin "out/font/subtitle/kerning_index.bin"
    
    .align 4
    fontKerningData_std:
      .incbin "out/font/default/kerning_data.bin"
    .align 4
    fontKerningData_narrow:
      .incbin "out/font/narrow/kerning_data.bin"
    .align 4
    fontKerningData_subtitle:
      .incbin "out/font/subtitle/kerning_data.bin"
    
    .align 4

    ;==================================
    ; $a0 = current char
    ; $a1 = previous char
    ; 
    ; returns kerning offset to correctly
    ; position current char relative to
    ; previous
    ;==================================
    
    getCharKerning:
  ;        jr $ra
  ;        li $v0,0
  ;        subiu $sp,8
  ;        sw $ra,0($sp)
  ;        sw $s0,4($sp)
        
        ;=====
        ; convert inputs from codepoints to raw indices
        ;=====
        
        subiu $a0,charRangeStart
        subiu $a1,charRangeStart
        
        ;=====
        ; get kerning index/data pointers for target font
        ;=====
        
        ; a2 = index pointer
        lw $t0,currentFontIndex
        la $a2,fontKerningIndexTable
        sll $t0,2
        addu $a2,$t0
        lw $a2,0($a2)
        
        ; a3 = data pointer
        la $a3,fontKerningDataTable
        addu $a3,$t0
        lw $a3,0($a3)
        
        ;=====
        ; first, check if either current or previous character
        ; has a no-kerning marker (0xFFFF) in the index.
        ; if so, always use null kerning.
        ;=====
        
        ; prev char
        sll $v0,$a1,1
        addu $v1,$a2,$v0
        lhu $v0,0($v1)
        ; a3 = fontKerningData
;        la.u $a3,fontKerningData
        nop
        beq $v0,kerningIndex_noKerningMarker,@@done
        li $v0,0
        
        ; next char
        sll $v0,$a0,1
        addu $v1,$a2,$v0
        lhu $v1,0($v1)
;        la.l $a3,fontKerningData
        nop
        beq $v1,kerningIndex_noKerningMarker,@@done
        li $v0,0
        
        ;=====
        ; look up kerning
        ;=====
        
        ; a3 = pointer to data start
  ;          la $a3,fontKerningData
        addu $a3,$v1
        ; a2 = current kerning value (initially -1)
        li $a2,-1
;        la $a2,fontDefaultKerningTable
;        addu $a2,$t0
;        lw $a2,0($a2)
;        nop
;        addiu $a2,-1
        
        @@checkLoop:
          ; v0 = next byte
          lbu $v0,0($a3)
          nop
          ; v1 = high nybble
          andi $v1,$v0,0xF0
          
          ; 0xF0 = terminator
          bne $v0,0xF0,@@notTerminator
          nop
            ; assume null kerning
            j @@done
            li $v0,0
          @@notTerminator:
          
          ; 0xF1-0xFF = kerning value change
          bne $v1,0xF0,@@notIncrement
          ; t0 = low nybble
          andi $t0,$v0,0x0F
            ; kerning += signed value
            lb $v0,0($a3)
            ; ++src
            addiu $a3,1
            j @@checkLoop
            addu $a2,$v0
          @@notIncrement:
          
          ; 0xE0-0xEF = repeat
          bne $v1,0xE0,@@notRepeat
          nop
            ; t1 = base value for repeat
            lbu $t1,1($a3)
            ; src += 2
            addiu $a3,2
            
            ; v1 = max value of check
            addu $v1,$t0,$t1
            addiu $v1,kerningRepeatLoopOffset
            
            blt $a1,$t1,@@checkLoop
            nop
            ble $a1,$v1,@@matched
            nop
            j @@checkLoop
            nop
          @@notRepeat:
          
          ; literal
          ; if ID of previous character, return current kerning
          bne $v0,$a1,@@checkLoop
          ; ++src
          addiu $a3,1
          
        @@matched:
        ; return current kerning
        move $v0,$a2
        
      @@done:
      ; offset result by global kerning
;      jr $ra
;      addiu $v0,kerningBaseOffset
      lw $a0,currentFontIndex
      la $a1,fontDefaultKerningTable
      sll $a0,2
      addu $a0,$a1
      lw $a0,0($a0)
      nop
      jr $ra
      addu $v0,$a0

    ;===============================================
    ; string width
    ;===============================================
    
    ; a0 = string
    ; returns v0 = width in pixels, using currently selected font/settings
    getStringWidth:
      subiu $sp,24
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
      sw $s3,16($sp)
      sw $s4,20($sp)
        
        ; s2 = width
        move $s2,$zero
        ; s3 = target font index * 4
        lw $s3,currentFontIndex
        nop
        sll $s3,2
        
        ; save print state
        la $v0,printState_srcPtr
        la $v1,printState_charStateWord
        lw $s0,0($v0)
        lw $s1,0($v1)
        ; write new strptr and settings
        sw $a0,0($v0)
        sw $zero,0($v1)
        
        @@loop:
          ; fetch next input
          jal fetchNextPrintByte
          nop
          
          ; done if terminator
          beq $v0,$zero,@@done
          nop
          
          ; check if char or op
          bge $v0,charRangeStart,@@isChar
          nop
          @@isOp:
            bne $v0,opcode_absoluteStringRedirect,@@notAbsRedirect
            nop
              jal doAbsoluteStrRedirect
              nop
              j @@opDone
              nop
            @@notAbsRedirect:
            
            bne $v0,opcode_tabledStringRedirect,@@notTableRedirect
            nop
              jal doTabledStrRedirect
              nop
              j @@opDone
              nop
            @@notTableRedirect:
            
            ; any other op (e.g. newline) should be considered a terminator
            ; for the purpose of calculating width
            j @@done
            nop
            
            @@opDone:
            j @@loop
            nop
          @@isChar:
            ; update current/last char fields
            li $a0,printState_currentChar
            lbu $v1,0($a0)
            sb $v0,0($a0)
            sb $v1,printState_lastChar
            move $a0,$v0
            move $a1,$v1
            
            ; s4 = char
            ; get kerning
            ; skip kerning if last char was space or null
            beq $v1,$zero,@@kerningDone
            move $s4,$v0
            beq $v1,char_space,@@kerningDone
            nop
              jal getCharKerning
              nop
              ; offset width by kerning
              addu $s2,$v0
            @@kerningDone:
      
            ; convert codepoint to raw font index
            subiu $v0,$s4,charRangeStart
            
            ; look up width
            li $a1,fontWidthTable
            addu $a1,$s3
            lw $a1,0($a1)
            nop
            addu $a1,$v0
            lbu $v0,0($a1)
            nop
            
            ; width += value
            j @@loop
            addu $s2,$v0
          
        @@done:
        
        ; restore original print state
        sw $s0,printState_srcPtr
        sw $s1,printState_charStateWord
      
      ; return width
      move $v0,$s2
      ; minus one if not zero, because final character includes full advance width
      ; ...but this could mess with highlighting so don't worry about it
;      beq $v0,$zero,@@isZero
;      nop
;        addiu $v0,-1
;      @@isZero:
      
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
      lw $s3,16($sp)
      lw $s4,20($sp)
      jr $ra
      addiu $sp,24

    ;==================================
    ; new strings
    ;==================================
    
      ;=====
      ; de-optimized status screen labels
      ;=====
      
      newStr_statusScreenAtk:
        .incbin "out/script/new/status-label-0.bin"
      
      newStr_statusScreenDef:
        .incbin "out/script/new/status-label-1.bin"
      
      .align 4

    ;==================================
    ; use new strings
    ;==================================
    
    doStatusScreenAtkLoadPatch:
      la.u $a2,0x801B3588
      j 0x80182EB0
      la.l $a2,0x801B3588
    
    doStatusScreenDefLoadPatch:
      la.u $a2,0x801B3594
      j 0x80182F50
      la.l $a2,0x801B3594

    ;==================================
    ; 
    ;==================================
    
    ; a0 = x-pos after accounting for unprinted digits
    ; t7 = target number of digits to display
    ; goal: set a0 to desired x-pos
    ; 
    ; the original code has numbers monospaced at 13px, but we use much less
    ; than that. we want numbers to be right-aligned to the same position that
    ; they would have been in the original game, so we add an extra offset
    ; per digit to account for the difference.
    doNumberPrintExtraRightOffset:
      ; add (outputDigitCount * (oldDigitSpacing - newDigitSpacing)) to x-pos
      li $v1,(oldDigitSpacing - newDigitSpacing)
      mult $t7,$v1
      mflo $t0
      addu $a0,$t0
      
      ; make up work
      ; set y position
      addu $a1, $t9, $zero
      j 0x8018126C
      nop

    ;==================================
    ; 
    ;==================================
    
    modifyActionSelTechDescWinPos:
      lbu $v0,currentMenuType_gp($gp)
      nop
      bne $v0,2,@@notTechMenu
      nop
        ; offset y-position to account for increased box height if tech menu
;        addiu $a3,8
        addiu $a3,6
      @@notTechMenu:
      
      jal setUpWindowPos
      nop
      
      j 0x801880E4
      nop
    
    modifyActionSelTechDescWinDimensions:
      lbu $v0,currentMenuType_gp($gp)
      nop
      bne $v0,2,@@notTechMenu
      addiu $a1, $zero, 0x0003
        ; taller window if tech menu
;        addiu $a1, $zero, 0x000B
        addiu $a1, $zero, 0x0009
      @@notTechMenu:
      
      j 0x80188108
      lb $a0, 0x0EDD($gp)

    ;==================================
    ; new strings for technique descriptions
    ;==================================
  
    .include "out/script/asm/techdesc_strings.inc"
    .align 4

    ;==================================
    ; use new world map chapter string
    ;==================================
    
    ; overwrite the Nth character in worldmap_chapterNumStr
    ; with the actual chapter number
    worldMapChapterNum_digitPos equ 8
    
    doNewWorldMapChapterNum:
      ; set hardcoded position of digit in "chapter X" string
      ; to the desired number
      lbu $v0,0x801CC884
      la $a2,worldmap_chapterNumStr
      addiu $v0,char_digitBase
;      sb $v0,worldmap_chapterNumStr+8
      sb $v0,8($a2)
      jal printText
      nop
      
      j 0x8010A864
      nop
      
    
    ;==================================
    ; new strings for world map
    ;==================================
    
    worldmap_chapterNumStr:
      .incbin "out/script/worldmap/worldmap-0x7E38.bin"
      .align 4

    ;==================================
    ; 
    ;==================================
    
    consecutivePrintBufferSize equ 0x100
    
    ; s0 = first string pointer
    ; s1 = second string pointer
    doNewPrintConsecutiveStrings:
/*      ; new print buffer on stack
      subiu $sp,consecutivePrintBufferSize
        addiu $a0,$sp,0
        jal copyStrContent
        move $a1,$s0
        
        ; v0 = pointer to terminator of result of copy
        ; use as destination for next copy
        move $a0,$v0
        jal copyStrContent
        move $a1,$s1
        
        ; make up work
        addiu $a0,$sp,0
        jal printPopUpDialogue
        addiu $a1, $zero, 0x0001
      addiu $sp,consecutivePrintBufferSize*/
      
      move $a0,$s0
      jal newPrintConsecutiveStrings
      move $a1,$s1
      
      j 0x80120CEC
      nop
    
    ; a0 = first string pointer
    ; a1 = second string pointer
    newPrintConsecutiveStrings:
      subiu $sp,12
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
        
        move $s0,$a0
        move $s1,$a1
        
        ; new print buffer on stack
        subiu $sp,consecutivePrintBufferSize
          addiu $a0,$sp,0
          jal copyStrContent
          move $a1,$s0
          
          ; v0 = pointer to terminator of result of copy
          ; use as destination for next copy
          move $a0,$v0
          jal copyStrContent
          move $a1,$s1
          
          addiu $a0,$sp,0
          jal printPopUpDialogue
          addiu $a1, $zero, 0x0001
        addiu $sp,consecutivePrintBufferSize
      
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      jr $ra
      addiu $sp,12
    
    ; copies a string from src to dst.
    ; if the first byte of src is a string jump command (tabled or absolute),
    ; it is "followed" to the source string first.
    ; if the next string also starts with a redirect, it will be followed,
    ; and so on.
    ;
    ; a0 = dst
    ; a1 = src
    ; returns v0 = pointer to terminator of copy result
    copyStrContent:
      subiu $sp,20
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
      sw $s3,16($sp)
        ; s0 = dst
        move $s0,$a0
        ; s1 = src
        move $s1,$a1
        ; preserve old print pointer
        la $s2,printState_srcPtr
        lw $s3,0($s2)
        
        @@opCheckLoop:
          ; check for script jump command
          lbu $v0,0($s1)
          nop
          bne $v0,opcode_absoluteStringRedirect,@@notAbsRedirect
          nop
            ; print pointer = src + 1 (to skip opcode)
            addiu $s1,1
            jal doAbsoluteStrRedirect
            sw $s1,0($s2)
            
            ; repeat check for new string
            ; (probably unnecessary, but might as well)
            lw $s1,0($s2)
            j @@opCheckLoop
            nop
          @@notAbsRedirect:
          
          bne $v0,opcode_tabledStringRedirect,@@notTableRedirect
          nop
            ; print pointer = src + 1 (to skip opcode)
            addiu $s1,1
            jal doTabledStrRedirect
            sw $s1,0($s2)
            
            ; repeat check for new string
            ; (probably unnecessary, but might as well)
            lw $s1,0($s2)
            j @@opCheckLoop
            nop
          @@notTableRedirect:
          
          @@doCopy:
          ; s1 = (possibly updated) src
;          lw $s1,0($s2)
          
          ; copy
          @@copyLoop:
            ; v0 = src++
            lbu $v0,0($s1)
            addiu $s1,1
            ; copy to dst
            beq $v0,$zero,@@copyDone
            sb $v0,0($s0)
            
            ; ++dst
            j @@copyLoop
            addiu $s0,1
          @@copyDone:
        
        ; restore old print pointer
        sw $s3,0($s2)
        ; return pointer to terminator
        move $v0,$s0
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
      lw $s3,16($sp)
      jr $ra
      addiu $sp,20
    
    printFanartMessage:
      ; "-san no sakuhin" message
;      la $a1,0x801082F4
      ; doesn't fucking work
;      jal printConsecutiveStrings
      
      ; reverse order to allow for translation of art content in dialogue
      la $a0,0x801082F4
      jal newPrintConsecutiveStrings
      move $a1,$s0
      
      j 0x8010D05C
      nop

    ;===============================================
    ; status effect anim ff checks
    ;===============================================
    
    statusEffect1AnimFunc equ 0x8017C0CC
    statusEffect2AnimFunc equ 0x8017C228
    statusEffect3AnimFunc equ 0x8017BE18
    statusEffect4AnimFunc equ 0x8017BF70
    statusEffect5AnimFunc equ 0x8017C3CC
    statusEffect6AnimFunc equ 0x8017D0B8
    statusEffect7AnimFunc equ 0x8017D448
    statusEffect8AnimFunc equ 0x8017D50C
    
    doStatusEffect1FfCheck:
      la.u $a0,statusEffect1AnimFunc
      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect1AnimFunc
    
    doStatusEffect2FfCheck:
      la.u $a0,statusEffect2AnimFunc
      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect2AnimFunc
    
    doStatusEffect3FfCheck:
      la.u $a0,statusEffect3AnimFunc
      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect3AnimFunc
    
    doStatusEffect4FfCheck:
      la.u $a0,statusEffect4AnimFunc
      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect4AnimFunc
    
    doStatusEffect5FfCheck:
      la.u $a0,statusEffect5AnimFunc
      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect5AnimFunc
    
    doStatusEffect6FfCheck:
      la.u $a0,statusEffect6AnimFunc
      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect6AnimFunc
    
    doStatusEffect7FfCheck:
      la.u $a0,statusEffect7AnimFunc
      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect7AnimFunc
    
    doStatusEffect8FfCheck:
      la.u $a0,statusEffect8AnimFunc
;      j doGenericStatusEffectFfCheck
      la.l $a0,statusEffect8AnimFunc
    ; !!! drop through !!!
    doGenericStatusEffectFfCheck:
      subiu $sp,8
      sw $ra,0($sp)
      sw $a0,4($sp)
        ; check if ff button pressed
        jal checkIfFfButtonPressed
        nop
;        lw $a0,4($sp)
        
        beq $v0,$zero,@@doIt
        nop
          ; skip only if in "skip all" mode
          jal getConfigSetting
          li $a0,ffModeOptionIndex
          
;          lw $a0,4($sp)
          
          bne $v0,$zero,@@done
          nop
          @@doIt:
            lw $a0,4($sp)
            nop
            ; make up work
            jalr $a0
            move $a0,$s0
      @@done:
      lw $ra,0($sp)
      addiu $sp,8
      jr $ra
      nop
    
    ;========================================
    ; 
    ;========================================
    
      doLevelUpAnimSkipCheck:
        jal checkIfFfButtonPressed
        nop
        beq $v0,$zero,@@doIt
        nop
          ; skip only if in "skip all" mode
          jal getConfigSetting
          li $a0,ffModeOptionIndex
          bne $v0,$zero,@@done
          nop
          @@doIt:
            ; make up work
            lw $a1, 0x050C($gp)
            jal 0x8017D614
            addu $a0, $s1, $zero
        @@done:
        j 0x80123758
        nop
    
    ;========================================
    ; 
    ;========================================
      
      doSpecialLevelUpAnimSkipCheck:
        jal checkIfFfButtonPressed
        nop
        beq $v0,$zero,@@doIt
        nop
          ; skip only if in "skip all" mode
          jal getConfigSetting
          li $a0,ffModeOptionIndex
          bne $v0,$zero,@@done
          nop
          @@doIt:
            ; make up work
            addu $a0, $s0, $zero
            jal 0x8017D614
            addiu $a1, $zero, 0x0001
        @@done:
        j 0x80119350
        nop
    
    ;========================================
    ; 
    ;========================================
      
      doDefenseAnimSkipCheck:
        ; make up work
        addu $s1, $zero, $zero
        sw $ra, 0x0044($sp)
        sw $s2, 0x0040($sp)
        
        jal checkIfFfButtonPressed
        nop
        beq $v0,$zero,@@doIt
        nop
          ; skip only if in "skip all" mode
          jal getConfigSetting
          li $a0,ffModeOptionIndex
          bne $v0,$zero,@@done
          nop
          @@doIt:
            ; make up work
            lui $v0, 0x801D
            lw $v0, 0xCDC0($v0)
            j 0x8017D930
            nop
        
        @@done:
        j 0x8017DAC4
        nop
        
    
    ;========================================
    ; 
    ;========================================
    
    doStreamedSubtitleQueueCheck:
      ; save ACMFILE.DAT string pointer that's currently in a1
      sw $a1,nextAcmFileStrPtr
      ; make up work
      jal strcmp
      addu $a0,$s1,$zero
      
      bne $v0,$zero,@@noMatch
      sll $v0, $s0, 16
        ; check last two bytes of 16-byte entry for subtitle code
        lw $a0,nextAcmFileStrPtr
        nop
        lhu $v0,14($a0)
        nop
        ; top 2 bits are reserved for type id, which currently isn't used
        ; and probably won't be
;        andi $v0,0x3FFF
        sw $v0,queuedStreamedSubtitleId
        
        ; save the channel number (indexNum % 32) of the target sound file
        ; so we can use it to apply a timing offset for the subtitles later
        andi $v0,$s0,0x1F
        sw $v0,queuedStreamedSubtitleChannelNum
        
        ; make up work
        j 0x8012AF74
        sll $v0, $s0, 16
      @@noMatch:
      ; make up work
      j 0x8012AF64
      nop
    
    nextAcmFileStrPtr:
      .dw 0
    queuedStreamedSubtitleId:
      .dw 0
    queuedStreamedSubtitleChannelNum:
      .dw 0
    
    ;========================================
    ; 
    ;========================================
    
    doStreamedSubtitleStartCheck:
      ; make up work
      jal triggerXaPlayback
      addu $a1, $s2, $zero
      
      ; flag last disc access as xa
      li $v0,1
      sw $v0,lastDiscAccessWasXa
      
      ; check if subtitle queued
      la $a0,queuedStreamedSubtitleId
      lw $v0,0($a0)
      nop
      beq $v0,$zero,@@done
      ; clear queued sub id
      sw $zero,0($a0)
        move $a0,$v0
        ; subtract 1 from nominal target value, since 0 is used to
        ; indicate no script
        jal startStreamedSubtitle
        addiu $a0,-1
      @@done:
      
      ; make up work
      j 0x8012B328
      nop
    
    ;========================================
    ; 
    ;========================================
    
    doStreamedSubtitleWaitFrameUpdate:
      jal updateStreamedSubtitle
      nop
      
;      sw $zero,printBufferSendsSinceLastFrameWait
      
      ; make up work
      jal 0x801447E8
      nop
      j 0x80189468
      nop
      
;    printBufferSendsSinceLastFrameWait:
;      .dw 0
    
    ;========================================
    ; 
    ;========================================
    
    doStopStreamedSoundSubtitleStop:
      jal stopStreamedSubtitle
      nop
      
      ; make up work
      jal waitFrame
      nop
      j 0x8012B534
      nop
    
    ;========================================
    ; 
    ;========================================
    
    doFadeSubtitleRenderingCheck:
      ; make up work
      bne $v1,$v0,@@render
      lw $v0,sSub_stringActive
        nop
        ; if screen is fully faded, the game normally would not render,
        ; but we need it to keep rendering if subtitles are being shown.
        ; the game continues to generate the fade polygon regardless,
        ; which makes this easier.
        bne $v0,$zero,@@render
        nop
          ; don't render
          j 0x8018950C
          nop
      @@render:
      j 0x801894F4
      nop
    
    ;========================================
    ; 
    ;========================================
    
    ; id of last valid unit id
    maxStandardUnitId equ 86
    
    ; s0 = unit struct pointer
    ; goal: set v1 to char name pointer
    checkForNewUnitNameAbbrev:
      ; v0 = unit id
      lw $v0,0($s0)
      ; default name = normal from struct
      lw $v1,4($s0)
      
      ; ignore units > last valid unit
      ; (an id of 0xFF indicates generic obstacles like trees)
      bgt $v0,maxStandardUnitId,@@done
      la $a0,unitNameAbbrevTable
      ; unit id shouldn't be 0, but check anyway
      beq $v0,$zero,@@done
      subiu $v0,1
      ; index (unit id - 1) into abbreviation table
      sll $v0,1
      addu $v0,$a0
      lh $a1,0($v0)
      nop
      
      ; done if index entry negative
      bltz $a1,@@done
      nop
        ; v1 = new string pointer
        addu $v1,$a0,$a1
      @@done:
      j 0x801847CC
      nop
    
    unitNameAbbrevTable:
      .incbin "out/script/unitabbrev/table.bin"
      .align 4
    
    ;========================================
    ; 
    ;========================================
    
/*    queuePolylinaSnackString:
      ; make up work to finish stack prep so we can call functions
      ; s0 = card num
      andi $s0, $s2, 0xFFFF
      sw $ra, 0x0020($sp)
      
      ; default 2-digit card string = 0x80116FD0
      la $s1,0x80116FD0
      ; check if card num <= 10
      bge $s0,10,@@cardNum2Digits
      nop
        la $s1,polylinaSnack1digitString
      @@cardNum2Digits:
      
      j 0x80189868
      nop*/
    
    queuePolylinaSnackString:
      ; default 2-digit card string
      la $s1,polylinaSnack2digitString
      ; check if card num <= 10
      bge $s2,10,@@cardNum2Digits
      nop
        la $s1,polylinaSnack1digitString
      @@cardNum2Digits:
      
      j 0x80189860
      nop
    
    doPolylinaSnackFirstDigit:
      addiu $v0,$v1,char_digitBase
      ; write to target position in string
;      sb $v0, 0x0001($s1)
      j 0x8018997C
      sb $v0,19($s1)
    
    doPolylinaSnackSecondDigit:
      addiu $v0,$v0,char_digitBase
      ; write to target position in string
;      sb $v0, 0x0003($s1)
      bge $a0,10,@@idIs2digit
      nop
      @@idIs1digit:
        j @@done
        sb $v0,19($s1)
      @@idIs2digit:
        sb $v0,20($s1)
      @@done:
      
      ; make up work
      j 0x801899A8
      move $a0,$s1
    
    polylinaSnack1digitString:
      .incbin "out/script/new/polysnack-0.bin"
    polylinaSnack2digitString:
      .incbin "out/script/new/polysnack-1.bin"
    .align 4
    
    ;========================================
    ; 
    ;========================================
    
    drawExtraOptionStates:
      ; make up work
      jal redrawObj
      sra $a0, $s0, 16
      
      ; handle new options
      
      ; subtitles
      jal getConfigSetting
      addiu $a0, $zero, subtitlesOptionIndex
      ; base obj index
      lhu $s0, 0x0FC8($gp)
      ; s1 = value of target option
      addu $s1, $v0, $zero
      ; target subobj index
      addiu $s0, $s0, 0x000A+numNewOptionItems
      sll $a0, $s0, 16
      jal hideObjAndNext
      sra $a0, $a0, 16
      ; show appropriate obj for current setting
      addu $s0, $s0, $s1
      sll $s0, $s0, 16
      jal redrawObj
      sra $a0, $s0, 16
      
      ; fast-forward
      jal getConfigSetting
      addiu $a0, $zero, ffModeOptionIndex
      ; base obj index
      lhu $s0, 0x0FC8($gp)
      ; s1 = value of target option
      addu $s1, $v0, $zero
      ; target subobj index
      addiu $s0, $s0, 0x000C+numNewOptionItems
      sll $a0, $s0, 16
      jal hideObjAndNext
      sra $a0, $a0, 16
      ; show appropriate obj for current setting
      addu $s0, $s0, $s1
      sll $s0, $s0, 16
      jal redrawObj
      sra $a0, $s0, 16
      
      j 0x801357C0
      nop
    
    ;========================================
    ; 
    ;========================================
    
    checkForExtraOptionsToggle:
      ; make up work
      lh $v1, 0x0FA2($gp)
      nop
      bne $v1,ffModeOptionIndex,@@notFirstExtraOption
      nop
        j 0x80135A0C
        li $a0,ffModeOptionIndex
      @@notFirstExtraOption:
      
      bne $v1,subtitlesOptionIndex,@@notSecondExtraOption
      nop
        j 0x80135A0C
        li $a0,subtitlesOptionIndex
      @@notSecondExtraOption:
      
      ; make up work
      j 0x801359D4
      addiu $v0, $zero, 0x0001
    
    ;========================================
    ; 
    ;========================================
    
    getConfigSetting_checkNewOptions:
      ; make up work (although this cast almost certainly isn't needed)
      sll $a0, $a0, 16
      sra $a0, $a0, 16
      
      ; first, check if target option is 1 (voices on/off),
      ; because we are piggybacking off of it by putting the new options
      ; within its halfword (so it will be saved and loaded to the memory card).
      ; this means we need to make sure to mask it to only bit 0.
      bne $a0,1,@@notVoicesOption
      nop
        ; load normal option
        lhu $v0,optionVoice_gp($gp)
        nop
        ; return only bit 0
        andi $v0,0x1
        j 0x801355DC
        nop
      @@notVoicesOption:
      
      ; check if new option
      bne $a0,ffModeOptionIndex,@@notFfModeOption
      nop
        ; load voices option
        lhu $v0,optionVoice_gp($gp)
        nop
        ; return only bit 1
        andi $v0,0x2
        j 0x801355DC
        srl $v0,1
      @@notFfModeOption:
      
      ; check if new option
      bne $a0,subtitlesOptionIndex,@@notSubtitlesOption
      nop
        ; load voices option
        lhu $v0,optionVoice_gp($gp)
        nop
        ; return only bit 2
        andi $v0,0x4
        j 0x801355DC
        srl $v0,2
      @@notSubtitlesOption:
      
      ; make up work
      j 0x80135580
      nop
    
    ;========================================
    ; 
    ;========================================
    
    toggleConfigSetting_checkNewOptions:
      ; make up work (although this cast almost certainly isn't needed)
      sll $a0, $a0, 16
      sra $a0, $a0, 16
      
      ; first, check if target option is 1 (voices on/off),
      ; because we are piggybacking off of it by putting the new options
      ; within its halfword (so it will be saved and loaded to the memory card).
      ; this means we need to make sure to mask it to only bit 0.
      bne $a0,1,@@notVoicesOption
      nop
        ; load normal option
        lhu $v0,optionVoice_gp($gp)
        nop
        ; flip bit 0
        xori $v0,0x1
        j 0x80135668
        sh $v0,optionVoice_gp($gp)
      @@notVoicesOption:
      
      ; check if new option
      bne $a0,ffModeOptionIndex,@@notFfModeOption
      nop
        ; load normal option
        lhu $v0,optionVoice_gp($gp)
        nop
        ; flip bit 1
        xori $v0,0x2
        j 0x80135668
        sh $v0,optionVoice_gp($gp)
      @@notFfModeOption:
      
      ; check if new option
      bne $a0,subtitlesOptionIndex,@@notSubtitlesOption
      nop
        ; load normal option
        lhu $v0,optionVoice_gp($gp)
        nop
        ; flip bit 2
        xori $v0,0x4
        j 0x80135668
        sh $v0,optionVoice_gp($gp)
      @@notSubtitlesOption:
      
      ; make up work
      j 0x801355F0
      nop
    
    ;========================================
    ; 
    ;========================================
    
    doExtraButtonRepeatLogic:
      ; set R2 bit in inverted button state bitfield so it is never
      ; regarded as pressed for purposes of deciding button repeat conditions
      ori $a0,button_r2
    updateButtonRepeatWithR2:
      ; make up work
      lbu $v0, 0x11A8($gp)
      j 0x80186E30
      lbu $a2, 0x11D0($gp)
    
    ;========================================
    ; 
    ;========================================
    
    useNewMultiplayerString_0:
      ; at this point, v0 = current player index (0 or 1)
      ; turn into 1-based digit and overwrite in new string
      la $v1,multiplayerString0
      addiu $v0,(char_digitBase+1)
      ; HACK
      sb $v0,0($v1)
      
      ; make up work
      j 0x8010A108
      addiu $v1,1
    
    useNewMultiplayerString_1:
      ; at this point, v0 = current player index (0 or 1)
      ; turn into 1-based digit and overwrite in new string
      la $a2,multiplayerString1
      addiu $v0,(char_digitBase+1)
      ; HACK
      sb $v0,9($a2)
      ; also overwrite player number (always same as slot number)
      sb $v0,0($a2)
      
      ; make up work
      addiu $a0, $zero, 0x002A
      j 0x8010BAA0
      addiu $a1, $zero, 0x0026
    
    useNewMultiplayerString_2:
      ; at this point, v0 = current player index (0 or 1)
      ; turn into 1-based digit and overwrite in new string
      la $a0,multiplayerString2
      addiu $v0,(char_digitBase+1)
      ; HACK
      sb $v0,9($a0)
      ; also overwrite player number (always same as slot number)
      sb $v0,0($a0)
      
      ; make up work
      lui $a1, 0x8011
      jal printTwoLineSysMessage
      addiu $a1, $a1, 0x8260
      j 0x8010C004
      nop
    
    useNewMultiplayerString_3_0:
      ; at this point, v0 = current player index (0 or 1)
      ; turn into 1-based digit and overwrite in new string
      la $a0,multiplayerString3
      addiu $v0,(char_digitBase+1)
      ; HACK
      sb $v0,9($a0)
      ; also overwrite player number (always same as slot number)
      sb $v0,0($a0)
      
      ; make up work
      j 0x8010B9C8
      addiu $a0,1
    
    useNewMultiplayerString_3_1:
      ; at this point, v0 = current player index (0 or 1)
      ; turn into 1-based digit and overwrite in new string
      la $a0,multiplayerString3
      addiu $v0,(char_digitBase+1)
      ; HACK
      sb $v0,9($a0)
      ; also overwrite player number (always same as slot number)
      sb $v0,0($a0)
      
      ; make up work
      la $a1, 0x801081BC
      la $a2, 0x801081CC
      j 0x8010BC00
      addiu $a0,1
    
    useNewMultiplayerString_3_2:
      ; at this point, v0 = current player index (0 or 1)
      ; turn into 1-based digit and overwrite in new string
      la $a0,multiplayerString3
      addiu $v0,(char_digitBase+1)
      ; HACK
      sb $v0,9($a0)
      ; also overwrite player number (always same as slot number)
      sb $v0,0($a0)
      
      ; make up work
      la $a1, 0x801081DC
      la $a2, 0x801081CC
      j 0x8010BC74
      addiu $a0,1
    
    useNewMultiplayerString_3_3:
      ; at this point, v0 = current player index (0 or 1)
      ; turn into 1-based digit and overwrite in new string
      la $a0,multiplayerString3
      addiu $v0,(char_digitBase+1)
      ; HACK
      sb $v0,9($a0)
      ; also overwrite player number (always same as slot number)
      sb $v0,0($a0)
      
      ; make up work
      la $a1, 0x801081DC
      la $a2, 0x801081CC
      j 0x8010BE74
      addiu $a0,1
    
    ; 
    multiplayerString0:
      .incbin "out/script/new/multiplayer-0.bin"
    multiplayerString1:
      .incbin "out/script/new/multiplayer-1.bin"
    multiplayerString2:
      .incbin "out/script/new/multiplayer-2.bin"
    multiplayerString3:
      .incbin "out/script/new/multiplayer-3.bin"
    .align 4
    
    ;========================================
    ; 
    ;========================================
    
    ; s1 = message string pointer
    setUpAlbumMemCardLabel:
      ; a1 = card label pointer
      lw $a1,0x8010C80C
      ; s0 = dst
      la $s0,newAlbumMessageBuffer
      ; init with null string
      beq $a1,$zero,@@noLabel
      sb $zero,0($s0)
        ; copy label to buffer
        jal copyStrContent
        move $a0,$s0
        ; new dst = buffer terminator
        move $s0,$v0
      @@noLabel:
      ; copy message string content
      move $a0,$s0
      jal copyStrContent
      move $a1,$s1
      
      ; return pointer to buffer
      la.u $v0,newAlbumMessageBuffer
      j 0x8010A508
      la.l $v0,newAlbumMessageBuffer
      
    newAlbumMessageBuffer:
      .fill 128,0x00
      .align 4
    
    ;========================================
    ; 
    ;========================================
    
    ; a0 = id
    startStreamedSubtitle:
      subiu $sp,8
      sw $ra,0($sp)
      sw $s0,4($sp)
        
        ; s0 = id
        move $s0,$a0
        
        ; check if subtitles disabled
        jal getConfigSetting
        li $a0,subtitlesOptionIndex
        
        sw $zero,sSub_scriptPtr
        bne $v0,$zero,@@done
        move $a0,$s0
          ; read table entry
          sll $a0,1
          la $v0,streamedSubtitleTable
          addu $a0,$v0
          lhu $a0,0($a0)
          la $v1,sSub_scriptPtr
          addu $a0,$v0
          ; set script pointer to target
          sw $a0,0($v1)
          
          ; reset timer
          sw $zero,sSub_timer
          
          ; initialize timer based on channel number of sound.
          ; channel 0 starts at sector 0, channel 1 at sector 1, etc.,
          ; with each sector representing 0.4 60fps frames.
          ; 
          ; multiply channel num by 0.4 to get delay before subtitle processing starts
          lw $v0,queuedStreamedSubtitleChannelNum
          li $a0,0x6666
          multu $v0,$a0
          mflo $a1
          srl $a1,16
          ; flip sign of result
          li $v0,-1
          xor $a1,$v0
          addiu $a1,1
          sw $a1,sSub_timer
          sw $a1,sSub_timer_offset
          
          ; vram pos override off
          sw $zero,sSub_VramOverrideOn
        @@done:
        
      lw $ra,0($sp)
      lw $s0,4($sp)
      jr $ra
      addiu $sp,8
    
    ;========================================
    ; 
    ;========================================
    
    stopStreamedSubtitle:
      subiu $sp,4
      sw $ra,0($sp)
;      sw $s0,4($sp)
        
        ; stop active string, if any
        jal stopStreamedSubtitleString
        nop
        
        ; zero script pointer
        sw $zero,sSub_scriptPtr
        
      lw $ra,0($sp)
;      lw $s0,4($sp)
      nop
      jr $ra
      addiu $sp,4
    
    ;========================================
    ; 
    ;========================================
    
    updateStreamedSubtitle:
      subiu $sp,12
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
        
        ; s0 = subtitle pointer
        lw $s0,sSub_scriptPtr
        ; s1 = current font index
        la $v0,currentFontIndex
        lw $s1,0($v0)
        ; set font index to subtitle font
        li $v1,fontIndex_subtitle
        sw $v1,0($v0)
        
        ; do nothing if subtitle pointer null
        beq $s0,$zero,@@skip
        nop
        
        ; do nothing if timer is negative
        ; (used to account for delays caused by playing a nonzero channel)
        lw $v0,sSub_timer
;        lw $v1,sSub_timer_offset
        nop
;        addu $v0,$v1
        bltz $v0,@@done
        nop
        
        @@loop:
          ; peek at next byte
          lbu $v0,0($s0)
          nop
          ; done if null
          bne $v0,$zero,@@notNull
          nop
            ; clear script ptr
            j @@done
            move $s0,$zero
          @@notNull:
          
          ; wait command?
          bne $v0,opcode_sSubWait,@@notWait
          nop
            ; a0 = target timer value
            lbu $a1,2($s0)
            lbu $a0,1($s0)
            sll $a1,8
            ; v0 = current timer value
            lw $v0,sSub_timer
;            lw $v1,sSub_timer_offset
;            nop
;            addu $v0,$v1
            or $a0,$a1
            ; break interpreter loop if current timer < target
            blt $v0,$a0,@@done
            nop
            
            ; advance past op
            j @@loopEnd
            addiu $s0,3
          @@notWait:
          
          ; off command?
          bne $v0,opcode_sSubOff,@@notOff
          nop
            jal stopStreamedSubtitleString
            nop
            
            j @@loopEnd
            addiu $s0,1
          @@notOff:
          
          ; vram override command?
          bne $v0,opcode_sSubVramOverride,@@notVramOverride
          nop
            ; base x
            lbu $a1,2($s0)
            lbu $a0,1($s0)
            sll $a1,8
            or $a0,$a1
            sw $a0,sSub_VramOverrideX
            sw $a0,sSub_VramOverrideBaseX
            
            ; base y
            lbu $a1,4($s0)
            lbu $a0,3($s0)
            sll $a1,8
            or $a0,$a1
            sw $a0,sSub_VramOverrideY
            sw $a0,sSub_VramOverrideBaseY
            
            ; enable override mode
            li $v0,1
            sw $v0,sSub_VramOverrideOn
            
            j @@loopEnd
            addiu $s0,5
          @@notVramOverride:
          
          ; any other value is assumed to be the start of a new string to display
          ; do NOT print if currently printing!!
          ; (printText can call waitFrame, which calls this routine,
          ; which will result in endless recursion)
          lw $v0,printState_currentlyPrinting
          nop
          bne $v0,$zero,@@noPrint
          nop
            jal startNewStreamedSubtitleString
            move $a0,$s0
          @@noPrint:
          
          ; update script ptr from returned value
          move $s0,$v0
          
          @@loopEnd:
          j @@loop
          nop
        @@done:
        
        ; update script pointer
        sw $s0,sSub_scriptPtr
        
        ; increment timer
/*        la $a0,sSub_timer
        lw $v1,0($a0)
        nop
        addiu $v1,1
        sw $v1,0($a0)*/
        
        ; thanks, programmers, for making no effort to have the game render
        ; consistently, resulting in constant delays every time a new image
        ; has to be loaded or something that mean we can't just increment a counter
        ; every render frame to make this work.
        ; let's instead go for the jugular and directly compute how much time
        ; has passed based on the current XA playback position.
        ; there will still be small desynchronizations during gaps where the game
        ; isn't rendering (since we can't really do anything about that),
        ; but they'll be corrected as soon as it resumes.
        
        ; v1 = (currentPos - basePos)
        lw $v1,currentXaPlaybackSector_gp($gp)
        lw $v0,baseXaPlaybackSector_gp($gp)
        lw $a0,endXaPlaybackSector_gp($gp)
        nop
        ; on the off chance current sector >= end, stop subtitles
        blt $v1,$a0,@@xaEndNotReached
        subu $v1,$v0
          jal stopStreamedSubtitle
          nop
          j @@skip
          nop
        @@xaEndNotReached:
        
        ; HACK: if xa time has not changed since the last update,
        ; assume 1 frame has passed.
        ; (the game simply stops updating the xa position under some circumstances,
        ; such as when scrolling over the large images in the intro story summary.)
        lw $a0,lastXaSector
        sw $v1,lastXaSector
        bne $a0,$v1,@@xaTimeHasChanged
        lw $v0,sSub_timer
          nop
          j @@setFinalTime
          addiu $v0,1
        @@xaTimeHasChanged:
        
        ; compute actual change in time.
        ; xa compression is 3.875 times more efficient
        ; compared to normal playback.
        ; so, by multiplying the raw sector change by 3.875 (0x0003E000),
        ; we get the effective change in CD frames for "normal" playback.
        ; 75 sectors = 1 second.
        ; we want the time to be in terms of 60fps frames,
        ; so divide by 75 and multiply by 60 (= multiply by 0.8).
        ; then divide by 8 because... uh, xa channel interleaving?
        ; so, multiply raw value by 0.3875?
        ;
        ; ...32 sectors = 25.6 60fps frames.
        ; so 1 sector = 0.8 60fps frames.
        ; drive reads at double speed, so it's actually 0.4.
        li $a0,0x6666
        multu $v1,$a0
        mflo $v0
        srl $v0,16
        ; this seems to run a bit behind, add a few frames to make up for it
;        addiu $v0,sSub_xaTimingOffset
        ; okay, actually, this seems to be caused by the delay between playback
        ; starting and the sector with the target channel actually being reached
        ; (a delay of 0-12 frames, with my old empirically determined fixed delay being
        ; -6 frames, right in the middle).
        ; so now we keep track of the needed offset and account for it here.
        lw $v1,sSub_timer_offset
        nop
        addu $v0,$v1
        bge $v0,$zero,@@noUnderflow
        nop
          move $v0,$zero
        @@noUnderflow:
        
        @@setFinalTime:
        sw $v0,sSub_timer
        
        @@skip:
        ; restore font index
        la $v1,currentFontIndex
        sw $s1,0($v1)
        
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      jr $ra
      addiu $sp,12
    
    lastXaSector:
      .dw 0
    
    ;========================================
    ; 
    ;========================================
    
    stopStreamedSubtitleString:
      subiu $sp,8
      sw $ra,0($sp)
      sw $s0,4($sp)
        
        ; do nothing if string not active
        la $a0,sSub_stringActive
        lw $v0,0($a0)
        nop
        ; clear string active flag
        beq $v0,$zero,@@done
        sw $zero,0($a0)
        
        ; if using override vram positioning, restore old slot x/y positions
        ; before freeing them if needed
        lw $v0,sSub_VramOverrideOn
        ; a2/a3 = start/end slot nums
        lw $a2,sSub_currentStringStartSlot
        beq $v0,$zero,@@noVramOverride
        lw $a3,sSub_currentStringEndSlot
          la $a0,sSub_VramOverrideOldXyArray
          beq $a2,$a3,@@noVramOverride
          la $a1,dispSlotVramDstArray
            ; t0 = initial dst offset
            sll $t0,$a2,3
            addu $a1,$t0
            @@vramOverrideRestoreLoop:
              ; copy old x/y
              lhu $v0,0($a0)
              lhu $v1,2($a0)
              sh $v0,0($a1)
              sh $v1,2($a1)
              
              ; advance to next pos in src/dst
              addiu $a0,4
              ; loop over all used slots
              addiu $a2,1
              bne $a2,$a3,@@vramOverrideRestoreLoop
              addiu $a1,8
            
            ; reset initial x/y pos for next string, if any
            lw $v0,sSub_VramOverrideBaseX
            lw $v1,sSub_VramOverrideBaseY
            sw $v0,sSub_VramOverrideX
            sw $v1,sSub_VramOverrideY
        @@noVramOverride:
        
        ; free current render slots
        lw $a0,sSub_currentStringStartSlot
        lw $a1,sSub_currentStringEndSlot
        jal freeRenderSlotSet
        nop
        
        ; reset current display slot to original value
        lw $a0,sSub_currentStringStartSlot
        nop
        sh $a0,currentDispSlot_gp($gp)
        
        ; clear render slot indices
;        sw $zero,sSub_currentStringStartSlot
;        sw $zero,sSub_currentStringEndSlot
        
      @@done:
      lw $ra,0($sp)
      lw $s0,4($sp)
      jr $ra
      addiu $sp,8
    
    ;========================================
    ; 
    ;========================================
    
    sSub_lineBufferSize equ 128
    sSub_maxNumLines equ 2
;    sSub_baseY equ 208
    sSub_baseY equ 192
    sSub_fullW equ 320
    sSub_lineHeight equ 16
    
    ; a0 = src string pointer
    ; returns one past src string's terminator
    startNewStreamedSubtitleString:
      subiu $sp,20
      sw $ra,0($sp)
      sw $s0,4($sp)
      sw $s1,8($sp)
      sw $s2,12($sp)
      sw $s3,16($sp)
        
        ; s0 = src string pointer
        move $s0,$a0
        ; s1 = line count
        li $s1,0
        
        ; if string is null, do nothing
        lbu $v0,0($s0)
        nop
        bne $v0,$zero,@@notNullString
        nop
          ; return one past terminator
          j @@done
          addiu $v0,$s0,1
        @@notNullString:
        
        ; stop any existing string
        jal stopStreamedSubtitleString
        nop
        
        ; s2 = dst
        la $s2,sSub_lineBuffers
        @@lineCopyLoop:
          ; copy next line to next buffer
          ; a1 = dst
          move $a1,$s2
          ; a0 = src
          jal copyNextStringLine
          move $a0,$s0
          
          ; advance to next buffer
          ; dst += buffer size
          addiu $s2,sSub_lineBufferSize
          
          ; s0 = pointer to terminator (either null or linebreak)
          move $s0,$v0
          ; if not null, continue with next line
          lbu $v0,0($s0)
          ; ++lineCount
          addiu $s1,1
          ; ++src (advancing past terminator)
          bne $v0,$zero,@@lineCopyLoop
          addiu $s0,1
        
        ; set starting display slot
        lhu $v0,currentDispSlot_gp($gp)
        nop
        sw $v0,sSub_currentStringStartSlot
        
        ; s2 = starting y-position
        li $s2,sSub_baseY
        ; offset by -(lineCount * lineHeight / 2) to center around base point
        li $v1,(sSub_lineHeight/2)
        mult $s1,$v1
        mflo $a0
        subu $s2,$a0
        
        ; s3 = src
        la $s3,sSub_lineBuffers
        @@lineDrawLoop:
          ; get string width
          jal getStringWidth
          move $a0,$s3
          
          ; set up for print
          
          ; a0 = x
          ; use width to determine x-pos for line centering
          ; (fullW - lineW) / 2
          li $a0,sSub_fullW
          subu $a0,$v0
          srl $a0,1
          
          ; a1 = y
          move $a1,$s2
          
          ; a2 = string ptr
          move $a2,$s3
          
          ; print
          ; a3 = color modifier (special token indicating subtitle decoding)
          jal newPrintText
          li $a3,charDecodingTable_subtitleCode
          
          ; y-pos += lineHeight
          addiu $s2,sSub_lineHeight
          
          ; --line counter
          addiu $s1,-1
          ; advance to next buffer
          ; dst += buffer size
          bne $s1,$zero,@@lineDrawLoop
          addiu $s3,sSub_lineBufferSize
        @@lineDrawLoopEnd:
        
        ; set ending display slot
        lhu $v0,currentDispSlot_gp($gp)
        nop
        sw $v0,sSub_currentStringEndSlot
          
        ; flag string as active
        li $a0,1
        sw $a0,sSub_stringActive
        
        ; return one past string terminator
        move $v0,$s0
      
      @@done:
      lw $ra,0($sp)
      lw $s0,4($sp)
      lw $s1,8($sp)
      lw $s2,12($sp)
      lw $s3,16($sp)
      jr $ra
      addiu $sp,20
    
    ;========================================
    ; 
    ;========================================
    
    ; a0 = src string
    ; a1 = dst
    ; returns pointer to line terminator
    ; (either a null or a linebreak)
    copyNextStringLine:
      @@loop:
        ; copy byte
        lbu $v1,0($a0)
        ; src is return value
        move $v0,$a0
        ; stop if null or linebreak
        beq $v1,$zero,@@done
        sb $v1,0($a1)
        ; ++src
        beq $v1,opcode_linebreak,@@done
        addiu $a0,1
        
        ; ++dst
        j @@loop
        addiu $a1,1
        
      @@done:
      ; always terminate result string with null
      jr $ra
      sb $zero,0($a1)
    
    ;========================================
    ; 
    ;========================================
    
    sSub_scriptPtr:
      .dw 0
    sSub_timer:
      .dw 0
    sSub_timer_offset:
      .dw 0
    sSub_stringActive:
      .dw 0
    sSub_currentStringStartSlot:
      .dw 0
    sSub_currentStringEndSlot:
      .dw 0
    sSub_lineBuffers:
      .fill (sSub_lineBufferSize*sSub_maxNumLines),0x00
    sSub_VramOverrideOn:
      .dw 0
    sSub_VramOverrideX:
      .dw 0
    sSub_VramOverrideY:
      .dw 0
    sSub_VramOverrideBaseX:
      .dw 0
    sSub_VramOverrideBaseY:
      .dw 0
    sSub_VramOverrideOldXyArray:
      .fill (40*4),0x00
      .align 4
    
    ;========================================
    ; new streamed subtitle resources
    ;========================================
    
    .align 4
    streamedSubtitleTable:
      .incbin "out/script/xa/table.bin"
    .align 4
    
    ;========================================
    ; new video subtitle resources
    ;========================================
    
    .align 4
    .include "asm/includes/video_new.asm"
    .align 4
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
  ;============================================================================
  ; NEW STUFF (speedup hack)
  ;============================================================================

    ;===============================================
    ; adv scrolling fast-forward
    ;===============================================
    
    ; a0 = reserved
    ; a1 = reserved
    ; a2 = y-change
/*    doAdvScrollFfDown:
      ; adjusts v0 according to fast-forward rules
      ; (increasing speed and capping if it exceeds the remaining distance)
      jal inline_doScrollFfCheckAndCap
      move $v0,$a2
      
      ; move result to needed param register
      move $a2,$v0
      
      ; make up work
      jal 0x8013E73C
      subu $a2, $zero, $a2
      j 0x8013332C
      nop
    
    ; a0 = reserved
    ; a1 = reserved
    ; a2 = y-change
    doAdvScrollFfUp:
      jal inline_doScrollFfCheckAndCap
      move $v0,$a2
      
      move $a2,$v0
      
      ; make up work
      jal 0x8013E73C
      addiu $s0, $s0, 0x0001
      j 0x80133384
      nop
    
    ; a0 = reserved
    ; a1 = x-change
    ; a2 = reserved
    doAdvScrollFfRight:
      jal inline_doScrollFfCheckAndCap
      move $v0,$a1
      
      move $a1,$v0
      
      ; make up work
      jal 0x8013E73C
      subu $a1, $zero, $a1
      j 0x801333E0
      nop
    
    ; a0 = reserved
    ; a1 = x-change
    ; a2 = reserved
    doAdvScrollFfLeft:
      jal inline_doScrollFfCheckAndCap
      move $v0,$a1
      
      move $a1,$v0
      
      ; make up work
      jal 0x8013E73C
      addiu $s0, $s0, 0x0001
      j 0x8013342C
      nop
    
    ; v0 = normal scrolling amount
    ; a0-a2 = reserved
    inline_doScrollFfCheckAndCap:
      ; get raw bitfield of buttons not pressed
      ; (in lieu of a call to getPressedButtons, because we
      ; don't want to destroy any registers)
      lw $v1,rawButtonsPressed
      nop
      andi $v1,button_r2
      ; bit set = not pressed, so do nothing
      bne $v1,$zero,@@doCapCheck
      nop
        ; button is pressed, so apply fast-forward
        ; multiply by 4
        sll $v0,2
      @@doCapCheck:
      
      ; make sure we don't travel further than the target distance
      
      ; v1 = current distance scrolled
      lhu $v1,advScroll_currentDistance_gp
      ; a3 = target total distance
      lhu $a3,advScroll_totalDistance_gp($gp)
      nop
      
      ; v1 = total - current
      subu $v1,$a3,$v1
      ; if our distance this frame exceeds that, cap it
      ble $v0,$v1,@@noCap
      nop
        move $v0,$v1
      @@noCap:
      
      jr $ra
      nop */
    
    ; v0 = reserved
    doAdvScrollFfCheck:
      ; make up work
      sb $v0, 0x801CC78E
      
      ; a0 = controller number
      ; (no need for multiplayer check --
      ; these scenes are only used in single player mode)
      jal getPressedButtons
      li $a0,1
      
      andi $v0,ffButton
      beq $v0,$zero,@@done
      lhu $v0,advScroll_totalDistance_gp($gp)
        ; fast-forward pressed:
        ; set pixels-per-iteration to difference of total distance and current
        lhu $v1,advScroll_currentDistance_gp($gp)
        nop
        
        subu $v0,$v1
        sh $v0,advScroll_pixelsPerIter_gp($gp)
      @@done:
      ; make up work (get direction)
      lh $v1, 0x124C($gp)
      j 0x801332D4
      nop
    
    ;===============================================
    ; cbc print start fast-forward
    ;===============================================

/*    doCbcPrintStartFfCheck:
      jal checkIfFfButtonPressed
      nop
      
      beq $v0,$zero,@@done
      la $v0,0x801CD410
        ; if ff on, clear cbc print flag to instantly print
        ; remaining text
        sw $zero,0($v0)
      @@done:
      ; make up work
      jal 0x80139C38
      nop
      j 0x80139E8C
      nop*/
    
    ;===============================================
    ; adv button prompt fast-forward
    ;===============================================
    
    doAdvButtonPromptFfCheck:
      ; make up work
      jal checkIfConfirmButtonTriggered
      nop
      
      ; return 1 if above call did
      bne $v0,$zero,@@done
      nop
      
      ; check if ff button pressed
/*      jal getPressedButtons
      li $a0,1
      
      andi $v0,ffButton
      beq $v0,$zero,@@done
      nop
        ; function expects a return value of exactly 1 (not just nonzero)
        li $v0,1
      @@done:
      j 0x8012F914
      nop */
      
        jal checkIfFfButtonPressed
        nop
      
      @@done:
      j 0x8012F914
      nop
    
    ; returns 0 if fast-forward button not pressed,
    ; 1 if pressed
    checkIfFfButtonPressed:
      subiu $sp,4
      sw $ra,0($sp)
      
        ; check if multiplayer on, and make sure the active player's
        ; controller is the one checked if so
        ; (could probably just directly read multiplayerCurrentControllerId,
        ; but this feels a little safer)
        lbu $v0,multiplayerFlag_gp($gp)
        li $a0,1
        beq $v0,$zero,@@notMultiplayer
        nop
          lbu $a0,multiplayerCurrentControllerId_gp($gp)
        @@notMultiplayer:
      
        ; get buttons
        jal getPressedButtons
        nop
        
        andi $v0,ffButton
        bne $v0,$zero,@@done_yes
        nop
        
        j @@done
        li $v0,0
        
        @@done_yes:
        li $v0,1
      @@done:
      lw $ra,0($sp)
      addiu $sp,4
      jr $ra
      nop
    
    checkIfFfButtonPressedAndAbilNotBlacklisted:
      subiu $sp,4
      sw $ra,0($sp)
        ; if ability is blacklisted, return zero (no skip allowed)
        lw $v0,currentAbilityIsBlacklisted
        nop
        bne $v0,$zero,@@done
        li $v0,0
        
        ; same if auto-flagged as unskippable
        lw $v0,currentAttackCannotBeSkipped
        nop
        bne $v0,$zero,@@done
        li $v0,0
        
        ; otherwise, return result from regular routine
        jal checkIfFfButtonPressed
        nop
      @@done:
      lw $ra,0($sp)
      addiu $sp,4
      jr $ra
      nop
      

    ;===============================================
    ; streamed sound wait ff check
    ;===============================================
    
    doStreamedSoundWaitFfCheck:
      ; check for ff button
      jal checkIfFfButtonPressed
      nop
      
      ; if not pressed, do normal checks
      beq $v0,$zero,@@done
      nop
        ; stop sound
        jal stopStreamedSound
        nop
        ; wait some frames
        ; (existing voice skip function does this -- appears to be important
        ; to avoid issues with sounds "stuttering" after being skipped.
        ; maybe in cases where game wants to play another sound immediately
        ; afterwards and un-mutes the volume before the sound has
        ; properly stopped? i didn't really look into it)
        jal waitFrame
        nop
        jal waitFrame
        nop
        jal waitFrame
        nop
        jal waitFrame
        nop
        
        ; otherwise, jump to end of function
;        j 0x8012B4AC
;        nop
        j 0x8012B4BC
        nop
      @@done:
      ; make up work
      li $v0,0x801CD440
      j 0x8012B488
      nop

    ;===============================================
    ; streamed sound start ff check
    ;===============================================
    
    doStreamedSoundStartFfCheck:
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      
      beq $v0,$zero,@@done
      nop
        ; if pressed, skip normal logic and return zero
        ; to indicate no playback
        j 0x8012AFC0
        li $v0,0
      @@done:
      ; make up work
      jal getConfigSetting
      li $a0, 1
      j 0x8012AF0C
      nop

    ;===============================================
    ; when about to use an ability, check if
    ; its effect is marked as no-skip
    ;===============================================
    
    ; a0-a3 = reserved
    ; s4 = ability id
    doAbilitySkipBlacklistCheck:
      la $v0,blacklistedAbilityArray
      sw $s4,currentBlastlistCheckAbility
      @@loop:
        lhu $v1,0($v0)
        addiu $v0,2
        ; done if 0xFFFF = terminator
        beq $v1,0xFFFF,@@done
        li $t0,0
        ; loop if this id doesn't match current ability
        bne $v1,$s4,@@loop
        li $t0,1
      @@done:
      sw $t0,currentAbilityIsBlacklisted
      
      ; make up work
      jal 0x8016E9E4
      nop
      j 0x8016E968
      nop
    
    blacklistedAbilityArray:
      .dh 0x07  ; teleport (polylina)
      .dh 0x1B  ; ocha tate (yoshika)
      .dh 0x35  ; crank in (miki)
;      .dh 0x37  ; ako comboable tech 1
;      .dh 0x3A  ; mako comboable tech 1
      .dh 0x40  ; teleport (misaki)
      ; FIXME: still skipped even if blacklisted,
      ; though the animation is correctly cleared out
      .dh 0x79  ; wave motion cannon
                ; (for the "mechanicannons" in the version of
                ; the jui-hua battle that takes place in front
                ; of the heavenfire missile)
      .dh 0x84  ; load wave motion cannon
                ; (for the "mechanicannons" in the version of
                ; the jui-hua battle that takes place in front
                ; of the heavenfire missile;
                ; this starts an animation which continues
                ; until the cannon fires on its next turn)
      .dh 0xFFFF
      .align 4
    
    currentAbilityIsBlacklisted:
      .dw 0
    currentBlastlistCheckAbility:
      .dw 0
    
    ;===============================================
    ; pre-attack graphics ff setup
    ;===============================================
    
    doPreAttackFfPrep:
      lw $v0,0x801CCBD8
      nop
      sw $v0,grpThingCountAtAttackStart
      
      ; make up work
      jal 0x8016F4D8
      nop
      
      j 0x8016F28C
      nop
    
    grpThingCountAtAttackStart:
      .dw 0
      
      .align 4
    
    ;===============================================
    ; post-attack graphics cleanup
    ;===============================================
    
    doPostAttackFfCleanup:
      subiu $sp,4
      sw $v0,0($sp)
        ; skip normal cleanup for certain blacklisted special attacks
        ; which deliberately leave residual animations
        lw $v1,currentAbilityIsBlacklisted
        lw $v0,currentBlastlistCheckAbility
        beq $v1,$zero,@@skipCheckDone
        nop
          ; attack 0x79 = mechanicannon "wave motion cannon"
          beq $v0,0x79,@@done
          nop
          ; attack 0x84 = mechanicannon "load wave motion cannon";
          ; starts an animation which continues until the cannon's
          ; next turn, when it fires
          beq $v0,0x84,@@done
          nop
        @@skipCheckDone:
        
        ; s0 = initial counter value at prep time
        lw $s0,grpThingCountAtAttackStart
        ; s1 = current counter value
        lw $s1,0x801CCBD8
        nop
        
        ; god, so i'm really playing with fire here and shouldn't be
        ; surprised by how that turns out...
        ; the game allocates some sort of animation slots
        ; at the beginning of an attack (and occasionally thereafter),
        ; keeping a count of how many are in use in 0x801CCBD8.
        ; normally, these are consumed in the action playback functions
        ; as they're needed, but since we're now sometimes bypassing those,
        ; we have to make sure the allocated resources get cleaned up.
        ; otherwise, the game will leak more and more slots before
        ; eventually running out, causing a lot of bad things to happen.
        ;
        ; i have not fully disassembled the logic here, but it seems that
        ; the function we're calling in this loop deallocates a slot.
        ; the idea here is that we note how many slots were in use when
        ; the attack started and forcibly deallocate any that were not
        ; in use. this will of course cause problems if there exists some
        ; attack that actually needs to allocate additional slots and have
        ; them remain, so i'm really hoping that never happens!
        @@loop:
          bge $s0,$s1,@@done
          nop
          
          ; ????????
          move $a0,$s0
          jal 0x801452B0
          li $a1,0
          
          j @@loop
          addiu $s0,1
        @@done:
      lw $v0,0($sp)
      addiu $sp,4
      
      ; make up work
      lw $ra, 0x00CC($sp)
      j 0x8016F4AC
      lw $fp, 0x00C8($sp)
    
    ;===============================================
    ; special attack (e.g. ako+mako) ff check
    ;===============================================
      
      doSpecialAttackNoSkipCheck:
        li $v0,1
        sw $v0,currentAttackCannotBeSkipped
        
        ; make up work
        addu $a0, $s3, $zero
        j 0x8016F0E4
        addu $a1, $s2, $zero
      
      resetSpecialAttackNoSkipCheck:
        sw $zero,currentAttackCannotBeSkipped
        
        ; make up work
        j 0x8016E9EC
        sw $s2, 0x00B0($sp)
        
      currentAttackCannotBeSkipped:
        .dw 0
    
    ;===============================================
    ; attack anim ff check
    ;===============================================
    
/*    doAttackAnimFfCheck:
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      
      beq $v0,$zero,@@done
      nop
        ; if pressed, stop animation
        j 0x80172B8C
        nop
      @@done:
      ; make up work
      lw $v1, 0($s4)
      j 0x80172B80
      nop */
      
    doAttackAnimFfCheck:
      subiu $sp,4
      sw $a0,0($sp)
        ; check if ff button pressed
        jal checkIfFfButtonPressedAndAbilNotBlacklisted
        nop
      lw $a0,0($sp)
      
      beq $v0,$zero,@@done
      addiu $sp,4
        ; if pressed, no animation
        li $v0,1
        sw $v0,lastAttackAnimWasSkipped
        j 0x801736E0
        li $v0,0
      @@done:
      ; make up work
      jal 0x8015F0B8
      addiu $s1, $zero, 0x0001
      sw $zero,lastAttackAnimWasSkipped
      j 0x80172418
      nop
      
    lastAttackAnimWasSkipped:
      .dw 0
      
      .align 4

    ;===============================================
    ; attack extras ff check
    ;===============================================
    
/*    attackLaserStuffFfCheck:
      ; check if initial attack was skipped
      lw $v0,lastAttackAnimWasSkipped
      nop
      ; if so, force this to be skipped as well
      bne $v0,$zero,@@skip
      nop
      
      ; check if ff button pressed
      jal checkIfFfButtonPressedAndAbilNotBlacklisted
      nop
      
      beq $v0,$zero,@@done
      nop
      @@skip:
        ; if ff pressed, skip
        j 0x80176E90
        li $v0,0
      @@done:
      ; make up work
      jal 0x8016235C
      addu $a0, $a0, $s0
      j 0x80173780
      nop */
    
    attackLaserStuffFfCheck:
      ; check if initial attack was skipped
      lw $v0,lastAttackAnimWasSkipped
      nop
      ; if so, force this to be skipped as well
      beq $v0,$zero,@@done
      nop
      @@skip:
        ; skip
        j 0x80176E90
        li $v0,0
      @@done:
      ; make up work
      jal 0x8016235C
      addu $a0, $a0, $s0
      j 0x80173780
      nop

    ;===============================================
    ; enemy reaction extras ff check
    ;===============================================
      
    enemyReactionExtraAnimFfCheck:
      lw $v0,lastAttackAnimWasSkipped
      nop
      bne $v0,$zero,@@done
      li $v0,0
        ; if attack anim not skipped, do animation here
        jal 0x80177D38
        nop
      @@done:
      j 0x80177C24
      nop
    
/*    enemyReactionExtraAnimFfCheck:
      subiu $sp,16
      sw $a0,0($sp)
      sw $a1,4($sp)
      sw $a2,8($sp)
      sw $a3,12($sp)
        ; check if ff button pressed
        jal checkIfFfButtonPressed
        nop
      lw $a0,0($sp)
      lw $a1,4($sp)
      lw $a2,8($sp)
      lw $a3,12($sp)
      addiu $sp,16 
      
      bne $v0,$zero,@@done
      li $v0,0
        ; if attack anim not skipped, do animation here
        jal 0x80177D38
        nop
      @@done:
      j 0x80177C24
      nop */

    ;===============================================
    ; counterattack ff check
    ;===============================================
    
/*    doCounterattackAnimFfCheck:
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      nop
        ; if ff pressed, skip anim wait loop
        j 0x8017EA98
        nop
      @@done:
      ; make up work
      jal 0x80154F2C
      nop
      j 0x8017EA68
      nop*/
    
/*    doCounterattackAnimFfCheck:
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      nop
        ; if ff pressed, skip anim wait loop
        j 0x8017EA98
        nop
      @@done:
      ; make up work
      jal 0x80154FA4
      addiu $a0, $zero, 0x0001
      j 0x8017EA54
      nop*/
    
    doCounterattackAnimFfCheck:
      subiu $sp,16
      sw $a0,0($sp)
      sw $a1,4($sp)
      sw $a2,8($sp)
      sw $a3,12($sp)
        ; check if ff button pressed
        jal checkIfFfButtonPressed
        nop
      lw $a0,0($sp)
      lw $a1,4($sp)
      lw $a2,8($sp)
      lw $a3,12($sp)
      addiu $sp,16
      
      beq $v0,$zero,@@done
      nop
        ; if ff pressed, skip animation stuff
        j 0x8017EB10
        nop
      @@done:
      ; make up work
      ; (alloc for and load data from COUNTER.YN3?)
      jal 0x8017F024
      nop
      j 0x8017E7DC
      nop

    ;===============================================
    ; counterattack extras ff check
    ;===============================================
    
/*    doCounterattackExtrasAnimFfCheck:
      ; make up work
      jal 0x80154FA4
      addiu $a0, $zero, 0x0001
      
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      nop
        ; if ff pressed, skip effect trigger code
        j 0x8017EB10
        nop
      @@done:
      j 0x8017EAD0
      nop */

    ;===============================================
    ; hp drain anim ff check
    ;===============================================
    
    doHpBarDrainAnimFfCheck:
      ; check if ff button pressed
/*      jal checkIfFfButtonPressed
      nop
      
      ; if ff pressed, continue iterating hp drain calculations
      ; without waiting for frame to display intermediate results
      bne $v0,$zero,@@done
      nop
        ; make up work
        jal waitFrame
        nop*/
      jal waitFrameIfFfNotPressed
      nop
      @@done:
      j 0x80185630
      nop
    
    ; call waitFrame if ff button not pressed
    waitFrameIfFfNotPressed:
      subiu $sp,4
      sw $ra,0($sp)
        ; check if ff button pressed
        jal checkIfFfButtonPressed
        nop
      
        ; if ff not pressed, wait frame
        bne $v0,$zero,@@done
        nop
          ; make up work
          jal waitFrame
          nop
      @@done:
      lw $ra,0($sp)
      addiu $sp,4
      jr $ra
      nop

    ;===============================================
    ; hp restore anim ff check
    ;===============================================
    
    doHpBarRestoreAnimFfCheck:
      ; check if ff button pressed
/*      jal checkIfFfButtonPressed
      nop
      
      ; if ff pressed, continue iterating
      ; without waiting for frame to display intermediate results
      bne $v0,$zero,@@done
      nop
        ; make up work
        jal waitFrame
        nop */
      jal waitFrameIfFfNotPressed
      nop
      @@done:
      j 0x801856D4
      nop

    ;===============================================
    ; post-change hp bar display ff check
    ;===============================================
      
    doPostHpDrainAnimFfCheck:
      ; make up work
      jal waitFrame
      addiu $s0, $s0, 0x0001
      
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      ; default number of frames to display modified hp bar after attack
      slti $v0, $s0, 60
        ; if ff pressed, display it for this many frames instead
        slti $v0, $s0, 60/2
      @@done:
      j 0x80185760
      nop

    ;===============================================
    ; ep drain anim ff check
    ;===============================================
    
    doEpBarDrainAnimFfCheck:
      ; check if ff button pressed
/*      jal checkIfFfButtonPressed
      nop
      
      ; if ff pressed, continue iterating ep drain calculations
      ; without waiting for frame to display intermediate results
      bne $v0,$zero,@@done
      nop
        ; make up work
        jal waitFrame
        nop */
      jal waitFrameIfFfNotPressed
      nop
      @@done:
      j 0x80185C28
      nop

    ;===============================================
    ; ep bar restore anim ff check
    ;===============================================
    
    doEpBarRestoreAnimFfCheck:
      ; check if ff button pressed
/*      jal checkIfFfButtonPressed
      nop
      
      ; if ff pressed, continue iterating
      ; without waiting for frame to display intermediate results
      bne $v0,$zero,@@done
      nop
        ; make up work
        jal waitFrame
        nop */
      jal waitFrameIfFfNotPressed
      nop
      @@done:
      j 0x80185CCC
      nop

    ;===============================================
    ; post-change ep bar display ff check
    ;===============================================
      
    doPostEpDrainAnimFfCheck:
      ; make up work
      jal waitFrame
      addiu $s0, $s0, 0x0001
      
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      ; default number of frames to display modified ep bar after attack
      slti $v0, $s0, 60
        ; if ff pressed, display it for this many frames instead
        slti $v0, $s0, 60/2
      @@done:
      j 0x80185D58
      nop

    ;===============================================
    ; healing(?) anim ff check
    ;===============================================
    
    doHealingAnimFfCheck:
      ; check if ff button pressed
      ; make up work
      sw $ra, 0x0040($sp)
      jal checkIfFfButtonPressed
      addu $s1, $zero, $zero
      
      ; make up work
      beq $v0,$zero,@@done
      lh $v0, 0x0C14($gp)
        ; if pressed, skip rest of function
        j 0x8017CD10
        li $v0,0
      @@done:
      j 0x8017CB98
      nop

    ;===============================================
    ; ep restore anim ff check
    ;===============================================
    
    doEpRestoreAnimFfCheck:
      ; check if ff button pressed
      ; make up work
      sw $ra, 0x0040($sp)
      jal checkIfFfButtonPressed
      addu $s1, $zero, $zero
      
      ; make up work
      beq $v0,$zero,@@done
      lh $v0, 0x0C14($gp)
        ; if pressed, skip rest of function
        j 0x8017C66C
        li $v0,0
      @@done:
      j 0x8017C540
      nop

    ;===============================================
    ; buff(?) anim ff check
    ;===============================================
    
    doBuffAnimFfCheck:
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      nop
        ; if pressed, skip rest of function
        j 0x8017CB64
        li $v0,0
      @@done:
      ; make up work
      jal waitFrame
      nop
      j 0x8017C964
      nop

    ;===============================================
    ; status effect heal anim ff check
    ;===============================================
    
    doStatusEffectHealAnimFfCheck:
      ; check if ff button pressed
      ; make up work
      sw $ra, 0x0040($sp)
      jal checkIfFfButtonPressed
      addu $s1, $zero, $zero
      
      beq $v0,$zero,@@done
      nop
        ; if pressed, skip rest of function
        j 0x8017CEB8
        li $v0,0
      @@done:
      j 0x8017CD40
      nop

    ;===============================================
    ; debuff(?) anim ff check
    ;===============================================
    
    doDebuffAnimFfCheck:
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      nop
        ; if pressed, skip rest of function
        j 0x8017D0A0
        li $v0,0
      @@done:
      ; make up work
      lw $a0,0x801B2F38
      jal 0x80132854
      li $a1,0
      j 0x8017CEF4
      nop

    ;===============================================
    ; explosion ff check
    ;===============================================
    
    doExplosionFfCheck:
      subiu $sp,16
      sw $a0,0($sp)
      sw $a1,4($sp)
      sw $a2,8($sp)
      sw $a3,12($sp)
        ; check if ff button pressed
        jal checkIfFfButtonPressed
        nop
      lw $a0,0($sp)
      lw $a1,4($sp)
      lw $a2,8($sp)
      lw $a3,12($sp)
      addiu $sp,16
      
      bne $v0,$zero,@@done
      nop
        ; if ff not on, do normal logic
        ; make up work
        jal 0x8017B978
        nop
        j 0x801794A0
        nop
      @@done:
      ; skip explosion setup + playback
      j 0x801795BC
      nop

    ;===============================================
    ; death blinkout ff check
    ;===============================================
    
    doDeathBlinkoutFfCheck:
      ; make up work
      jal 0x80154FA4
      addiu $s0, $s0, 0x0001
      
      ; check if ff button pressed
      jal checkIfFfButtonPressed
      nop
      beq $v0,$zero,@@done
      ; default number of frames to blink
      slti $v0, $s0, 61
        ; if ff pressed, blink this many frames instead
        slti $v0, $s0, 31
      @@done:
      j 0x80179698
      nop

    ;===============================================
    ; mission objective ff check
    ;===============================================
    
    doMissionObjectiveFfCheck:
      jal checkIfFfButtonPressed
      nop
      
      beq $v0,$zero,@@done
      nop
        ; ff pressed: break loop
        j 0x8012C844
        nop
      @@done:
      ; make up work
      sll $v0, $s1, 16
      sra $v0, $v0, 16
      j 0x8012C7C4
      slti $v0, $v0, 0x0079

    ;===============================================
    ; button trigger detection fix
    ;===============================================
    
    doButtonTriggerFix:
      subiu $sp,8
      sw $ra,0($sp)
      sw $s0,4($sp)
        ; check buttons triggered, purely so that the game will keep track
        ; of the actual state of the buttons
        jal getTriggeredButtons
        move $s0,$a0
      
        ; make up work
        jal getPressedButtons
        move $a0,$s0
      lw $ra,0($sp)
      lw $s0,4($sp)
      jr $ra
      addiu $sp,8

    ;===============================================
    ; talk.pic caching scheme
    ;===============================================
      
    talkPicIndexByteSize equ 0x2800
    talkPicByteSize equ 0xE9800
    talkPicSectorSize equ talkPicByteSize/0x800
    
    talkPicStrAddr equ 0x80112C40
    
    lastVoicePlayedFilenameBuffer equ 0x801CD478

      ;======
      ; 
      ;======
        
      allocForTalkPicIndexCache:
        li $a0,talkPicIndexByteSize
        jal allocHeap
        nop
        
        sw $v0,talkPicIndexPointer
        
        ; make up work
        la $a0,0x801CC7E8
        jal 0x8012CBD4
        nop
        j 0x8012CD6C
        nop
      
      talkPicIndexPointer:
        .dw 0
      talkPicIndexCached:
        .dw 0
      currentlyTargetingTalkPic:
        .dw 0

      ;======
      ; 
      ;======
      
      doTalkPicCacheNeededCheck:
        ; check if target file == TALK.PIC
        la $a0,talkPicStrAddr
        jal strcmp
        move $a1,$s1
        
        beq $v0,$zero,@@isTalkPic
        li $v1,1
          sw $zero,currentlyTargetingTalkPic
          ; if not TALK.PIC
          ; make up work
          jal allocHeap
          addu $a0, $s7, $zero
          j 0x80132A64
          nop
        @@isTalkPic:
        sw $v1,currentlyTargetingTalkPic
        ; return pointer to (possibly not yet cached) content
        lw $v0,talkPicIndexPointer
        j 0x80132A64
        nop
        ; already cached?
;        lw $v0,talkPicIndexCached
;        nop
;        bne $v0,$zero,@@alreadyCached
;        nop
;          
;        @@alreadyCached:

      ;======
      ; 
      ;======
      
      doTalkPicCacheLoadNeededCheck:
        lw $v0,currentlyTargetingTalkPic
        addu $s2, $zero, $zero
        beq $v0,$zero,@@makeUp
        la $t0,talkPicIndexCached
          ; if targeting talk.pic, and talk.pic is already cached,
          ; skip loading index here
          lw $v0,0($t0)
          li $v1,1
          ; flag talk.pic as cached no matter what
          ; (it will always be cached after this call)
          bne $v0,$zero,@@done
          sw $v1,0($t0)
        @@makeUp:
        ; make up work
        ; prep for index load
        jal 0x80132644
        nop
        ; wait for index load
        jal 0x80132914
        nop
        
        @@done:
        j 0x80132AC0
        nop

      ;======
      ; 
      ;======
      
      freeIfNotTalkPic:
        subiu $sp,4
        sw $ra,0($sp)
          lw $v0,currentlyTargetingTalkPic
          nop
          bne $v0,$zero,@@noFree
          nop
            jal freeHeap
            nop
          @@noFree:
        lw $ra,0($sp)
        addiu $sp,4
        
        jr $ra
        nop

    ;===============================================
    ; 
    ;===============================================
    
    doLookingUpTalkPicCheck:
      ; make up work
      ; save prepped filename buffer to s1
      jal 0x80131EF4
      nop
      addu $s1, $v0, $zero
      
      ; check if file being targeted is TALK.PIC
      la $a0,talkPicStrAddr
      jal strcmp
      move $a1,$s1
      
      sw.u $zero,isLookingUpTalkPic
;      bne $v0,$zero,@@done
      beq $v0,$zero,@@targetingTalkPic
      sw.l $zero,isLookingUpTalkPic
      @@notTargetingTalkPic:
        ; flag most recent disc access as not xa
        ; (only if NOT talkpic, so that if multiple talkpic images are
        ; accessed sequentially right after a voice file, the same instance
        ; of talkpic will continue to be targeted for each load)
        ; ...oh, and actually, this function is also called when looking up the
        ; xa files themselves, so this doesn't quite work as originally intended.
        ; but the xa playback function itself now sets lastDiscAccessWasXa,
        ; so the ultimate result is what we want.
        sw.u $zero,lastDiscAccessWasXa
        j @@done
        sw.l $zero,lastDiscAccessWasXa
      @@targetingTalkPic:
        ; do not do talkpic redirect if last disc access was not xa playback
        ; (so that the normal version of the file listed in the index will be
        ; used instead)
        lw $v0,lastDiscAccessWasXa
        nop
        beq $v0,$zero,@@done
        nop
          ; targeting TALK.PIC
          ; ensure last voice file accessed is not null
          la $a1,lastVoicePlayedFilenameBuffer
          lbu $v0,0($a1)
          li $v1,1
          beq $v0,$zero,@@done
          move $a0,$s1
            ; copy over "TALK.PIC" with name of most recently accessed voice file
            sw.u $v1,isLookingUpTalkPic
            jal strcpy
            sw.l $v1,isLookingUpTalkPic
      @@done:
      j 0x80131FA8
      nop
    
    lastDiscAccessWasXa:
      .dw 0
    isLookingUpTalkPic:
      .dw 0
    
    doLookingUpTalkPicSubstitutionCheck:
      lw $v0,isLookingUpTalkPic
      addu $a1, $s0, $zero
      beq $v0,$zero,@@done
      ; add filesize of voice file to get location of nearest TALK.PIC
      lw $v0,4($s0)
        ; looking up talkpic
        
        ; subtract size of talkpic from sector position of the voice file that
        ; we previously made this function look up instead of talkpic
;        subiu $a0,talkPicSectorSize
;        ; make up work
;        jal sectorNumToBcdMsf
;        nop
        
        nop
        ; divide byte size by 0x800 to get sector size
        ; (we assume the byte size is divisible by 0x800;
        ; this should always be the case for the voice files)
        srl $v0,11
        ; make up work
        jal sectorNumToBcdMsf
        addu $a0,$v0
        
        ; copy over filename with TALK.PIC
        la $a1,talkPicStrAddr
        jal strcpy
        addiu $a0,$s0,8
        
        ; copy over size with what we know the size to be
        li $v0,talkPicByteSize
        j @@end
        sw $v0,4($s0)
      @@done:
      ; make up work
      jal sectorNumToBcdMsf
      nop
      
      @@end:
      j 0x8013204C
      nop
      
  .endarea
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
/*;============================================================================
; NEW STUFF (speedup hack)
;============================================================================
   
  .org newCodeStartSection
  .area newCodeEndSection-.,0xFF
  
    .align 4
    
    
    
    

    ;============================================================================
    ; pad to end of sector
    ;============================================================================
    
    .align 0x800
      
  .endarea*/
  
.close

;============================================================================
; other files to include
;============================================================================

.include "asm/LONGMAP.BIN.asm"
.include "asm/OMAKE.BIN.asm"
.include "asm/BMS096.BIN.asm"
.include "asm/ALBUM.BIN.asm"

