#ifndef _PARSER_LD_H_
#define _PARSER_LD_H_

/*TODO: factor these out*/
#define RESOLVED 	-1
#define FINAL	2

/**
 *possible LD line statuses
*/
enum
{
    STATUS_UNRESOLVED,
    STATUS_RESOLVED,
    STATUS_FINAL,
    STATUS_ERROR,
    N_STATUS
}LD_STATUS;

/**
 *accepted LD symbols: 0-9 for digits, and
 */
enum
{
    /// LD specific operators:
    LD_BLANK = 10,  ///blank character
    LD_AND,         ///-
    LD_NOT,         ///!
    LD_OR,          ///|
    LD_NODE,        ///+
    LD_COIL,        ///( contact coil
    LD_SET,         ///[ set
    LD_RESET,       ///] reset,
    LD_DOWN,    	///) negate coil
    N_LD_SYMBOLS
}LD_SYMBOLS;

#define IS_COIL(x)  (x>=LD_COIL && x<=LD_DOWN) 
#define IS_VERTICAL(x)  (x>=LD_OR && x<=LD_NODE)

typedef struct ld_line 
{
    char *buf;
    BYTE status;
    unsigned int cursor;
    item_t stmt;
} *ld_line_t;


/**
 * @brief horizontal parse
 * parse up to coil or '+' 
 * -> blank or '|' : discard expression for line
 * -> operand: add AND expression
 * -> coil: add assignment statement
 * @param the ld line
 * @return ok or error code
*/
int parse_ld_line(ld_line_t line);

/**
 * @brief for an array arr of integers ,return the smallest of indices i so that arr[i] =  min(arr) >= min
 * @param arr
 * @param min
 * @param max
 * @return the smallest of indices i
 */
int minmin(const int *arr, const int min, const int max);

/**
 * @brief LD main task
 * @param pointer to PLC registers
 * @return
 */
int LD_task(plc_t p);

/**
  * @brief construct array of ld lines and initialize with text lines
  * @param the pre allocated text lines
  * @param the number of lines
  * @return newly allocated array
  */ 
ld_line_t * construct_program(char lines[][MAXSTR], const unsigned int length);

/**
  * @brief deallocate memory of ld program
  * @param the program
  * @param the length 
  */
void destroy_program(const unsigned int length, ld_line_t * program);

/**
 * @brief read ONE character from line at index
 * and parse grammatically
 * @param line
 * @param c index
 * @return LD symbol
 */
BYTE read_char(const char * line, unsigned int c);

/**
  * @brief parse each program line horizontally up to coil or '+' 
  * -> blank or '|' : discard expression for line
  * -> operand: add AND expression
  * -> coil: add assignment statement
  * @param program length (total lines)
  * @param the program (allocated array of lines)
  * @return OK or error
  */
int horizontal_parse(const unsigned int length, ld_line_t * program);

/**
  * @brief parse all lines vertically at cursor position 
  * -> '+': add push OR
  * -> '|': continue
  * -> default: replace all nodes with OR of all nodes
  * @param line to start at
  * @param program length 
  * @param program
  * @return OK or error
  */
int vertical_parse(const unsigned int start,
                   const unsigned int length, 
                   ld_line_t * program);

/** @brief find next valid node for vertical parse.
  * status unresolved,
  * the smallest index of those with the smallest cursor larger than pos
  * @param buffer of ld lines
  * @param current horizontal position
  * @param total number of lines 
  * @return index of next node or error
  */
int find_next_node(const ld_line_t * program,
                   const unsigned int start, 
                   const unsigned int lines);

/**
  * @brief parse LD program
  * @param the program as an allocated buffer of allocated strings
  * @param the plc to store the generated microcode to
  * @return ok or error code
  */
int parse_ld_program(char lines[][MAXSTR], plc_t p);


//ld_line_t * mk_lines(char lines[MAXBUF][MAXSTR]);

#endif //_PARSER_LD_H

