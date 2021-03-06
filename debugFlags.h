#define DEBUG_MODE			0


#define SHOW_SYMBOL_TABLE	(DEBUG_MODE*1)
#define SHOW_LABLE			(DEBUG_MODE*1)
#define SHOW_COMMAND		(DEBUG_MODE*1)
#define SHOW_OPERANDS		(DEBUG_MODE*1)
#define SHOW_IC				(DEBUG_MODE*1)
#define SHOW_DC				(DEBUG_MODE*1)
#define SHOW_FINAL_COUNTERS	(DEBUG_MODE*1)
#define SHOW_DATA_ARR_I		(DEBUG_MODE*0)
#define SHOW_LINE			(DEBUG_MODE*1)
#define SHOW_VALID_CASE		(DEBUG_MODE*1)

#define SHOW_TAKE			(DEBUG_MODE*3) /* can be 0,1,2,3 */
#define SHOW_ENCODING		(DEBUG_MODE*1)
#define SHOW_GENERAL		(DEBUG_MODE*1)

#define FILE_BINARY_PRINT	(DEBUG_MODE*0)

#define VALIDATE_LABLE		((!DEBUG_MODE)+1)
#define VALIDATE_OPERANDS	((!DEBUG_MODE)+1)

