// inequalities
// target

/**
 * @brief Solve Linear Programming problem with simplex method
 * 
 * @param object the coeff of the object function as a n vector
 * @param ineq_lhs the coeff of the restraints as a mxn matrix。 Using <=, by default
 * @param ineq_rhs the right hand side constant of the restraints as a n vector
 * @return int 
 */
int LinearProgrammingSolver(int m, int n, float* object, float** ineq_lhs, float* ineq_rhs);


/**
 * @brief 
 * 
 * @param m 
 * @param n 
 * @param object 
 * @param ineq_lhs 
 * @param ineq_rhs 
 * @return float** 
 */
float** ToTableau(int m, int n, float* object, float** ineq_lhs, float* ineq_rhs);



// #include "Simplex.hpp"