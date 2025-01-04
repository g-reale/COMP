/**
 * @file dgen.h
 * @author Gabriel Reale - Stefanini Rafael (groliveira5@stefanini.com)
 * @brief generate a dfa.
 * This file defines the prototypes for dfa generation.
 * @version 0.1
 * @date 2025-01-04
 */

/**
 * @brief main dfa table.
 * Each line of the table relates to the state. The columns describe the transition taken for each char.
 */
extern unsigned char DFA[256][256];

/**
 * @brief This struct describes a transitions of the DFA
 */
typedef struct{
    /**
     * @brief A list of source states where the transition applies
     */
    const unsigned char * sources;
    /**
     * @brief The destination state
     */
    unsigned char destination;
    /**
     * @brief A list of characters that trigger the transition from any of the sources to the destination.<br>
     * If the transitions vector starts with `**` the transition will occur for any value (i.e any). <br>
     * If the transitions vector starts with `--{chars}` the transitions will occur for any values except `{chars}` (i.e reverse).<br>
     * If the transitions vector starts neither with `--` or `**` the transition will occur for the listed characters (i.e normal).  
     */
    const unsigned char * transitions;
}transition;

/**
 * @brief Separator used for rule scaning
 */
#define sep '_'
/**
 * @brief Separator used for rule scaning
 */
#define open '_'
/**
 * @brief Separator used for rule scaning
 */
#define close '_'

/**
 * @brief Scan a single character from the input.
 * This scanner is a switch case implementation of a dfa.
 * It will consume chars trying to detect transitions. 
 * When a transition is detected @ref categorize is called.
 * Each transition/rule has the format:
 * ```
 * <open><sources><sep><destination><sep><transitions><close>
 * ```
 * where: @ref open, @ref close and @ref sep are macros configured to `_`.
 * creating the rules manually is pretty hard see: @ref configureDFA to check how 
 * this process was facilitated
 * @param c Current character of the argument
 */
void scan(unsigned char c);
/**
 * @brief Categorizes a transition between normal, reverse or any. 
 * Will call @ref createAllTransitions, @ref createTransition and @ref createReverseTransition accordingly.
 * @param t Transition
 */
void categorize(transition t);
/**
 * @brief Load a transition of the normal kind on the @ref DFA
 * @param t Transition
 */
void createTransition(transition t);
/**
 * @brief Load a transition of the reverse kind on the @ref DFA
 * @param t Transition
 */
void createReverseTransition(transition t);
/**
 * @brief Load a transition of the ANY kind on the @red DFA
 * @param t Transition
 */
void createAllTransitions(transition t);
/**
 * @brief Export the current state of the DFA to a C matrix
 * @param path File to export to
 */
void exportDfa(const char * path);
