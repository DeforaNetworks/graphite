#include <cstdlib>
#include <iostream>
#include <vector>
#include <time.h>
#include "Code.h"
#include "Rule.h"
#include "Silf.h"
#include "Face.h"

using namespace graphite2;
using namespace vm;
typedef Machine::Code  Code;

const byte simple_prog[] = 
{
        PUSH_BYTE, 43, 
        PUSH_BYTE, 42,
//        PUSH_LONG, 1,2,3,4,                   // Uncomment to cause an overflow
            PUSH_BYTE, 11, PUSH_BYTE, 13, ADD,
            PUSH_BYTE, 4, SUB,
        COND,
//        COND,                                 // Uncomment to cause an underflow
//    POP_RET
};

#define _msg(m) #m

const char * prog_error_msg[] = {
    _msg(loaded),
    _msg(alloc_failed), 
    _msg(invalid_opcode), 
    _msg(unimplemented_opcode_used),
    _msg(jump_past_end),
    _msg(arguments_exhausted),
    _msg(missing_return)
};

const char * run_error_msg[] = {
    _msg(finished),
    _msg(stack_underflow),
    _msg(stack_not_empty),
    _msg(stack_overflow),
    _msg(slot_offset_out_bounds)
};

class graphite2::Segment {};

//std::vector<byte> fuzzer(int);


int main(int argc, char *argv[])
{
    const char * font_path = argv[1];
    size_t repeats = 1,
           copies  = ((1 << 20)*4 + sizeof(simple_prog)-1) / sizeof(simple_prog);
    
    if (argc < 2)
    {
        std::cerr << argv[0] << ": GRAPHITE-FONT [repeats] [copies]" << std::endl;
        exit(1);
    }
    if (argc > 2)
        repeats = atoi(argv[2]);
    if (argc > 3)
        copies  = atoi(argv[3]);
    
    std::cout << "simple program size:    " << sizeof(simple_prog)+1 << " bytes" << std::endl;
    // Replicate the code copies times.
    std::vector<byte> big_prog;
    big_prog.push_back(simple_prog[0]);
    big_prog.push_back(simple_prog[1]);
    for(; copies; --copies)
        big_prog.insert(big_prog.end(), &simple_prog[2], simple_prog + sizeof(simple_prog));
    big_prog.push_back(POP_RET);
    std::cout << "amplified program size: " << big_prog.size() << " bytes" << std::endl;
    
    // Load the code.
    Silf silf;
    gr_face *face = gr_make_file_face(font_path, gr_face_dumbRendering);
    if (!face)
    {
        std::cerr << argv[0] << ": failed to load graphite tables for font: " << font_path << std::endl;
        exit(1);
    }
    Code prog(false, &big_prog[0], &big_prog[0] + big_prog.size(), 0, 0, silf, *face);
    if (!prog) {    // Find out why it did't work
        // For now just dump an error message.
        std::cerr << "program failed to load due to: " 
                << prog_error_msg[prog.status()] << std::endl;
        return 1;
    }
    std::cout << "loaded program size:    " 
              << prog.dataSize() + prog.instructionCount()*sizeof(instr) 
              << " bytes" << std::endl
              << "                        " 
              << prog.instructionCount() << " instructions" << std::endl;
    
    // run the program
    Segment seg;
    uint32 ret = 0;
    SlotMap smap(seg);
    Machine m(smap);
    smap.pushSlot(0);
    slotref * map = smap.begin();
    for(size_t n = repeats; n; --n) {
        ret = prog.run(m, map);
        switch (m.status()) {
            case Machine::stack_underflow:
            case Machine::stack_overflow:
                std::cerr << "program terminated early: " 
                          << run_error_msg[m.status()] << std::endl;
                std::cout << "--------" << std::endl
                          << "between " << prog.instructionCount()*(repeats-n) 
                          << " and "    << prog.instructionCount()*(repeats-std::min(n-1,repeats))
                          << " instructions executed" << std::endl;
                return 2;
            case Machine::stack_not_empty:
                std::cerr << "program completed but stack not empty." << std::endl;
                repeats -= n-1;
                n=1;
                break;
            default:
            {
                // noop
            }
        }
    }
    
    gr_face_destroy(face);
    
    std::cout << "result of program: " << ret << std::endl
              << "--------" << std::endl
              << "equivalent of " << prog.instructionCount()*repeats 
              << " instructions executed" << std::endl;
        
    return 0;
}


std::vector<byte> random_sequence(size_t n)
{
    std::vector<bool> done(n);
    std::vector<byte> seq(n);
    
    srand(static_cast<unsigned int>(time(NULL)));
    
    while(n)
    {
        const size_t r = (rand()*n + RAND_MAX/2)/RAND_MAX;
        
        if (done[r]) continue;
        
        done[r] = true;
        seq[r]  = r;
        --n;
    }
    
    return seq;
}

/*
std::vector<byte> fuzzer(int n)
{
    std::vector<byte>   code(256);
    std::vector<bool>   covered(256);
    
    // Track stack depth to ensure we don't create programs that
    //  overflow or underflow the stack.
    size_t stack_depth = 0;
    
    return code;   
}
*/
