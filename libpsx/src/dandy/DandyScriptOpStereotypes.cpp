#include "dandy/DandyScriptOpStereotypes.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Psx {


// old table from before i discovered the game itself contained
// most of the argument information so it could skip over
// conditional blocks
/*const static DandyScriptOpStereotype opStereotypeList[] = {
  // maybe??
  DandyScriptOpStereotype(0x000, "nop?_op000", "x"),
  DandyScriptOpStereotype(0x001, "dlog", "T"),
  // param = index number of label within script label block
  DandyScriptOpStereotype(0x002, "jumpToLabelByIndex", "NI"),
  // ???
  DandyScriptOpStereotype(0x003, "op003", "L1 L1 L1"),
  // possibly increments a script variable of the given index.
  // or something completely different.
  DandyScriptOpStereotype(0x004, "op004", "L1"),
  // not checked in-game
  // wait for something??
  DandyScriptOpStereotype(0x005, "op005", "x"),
  // THIS DOES A CONDITIONAL BRANCH
  // stupidly complicated, look into this
  // the possible branch immediately follows the instruction
  // and is terminated with a "00 00"????
  DandyScriptOpStereotype(0x006, "CONDITIONAL?", "L1 L1 L1 L1 L1 L1 L1 L1 L1"),
  // ??????????
  // used near an op006 conditional branch...
  DandyScriptOpStereotype(0x007, "op007", "L1"),
  // loading wait? synchronous?
  DandyScriptOpStereotype(0x008, "op008", "x"),
  // ??? not checked in-game but this is probably close enough
  // string param = e.g. "GUARD"
  DandyScriptOpStereotype(0x009, "op009", "L2 L2 S L2 L2"),
  DandyScriptOpStereotype(0x00a, "op00a", ""),
  DandyScriptOpStereotype(0x00b, "op00b", ""),
  DandyScriptOpStereotype(0x00c, "openTextBox", "x"),
  DandyScriptOpStereotype(0x00d, "closeTextBox", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x00e, "op00e", "L2"),
  // ?
  DandyScriptOpStereotype(0x00f, "op00f", "L1 L2 L2 L2"),
  DandyScriptOpStereotype(0x010, "op010", "L1"),
  DandyScriptOpStereotype(0x011, "op011", ""),
  DandyScriptOpStereotype(0x012, "op012", ""),
  DandyScriptOpStereotype(0x013, "op013", ""),
  DandyScriptOpStereotype(0x014, "op014", "x"),
  DandyScriptOpStereotype(0x015, "op015", "L1 L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x016, "op016", "L1"),
  // ??
  DandyScriptOpStereotype(0x017, "op017", "L2 L2 L2 L2"),
  // ??
  DandyScriptOpStereotype(0x018, "op018", "L2 L2 L2 L2"),
  // this can apparently trigger/wait for fade-in??
  // or just a general "delay" op?
  DandyScriptOpStereotype(0x019, "delay", "L2"),
  // definitely ends processing current script -- scriptpos is not even advanced
  DandyScriptOpStereotype(0x01a, "stopScript?", "x"),
  DandyScriptOpStereotype(0x01b, "op01b", "x"),
  DandyScriptOpStereotype(0x01c, "op01c", "x"),
  // ???
  DandyScriptOpStereotype(0x01d, "op01d", "L1 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x01e, "op01e", "L2"),
  DandyScriptOpStereotype(0x01f, "op01f", "x"),
  DandyScriptOpStereotype(0x020, "op020", ""),
  // ???
  DandyScriptOpStereotype(0x021, "op021", "L1 L1 L1 L1 L1 L1"),
  // ????????
  DandyScriptOpStereotype(0x022, "op022", "L1 L2 L2 L2 L2 L1 L2 L2"),
  // ??????
  // not checked in-game
  DandyScriptOpStereotype(0x023, "op023", "L2 L2 S L2 L2 L1"),
  // ????
  // CHECK LAST PARAM
  // not checked in-game
  DandyScriptOpStereotype(0x024, "op024", "L2 L2 S L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x025, "op025", "L1"),
  // ?
  DandyScriptOpStereotype(0x026, "op026", "L2 L2 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x027, "op027", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x028, "op028", "L1"),
  // ??
  DandyScriptOpStereotype(0x029, "op029", "L1 L1"),
  // not checked in-game
  // string param = e.g. "G13_SP_3"
  DandyScriptOpStereotype(0x02a, "op02a", "L2 L2 S L2 L2 L2 L2"),
  // ?
  // string param = e.g. "G_FAIL1"
  DandyScriptOpStereotype(0x02b, "op02b", "L2 L2 S L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x02c, "messageInMenus???", "T"),
  DandyScriptOpStereotype(0x02d, "op02d", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x02e, "op02e", "x"),
  // ???
  // string param = e.g. "MAMO"
  DandyScriptOpStereotype(0x02f, "op02f", "L1 S L1 L1"),
  // ??
  DandyScriptOpStereotype(0x030, "op030", "L1 L1"),
  DandyScriptOpStereotype(0x031, "op031", "x"),
  // loop until ?
  DandyScriptOpStereotype(0x032, "waitFor?_032", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x033, "op033", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x034, "op034", "L2"),
  // ?
  DandyScriptOpStereotype(0x035, "op035", "L2 L2 L2 L2"),
  // ????
  // string param = e.g. "PUNCH_L"
  DandyScriptOpStereotype(0x036, "op036", "L2 L2 S L1 L2 L2 L2 L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x037, "op037", "L2 L2 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x038, "op038", "x"),
  // ?
  DandyScriptOpStereotype(0x039, "op039", "L1 L1"),
  DandyScriptOpStereotype(0x03a, "op03a", "L1"),
  // ???
  DandyScriptOpStereotype(0x03b, "op03b", "L1 L1 L1 L1"),
  DandyScriptOpStereotype(0x03c, "op03c", "L1"),
  // ?
  // string param = e.g. "BIL_CRASH1"
//  DandyScriptOpStereotype(0x03d, "op03d", "L2 L2 S L1"),
  DandyScriptOpStereotype(0x03d, "op03d", "L2 L2 S S"),
  // not checked in-game
  DandyScriptOpStereotype(0x03e, "op03e", "L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x03f, "op03f", "L1"),
  // ?
  DandyScriptOpStereotype(0x040, "op040", "L1 S"),
  DandyScriptOpStereotype(0x041, "op041", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x042, "op042", "L2"),
  // ? loop until ?
  DandyScriptOpStereotype(0x043, "waitFor?_043", "x"),
  // ??
  DandyScriptOpStereotype(0x044, "op044", "L1"),
  // ???
  DandyScriptOpStereotype(0x045, "op045", "L2 L2 L1 L1 L1 L1 L1 L1 L1"),
  // long synchronous load?
  DandyScriptOpStereotype(0x046, "op046", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x047, "op047", "L2 L2 S L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x048, "op048", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x049, "op049", "L1"),
  DandyScriptOpStereotype(0x04a, "op04a", ""),
  // ?
  // this one takes a long time just for executing the actual op.
  // synchronous loading/wait?
  DandyScriptOpStereotype(0x04b, "op04b", "L1"),
  DandyScriptOpStereotype(0x04c, "op04c", ""),
  // ? long...
  DandyScriptOpStereotype(0x04d, "op04d", "L1 L1"),
  DandyScriptOpStereotype(0x04e, "op04e", ""),
  // ??????????????????
  // no idea
  DandyScriptOpStereotype(0x04f, "op04f", "L2 L2 L2 L2 L2 L2 L2 L2 L2 L2 L2 L2"),
  // ?????
  // CHECK THIS
  // not checked in-game
  DandyScriptOpStereotype(0x050, "op050", "L2 L2 S L2 L2 L2 L2 L1 L1 L1 L1"),
  // loop until ?
  // e.g. mission preview map confirmed
  DandyScriptOpStereotype(0x051, "waitFor?_051", "x"),
  // ?
  DandyScriptOpStereotype(0x052, "op052", "L1 L1"),
  DandyScriptOpStereotype(0x053, "op053", ""),
  // ? maybe fade??
  // something + frame count?
  // maybe param 1 = fade to normal scene if 0000, solid color otherwise?
  DandyScriptOpStereotype(0x054, "op054", "L2 L2"),
  DandyScriptOpStereotype(0x055, "op055", "L1"),
  DandyScriptOpStereotype(0x056, "op056", "x"),
  // ???
  DandyScriptOpStereotype(0x057, "op057", "L2 L2 L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x058, "op058", "L2"),
  DandyScriptOpStereotype(0x059, "op059", "x"),
  // param 1 = dst base?
  // param 2 = dst offset?
  // param 3 = characters to send
  DandyScriptOpStereotype(0x05a, "charSequenceToVram", "L2 L2 T"),
  DandyScriptOpStereotype(0x05b, "vramSeqToScreen?", "L1 L1 L1 L1 L1 L1 L1 L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x05c, "op05c", "L1"),
  // ???
  DandyScriptOpStereotype(0x05d, "op05d", "L1 L1 L1 L1 L1 L1 L1 L1 L1"),
  DandyScriptOpStereotype(0x05e, "op05e", "L1"),
  // ??
  DandyScriptOpStereotype(0x05f, "op05f", "L1 L1 L1"),
  DandyScriptOpStereotype(0x060, "op060", "x"),
  DandyScriptOpStereotype(0x061, "op061", "x"),
  DandyScriptOpStereotype(0x062, "op062", "x"),
  DandyScriptOpStereotype(0x063, "op063", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x064, "op064", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x065, "op065", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x066, "op066", "L1"),
  DandyScriptOpStereotype(0x067, "op067", "L1"),
  DandyScriptOpStereotype(0x068, "op068", ""),
  // can take time...
  DandyScriptOpStereotype(0x069, "op069", "L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x06a, "op06a", "L1"),
  DandyScriptOpStereotype(0x06b, "setCamera?", "S"),
  DandyScriptOpStereotype(0x06c, "op06c", "L1"),
  // ?
  // string param = e.g. "C001"
  DandyScriptOpStereotype(0x06d, "op06d", "S L2 L2"),
  // ?
  // string param = e.g. "RL01"
  DandyScriptOpStereotype(0x06e, "op06e", "L2 L2 S L2 L2"),
  // ?
  // string param = e.g. "ENEM"
  DandyScriptOpStereotype(0x06f, "op06f", "L2 L2 L2 L2 S"),
  DandyScriptOpStereotype(0x070, "op070", "x"),
  // loops until ??
  DandyScriptOpStereotype(0x071, "waitFor?_071", "L2 L2"),
  DandyScriptOpStereotype(0x072, "op072", "x"),
  DandyScriptOpStereotype(0x073, "op073", "x"),
  // ?
  DandyScriptOpStereotype(0x074, "op074", "L2 L2 L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x075, "op075", ""),
  // this will loop until ???
  DandyScriptOpStereotype(0x076, "waitFor?_076", "L2 L2"),
  DandyScriptOpStereotype(0x077, "op077", ""),
  DandyScriptOpStereotype(0x078, "op078", ""),
  DandyScriptOpStereotype(0x079, "setPortrait", "L1"),
  DandyScriptOpStereotype(0x07a, "setBodyPortait??", "L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x07b, "op07b", "L2 L2 L2 L2"),
  // ?
  // string param = e.g. "SELE"
  DandyScriptOpStereotype(0x07c, "op07c", "L2 L2 S"),
  // can take time...
  DandyScriptOpStereotype(0x07d, "op07d", "L1 L1"),
  // ?
  DandyScriptOpStereotype(0x07e, "op07e", "L1 L1 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x07f, "op07f", "L2"),
  DandyScriptOpStereotype(0x080, "op080", "L1"),
  // ???
  // letters appearing on title cards...?
  // maybe coordinate positions somewhere in here?
  DandyScriptOpStereotype(0x081, "op081", "L1 L1 L1 L1 L1 L1 L1 L1 L1"),
  // ?
  // fade??
  DandyScriptOpStereotype(0x082, "op082", "L1"),
  DandyScriptOpStereotype(0x083, "op083", "x"),
  // string param = e.g. "MUSIC.XAP"
  // additional params are a complete guess...
  DandyScriptOpStereotype(0x084, "op084", "S L2 L2 L2 L2 L2 L2 L1 L1 L1"),
  DandyScriptOpStereotype(0x085, "op085", "x"),
  DandyScriptOpStereotype(0x086, "op086", "L1"),
  DandyScriptOpStereotype(0x087, "op087", "x"),
  // sets up a sound to be triggered from text string with /v command
  // (param 1 == id number for use in string?)
  // string param = e.g. "VOICE.XAP"
  // additional params are a complete guess...
  DandyScriptOpStereotype(0x088, "setUpVoice", "L1 S L2 L2 L2 L2 L1"),
  DandyScriptOpStereotype(0x089, "waitForFileLoad", "x"),
  // ? loading?
  DandyScriptOpStereotype(0x08a, "op08a", "L1 L1"),
  // ?
  DandyScriptOpStereotype(0x08b, "op08b", "L1 L1"),
  DandyScriptOpStereotype(0x08c, "op08c", "L1"),
  DandyScriptOpStereotype(0x08d, "op08d", "L1"),
  // ?
  DandyScriptOpStereotype(0x08e, "op08e", "L1 L1"),
  DandyScriptOpStereotype(0x08f, "op08f", "L1"),
  // ??
  DandyScriptOpStereotype(0x090, "op090", "L2 L2 L2 L2 L2 L2 L2 L2 L2 L2 L2 L2 L2 L2 L1"),
  DandyScriptOpStereotype(0x091, "op091", "L1"),
  DandyScriptOpStereotype(0x092, "op092", "L1"),
  // ??? wait? for ??
  DandyScriptOpStereotype(0x093, "op093", "x"),
  DandyScriptOpStereotype(0x094, "op094", "x"),
  // string param = e.g. "MAMO"
  DandyScriptOpStereotype(0x095, "op095", "S"),
  DandyScriptOpStereotype(0x096, "op096", "x"),
  DandyScriptOpStereotype(0x097, "op097", "x"),
  DandyScriptOpStereotype(0x098, "op098", "x"),
  // loop until "mission failed" sequence completes?
  DandyScriptOpStereotype(0x099, "waitFor?_099", "x"),
  DandyScriptOpStereotype(0x09a, "op09a", "x"),
  // synchronous load??
  DandyScriptOpStereotype(0x09b, "op09b", "x"),
  DandyScriptOpStereotype(0x09c, "op09c", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x09d, "op09d", "L2 L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x09e, "op09e", "L2 L2"),
  DandyScriptOpStereotype(0x09f, "op09f", ""),
  // guessing...
  // string param = e.g. "G_02_02"
  DandyScriptOpStereotype(0x0a0, "op0a0", "L2 L2 S L1"),
  DandyScriptOpStereotype(0x0a1, "op0a1", "L1"),
  // NOTE: assigned same handler function as op B7
  DandyScriptOpStereotype(0x0a2, "op0a2", "L1 L1"),
  // ??
  DandyScriptOpStereotype(0x0a3, "op0a3", "L2"),
  // not checked in-game
  // string params = e.g. "GEN_CRASH", "HATU"
  DandyScriptOpStereotype(0x0a4, "op0a4", "L2 L2 S S"),
  DandyScriptOpStereotype(0x0a5, "op0a5", ""),
  // guessing...
  // string param = e.g. "BAD_END"
  DandyScriptOpStereotype(0x0a6, "op0a6", "L2 L2 S L2 L2"),
  // ?
  // string param = e.g. "BAD_END"
  DandyScriptOpStereotype(0x0a7, "op0a7", "L2 L2 S"),
  // ?
  // string param = e.g. "FOOT_FORWARE"
  DandyScriptOpStereotype(0x0a8, "op0a8", "L2 L2 S L1 L2 L2 L2 L2"),
  // ?
  // string param = e.g. "WALK"
  DandyScriptOpStereotype(0x0a9, "op0a9", "L2 L2 S"),
  DandyScriptOpStereotype(0x0aa, "op0aa", ""),
  DandyScriptOpStereotype(0x0ab, "op0ab", ""),
  DandyScriptOpStereotype(0x0ac, "op0ac", ""),
  DandyScriptOpStereotype(0x0ad, "op0ad", ""),
  DandyScriptOpStereotype(0x0ae, "op0ae", "x"),
  DandyScriptOpStereotype(0x0af, "op0af", "L1"),
  DandyScriptOpStereotype(0x0b0, "op0b0", "x"),
  DandyScriptOpStereotype(0x0b1, "op0b1", "L1"),
  DandyScriptOpStereotype(0x0b2, "op0b2", ""),
  // not checked in-game
  // terminator????
  DandyScriptOpStereotype(0x0b3, "op0b3", "x"),
  DandyScriptOpStereotype(0x0b4, "op0b4", "x"),
  // ???
  DandyScriptOpStereotype(0x0b5, "op0b5", "L2 L2 L2 L2 L2 L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x0b6, "op0b6", "L1 L2"),
  // NOTE: assigned same handler function as op A2
  DandyScriptOpStereotype(0x0b7, "op0b7", "L1 L1"),
  // ??
  DandyScriptOpStereotype(0x0b8, "op0b8", "L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x0b9, "op0b9", "L1 L2"),
  // ???
  DandyScriptOpStereotype(0x0ba, "op0ba", "L1 L1 L1 L1 L1 L1 L1 L1"),
  // loop until ?
  DandyScriptOpStereotype(0x0bb, "waitForAnimation?", "L1"),
  // ?
  DandyScriptOpStereotype(0x0bc, "op0bc", "L1 L1"),
  // ?????
  // not checked in-game
  DandyScriptOpStereotype(0x0bd, "op0bd", "L1"),
  DandyScriptOpStereotype(0x0be, "queueLoadThing?Img??", "L1 L1"),
  DandyScriptOpStereotype(0x0bf, "op0bf", "L1"),
  DandyScriptOpStereotype(0x0c0, "op0c0", "L1"),
  // body portait vanish??
  DandyScriptOpStereotype(0x0c1, "op0c1", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0c2, "op0c2", "L2"),
  // guess...
  DandyScriptOpStereotype(0x0c3, "op0c3", "L1 L1 L1 L1"),
  // guess...
  DandyScriptOpStereotype(0x0c4, "op0c4", "L1 L1 L1 L1"),
  // ???
  DandyScriptOpStereotype(0x0c5, "op0c5", "L1 L1 L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0c6, "op0c6", "x"),
  DandyScriptOpStereotype(0x0c7, "op0c7", "x"),
  DandyScriptOpStereotype(0x0c8, "op0c8", "x"),
  DandyScriptOpStereotype(0x0c9, "op0c9", "L1"),
  DandyScriptOpStereotype(0x0ca, "op0ca", ""),
  // ??????
  // not checked in-game
  DandyScriptOpStereotype(0x0cb, "op0cb", "L1 L1 L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x0cc, "op0cc", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x0cd, "op0cd", "L2 L2"),
  // ???
  // string param = e.g. "CHANGE_A" (which is a script label name)
  // queue script for future execution??
  DandyScriptOpStereotype(0x0ce, "op0ce", "S L1 L1 L1 L1"),
  // ?
  // string param = e.g. "F_03_05"
  DandyScriptOpStereotype(0x0cf, "op0cf", "L2 L2 S"),
  // ?
  // string param = e.g. "FLAG_CLEAR"
  DandyScriptOpStereotype(0x0d0, "op0d0", "L2 L2 S"),
  // ?????????????????????????????????????????????????????
  // CHECK THIS
  // not checked in-game
  // string param = e.g. "BL3"
  DandyScriptOpStereotype(0x0d1, "op0d1", "S L2 L2"),
  // ?????
  // string param = e.g. "MIYU"
  // not checked in-game
  DandyScriptOpStereotype(0x0d2, "op0d2", "S L1 L1 L1 L1 L1 L1 L1"),
  DandyScriptOpStereotype(0x0d3, "op0d3", ""),
  // ?
  // string param = e.g. "SP_4"
  DandyScriptOpStereotype(0x0d4, "op0d4", "L2 L2 S L2 L2"),
  DandyScriptOpStereotype(0x0d5, "op0d5", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x0d6, "op0d6", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0d7, "op0d7", "L2 L2"),
  // ?
  DandyScriptOpStereotype(0x0d8, "op0d8", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x0d9, "op0d9", "L2 L2 L2 L2"),
  // not checked in-game
  // string param = e.g. "SCHL"
  DandyScriptOpStereotype(0x0da, "op0da", "L2 L2 S L2 L2"),
  // ??
  DandyScriptOpStereotype(0x0db, "op0db", "L2 L2"),
  // not checked in-game
  // string param = e.g. "EL02"
  DandyScriptOpStereotype(0x0dc, "op0dc", "L2 L2 S"),
  // ?
  DandyScriptOpStereotype(0x0dd, "op0dd", "L2 L2 L2 L2"),
  // ??
  DandyScriptOpStereotype(0x0de, "op0de", "L2 L2 L2 L2 L2 L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x0df, "op0df", "L2 L2 L2 L2"),
  // ???
  // string param = e.g. "A000"
  DandyScriptOpStereotype(0x0e0, "op0e0", "L1 S"),
  // ???
  // string param = e.g. "A_00_02"
  DandyScriptOpStereotype(0x0e1, "op0e1", "L2 L2 S L1 L2 L2"),
  // not checked in-game
  // CHECK LAST PARAM
  DandyScriptOpStereotype(0x0e2, "op0e2", "L2 L2 S L1"),
  // ?
  DandyScriptOpStereotype(0x0e3, "op0e3", "L2 L2 L2 L2"),
  // ?????
  // CHECK THIS
  // not checked in-game
  // string param = e.g. "SAHA_WAIT"
  DandyScriptOpStereotype(0x0e4, "op0e4", "L2 L2 S L2 L2"),
  DandyScriptOpStereotype(0x0e5, "op0e5", "L2 L2"),
  DandyScriptOpStereotype(0x0e6, "op0e6", "L2 L2 S"),
  // ?
  DandyScriptOpStereotype(0x0e7, "op0e7", "L2 L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x0e8, "op0e8", "L2 L2 L2 L2"),
  // ???
  DandyScriptOpStereotype(0x0e9, "op0e9", "L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x0ea, "op0ea", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0eb, "op0eb", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ec, "op0ec", "x"),
  // not checked in-game
  // string param = e.g. "SINKOU.S3M"
  DandyScriptOpStereotype(0x0ed, "loadOverlay?", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ee, "op0ee", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ef, "op0ef", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f0, "op0f0", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f1, "op0f1", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f2, "op0f2", "L2 L2"),
  DandyScriptOpStereotype(0x0f3, "op0f3", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f4, "op0f4", "L2 L2 S"),
  DandyScriptOpStereotype(0x0f5, "op0f5", "x"),
  // ???
  DandyScriptOpStereotype(0x0f6, "op0f6", "L2 L2"),
  // ??
  // used before a conditional, with parameter of 1000...
  // a value to check against??
  DandyScriptOpStereotype(0x0f7, "op0f7", "L4"),
  DandyScriptOpStereotype(0x0f8, "op0f8", ""),
  // ???
  // total guess...
  DandyScriptOpStereotype(0x0f9, "op0f9", "L1 L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fa, "op0fa", "L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fb, "op0fb", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fc, "op0fc", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fd, "op0fd", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fe, "op0fe", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ff, "op0ff", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x100, "op100", "x"),
  // ?????????
  // not checked in-game
  // string param = e.g. "VOICE.XAP"
  DandyScriptOpStereotype(0x101, "op101", "S L2 L2 L2 L2 L2 L2 L1 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x102, "op102", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x103, "op103", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x104, "op104", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x105, "op105", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x106, "op106", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x107, "op107", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x108, "op108", "x"),
  // ?????
  // not checked in-game
  DandyScriptOpStereotype(0x109, "op109", "L1 L1 L1 L1 L1"),
  DandyScriptOpStereotype(0x10a, "op10a", "x"),
  DandyScriptOpStereotype(0x10b, "op10b", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x10c, "op10c", "L2 L2 S L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x10d, "op10d", "L2 L2 S L2 L2"),
  DandyScriptOpStereotype(0x10e, "op10e", ""),
  DandyScriptOpStereotype(0x10f, "op10f", ""),
  // loading?...
  DandyScriptOpStereotype(0x110, "op110", "x"),
  // ?
  // string param = e.g. "BIL_CRASH1"
//  DandyScriptOpStereotype(0x111, "op111", "L2 L2 S L1"),
  DandyScriptOpStereotype(0x111, "op111", "L2 L2 S S"),
  // not checked in-game
  DandyScriptOpStereotype(0x112, "op112", "L2 L2 S L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x113, "op113", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x114, "op114", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x115, "op115", "L2 L2"),
  // guess...
  DandyScriptOpStereotype(0x116, "op116", "L1 L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x117, "op117", "L2 L2 L2 L2"),
  // not checked in-game
  // string param = e.g. "JET_DASH1"
  DandyScriptOpStereotype(0x118, "op118", "L2 L2 S L2 L2"),
  // ?
  DandyScriptOpStereotype(0x119, "op119", "L2 L2"),
  // ???
  DandyScriptOpStereotype(0x11a, "op11a", "L1 L1 L1 L2 L2"),
  // loop until ?
  DandyScriptOpStereotype(0x11b, "waitFor?_11b", "L1"),
  DandyScriptOpStereotype(0x11c, "op11c", ""),
  DandyScriptOpStereotype(0x11d, "op11d", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x11e, "op11e", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x11f, "op11f", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x120, "op120", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x121, "op121", "L2 L2 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x122, "op122", "L2 L2 L2 L2 L2 L2 L2 L2"),
  // not checked in-game
  // function pointer is 800B54BC,
  // which is not in main exe, ADV, GAME, SINKOU, or SONIC.AOE-151
  DandyScriptOpStereotype(0x123, "op123", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x124, "op124", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x125, "op125", "L2 L2 L2 L2 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x126, "op126", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x127, "op127", "L2 L2 L2 L2 S"),
  DandyScriptOpStereotype(0x128, "op128", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x129, "op129", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x12a, "op12a", "L2 L2 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x12b, "op12b", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x12c, "op12c", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x12d, "op12d", "x"),
  // ?
  // appearance of text overlay in intro...?
  DandyScriptOpStereotype(0x12e, "op12e", "L2 L2 L2 L2"),
  // disappearance of text overlay in intro...?
  DandyScriptOpStereotype(0x12f, "op12f", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x130, "op130", "L2 L2"),
  // not checked in-game
  // string param = e.g. "P_28_01"
  DandyScriptOpStereotype(0x131, "op131", "L2 L2 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x132, "op132", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x133, "op133", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x134, "op134", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x135, "op135", "L2 L2 L2 L2 S"),
  DandyScriptOpStereotype(0x136, "op136", ""),
  DandyScriptOpStereotype(0x137, "op137", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x138, "op138", "L2 L2 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x139, "op139", "L2 L2 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x13a, "op13a", "L2 L2 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x13b, "op13b", "L2 L2 L2 L2 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x13c, "op13c", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x13d, "op13d", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x13e, "op13e", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x13f, "op13f", "L2 L2"),
  DandyScriptOpStereotype(0x140, "op140", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x141, "op141", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x142, "op142", "L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x143, "op143", "L2 L2 S"),
  // ?????
  // not checked in-game
  DandyScriptOpStereotype(0x144, "op144", "L2 L2 L2 L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x145, "op145", "L2 L2 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x146, "op146", "L2 L2"),
  // ?
  DandyScriptOpStereotype(0x147, "op147", "L2 L2"),
  DandyScriptOpStereotype(0x148, "op148", ""),
  // end? wait? load?
  // this is the last op that runs before the
  // "now loading" screen; script execution
  // resumes from where it left off after that finishes
  DandyScriptOpStereotype(0x149, "end??", "x"),
  DandyScriptOpStereotype(0x14a, "op14a", ""),
  DandyScriptOpStereotype(0x14b, "op14b", ""),
  // not checked in-game
  DandyScriptOpStereotype(0x14c, "op14c", "x"),
  // not checked in-game
  // CHECK THIS
//  DandyScriptOpStereotype(0x14d, "op14d", "L2 L2"),
  DandyScriptOpStereotype(0x14d, "op14d", "L2 L2 L2 L2 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x14e, "op14e", "x"),
  // guessing...
  DandyScriptOpStereotype(0x14f, "op14f", "L1 L1 L1 L1"),
  // ??????????
  // not checked in-game
  DandyScriptOpStereotype(0x150, "op150", "L2 L2 L2 L2 L2 L2 L1")
}; */

const static DandyScriptOpStereotype opStereotypeList[] = {
  // acts as nop.
  // not really used -- this is a placeholder value
  // to mark the end of conditional blocks.
  DandyScriptOpStereotype(0x000, "ENDBLOCK", "", "x"),
  DandyScriptOpStereotype(0x001, "dlog", "", "T"),
  // param = index number of label within script label block
  DandyScriptOpStereotype(0x002, "jumpToLabel", "jumpToLabelByIndex", "NI"),
  // ???
  DandyScriptOpStereotype(0x003, "op003", "", "L1 L1 L1"),
  // possibly increments a script variable of the given index.
  // or something completely different.
  DandyScriptOpStereotype(0x004, "op004", "", "L1"),
  // not checked in-game
  // wait for something??
//  DandyScriptOpStereotype(0x005, "op005", "", "x"),
  DandyScriptOpStereotype(0x005, "op005", "", "L1"),
  // THIS DOES A CONDITIONAL BRANCH
  // stupidly complicated, look into this
  // the possible branch immediately follows the instruction
  // and is terminated with a "00 00"????
  DandyScriptOpStereotype(0x006, "CONDITIONAL", "", "L1 L1 L1 L1 L1 L1 L1 L1 L1"),
  // ??????????
  // used near an op006 conditional branch...
  DandyScriptOpStereotype(0x007, "op007", "", "L1"),
  // loading wait? synchronous?
  DandyScriptOpStereotype(0x008, "op008", "", "x"),
  // ??? not checked in-game but this is probably close enough
  // string param = e.g. "GUARD"
  DandyScriptOpStereotype(0x009, "op009", "", "L4 S L4"),
  // not used?
  DandyScriptOpStereotype(0x00a, "op00a", "", "L4 S L4"),
  // not used?
  DandyScriptOpStereotype(0x00b, "op00b", "", "L1"),
  DandyScriptOpStereotype(0x00c, "openTextBox", "", "x"),
  DandyScriptOpStereotype(0x00d, "closeTextBox", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x00e, "op00e", "", "L1 L1"),
  // ?
  DandyScriptOpStereotype(0x00f, "op00f", "", "L1 L2 L2 L2"),
  DandyScriptOpStereotype(0x010, "op010", "", "L1"),
  // not used?
  DandyScriptOpStereotype(0x011, "op011", "", "L1"),
  // not used?
  DandyScriptOpStereotype(0x012, "op012", "", "L1 L1"),
  // not used?
  DandyScriptOpStereotype(0x013, "op013", "", "L1"),
  DandyScriptOpStereotype(0x014, "op014", "", "x"),
  DandyScriptOpStereotype(0x015, "op015", "", "L1 L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x016, "op016", "", "L1"),
  // ??
  DandyScriptOpStereotype(0x017, "fadeIn", "fadeIn?", "L2 L2 L2 L2"),
  // ??
  DandyScriptOpStereotype(0x018, "fadeOut", "fadeOut?", "L2 L2 L2 L2"),
  // this can apparently trigger/wait for fade-in??
  // or just a general "delay" op?
  DandyScriptOpStereotype(0x019, "delay", "", "L2"),
  // definitely ends processing current script -- scriptpos is not even advanced
  DandyScriptOpStereotype(0x01a, "stopScript", "stopScript?", "x"),
  DandyScriptOpStereotype(0x01b, "op01b", "somethingOn_1b", "x"),
  DandyScriptOpStereotype(0x01c, "op01c", "somethingOff_1b", "x"),
  // ???
  DandyScriptOpStereotype(0x01d, "op01d", "", "L1 L2 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x01e, "op01e", "", "L2"),
  DandyScriptOpStereotype(0x01f, "op01f", "", "x"),
  DandyScriptOpStereotype(0x020, "op020", "", ""),
  // ???
  DandyScriptOpStereotype(0x021, "op021", "", "L1 L2 L2 L1"),
  // ????????
  DandyScriptOpStereotype(0x022, "setBgPic", "setBgPic?", "L1 L2 L2 L2 L2 L2 L2 L1"),
  // ??????
  // not checked in-game
  DandyScriptOpStereotype(0x023, "op023", "", "L4 S L1 L4"),
  // ????
  // not checked in-game
  DandyScriptOpStereotype(0x024, "op024", "", "L4 S L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x025, "op025", "", "L1"),
  // ?
  DandyScriptOpStereotype(0x026, "op026", "", "L4 L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x027, "op027", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x028, "op028", "", "L1"),
  // ??
  DandyScriptOpStereotype(0x029, "op029", "", "L1 L1"),
  // not checked in-game
  // string param = e.g. "G13_SP_3"
  DandyScriptOpStereotype(0x02a, "op02a", "", "L4 S L4 L4"),
  // ?
  // string param = e.g. "G_FAIL1"
  DandyScriptOpStereotype(0x02b, "op02b", "", "L4 S L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x02c, "messageInMenus", "messageInMenus???", "T"),
  DandyScriptOpStereotype(0x02d, "op02d", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x02e, "op02e", "", "x"),
  // ???
  // string param = e.g. "MAMO"
  DandyScriptOpStereotype(0x02f, "op02f", "", "L1 S L1 L1"),
  // ??
  DandyScriptOpStereotype(0x030, "op030", "", "L1 L1"),
  DandyScriptOpStereotype(0x031, "op031", "", "x"),
  // loop until ?
  DandyScriptOpStereotype(0x032, "wait_032", "waitFor?_032", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x033, "op033", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x034, "op034", "", "L4"),
  // ?
  DandyScriptOpStereotype(0x035, "op035", "", "L4 L4"),
  // ????
  // string param = e.g. "PUNCH_L"
  DandyScriptOpStereotype(0x036, "op036", "", "L4 S L1 L4 L4 L4"),
  // ?
  DandyScriptOpStereotype(0x037, "op037", "", "L4 L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x038, "op038", "", "x"),
  // ?
  DandyScriptOpStereotype(0x039, "op039", "", "L1 L1"),
  DandyScriptOpStereotype(0x03a, "op03a", "", "L1"),
  // ???
  DandyScriptOpStereotype(0x03b, "op03b", "", "L1 L1 L2"),
  DandyScriptOpStereotype(0x03c, "op03c", "", "L1"),
  // ?
  // string param = e.g. "BIL_CRASH1"
  DandyScriptOpStereotype(0x03d, "op03d", "", "L4 S S"),
  // not checked in-game
  DandyScriptOpStereotype(0x03e, "op03e", "", "L1 L1 L1 L2 L2 L1"),
  DandyScriptOpStereotype(0x03f, "op03f", "", "L1"),
  // ?
  DandyScriptOpStereotype(0x040, "op040", "", "L1 S"),
  DandyScriptOpStereotype(0x041, "op041", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x042, "op042", "", "x"),
  // ? loop until ?
  DandyScriptOpStereotype(0x043, "wait_043", "waitFor?_043", "x"),
  // ??
  DandyScriptOpStereotype(0x044, "op044", "", "L1"),
  // ???
  DandyScriptOpStereotype(0x045, "op045", "", "L1 L1 L1 L1 L1 L2 L2 L2"),
  // long synchronous load?
  DandyScriptOpStereotype(0x046, "op046", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x047, "op047", "", "L4 S L4 L4"),
  // not used?
  DandyScriptOpStereotype(0x048, "op048", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x049, "op049", "", "L1"),
  // not used?
  DandyScriptOpStereotype(0x04a, "op04a", "", "L4 L4 L4 L2 L2 L2"),
  // ?
  // this one takes a long time just for executing the actual op.
  // synchronous loading/wait?
  DandyScriptOpStereotype(0x04b, "op04b", "", "L1"),
  // not used?
  DandyScriptOpStereotype(0x04c, "op04c", "", "L4 L4 L4 L2 L2 L2"),
  // ? long...
  DandyScriptOpStereotype(0x04d, "op04d", "", "L1 L1"),
  // not used?
  DandyScriptOpStereotype(0x04e, "op04e", "", "L1 L1 L4 L4 L4 L2 L2 L2"),
  // ??????????????????
  // no idea
  DandyScriptOpStereotype(0x04f, "op04f", "", "L4 L2 L2 L2 L2 L2 L2 L2 L2 L4"),
  // ?????
  // CHECK THIS
  // not checked in-game
  DandyScriptOpStereotype(0x050, "op050", "", "L4 S L4 L4 L4"),
  // loop until ?
  // e.g. mission preview map confirmed
  DandyScriptOpStereotype(0x051, "wait_051", "waitFor?_051", "x"),
  // ?
  DandyScriptOpStereotype(0x052, "op052", "", "L1 L1"),
  // not used?
  DandyScriptOpStereotype(0x053, "op053", "", "L2"),
  // ? maybe fade??
  // something + frame count?
  // maybe param 1 = fade to normal scene if 0000, solid color otherwise?
  DandyScriptOpStereotype(0x054, "op054", "", "L2 L2"),
  DandyScriptOpStereotype(0x055, "op055", "", "L1"),
  DandyScriptOpStereotype(0x056, "op056", "", "x"),
  // ???
  DandyScriptOpStereotype(0x057, "op057", "", "L4 L4 L4"),
  DandyScriptOpStereotype(0x058, "op058", "", "L1 L1"),
  DandyScriptOpStereotype(0x059, "op059", "", "x"),
  // param 1 = dst base?
  // param 2 = dst offset?
  // param 3 = characters to send
  DandyScriptOpStereotype(0x05a, "charSeqToVram", "charSequenceToVram", "L2 L2 T"),
  DandyScriptOpStereotype(0x05b, "vramSeqToScreen", "vramSeqToScreen?", "L1 L1 L1 L2 L2 L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x05c, "op05c", "", "L1"),
  // ???
  DandyScriptOpStereotype(0x05d, "openWindow", "generateWindow", "L1 L2 L2 L2 L2"),
  DandyScriptOpStereotype(0x05e, "closeWindow", "op05e", "L1"),
  // ??
  // initializes 16x16 "texture display buffers"???
  DandyScriptOpStereotype(0x05f, "initCharTexBufs", "initCharTextureDisplayBuffers", "L1 L2"),
  DandyScriptOpStereotype(0x060, "destroyCharTexBufs", "op060", "x"),
  DandyScriptOpStereotype(0x061, "charBufsOn", "op061", "x"),
  DandyScriptOpStereotype(0x062, "charBufsOff", "op062", "x"),
  // not used?
  DandyScriptOpStereotype(0x063, "op063", "", "L2 L2 L1 L1 L1 L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x064, "op064", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x065, "op065", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x066, "op066", "", "L1"),
  // skip listing is L1 L1 L4 L4 L4 L4,
  // which seems to be totally wrong
  DandyScriptOpStereotype(0x067, "op067", "", "L1"),
  // not used?
  DandyScriptOpStereotype(0x068, "op068", "", "L1"),
  // can take time...
  DandyScriptOpStereotype(0x069, "op069", "", "L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x06a, "op06a", "", "L1"),
  DandyScriptOpStereotype(0x06b, "setCam", "setCamera?", "S"),
  DandyScriptOpStereotype(0x06c, "op06c", "", "L1"),
  // ?
  // string param = e.g. "C001"
  DandyScriptOpStereotype(0x06d, "op06d", "", "S L4"),
  // ?
  // string param = e.g. "RL01"
  // skip listing is L4 string,
  // which seems to be wrong
  DandyScriptOpStereotype(0x06e, "op06e", "", "L4 S L4"),
  // ?
  // string param = e.g. "ENEM"
  DandyScriptOpStereotype(0x06f, "op06f", "", "L4 L4 S"),
  DandyScriptOpStereotype(0x070, "op070", "", "x"),
  // loops until ??
  DandyScriptOpStereotype(0x071, "wait_071", "waitFor?_071", "L4"),
  DandyScriptOpStereotype(0x072, "op072", "", "x"),
  DandyScriptOpStereotype(0x073, "op073", "", "x"),
  // ?
  DandyScriptOpStereotype(0x074, "op074", "", "L4 L4 L4"),
  // not used?
  DandyScriptOpStereotype(0x075, "op075", "", "L1"),
  // this will loop until ???
  DandyScriptOpStereotype(0x076, "wait_076", "waitFor?_076", "L4"),
  // not used?
  DandyScriptOpStereotype(0x077, "op077", "", "L1 L1 L1 L1 L2 L2 L1"),
  // not used?
  DandyScriptOpStereotype(0x078, "op078", "", "L1 L1 L1 L2 L2 L1"),
  DandyScriptOpStereotype(0x079, "setPortrait", "", "L1"),
  DandyScriptOpStereotype(0x07a, "setBodyPortrait", "setBodyPortait??", "L1 L1 L2 L2"),
  // ?
  // SHARED HANDLER
  // L1_type04? L1_type0C? L1_type0B? L2_type05? L2_type05? L1_type04? 
  DandyScriptOpStereotype(0x07b, "op07b", "", "L1 L1 L1 L2 L2 L1"),
  // ?
  // string param = e.g. "SELE"
  DandyScriptOpStereotype(0x07c, "op07c", "", "L4 S"),
  // can take time...
  DandyScriptOpStereotype(0x07d, "op07d", "", "L1 L1"),
  // ?
  DandyScriptOpStereotype(0x07e, "op07e", "", "L1 L1 S"),
  // not used?
  DandyScriptOpStereotype(0x07f, "op07f", "", "L1"),
  // param1 = number of 32x32 "cells" to load from chapter title graphic?
  DandyScriptOpStereotype(0x080, "initChapTitleSlots", "loadChapterTitleGraphicChars", "L1"),
  // ???
  // letters appearing on title cards...?
  // maybe coordinate positions somewhere in here?
  DandyScriptOpStereotype(0x081, "setChapTitleGrp", "chapterTitleGraphicToScreen?", "L1 L2 L2 L1 L1 L1 L1"),
  // ?
  // fade??
  // 00 = some kind of fadeout??
  // 01 = "zoom" effect after chapter title display?
  // 02 = nothing?
  DandyScriptOpStereotype(0x082, "doChapTitleEffect", "doChapterTitleSpecialEffect?", "L1"),
  DandyScriptOpStereotype(0x083, "op083", "", "x"),
  // string param = e.g. "MUSIC.XAP"
  DandyScriptOpStereotype(0x084, "playMusic", "playMusic?", "S L4 L4 L1 L4 L2"),
  DandyScriptOpStereotype(0x085, "op085", "", "x"),
  DandyScriptOpStereotype(0x086, "op086", "", "L1"),
  DandyScriptOpStereotype(0x087, "op087", "", "x"),
  // sets up a sound to be triggered from text string with /v command
  // (param 1 == id number for use in string?)
  // string param = e.g. "VOICE.XAP"
  DandyScriptOpStereotype(0x088, "setUpVoice", "", "L1 S L4 L4 L1"),
  DandyScriptOpStereotype(0x089, "wait_089", "waitForFileLoad?", "x"),
  // ? loading?
  DandyScriptOpStereotype(0x08a, "op08a", "", "L1 L1"),
  // ?
  DandyScriptOpStereotype(0x08b, "op08b", "", "L1 L1"),
  DandyScriptOpStereotype(0x08c, "op08c", "", "L1"),
  DandyScriptOpStereotype(0x08d, "op08d", "", "L1"),
  // ?
  DandyScriptOpStereotype(0x08e, "op08e", "", "L1 L1"),
  DandyScriptOpStereotype(0x08f, "op08f", "", "L1"),
  // ??
  DandyScriptOpStereotype(0x090, "playSound", "playSoundEffect?", "L4 L4 L4 L4 L4 L4 L4 L1"),
  DandyScriptOpStereotype(0x091, "op091", "", "L1"),
  DandyScriptOpStereotype(0x092, "op092", "", "L1"),
  // ??? wait? for ??
  DandyScriptOpStereotype(0x093, "turnOffMenus", "op093", "x"),
  DandyScriptOpStereotype(0x094, "op094", "", "x"),
  // string param = e.g. "MAMO"
  // skip entry lists this as gametext, but it's wrong?
  DandyScriptOpStereotype(0x095, "op095", "", "S"),
  DandyScriptOpStereotype(0x096, "op096", "", "x"),
  DandyScriptOpStereotype(0x097, "op097", "", "x"),
  DandyScriptOpStereotype(0x098, "op098", "", "x"),
  // loop until "mission failed" sequence completes?
  DandyScriptOpStereotype(0x099, "wait_099", "waitFor?_099", "x"),
  DandyScriptOpStereotype(0x09a, "op09a", "", "x"),
  // synchronous load??
  DandyScriptOpStereotype(0x09b, "op09b", "", "x"),
  DandyScriptOpStereotype(0x09c, "op09c", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x09d, "op09d", "", "L4 L4"),
  // ?
  DandyScriptOpStereotype(0x09e, "op09e", "", "L4"),
  // not used?
  DandyScriptOpStereotype(0x09f, "op09f", "", "L4 S L1"),
  // string param = e.g. "G_02_02"
  DandyScriptOpStereotype(0x0a0, "op0a0", "", "L4 S L1"),
  DandyScriptOpStereotype(0x0a1, "op0a1", "", "L1"),
  // NOTE: assigned same handler function as op B7
  DandyScriptOpStereotype(0x0a2, "op0a2", "", "L1 L1"),
  // ??
  DandyScriptOpStereotype(0x0a3, "op0a3", "", "L1 L1"),
  // not checked in-game
  // string params = e.g. "GEN_CRASH", "", "HATU"
  DandyScriptOpStereotype(0x0a4, "op0a4", "", "L4 S S"),
  // not used?
  DandyScriptOpStereotype(0x0a5, "op0a5", "", "L4 S L1 L4 L4 L4 L4 L4"),
  // guessing...
  // string param = e.g. "BAD_END"
  DandyScriptOpStereotype(0x0a6, "op0a6", "", "L4 S L4"),
  // ?
  // string param = e.g. "BAD_END"
  DandyScriptOpStereotype(0x0a7, "op0a7", "", "L4 S"),
  // ?
  // string param = e.g. "FOOT_FORWARE"
  DandyScriptOpStereotype(0x0a8, "op0a8", "", "L4 S L1 L4 L4"),
  // ?
  // string param = e.g. "WALK"
  DandyScriptOpStereotype(0x0a9, "op0a9", "", "L4 S"),
  // not used?
  DandyScriptOpStereotype(0x0aa, "op0aa", "", "L1 L2 L2 L1 L1 L1 L1 L1 L1"),
  // not used?
  DandyScriptOpStereotype(0x0ab, "op0ab", "", "L1 L2 L2 L1 L1"),
  // not used?
  DandyScriptOpStereotype(0x0ac, "op0ac", "", "L1 L2 L2"),
  // not used?
  DandyScriptOpStereotype(0x0ad, "op0ad", "", "x"),
  DandyScriptOpStereotype(0x0ae, "op0ae", "", "x"),
  DandyScriptOpStereotype(0x0af, "op0af", "", "L1"),
  DandyScriptOpStereotype(0x0b0, "op0b0", "", "x"),
  DandyScriptOpStereotype(0x0b1, "op0b1", "", "L1"),
  // not used?
  DandyScriptOpStereotype(0x0b2, "op0b2", "", "L1"),
  // not checked in-game
  // terminator????
  DandyScriptOpStereotype(0x0b3, "op0b3", "", "x"),
  DandyScriptOpStereotype(0x0b4, "op0b4", "", "x"),
  // ???
  DandyScriptOpStereotype(0x0b5, "op0b5", "", "L4 L2 L2 L2 L2 L4"),
  // ?
  DandyScriptOpStereotype(0x0b6, "op0b6", "", "L1 L2"),
  // NOTE: assigned same handler function as op A2
  DandyScriptOpStereotype(0x0b7, "op0b7", "", "L1 L1"),
  // ??
  DandyScriptOpStereotype(0x0b8, "op0b8", "", "L1 L1 L2 L2"),
  // ?
  DandyScriptOpStereotype(0x0b9, "op0b9", "", "L1 L2"),
  // ???
  DandyScriptOpStereotype(0x0ba, "op0ba", "", "L1 L1 L1 L2 L2 L1"),
  // loop until ?
  DandyScriptOpStereotype(0x0bb, "wait_0bb", "waitForAnimation?", "L1"),
  // ?
  DandyScriptOpStereotype(0x0bc, "op0bc", "", "L1 L1"),
  // ?????
  // not checked in-game
  DandyScriptOpStereotype(0x0bd, "op0bd", "", "L1"),
//  DandyScriptOpStereotype(0x0be, "queueLoad_0be", "queueLoadThing?Img??", "L1 L1"),
  DandyScriptOpStereotype(0x0be, "queueLoadAdv2", "queueLoad_0be", "L1 L1"),
  DandyScriptOpStereotype(0x0bf, "op0bf", "", "L1"),
  DandyScriptOpStereotype(0x0c0, "op0c0", "", "L1"),
  // body portait vanish??
  DandyScriptOpStereotype(0x0c1, "op0c1", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0c2, "op0c2", "", "L1 L1"),
  // guess...
  DandyScriptOpStereotype(0x0c3, "op0c3", "", "L4"),
  // guess...
  DandyScriptOpStereotype(0x0c4, "op0c4", "", "L4"),
  // ???
  DandyScriptOpStereotype(0x0c5, "op0c5", "", "L1 L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0c6, "op0c6", "", "x"),
  DandyScriptOpStereotype(0x0c7, "op0c7", "", "x"),
  DandyScriptOpStereotype(0x0c8, "op0c8", "", "x"),
  DandyScriptOpStereotype(0x0c9, "op0c9", "", "L1"),
  // not used?
  DandyScriptOpStereotype(0x0ca, "op0ca", "", "L1 L1 L2 L2 L2 L2"),
  // ??????
  // not checked in-game
  DandyScriptOpStereotype(0x0cb, "op0cb", "", "L1 L1 L2 L2 L2 L2"),
  // not used?
  DandyScriptOpStereotype(0x0cc, "op0cc", "", "L1 L1 L1 L2 L2 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0cd, "setTopWinLabel", "setTopWindowLabel?", "L1 L2 L1"),
  // ???
  // string param = e.g. "CHANGE_A" (which is a script label name)
  // queue script for future execution??
  DandyScriptOpStereotype(0x0ce, "op0ce", "", "S L4"),
  // ?
  // string param = e.g. "F_03_05"
  DandyScriptOpStereotype(0x0cf, "op0cf", "", "L4 S"),
  // ?
  // string param = e.g. "FLAG_CLEAR"
  DandyScriptOpStereotype(0x0d0, "op0d0", "", "L4 S"),
  // ?????????????????????????????????????????????????????
  // CHECK THIS
  // not checked in-game
  // string param = e.g. "BL3"
  DandyScriptOpStereotype(0x0d1, "op0d1", "", "S L4"),
  // ?????
  // string param = e.g. "MIYU"
  // not checked in-game
  DandyScriptOpStereotype(0x0d2, "op0d2", "", "S L4 L1 L1 L1"),
  // not used?
  DandyScriptOpStereotype(0x0d3, "op0d3", "", "L4 S L4 L4"),
  // ?
  // string param = e.g. "SP_4"
  DandyScriptOpStereotype(0x0d4, "op0d4", "", "L4 S L4"),
  // not used?
  DandyScriptOpStereotype(0x0d5, "op0d5", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0d6, "op0d6", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0d7, "op0d7", "", "L4"),
  // ?
  DandyScriptOpStereotype(0x0d8, "op0d8", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0d9, "op0d9", "", "L4 L4"),
  // not checked in-game
  // string param = e.g. "SCHL"
  DandyScriptOpStereotype(0x0da, "op0da", "", "L4 S L4"),
  // ??
  DandyScriptOpStereotype(0x0db, "op0db", "", "L4"),
  // not checked in-game
  // string param = e.g. "EL02"
  DandyScriptOpStereotype(0x0dc, "op0dc", "", "L4 S"),
  // ?
  DandyScriptOpStereotype(0x0dd, "op0dd", "", "L4 L4"),
  // ??
  DandyScriptOpStereotype(0x0de, "op0de", "", "L4 L4 L4 L4"),
  // ?
  DandyScriptOpStereotype(0x0df, "op0df", "", "L4 L4"),
  // ???
  // string param = e.g. "A000"
  DandyScriptOpStereotype(0x0e0, "op0e0", "", "L1 S"),
  // ???
  // string param = e.g. "A_00_02"
  DandyScriptOpStereotype(0x0e1, "op0e1", "", "L4 S L1 L4"),
  // not checked in-game
  // CHECK LAST PARAM
  DandyScriptOpStereotype(0x0e2, "op0e2", "", "L4 S L1"),
  // ?
  DandyScriptOpStereotype(0x0e3, "op0e3", "", "L4 L4"),
  // ?????
  // CHECK THIS
  // not checked in-game
  // string param = e.g. "SAHA_WAIT"
  DandyScriptOpStereotype(0x0e4, "op0e4", "", "L4 S L4"),
  DandyScriptOpStereotype(0x0e5, "op0e5", "", "L4"),
  DandyScriptOpStereotype(0x0e6, "op0e6", "", "L4 S"),
  // ?
  DandyScriptOpStereotype(0x0e7, "op0e7", "", "L4 L4"),
  // ?
  DandyScriptOpStereotype(0x0e8, "op0e8", "", "L4 L4"),
  // ???
  // param 1 = mission number?
  // param 2 = if 1, do not do the usual checks for triggered events, etc.?
  //           note that this is always 0 in the original scripts
  DandyScriptOpStereotype(0x0e9, "prepPostMissionEvents", "op0e9", "L4 L4"),
  DandyScriptOpStereotype(0x0ea, "op0ea", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0eb, "op0eb", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ec, "op0ec", "", "x"),
  // not checked in-game
  // string param = e.g. "SINKOU.S3M"
  DandyScriptOpStereotype(0x0ed, "loadOverlay", "loadOverlay?", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ee, "op0ee", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ef, "op0ef", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f0, "op0f0", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f1, "op0f1", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f2, "op0f2", "", "L4"),
  DandyScriptOpStereotype(0x0f3, "op0f3", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0f4, "op0f4", "", "L4 S"),
  DandyScriptOpStereotype(0x0f5, "op0f5", "", "x"),
  // ???
  DandyScriptOpStereotype(0x0f6, "op0f6", "", "L4"),
  // ??
  // used before a conditional, with parameter of 1000...
  // a value to check against??
  DandyScriptOpStereotype(0x0f7, "op0f7", "", "L4"),
  DandyScriptOpStereotype(0x0f8, "op0f8", "", ""),
  // ???
  // total guess...
  DandyScriptOpStereotype(0x0f9, "op0f9", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fa, "op0fa", "", "L1 L1 L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fb, "op0fb", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fc, "op0fc", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fd, "op0fd", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0fe, "op0fe", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x0ff, "op0ff", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x100, "op100", "", "x"),
  // ?????????
  // not checked in-game
  // string param = e.g. "VOICE.XAP"
//  DandyScriptOpStereotype(0x101, "op101", "", "S L2 L2 L2 L2 L2 L2 L1 L2 L2"),
  DandyScriptOpStereotype(0x101, "op101", "", "S L4 L4 L1 L4 L2"),
  // not checked in-game
  DandyScriptOpStereotype(0x102, "op102", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x103, "op103", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x104, "jumpToMission", "op104", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x105, "op105", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x106, "op106", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x107, "op107", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x108, "op108", "", "x"),
  // ?????
  // not checked in-game
  DandyScriptOpStereotype(0x109, "op109", "", "L1 L1 L1"),
  DandyScriptOpStereotype(0x10a, "op10a", "", "x"),
  // not used?
  DandyScriptOpStereotype(0x10b, "op10b", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x10c, "op10c", "", "L4 S L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x10d, "op10d", "", "L4 S L4"),
  // not used?
  DandyScriptOpStereotype(0x10e, "op10e", "", "L4 S L4"),
  // not used?
  DandyScriptOpStereotype(0x10f, "op10f", "", "L4 S L4"),
  // loading?...
  DandyScriptOpStereotype(0x110, "op110", "", "x"),
  // ?
  // string param = e.g. "BIL_CRASH1"
//  DandyScriptOpStereotype(0x111, "op111", "", "L2 L2 S L1"),
  DandyScriptOpStereotype(0x111, "op111", "", "L4 S S"),
  // not checked in-game
  DandyScriptOpStereotype(0x112, "op112", "", "L4 S L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x113, "op113", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x114, "op114", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x115, "op115", "", "L4"),
  // guess...
  DandyScriptOpStereotype(0x116, "op116", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x117, "op117", "", "L4 L4"),
  // not checked in-game
  // string param = e.g. "JET_DASH1"
  DandyScriptOpStereotype(0x118, "op118", "", "L4 S L4"),
  // ?
  DandyScriptOpStereotype(0x119, "op119", "", "L4"),
  // ???
  DandyScriptOpStereotype(0x11a, "op11a", "", "L1 L2 L2 L2"),
  // loop until ?
  DandyScriptOpStereotype(0x11b, "wait_11b", "waitFor?_11b", "L1"),
  // not used?
  DandyScriptOpStereotype(0x11c, "op11c", "", "L2 L2 L2"),
  // not used?
  DandyScriptOpStereotype(0x11d, "op11d", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x11e, "op11e", "", "L4"),
  // not checked in-game
  // skip listing is L1 S;
  // only use in game has param of "ENE2",
  // which i'm assuming is for "enemy" and so there is no L1 param
  DandyScriptOpStereotype(0x11f, "op11f", "", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x120, "op120", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x121, "op121", "", "L4 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x122, "op122", "", "L4 L4 L4 L4"),
  // not checked in-game
  // function pointer is 800B54BC,
  // which is not in main exe, ADV, GAME, SINKOU, or SONIC.AOE-151
  DandyScriptOpStereotype(0x123, "op123", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x124, "op124", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x125, "op125", "", "L1"),
  // not checked in-game
  DandyScriptOpStereotype(0x126, "op126", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x127, "op127", "", "L4 L4 S"),
  // not used?
  DandyScriptOpStereotype(0x128, "op128", "", "L4 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x129, "op129", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x12a, "op12a", "", "L4 L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x12b, "op12b", "", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x12c, "op12c", "", "S"),
  // not checked in-game
  DandyScriptOpStereotype(0x12d, "op12d", "", "x"),
  // ?
  // appearance of text overlay in intro...?
  DandyScriptOpStereotype(0x12e, "op12e", "", "L4 L4"),
  // disappearance of text overlay in intro...?
  DandyScriptOpStereotype(0x12f, "op12f", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x130, "op130", "", "L4"),
  // not checked in-game
  // string param = e.g. "P_28_01"
  DandyScriptOpStereotype(0x131, "op131", "", "L4 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x132, "op132", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x133, "op133", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x134, "op134", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x135, "op135", "", "L4 L4 S"),
  // not used?
  DandyScriptOpStereotype(0x136, "op136", "", "L4 S"),
  // not used?
  DandyScriptOpStereotype(0x137, "op137", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x138, "op138", "", "L4 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x139, "op139", "", "L4 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x13a, "op13a", "", "L4 S"),
  // not checked in-game
  DandyScriptOpStereotype(0x13b, "op13b", "", "L4 L4 L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x13c, "op13c", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x13d, "op13d", "", "x"),
  // not checked in-game
  DandyScriptOpStereotype(0x13e, "op13e", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x13f, "op13f", "", "L4"),
  // not used?
  DandyScriptOpStereotype(0x140, "op140", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x141, "op141", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x142, "op142", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x143, "op143", "", "L4 S"),
  // ?????
  // not checked in-game
  DandyScriptOpStereotype(0x144, "op144", "", "L4 L4 L4"),
  // ?
  DandyScriptOpStereotype(0x145, "op145", "", "L4 L4"),
  // ?
  DandyScriptOpStereotype(0x146, "op146", "", "L4"),
  // ?
  DandyScriptOpStereotype(0x147, "op147", "", "L4"),
  // not used?
  DandyScriptOpStereotype(0x148, "op148", "", "x"),
  // end? wait? load?
  // this is the last op that runs before the
  // "now loading" screen; script execution
  // resumes from where it left off after that finishes
  DandyScriptOpStereotype(0x149, "wait_149", "end??", "x"),
  // not used?
  DandyScriptOpStereotype(0x14a, "op14a", "", "L1 L2"),
  // not used?
  DandyScriptOpStereotype(0x14b, "op14b", "", "L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x14c, "op14c", "", "x"),
  // not checked in-game
  // CHECK THIS
  DandyScriptOpStereotype(0x14d, "op14d", "", "L4 L4 L4"),
  // not checked in-game
  DandyScriptOpStereotype(0x14e, "op14e", "", "x"),
  // guessing...
  DandyScriptOpStereotype(0x14f, "op14f", "", "L4"),
  // ??????????
  // not checked in-game
  DandyScriptOpStereotype(0x150, "op150", "", "L4 L4 L1 L4")
};
const static int numOpStereotypes
  = sizeof(opStereotypeList)/sizeof(DandyScriptOpStereotype);

const DandyScriptOpStereotype&
    DandyScriptOpStereotypes::matchStereotypeByName(std::string name) {
  for (int i = 0; i < numOpStereotypes; i++) {
    // check primary name
    if (opStereotypeList[i].name.compare(name) == 0)
      return opStereotypeList[i];
    
    // check alises
    const std::string& nameList = opStereotypeList[i].aliases;
    
    TBufStream ifs;
    ifs.writeString(nameList);
    ifs.seek(0);
    
    // check list of op names/aliases
    while (true) {
      std::string opName;
      while (!ifs.eof() && (ifs.peek() != '/')) {
        opName += ifs.get();
      }
      
      if (name.compare(opName) == 0)
        return opStereotypeList[i];
      
      if (ifs.eof()) break;
      ifs.get();
    }
  }
  
  throw TGenericException(T_SRCANDLINE,
                          "DandyScriptOpStereotypes::getStereotypeByName()",
                          std::string("Could not match named script op '")
                            + name
                            + "'");
}

const DandyScriptOpStereotype& 
    DandyScriptOpStereotypes::matchStereotypeByOpcode(int opcode) {
  // there's really no reason this wouldn't always be the case, but...
  if ((opcode < numOpStereotypes)
      && (opStereotypeList[opcode].opcode == opcode)) {
    return opStereotypeList[opcode];
  }
  
  for (int i = 0; i < numOpStereotypes; i++) {
    if (opStereotypeList[i].opcode == opcode)
      return opStereotypeList[i];
  }
  
  throw TGenericException(T_SRCANDLINE,
                          "DandyScriptOpStereotypes::getStereotypeByOpcode()",
                          std::string("Could not match script opcode ")
                            + TStringConversion::intToString(opcode,
                                  TStringConversion::baseHex));
}


}
