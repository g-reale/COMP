#include "Assembler.hpp"

using namespace std;

Assembler::Assembler() : contexts({string(""),{}})
{
    contexts.insert({string("root"),{}});
    global = &contexts.current();
    address = (int)reserved_t::_COUNT;
    next_recycled = bin.end();

    global->mapping["SWICHADDR"] = {
        .type = type_t::SCALAR,
        .address = (size_t)reserved_t::SWITCH
    };
    global->mapping["DISPADDR"] = {
        .type = type_t::SCALAR,
        .address = (size_t)reserved_t::DISP
    };
}

void Assembler::allocate(const string& name, size_t size, size_t value, type_t type, bool global_context){
    context_t& context = global_context ? *global : contexts.current();
    if(context.mapping.count(name))
        throw runtime_error("lexeme redefinition: " + name);
    
    context.mapping[name] = {
        .type = type,
        .address = address,
        .value = value,
    };
    address += size;
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

void Assembler::pushvar(size_t destination, type_t type, size_t value){
    stack.push_back({
        .type = type,
        .address = destination,
        .value= value,
    });
}

void Assembler::dma(size_t destination, size_t value){
    labels[destination] = value;
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

size_t Assembler::recycle(){
    if(next_recycled == bin.end()){
        recycled = address;
        bin.push_back(address);
        address++;
    }
    else{
        recycled = *next_recycled;
        next_recycled++;
    }
    return recycled;
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
                    allocate(name,
                        0,
                        program.size(),
                        nonterminal == nonterminal_t::VOID_FUN_DECL ? type_t::VOID_FUNCTION : type_t::INT_FUNCTION,
                        true
                    );
                    dma(address,program.size());
                    address++;
                    push(name);
                    next_recycled = bin.begin();
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
                    allocate(name,stoull(size) + 1,address + 1,type_t::POINTER);
                    return;
                }
                break;

                case nonterminal_t::SCALAR_DECL:{
                    const string & name = lexemes[node->parent];
                    allocate(name);
                    return;
                }
                break;

                case nonterminal_t::SCALAR_PARAM:
                case nonterminal_t::VEC_PARAM:{
                    const string & name = lexemes[node->parent];
                    program.insert(program.end(),{
                        {operator_t::SETDS,address,(size_t)reserved_t::STACK},
                        {operator_t::SUB,(size_t)reserved_t::STACK,(size_t)reserved_t::STACK,(size_t)reserved_t::ONE_CT},
                    });
                    allocate(
                        name,
                        1,
                        nonterminal == nonterminal_t::SCALAR_PARAM ? 0 : address,
                        nonterminal == nonterminal_t::SCALAR_PARAM ? type_t::SCALAR : type_t::POINTER
                    );
                    return;
                }
                break;

                case nonterminal_t::NUMERAL:{
                    abstract_tree.focused = node;
                    abstract_tree.end();
                    const string & name = lexemes[*abstract_tree.cursor];
                    variable_t variable;
                    if(!find(name,variable,true))
                        allocate(name,1,stoull(name),type_t::SCALAR,true);
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
                case nonterminal_t::INT_ITERATION_DECL:
                    dma(address,program.size());
                    pushvar(address);
                    break;
                
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
                    program.push_back({
                        .operation = operator_t::SET,
                        .destination = (size_t)reserved_t::PC,
                        .argument_a = address
                    });
                    variable_t jumpvar;
                    do{
                        jumpvar = popvar();
                    }while(jumpvar.type != type_t::LABEL);
                    dma(jumpvar.address,program.size());
                    pushvar(address,type_t::LABEL);
                    pop();
                }
                break;
                
                case nonterminal_t::INT_SELECTION_DECL:
                case nonterminal_t::VOID_SELECTION_DECL:{
                    variable_t jumpvar;
                    do{
                        jumpvar = popvar();
                    }while(jumpvar.type != type_t::LABEL);
                    dma(jumpvar.address,program.size());
                }
                break;
                
                case nonterminal_t::INT_ITERATION_DECL:
                case nonterminal_t::VOID_ITERATION_DECL:{
                    cout << stack.size() << endl;
                    variable_t conditionvar = popvar();
                    variable_t jumpstart = popvar();
                    program.push_back({
                        .operation = operator_t::SET,
                        .destination = (size_t)reserved_t::PC,
                        .argument_a = jumpstart.address
                    });
                    dma(conditionvar.address,program.size());
                }
                break;

                case nonterminal_t::VOID_FUN_DECL:
                case nonterminal_t::INT_FUN_DECL:
                    program.insert(program.end(),{
                        {operator_t::SETDS,recycle(),(size_t)reserved_t::STACK},
                        {operator_t::SUB,(size_t)reserved_t::STACK,(size_t)reserved_t::ONE_CT},
                        {operator_t::SET,(size_t)reserved_t::PC,recycled}
                    });
                    [[fallthrough]];
                    
                case nonterminal_t::VOID_ELSE_BODY:
                case nonterminal_t::INT_ELSE_BODY:
                case nonterminal_t::INT_WHILE_BODY:
                case nonterminal_t::VOID_WHILE_BODY:
                    pop();
                    break;

                case nonterminal_t::INT_RETURN_DECL:{
                    variable_t variable = popvar();
                    program.push_back((assembly_t){
                        .operation = operator_t::SET,
                        .argument_a = (size_t)reserved_t::RETURN_REGISTER,
                        .argument_b = variable.address
                    });
                }
                [[fallthrough]];

                case nonterminal_t::VOID_RETURN_DECL:
                    program.insert(program.end(),{
                        {operator_t::SETDS,recycle(),(size_t)reserved_t::STACK},
                        {operator_t::SUB,(size_t)reserved_t::STACK,(size_t)reserved_t::ONE_CT},
                        {operator_t::SET,(size_t)reserved_t::PC,recycled}
                    });
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
                    
                    array<variable_t,2> variables;
                    array<size_t,2> addresses;
                    
                    for(size_t i = 0; i < addresses.size(); i++){
                        variable_t variable = popvar();
                        variables[i] = variable;
                        addresses[i] = variable.address;
                    }

                    switch (operation){

                        case token_t::OPEN_SQUARE:
                            program.push_back({
                                .operation = operator_t::SUM,
                                .destination = recycle(),
                                .argument_a = addresses[1],
                                .argument_b = addresses[0]
                            });
                            pushvar(recycled,type_t::POINTER);
                        break;

                        case token_t::EQUAL:{
                            if(variables[0].type == type_t::POINTER){
                                program.push_back({
                                    .operation = operator_t::SETDS,
                                    .destination = recycle(),
                                    .argument_a = addresses[0]
                                });
                                addresses[0] = recycled;
                            }

                            operator_t operation = variables[1].type == type_t::POINTER ? operator_t::SETDD : operator_t::SET;
                            program.push_back({
                                .operation = operation,
                                .destination = addresses[1],
                                .argument_a = addresses[0]
                            });
                        }
                        break;
                    
                        default:{
                            array<size_t,2> auxiliaries = {recycle(),recycle()};

                            for(size_t i = 0; i < addresses.size(); i++){
                            if(variables[i].type == type_t::POINTER){
                                program.push_back({
                                    .operation = operator_t::SETDS,
                                    .destination = auxiliaries[i],
                                    .argument_a = addresses[i]
                                });
                                addresses[i] = auxiliaries[i];
                            }}
                            program.push_back({
                                .operation = (operator_t)(int)operation,
                                .destination = recycle(),
                                .argument_a = addresses[1],
                                .argument_b = addresses[0],
                            });
                            pushvar(recycled);
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
                    
                    if(function.type != type_t::VOID_FUNCTION &&  function.type != type_t::INT_FUNCTION)
                        throw runtime_error("expected function");
                        
                    size_t amount = stack.size() - depth;
                    
                    program.insert(program.end(),{
                        {operator_t::SUM,(size_t)reserved_t::STACK,(size_t)reserved_t::STACK,(size_t)reserved_t::ONE_CT},
                        {operator_t::SETDD,(size_t)reserved_t::STACK,recycle()}
                    });
                    
                    for(size_t i = stack.size() - amount; i < stack.size(); i++){
                        program.insert(program.end(),{
                            {operator_t::SUM,(size_t)reserved_t::STACK,(size_t)reserved_t::STACK,(size_t)reserved_t::ONE_CT},
                            {operator_t::SETDD,(size_t)reserved_t::STACK,stack[i].address}
                        });
                    }
                    
                    dma(recycled,program.size()+1);
                    
                    while(amount){
                        popvar();
                        amount--;
                    }

                    program.push_back({
                        .operation = operator_t::SET,
                        .destination = (size_t)reserved_t::PC,
                        .argument_a = function.address
                    });
                    
                    if(function.type == type_t::INT_FUNCTION)
                        pushvar((size_t)reserved_t::RETURN_REGISTER,type_t::SCALAR);
                }
                break;

                case nonterminal_t::CONDITION:{
                    variable_t condition = popvar();
                    program.insert(program.end(),{{
                        .operation = operator_t::FJMP,
                        .destination = (size_t)reserved_t::TRASH,
                        .argument_a = condition.address,
                        .argument_b = recycle()
                    }});
                    pushvar(recycled,type_t::LABEL);
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
    binary.resize(address + program.size());
    
    variable_t main;
    if(!find("main",main,true))
        throw runtime_error("main not found");

    binary[(size_t)reserved_t::PC] = main.value + address;
    binary[(size_t)reserved_t::ASSEMBLY] = address;
    binary[(size_t)reserved_t::ONE_CT] = 1;
    binary[(size_t)reserved_t::STACK] = binary.size();

    contexts.traverse([&](tree_t<context_t>::node_t* node){
        std::unordered_map<std::string,variable_t> mapping = node->data.mapping;
        for(auto& [name,variable] : mapping)
            binary[variable.address] = variable.value;
    },
    [](tree_t<context_t>::node_t*){});

    for(auto label : labels){
        auto [index, value] = label;
        binary[index] = value + address;
    }
    
    size_t i = address;
    for(auto line : program){
        size_t op = (size_t)line.operation;
        size_t destination = (size_t)line.destination;
        size_t argument_a = (size_t)line.argument_a;
        size_t argument_b = (size_t)line.argument_b;
        binary[i] = (op << 30) + (destination << 20) + (argument_a << 10) + argument_b;
        i++;
    }
}

void Assembler::writeBinary(const std::string& path){    
    cout << binary;
    ofstream file(path);

    if(!file.is_open())
        throw runtime_error("could not open file: " + path);

    for(size_t line : binary)
        file << line << '\n';
}