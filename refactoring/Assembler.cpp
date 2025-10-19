#include "Assembler.hpp"

using namespace std;

size_t Segment::allocate(size_t size){
    last_used = used;
    used = address;
    address+=size;
    return used;
}

size_t Segment::recycle(){
    if(next_recycled == bin.end() || locked){
        allocate();
        bin.push_back(used);
        locked = true;
        return used;
    }
    else{
        last_used = used;
        used = *next_recycled;
        next_recycled++;
        return used;
    }
}

void Segment::reset(){
    next_recycled = bin.begin();
    locked = false;
}

Segment::Segment(){
    last_used = 0;
    used = 0;
    address = 0;
    locked = true;
}

size_t& Segment::operator[](size_t destination){
    if(segment.size() < address)
        segment.resize(address);
    return segment[destination];
}

const size_t& Segment::operator[](size_t destination) const{
    return segment[destination];
}

size_t Segment::size(){
    if(segment.size() < address)
        segment.resize(address);
    return address;
}

Assembler::Assembler() : contexts({string(""),{}}){
    contexts.insert({string("root"),{}});
    global = &contexts.current();
    segments[(size_t)segment_t::RESERVED].allocate((size_t)reserved_t::_COUNT);
    global->mapping["SWICHADDR"] = {
        .segment = (size_t)segment_t::RESERVED,
        .address = (size_t)reserved_t::SWITCH,
    };
    global->mapping["DISPADDR"] = {
        .segment = (size_t)segment_t::RESERVED,
        .address = (size_t)reserved_t::DISP
    };
    global->mapping["LCDADDR"] = {
        .segment = (size_t)segment_t::RESERVED,
        .address = (size_t)reserved_t::LCD
    };
}

void Assembler::declare(const string& name, const variable_t& variable, bool global_context){
    context_t& context = global_context ? * global : contexts.current();
    if(context.mapping.count(name))
        throw runtime_error("lexeme redefinition: " + name);
    context.mapping[name] = variable;
}

void Assembler::push(const std::string& name){
    contexts.down();
    contexts.insert((context_t){
        .name = name
    });
    contexts.left();
}

void Assembler::pop(){
    contexts.up();
}

Assembler::variable_t Assembler::popvar(){

    if(stack.empty())
        throw runtime_error("stack empty");

    variable_t variable = stack.back();
    stack.pop_back();
    return variable;
}

void Assembler::pushvar(const variable_t& variable){
    stack.push_back(variable);
}

bool Assembler::find(const std::string& name, variable_t& variable, bool global_context){

    if(global_context){
        if(global->mapping.count(name)){
            variable = global->mapping[name];
            return true;
        }
        return false;
    }

    auto focused = contexts.focused;
    auto cursor = contexts.cursor;
    bool found = false;

    do{
        context_t & context = contexts.current();
        if(context.mapping.count(name)){
            variable = context.mapping[name];
            found = true;
        }
    }while(!found && contexts.up());

    contexts.focused = focused;
    contexts.cursor = cursor;
    return found;
}

Assembler::variable_t Assembler::allocate(Assembler::segment_t segment,size_t size){
    return {
        (size_t)segment,
        segments[(size_t)segment].allocate(size),
    };
}

Assembler::variable_t Assembler::recycle(Assembler::segment_t segment){
    return {
        (size_t)segment,
        segments[(size_t)segment].recycle(),
    };
}

void Assembler::set(const Assembler::variable_t& variable, size_t value){
    segments[variable.segment][variable.address] = value;
}

void Assembler::assemble(assemblable_t & assemblable){
    auto& abstract_tree = assemblable.tree;
    auto& lexemes = assemblable.lexemes;
    size_t depth;

    std::function<void(syntax_node_t)> traverse = [&](syntax_node_t node){  
        
        if(holds_alternative<nonterminal_t>(node->data)){
            nonterminal_t nonterminal = get<nonterminal_t>(node->data);
            switch (nonterminal){
                
                case nonterminal_t::VOID_FUN_DECL:
                case nonterminal_t::INT_FUN_DECL:{
                    const string & name = lexemes[node->parent];
                    segment_t segment = nonterminal == nonterminal_t::VOID_FUN_DECL ? segment_t::VOID_FUNCTIONS : segment_t::INT_FUNCTIONS;
                    variable_t function = allocate(segment);
                    set(function,program.size());
                    segments[(size_t)segment_t::SCALARS].reset();
                    segments[(size_t)segment_t::POINTERS].reset();
                    declare(name,function);
                    push(name);
                }
                break;

                case nonterminal_t::INT_WHILE_BODY:
                case nonterminal_t::VOID_WHILE_BODY:
                case nonterminal_t::VOID_IF_BODY:
                case nonterminal_t::INT_IF_BODY:
                case nonterminal_t::VOID_ELSE_BODY:
                case nonterminal_t::INT_ELSE_BODY:
                    push("");
                    break;

                case nonterminal_t::VEC_DECL:{
                    abstract_tree.focused = node;
                    abstract_tree.end();
                    abstract_tree.left();
                    const string & name = lexemes[node->parent];
                    const string & size = lexemes[*abstract_tree.cursor];
                    variable_t scalars = allocate(segment_t::SCALARS,stoull(size));
                    variable_t pointer = allocate(segment_t::USER_POITERS);
                    set(pointer,scalars.address);
                    declare(name,pointer);
                    return;
                }
                break;

                case nonterminal_t::SCALAR_DECL:{
                    const string & name = lexemes[node->parent];
                    variable_t scalar = allocate(segment_t::SCALARS);
                    declare(name,scalar);
                    return;
                }
                break;

                case nonterminal_t::SCALAR_PARAM:
                case nonterminal_t::VEC_PARAM:{
                    const string & name = lexemes[node->parent];
                    segment_t segment = nonterminal == nonterminal_t::SCALAR_PARAM ? segment_t::SCALARS : segment_t::USER_POITERS;
                    variable_t parameter = allocate(segment);
                    declare(name,parameter);

                    program.insert(program.end(),{
                        {operator_t::SETDS,
                            parameter,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK}
                        },
                        {operator_t::SUB,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::ONE_CT}
                        }
                    });
                    return;
                }
                break;

                case nonterminal_t::NUMERAL:{
                    abstract_tree.focused = node;
                    abstract_tree.end();
                    const string & name = lexemes[*abstract_tree.cursor];
                    variable_t number;
                    if(!find(name,number,true)){
                        number = allocate(segment_t::SCALARS);
                        set(number,stoull(name));
                        declare(name,number,true);
                    }
                }
                break;

                case nonterminal_t::NAME:{
                    abstract_tree.focused = node;
                    abstract_tree.end();
                    const string & name = lexemes[*abstract_tree.cursor];
                    variable_t variable;
                    if(!find(name,variable))
                        throw runtime_error("identifier: " + name + " used but never defined");
                }
                break;

                case nonterminal_t::FUNCTION_ACTIVATION:
                    depth = stack.size();
                    break;

                case nonterminal_t::VOID_ITERATION_DECL:
                case nonterminal_t::INT_ITERATION_DECL:{
                    variable_t label = allocate(segment_t::LABELS);
                    set(label,program.size());
                    pushvar(label);
                    break;
                }

                default:
                break;
            }
        }

        
        for (auto child = node->children.rbegin(); child != node->children.rend(); child++) {
            traverse(*child);
        }
        
        if(holds_alternative<nonterminal_t>(node->data)){
            nonterminal_t nonterminal = get<nonterminal_t>(node->data);
            switch (nonterminal){
                
                case nonterminal_t::VOID_IF_BODY:
                case nonterminal_t::INT_IF_BODY:{
                    variable_t label = allocate(segment_t::LABELS);
                    program.push_back({
                        operator_t::SET, 
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::PC}, 
                            label
                    });

                    variable_t jumpvar;
                    do{
                        jumpvar = popvar();
                    }while(jumpvar.segment != (size_t)segment_t::LABELS);
                    set(jumpvar,program.size());
                    pushvar(label);
                    pop();
                }
                break;
                
                case nonterminal_t::INT_SELECTION_DECL:
                case nonterminal_t::VOID_SELECTION_DECL:{
                    variable_t jumpvar;
                    do{
                        jumpvar = popvar();
                    }while(jumpvar.segment != (size_t)segment_t::LABELS);
                    set(jumpvar,program.size());
                }
                break;
                
                case nonterminal_t::INT_ITERATION_DECL:
                case nonterminal_t::VOID_ITERATION_DECL:{
                    variable_t conditionvar = popvar();
                    variable_t jumpstart = popvar();
                    program.push_back({
                        operator_t::SET,
                        {(size_t)segment_t::RESERVED,(size_t)reserved_t::PC},
                        jumpstart
                    });
                    set(conditionvar,program.size());
                }
                break;

                case nonterminal_t::VOID_FUN_DECL:
                case nonterminal_t::INT_FUN_DECL:{
                    
                    variable_t scalar = recycle(segment_t::SCALARS);
                    program.insert(program.end(),{
                        {operator_t::SETDS,
                            scalar,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK}
                        },
                        {operator_t::SUB,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::ONE_CT}
                        },
                        {operator_t::SET,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::PC},
                            scalar
                        }
                    });
                }
                    [[fallthrough]];
                    
                case nonterminal_t::VOID_ELSE_BODY:
                case nonterminal_t::INT_ELSE_BODY:
                case nonterminal_t::INT_WHILE_BODY:
                case nonterminal_t::VOID_WHILE_BODY:
                    pop();
                    break;

                case nonterminal_t::INT_RETURN_DECL:{
                    variable_t scalar = popvar();
                    program.push_back({
                        operator_t::SET,
                        {(size_t)segment_t::RESERVED,(size_t)reserved_t::RETURN_REGISTER},
                        scalar
                    });
                }
                [[fallthrough]];

                case nonterminal_t::VOID_RETURN_DECL:{

                    variable_t scalar = recycle(segment_t::SCALARS);
                    program.insert(program.end(),{
                        {operator_t::SETDS,
                            scalar,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                        },
                        {operator_t::SUB,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::ONE_CT}
                        },
                        {operator_t::SET,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::PC},
                            scalar
                        }
                    });
                }
                break;
                
                case nonterminal_t::EQUALITY:
                case nonterminal_t::INDEX:
                case nonterminal_t::RELATIONAL:
                case nonterminal_t::MUL_DIV:
                case nonterminal_t::PLUS_MINUS:{

                    abstract_tree.focused = node;
                    abstract_tree.end();
                    abstract_tree.left();
                    token_t operation = get<token_t>(abstract_tree.current());
                    array<variable_t,2> variables = {popvar(),popvar()};
                    
                    switch (operation){

                        case token_t::OPEN_SQUARE:{
                            variable_t pointer = recycle(segment_t::POINTERS);
                            program.push_back({operator_t::SUM,pointer,variables[0], variables[1]});
                            pushvar(pointer);
                        }
                        break;

                        case token_t::EQUAL:{
                            if(variables[0].segment == (size_t)segment_t::POINTERS){
                                variable_t deferenced = recycle(segment_t::SCALARS);
                                program.push_back({operator_t::SETDS,deferenced,variables[0]});
                                variables[0] = deferenced;
                            }
                            operator_t operation = variables[1].segment == (size_t)segment_t::POINTERS ? operator_t::SETDD : operator_t::SET;
                            program.push_back({operation,variables[1],variables[0]});
                        }
                        break;
                    
                        default:{
                            for(size_t i = 0; i < variables.size(); i++){
                            if(variables[i].segment == (size_t)segment_t::POINTERS){
                                variable_t deferenced = recycle(segment_t::SCALARS);
                                program.push_back({operator_t::SETDS,deferenced,variables[i]});
                                variables[i] = deferenced;
                            }}
                            variable_t result = allocate(segment_t::SCALARS);
                            program.push_back({(operator_t)(int)operation,result,variables[1],variables[0]});
                            pushvar(result);
                        }
                        break;
                    }
                }break;

                case nonterminal_t::FUNCTION_ACTIVATION:{
                    
                    abstract_tree.focused = node;
                    abstract_tree.end();
                    const string & name = lexemes[*abstract_tree.cursor];
                    variable_t function;
                    find(name,function,true);
                    
                    if(function.segment != (size_t)segment_t::VOID_FUNCTIONS && function.segment != (size_t)segment_t::INT_FUNCTIONS)
                        throw runtime_error("expected function");
                        
                    size_t amount = stack.size() - depth;
                    variable_t label = allocate(segment_t::LABELS);
                    
                    program.insert(program.end(),{
                        {operator_t::SUM,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::ONE_CT}
                        },
                        {operator_t::SETDD,
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                            label
                        }
                    });
                    
                    for(size_t i = stack.size() - amount; i < stack.size(); i++){
                        variable_t argument = stack[i];

                        if(argument.segment == (size_t)segment_t::POINTERS){
                            variable_t dereferenced = recycle(segment_t::SCALARS);
                            program.push_back({operator_t::SETDS,dereferenced,argument});
                            argument = dereferenced;
                        }

                        program.insert(program.end(),{
                            {operator_t::SUM,
                                {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                                {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                                {(size_t)segment_t::RESERVED,(size_t)reserved_t::ONE_CT}
                            },
                            {operator_t::SETDD,
                                {(size_t)segment_t::RESERVED,(size_t)reserved_t::STACK},
                                argument
                            }
                        });
                    }
                    
                    set(label,program.size()+1);
                    
                    while(amount){
                        popvar();
                        amount--;
                    }

                    program.push_back({
                        operator_t::SET,
                        {(size_t)segment_t::RESERVED,(size_t)reserved_t::PC},
                        function
                    });

                    if(function.segment == (size_t)segment_t::INT_FUNCTIONS)
                        pushvar({(size_t)segment_t::RESERVED,(size_t)reserved_t::RETURN_REGISTER});
                }
                break;

                case nonterminal_t::CONDITION:{
                    variable_t condition = popvar();
                    variable_t label = allocate(segment_t::LABELS);
                    program.push_back({
                        operator_t::FJMP, 
                            {(size_t)segment_t::RESERVED,(size_t)reserved_t::TRASH}, 
                            condition,
                            label
                    });
                    pushvar(label);
                }
                break;

                case nonterminal_t::NUMERAL:
                case nonterminal_t::NAME:{
                    abstract_tree.focused = node;
                    abstract_tree.end();
                    const string & name = lexemes[*abstract_tree.cursor];
                    variable_t variable;
                    find(name,variable,nonterminal == nonterminal_t::NUMERAL);
                    pushvar(variable);
                }
                break;

                default:
                break;
            }
        }
    };

    traverse(&(assemblable.tree.root));

    variable_t main;
    if(!find("main",main,true))
        throw runtime_error("main not found");

    if(main.segment != (size_t)segment_t::VOID_FUNCTIONS)
        throw runtime_error("main must be a void function");

    array<size_t,(size_t)segment_t::_COUNT + 1> offsets;
    offsets[0] = 0;
    for(size_t i = 1; i < segments.size() + 1; i++)
        offsets[i] = offsets[i-1] + segments[i-1].size();
    binary.resize(program.size() + offsets.back());

    for(size_t i = 0; i < segments.size(); i++){
        Segment & segment = segments[i];
        size_t offset = offsets[i];
        
        switch ((segment_t)i){

            case segment_t::USER_POITERS:
            case segment_t::POINTERS:
                for(size_t j = 0; j < segment.size(); j++)
                    binary[j + offset] = segment[j] + offsets[(size_t)segment_t::SCALARS];
                break;

            case segment_t::VOID_FUNCTIONS:
            case segment_t::INT_FUNCTIONS:
            case segment_t::LABELS:
                for(size_t j = 0; j < segment.size(); j++)
                    binary[j + offset] = segment[j] + offsets.back();
                break;
            
            case segment_t::RESERVED:
                binary[(size_t)reserved_t::ONE_CT   + offset] = 1;
                binary[(size_t)reserved_t::ASSEMBLY + offset] = offsets.back();
                binary[(size_t)reserved_t::STACK    + offset] = offsets.back() + program.size();
                binary[(size_t)reserved_t::PC       + offset] = offsets.back() + segments[(size_t)segment_t::VOID_FUNCTIONS][main.address];
                break;

            default:
                for(size_t j = 0; j < segment.size(); j++)
                    binary[j + offset] = segment[j];
                break;
        }
    }
    
    for(size_t i = 0; i < program.size(); i++){
        size_t op = (size_t)program[i].op;
        variable_t destination = program[i].destination;
        variable_t argument_a = program[i].argument_a;
        variable_t argument_b = program[i].argument_b;
        
        binary[i + offsets.back()] = ((op) << 3 * ARGUMENT_LENGTH) +
                                     ((offsets[destination.segment] + destination.address) << 2 * ARGUMENT_LENGTH) +
                                     ((offsets[argument_a.segment]  + argument_a.address)  << ARGUMENT_LENGTH) +
                                     ((offsets[argument_b.segment]  + argument_b.address));
    }
}

void Assembler::writeBinary(const std::string& path){    
    cout << *this;
    ofstream file(path);

    if(!file.is_open())
        throw runtime_error("could not open file: " + path);

    for(size_t line : binary)
        file << hex << line << '\n';
}

std::ostream& operator<<(std::ostream& os, const Assembler::segment_t& type){
    std::string text;
    switch (type) {
        #define SEGMENT(name) case Assembler::segment_t::name: text = #name; break;
        SEGMENT_TYPES
        #undef SEGMENT
        default:
            text = "undefined operator!";
    }
    return os << text;
}

std::ostream& operator<<(std::ostream& os, const Assembler::reserved_t& type){
    std::string text;
    switch (type) {
        #define RESERVED(name) case Assembler::reserved_t::name: text = #name; break;
        RESERVED_TYPES
        #undef RESERVED
        default:
            text = "undefined operator!";
    }
    return os << text;
}

std::ostream& operator<<(std::ostream& os, const Assembler::variable_t& variable){
    if(variable.segment == (size_t)Assembler::segment_t::RESERVED)
        os << (Assembler::reserved_t)variable.address;
    else
        os << variable.address << "\t" << ((Assembler::segment_t)variable.segment);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Assembler::assembly_t& assembly){
    os << assembly.op << "(\n\t" << assembly.destination << ",\n\t" << assembly.argument_a;
    if(assembly.op < operator_t::SET)
        os << ",\n\t" << assembly.argument_b;
    os << "\n)";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Segment& s){
    for(size_t i = 0; i < s.segment.size(); i++)
        os << i << ":\t" << s.segment[i] << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Assembler::context_t& context){
    os << context.name << ":";
    for(auto variable : context.mapping)
        os << "\t" << variable.first << "\t" << variable.second << "\t-";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Assembler& assembler){
    size_t main = assembler.binary[0];
    size_t assembly = assembler.binary[1];

    os << "0: " << main << " (entrypoint)" << std::endl;
    os << "1: " << assembly << " (code start)" << std::endl;

    for(size_t i = 2; i < assembly; i++)
        os << i << ": " << assembler.binary[i] << std::endl;

    for(size_t i = assembly; i < assembler.binary.size(); i++){
        size_t word = assembler.binary[i];
        os << i << ": " << std::setw(11) << std::setfill('0') << word << "\t" << (operator_t)((word >> 30) & 0x3FF) << "(" << ((word >> 20) & 0x3FF) << "," << ((word >> 10) & 0x3FF)  << "," << (word & 0x3FF) << ")" << endl;
    }
    return os;
}