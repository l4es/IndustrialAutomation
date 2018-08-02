typedef struct symentry_t  {
    unsigned int entry;        /* entry number */
    struct symentry_t *next;   /* link to next symentry */
    char *id;                  /* pointer to name */
    char sclass;               /* storage class code */
    char typequal;             /* type qualifier */
    char level;                /* scope level */
    char nspace;               /* name space identifier */
    char ass;                  /* used to flag assignment */
    type_expr * type;          /* object type */
    val_t  val;                /* symbol value information */
}symentry_t;
