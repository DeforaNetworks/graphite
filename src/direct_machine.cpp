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
// This direct threaded interpreter implmentation for machine.h
// Author: Tim Eves

// Build either this interpreter or the call_machine implementation.
// The direct threaded interpreter is relies upon a gcc feature called 
// labels-as-values so is only portable to compilers that support the 
// extension (gcc only as far as I know) however it should build on any
// architecture gcc supports. 
// This is twice as fast as the call threaded model and is likely faster on 
// inorder processors with short pipelines and little branch prediction such 
// as the ARM and possibly Atom chips.


#include <cassert>
#include <graphiteng/SlotHandle.h>
#include "Machine.h"
#include "GrSegment.h"

#define STARTOP(name)           name: {
#define ENDOP                   }; goto *((sp - sb)/Machine::STACK_MAX ? &&end : *++ip);
#define EXIT(status)            push(status); goto end

#define do_(name)               &&name

using namespace vm;
using namespace org::sil::graphite::v2;

namespace {

const void * direct_run(const bool          get_table_mode,
                        const instr       * program,
                        const byte        * data,
                        Machine::stack_t  * stack,
                        GrSegment     * const seg_ptr,
                        slotref           & islot_idx,
                        slotref             iStart)
{
    // We need to define and return to opcode table from within this function 
    // other inorder to take the addresses of the instruction bodies.
    #include "opcode_table.h"
    if (get_table_mode)
        return opcode_table;

    // Declare virtual machine registers
    const instr       * ip = program;
    const byte        * dp = data;
    Machine::stack_t  * sp = stack + Machine::STACK_GUARD,
                * const sb = sp;
    GrSegment       & seg = *seg_ptr;
    slotref         is=islot_idx, isf=-1, isl=-1;
    const slotref   isb=iStart;
    Position        endPos=Position();
    int8           copies[64] = {-1};
    
    // start the program
    goto **ip;

    // Pull in the opcode definitions
    #include "opcodes.h"
    
    end:
    islot_idx = is;
    return sp;
}

}

const opcode_t * Machine::getOpcodeTable() throw()
{
    int is_dummy;
    return static_cast<const opcode_t *>(direct_run(true, 0, 0, 0, 0, is_dummy, 0));
}


Machine::stack_t  Machine::run(const instr  * program,
                               const byte   * data,
                               GrSegment      & seg,
                               slotref      & islot_idx,
                               slotref        iStart,
                               status_t     & status)
{
    assert(program != 0);
    
    const stack_t *sp = static_cast<const stack_t *>(
                direct_run(false, program, data, _stack, &seg, islot_idx, iStart));
    const stack_t ret = sp == _stack+STACK_GUARD+1 ? *sp-- : 0;
    check_final_stack(sp, status);
    return *sp;
}

