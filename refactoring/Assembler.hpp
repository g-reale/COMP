#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "Globals.hpp"
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>
#include <unordered_map>


class Assembler{
    private:
        enum class reserved_t{
            PC,
            ASSEMBLY,
            STACK,
            RETURN_REGISTER,
            TRASH,
            ONE_CT,
            DISP,
            SWITCH,
            _COUNT
        };
        
        enum class type_t{
            SCALAR,
            POINTER,
            LABEL,
            VOID_FUNCTION,
            INT_FUNCTION,
            _COUNT
        };

    public:
        
        struct variable_t{
            type_t type;
            size_t address;
            size_t value;
        };

        struct context_t{
            std::string name;
            std::unordered_map<std::string,variable_t> mapping;
        };

    private:
        void pop();
        void push(const std::string& name);
        void pushvar(const variable_t& variable);
        void pushvar(size_t destination, type_t type = type_t::SCALAR, size_t value = 0);
        void dma(size_t destination, size_t value);
        variable_t popvar();
        void allocate(const std::string& name, size_t size = 1, size_t value = 0, type_t type = type_t::SCALAR, bool global_context = false);
        bool find(const std::string& name, variable_t& variable, bool global_context = false);
    
        size_t address;
        tree_t<context_t> contexts;
        context_t * global;
        std::vector<variable_t> stack;
        std::unordered_map<size_t,size_t> labels;
        binary_t binary;

        program_t program;
        std::list<size_t>::iterator next_recycled;
        std::list<size_t> bin;
        size_t recycled;
        size_t recycle();
        
    public:
        Assembler();
        void assemble(assemblable_t & assemble);
        void writeBinary(const std::string& path);

    friend std::ostream& operator<<(std::ostream& os, const type_t& type){
        switch (type){
            case type_t::POINTER:
                os << "POINTER";
                break;
            case type_t::SCALAR:
                os << "SCALAR";
                break;
            case type_t::LABEL:
                os << "LABEL";
                break;
            case type_t::INT_FUNCTION:
                os << "INT_FUNCTION";
                break;
            case type_t::VOID_FUNCTION:
                os << "VOID_FUNCTION";
                break;
            default:
                os << "invalid type!";
                break;
        }
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const variable_t& variable){
        os << variable.address << "-" << variable.type << "-" << variable.value;
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const context_t& context) {
        os << context.name << " : ";
        for (const auto& mapping : context.mapping)
            os << mapping.first << "-" << mapping.second << " ";
        return os;
    }

};

std::ostream& operator<<(std::ostream& os, const Assembler::type_t& type);
std::ostream& operator<<(std::ostream& os, const Assembler::context_t& context);
std::ostream& operator<<(std::ostream& os, const Assembler::variable_t& variable);
#endif