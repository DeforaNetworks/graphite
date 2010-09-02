/*  GRAPHITENG LICENSING

    Copyright 2010, SIL International
    All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should also have received a copy of the GNU Lesser General Public
    License along with this library in the file named "LICENSE".
    If not, write to the Free Software Foundation, Inc., 59 Temple Place, 
    Suite 330, Boston, MA 02111-1307, USA or visit their web page on the 
    internet at http://www.fsf.org/licenses/lgpl.html.
*/
#include "XmlTraceLogTags.h"

#ifndef DISABLE_TRACING


namespace org { namespace sil { namespace graphite { namespace v2 {







// start this at same line number as in XmlTraceLogTags.h
const XmlTraceLogTag xmlTraceLogElements[NumElements] = {
    XmlTraceLogTag("GraphitengLog", GRLOG_ALL),
    XmlTraceLogTag("Face", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("Glyphs", GRLOG_FACE),
    XmlTraceLogTag("GlyphFace", GRLOG_FACE),
    XmlTraceLogTag("Attr", GRLOG_FACE),
    XmlTraceLogTag("Silf", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("SilfSub", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("Pass", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("Pseudo", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("ClassMap", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("LookupClass", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("Lookup", GRLOG_FACE | GRLOG_PASS),
    XmlTraceLogTag("Range", GRLOG_PASS),
    XmlTraceLogTag("RuleMap", GRLOG_PASS),
    XmlTraceLogTag("Rule", GRLOG_PASS),
    XmlTraceLogTag("StartState", GRLOG_PASS),
    XmlTraceLogTag("StateTransitions", GRLOG_PASS),
    XmlTraceLogTag("TR", GRLOG_PASS),
    XmlTraceLogTag("TD", GRLOG_PASS),
    XmlTraceLogTag("Constraint", GRLOG_PASS),
    XmlTraceLogTag("Constraints", GRLOG_PASS),
    XmlTraceLogTag("Actions", GRLOG_PASS),
    XmlTraceLogTag("Action", GRLOG_PASS),
    XmlTraceLogTag("Features", GRLOG_PASS),
    XmlTraceLogTag("Feature", GRLOG_PASS),
    XmlTraceLogTag("FeatureSetting", GRLOG_PASS),
    XmlTraceLogTag("Segment", GRLOG_SEGMENT),
    XmlTraceLogTag("Slot", GRLOG_SEGMENT),
    XmlTraceLogTag("Text", GRLOG_SEGMENT),
    XmlTraceLogTag("OpCode", GRLOG_OPCODE),
    XmlTraceLogTag("TestRule", GRLOG_OPCODE),
    XmlTraceLogTag("DoRule", GRLOG_OPCODE),
    XmlTraceLogTag("RunPass", GRLOG_OPCODE),
    XmlTraceLogTag("Params", GRLOG_OPCODE),
    XmlTraceLogTag("Push", GRLOG_OPCODE),

    XmlTraceLogTag("Error", GRLOG_ALL),
    XmlTraceLogTag("Warning", GRLOG_ALL)
    // Nothing corresponds to NumElements
};



// start this at same line number as in XmlTraceLogTags.h
const char * xmlTraceLogAttributes[NumAttributes] = {
    "index",
    "version",
    "major",
    "minor",
    "num",
    "glyphId",
    "advance",
    "advanceX",
    "advanceY",
    "attrId",
    "attrVal",
    "compilerMajor",
    "compilerMinor",
    "numPasses",//AttrNumPasses,
    "subPass",//AttrSubPass,
    "posPass",//AttrPosPass,
    "justPass",//AttrJustPass,
    "bidiPass",//AttrBidiPass,
    "preContext",//AttrPreContext,
    "postContext",//AttrPostContext,
    "pseudoGlyph",//AttrPseudoGlyph,
    "breakWeight",//AttrBreakWeight,
    "directionality",//AttrDirectionality,
    "numJustLevels",//AttrNumJustLevels,
    "numLigCompAttr",//AttrLigComp,
    "numUserDefinedAttr",//AttrUserDefn,
    "maxNumLigComp",//AttrNumLigComp,
    "numCriticalFeatures",//AttrNumCritFeatures,
    "numScripts",//AttrNumScripts
    "lineBreakglyph",//,AttrLBGlyph,
    "numPseudo",
    "numClasses",
    "numLinear",
    "passId",//AttrPassId,
    "flags",//AttrFlags,
    "maxRuleLoop",//AttrMaxRuleLoop,
    "maxRuleContext",//AttrMaxRuleContext,
    "maxBackup",//AttrMaxBackup,
    "numRules",//AttrNumRules,
    "numRows",//AttrNumRows,
    "numTransitionStates",//AttrNumTransition,
    "numSuccessStates",//AttrNumSuccess,
    "numColumns",//AttrNumColumns,
    "numRanges",//AttrNumRanges,
    "minPrecontext",//AttrMinPrecontext,
    "maxPrecontext",//AttrMaxPrecontext,
    "firstId",
    "lastId",
    "colId",
    "successId",
    "ruleId",
    "contextLength",
    "state",
    "value",
    "sortKey",
    "precontext",
    "action",
    "actionCode",
    "arg1",
    "arg2",
    "arg3",
    "arg4",
    "arg5",
    "arg6",
    "arg7",
    "arg8",
    "label",
    "length",
    "x",
    "y",
    "before",
    "after",
    "encoding",
    "name",
    "result",
    "default"
};

}}}} // namespace
#endif
