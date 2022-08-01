// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include <string.h>
#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "hash.h"

Bits codeword(char *attr_value, int m, int k) {
    int nbits = 0;
    Bits cword = newBits(m);
    srandom(hash_any(attr_value, strlen(attr_value)));

    while (nbits < k) {
        int i = random() % m;
        if (!bitIsSet(cword, i)) {
            setBit(cword, i);
            nbits++;
        }
    }
    return cword;
}

Bits makePageSig(Reln r, Tuple t) {
    assert(r != NULL && t != NULL);
    //TODO
    Bits psig = newBits(psigBits(r));
    char **tupval = tupleVals(r, t);
    for (int i = 0; i < nAttrs(r); i++) {
        // skip when we scan '?'
        if (tupval[i][0] == '?') {
            continue;
        }
        // codewords for each attribute
        Bits cw = codeword(tupval[i], psigBits(r), codeBits(r));
        orBits(psig, cw);
    }
    return psig;
}

void findPagesUsingPageSigs(Query q) {
    assert(q != NULL);
    //TODO
    Reln r = q->rel;
    // query signature
    Bits qsig = makePageSig(r, q->qstring);
    // page signature
    Bits psig = newBits(psigBits(r));
    // Pages = AllZeroBits
    unsetAllBits(q->pages);
    int pid;

    // iterate all the pages
    for (int i = 0; i < nPsigPages(r); i++) {
        pid = i;
        Page p = getPage(r->psigf, pid);

        // for each tuple
        for (int j = 0; j < pageNitems(p); j++) {

            // get the signature for each tuple store in the psig
            getBits(p, j, psig);
            if (isSubset(qsig, psig)) {
                // means it matches
                int located_page = q->nsigs;
                // include PID in Pages
                setBit(q->pages, located_page);
            }
            // the number of signature we scanned
            q->nsigs++;
        }
        q->nsigpages++;
    }
}

