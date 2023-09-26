# backgroud knowledge

# problem statement
createplam.c

```bash
inner relation
                ->  Hash node(make HashTable) ->    Hash join operator
outer relation

Hash node will retrieve Tuple repetely and insert into HashTable
```

# nodeHashJoin.c
ExecHashJoinImpl:
```c
HashState *hashNodeInner
HashState *hashNodeOuter
HashJoinTable hashtableInner
HashJoinTable hashtableOuter
TupleTableSlot *outerTupleSlot
TupleTableSlot *innerTupleSlot
```

HashJoin State Machine:
```c
#define HJ_BUILD_HASHTABLE		1 
#define HJ_NEED_NEW_OUTER		2
#define HJ_PROBE_INNER			3
#define HJ_NEED_NEW_INNER		4
#define HJ_PROBE_OUTER			5
#define HJ_FILL_TUPLES	 		6
```

# ass2 handbook

In this assignment, you will impl a new symmetric hash join operator to replace current hashjoin, to impl it, you should modify PostgreSQL's optimizer and executor.

## intro

there are two stage of SymHashJoin:
* building phase: creating hash table
* probing phase: repeatly probing the hashtable that we build, finding if it contains satisfactory tuple or not.

```bash
hewen=# explain select * from employee
left join salary
on employee.id = salary.id;
                               QUERY PLAN                                
-------------------------------------------------------------------------
 Hash Right Join  (cost=32.59..51.28 rows=1004 width=53)
   Hash Cond: (salary.id = employee.id)
   ->  Hash  (cost=16.04..16.04 rows=1004 width=10)
         ->  Seq Scan on salary  (cost=0.00..16.04 rows=1004 width=10)
   ->  Hash  (cost=20.04..20.04 rows=1004 width=43)
         ->  Seq Scan on employee  (cost=0.00..20.04 rows=1004 width=43)
(6 rows)
```

comparing hybrid hash join and symmetric hash join, hybrid hashjoin only create a hash table for inner table, but for symmetric hash join, it uses a build-as-you-go form of probing to find matching data, the steps is:
* read a tuple from the inner realtion, and use inner-realtion-hash-function to calculate its hash value, and insert it into the corresponding bucket of inner hashtable. after that, it use this tuple to probe the outer hash table for any matched tuple with the outer-hash-function.
* the same as tuple from outer realtion
* repeat both operations above until both realtion exhausted.

## points to be modified

1. since the workshop requrie us only using hashjoin instead of nestloop or mergejoin, so we need to turn off them in postgresql.conf
2. since hybrid-hash-join using multiple-batch to handle tuple, we need to turn off it by modifying nodehash.c -> ExecHashTableCreate -> growEnabled.
3. during hash-join, DBMS will turn pathTree into planTree, but in hybrid hash join, it only do it for inner table, so we need to modify it to support both innerTable and OuterTable by modifying createplan.c
4. HashJoinState is a critical data structure in Hash Join process, it maintains hashjoin's state, but in hybrid hash join it only maintian inner table's state. we need to modify it to support both inner and outer in file execnodes.h.
5. nodeHash.c mainly responsible for creating and maintaining hash table, and in hybrid hashjoin, it doesn't impl pipeline-hash-table-creating, so we need to impl it in ExecHash. Besides, we need to modify the code probing hashtable to find matched-value previously. Finally we need to impl function to find the matching value from the inner table and the matching value from the outer table.
6. nodeHash.h containing the nodeHash.c function prototypes, so in this file we need to impl some function, like ExecProbeInnerHashBucketWithOuterTuple and ExecProbeInnerHashBucketWithInnerTuple
7. in the last, we need to modify nodeHashJoin.c, this file mainly responsible for implementing hash join calculation and deciding which value need to join and return. In this file we need to modify threee function:
    * ExecHashJoinImpl: perform node connection return (determine connection)
    * ExecInitHashJoin: initializing the hashjoin state
    * ExecEndHashJoin: Shuttig down the hashJoin state
    * ExecHashJoinImpl: this function impl a state-machine, and we need to modify some state-case.
    ![Alt text](image-1.png)