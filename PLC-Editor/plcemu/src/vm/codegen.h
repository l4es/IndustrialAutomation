#ifndef _CODEGEN_H_
#define _CODEGEN_H_

/**
 * @brief generate code from an expression
 * @param expression
 * @param the rung to insert the code to
 * @param the operation to generate if called in a recursion
 * @return ok or error code
 */
int gen_expr(const item_t expression, rung_t rung, BYTE recursive); 

/**
 * @brief generate code for the left side of an expression
 * @param expression or identifier
 * @param the rung to insert the code to
 * @param the operation to generate if called in a recursion
 * @return ok or error code
 */
int gen_expr_left(const item_t left, rung_t rung, BYTE recursive); 

/**
 * @brief generate code for the right side of an expression
 * @param expression or identifier
 * @param the rung to insert the code to
 * @param the operation to generate 
 * @param the modifier 
 * @return ok or error code
 */
int gen_expr_right(const item_t right, rung_t rung, BYTE op, BYTE mod); 

/**
 * @brief generate code from an assignment statement
 * @param an assignment statement
 * @param the rung to insert the code to
 * @return ok or error code
 */
int gen_ass(const item_t assignment, rung_t rung);
 
#endif //_CODEGEN_H_
