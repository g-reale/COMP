#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "Globals.hpp"
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>
#include <unordered_map>

#define SEGMENT_TYPES\
    SEGMENT(RESERVED)\
    SEGMENT(SCALARS)\
    SEGMENT(POINTERS)\
    SEGMENT(USER_POITERS)\
    SEGMENT(VOID_FUNCTIONS)\
    SEGMENT(INT_FUNCTIONS)\
    SEGMENT(LABELS)

#define RESERVED_TYPES\
    RESERVED(PC)\
    RESERVED(ASSEMBLY)\
    RESERVED(OFFSET)\
    RESERVED(STACK)\
    RESERVED(RETURN_REGISTER)\
    RESERVED(TRASH)\
    RESERVED(ONE_CT)\
    RESERVED(DISP)\
    RESERVED(SWITCH)

class Segment{
    private:
        size_t last_used;
        size_t used;
        std::list<size_t>::iterator next_recycled;
        std::list<size_t> bin;
        size_t address;
        std::vector<size_t> segment;
        bool locked;
    public:
        Segment();
        void reset();
        size_t recycle();
        size_t allocate(size_t size = 1);
        size_t& operator[](size_t destination);
        const size_t& operator[](size_t destination) const;
        size_t size();
        friend std::ostream& operator<<(std::ostream& os, const Segment& s);
};

class Assembler{
    private:

        enum class segment_t{
            #define SEGMENT(name) name,
            SEGMENT_TYPES
            #undef SEGMENT
            _COUNT
        };

        struct variable_t{
            size_t segment;
            size_t address;
        };

        struct assembly_t{
            operator_t op;
            variable_t destination;
            variable_t argument_a;
            variable_t argument_b;
        };

        enum class reserved_t{
            #define RESERVED(name) name,
            RESERVED_TYPES
            #undef RESERVED
            _COUNT
        };

        struct context_t{
            std::string name;
            std::unordered_map<std::string,variable_t> mapping;
        };

        using binary_t = std::vector<size_t>;

        void pop();
        void push(const std::string& name);
        void declare(const std::string& name, const variable_t& variable, bool global_context = false);
        variable_t allocate(segment_t segment, size_t size = 1);
        variable_t recycle(segment_t segment);
        void set(const variable_t& variable, size_t value);
    
        void pushvar(const variable_t& variable);
        variable_t popvar();
        bool find(const std::string& name, variable_t& variable, bool global_context = false);
    
        static constexpr size_t ARGUMENT_LENGTH = 10;
        static constexpr size_t OPERATOR_LENGTH = 4;

        tree_t<context_t> contexts;
        context_t * global;
        std::vector<variable_t> stack;
        std::array<Segment,(size_t)segment_t::_COUNT> segments;
        std::vector<assembly_t> program;
        binary_t binary;
        
    public:
        Assembler();
        void assemble(assemblable_t & assemble);
        void writeBinary(const std::string& path);

        friend std::ostream& operator<<(std::ostream& os, const segment_t& type);
        friend std::ostream& operator<<(std::ostream& os, const reserved_t& type);
        friend std::ostream& operator<<(std::ostream& os, const variable_t& variable);
        friend std::ostream& operator<<(std::ostream& os, const assembly_t& assembly);
        friend std::ostream& operator<<(std::ostream& os, const context_t& context);
        friend std::ostream& operator<<(std::ostream& os, const Assembler& assembler);
};



// std::ostream& operator<<(std::ostream& os, const Assembler::context_t& context);
// std::ostream& operator<<(std::ostream& os, const Assembler::variable_t& variable);
#endif