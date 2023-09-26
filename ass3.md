# some code about .h file
```c
// /include/optimizer/pathnode.h
extern SymHashPath *create_symhashjoin_path()
// /include/nodes/plannodes.h
typedef struct SymHashJoin{}
// /include/nodes/pathnodes.h
typedef struct SymHashPath{}
// /include/nodes/nodes.h
// nodeTag
T_SymHashJoin
T_SymHashPath

// /include/nodes/execnodes.h
typedef struct HashJoinState{
    ...
    //inner outer
}

// /include/executor/nodeHash.h
extern HashJoinTuple ExecHashTableInsert()
extern bool ExecScanHashTableForUnmatchedSymmetric()
extern bool ExecProbeInnerHashBucketWithOuterTuple()
extern bool ExecProbeOuterHashBucketWithInnerTuple()

```

# some interpretation about code

hashclauses is a variable that likely represents a set of hash join clauses associated with a 
Symmetric Hash Join operation.
hash clauses are the conditions or expressions used to determine how rows from two input relations should be joined together.
for examples:
```sql
select *
from table1
join table2 on table1.colA = table2.colB
```
in this example, table1.colA = table2.colB is a hash clause 

TupleTableSlot is a standardized interface for passing rows of data between 
different components of the query execution plan. Regradless of the specific
operation being performed(like projection, filter and join), the output of one
operation can be easily passed as input to the next operation using it.


econtext is an abbreviation for "Expression COntext". In the context of a database query execution engine, and it provides an environment for evaluating expression(encapsulates variables, parameters, and state information requried for expression execution). It can doing Variable Bindings(Variable can be used in expressions to store intermediate results or to access values from outer query blocks in the case of nested queries).
This class holds the "current context" information
 *		needed to evaluate expressions for doing tuple qualifications
 *		and tuple projections.  For example, if an expression refers
 *		to an attribute in the current inner tuple then we need to know
 *		what the current inner tuple is and so we look at the expression
 *		context.

 How to create a hash-table?
 * initializes the hash table control block, including metadata about the hash table'size, state and memory usage
 * sets up memory contexts for storing the hash table's working storage, including hash buckets and tuples.
 * initialize hash functions, join operator information and collations for the keys involved in the hash join.
 * allocates memory for hash buckets and sets them to an initial emoty state
 * finally initialized HashJoinTable data structure representing the hash table for the hash join operation.


 ExecHashGetHashValue is a function like hash(key) which take a tuple as key and then calculated its hash-value. it has several steps:
 * its take as input a HashJoinTable, an ExprContext(used for evaluating expressions), a list of hash keys, flags indicating whether the tuple comes from the outer or inner table.
 * it calculates a hash value for a tuple based on a list of hash keys, and these hash keys are all numerical, not a boolean. during calculating(which is a loop, function Continully calculates the value of key using different hash keys, and once one of the result of calculates got NULL, function will immediately return false), function will check key in this bucket or not, a little bit like boolean filter.
 * For each hash key expression:
    * it move the current hash key value left by one bit
    * if the hash key expression evaluates to NULL and the join operators is strict, the function immediately returns false, indicating that the tuple cannot not match
    * if the hash key expression evaluates to non-NULL value, it computes the hash code for that value using a hash function specified for the key. The resulting hash code is XORed with current hash key value.
* finally, the calculated hash value is stored in the hashvalue pointer, and the function returns true, indicating a successful calculation.

sometime although we considered two tuples are matched through hash join, but there are still some joinqual and otherqual, which will add some filter or conditions on the matched tuples, so we have to add a if-else to judge it.

null-fill in the context of a hash join operation is the process of generating fake join tuples with NULL values for columns corresponding to the non-matching table when performing left or full outer joins. These fake join tuples ensure that unmatched rows are represented in the result set will NULL values, as expected in the output of a left or full outer join. This handling of unmatched tuples is an essential part of implementing the semantics of these types of joins in a hash join algorithm.

so what exactly does the symmetric do? let's talk about it.
the symmetric hash join operator maintains two hash tables. one for each relation. Each hash table uses a different hash function. It suports the traditional demand-pull pipeline interface. The symmetric hash join work as follows:
* read a tuple from the inner relation and insert it into the inner relation's hash table, using the inner relation's hash function. Then, use teh new tuple to probe the outer relation;s hash table for matches. To probe, use the outer relation's function.
* when probing with the inner tuples finds no more matches, read a tuple from the outer relation. Insert it into the outer realtion's hash table using the outer relation's hash function. Then, use the outer tuple to probe the inner relation's hash table for matches. using the inner table's hash function.

These two steps are repeated until there are no more tuples to be read from either of the two input relations. That is, the algorihtm alternates between getting an inner tuple and getting an outer tuple untill one of the two input relations is exhausted, at which point the algorihtm readsthe remaining tuples from the other relation.

demand-pull pipeline operator is what?
it just like a design pattern, in which processing is initiated or driven by the consumer of the data, rather than the data producer pushing data to consumers