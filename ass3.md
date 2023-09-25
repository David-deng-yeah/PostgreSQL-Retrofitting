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
