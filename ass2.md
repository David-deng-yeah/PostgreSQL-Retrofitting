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