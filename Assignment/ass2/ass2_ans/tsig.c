// tsig.c ... functions on Tuple Signatures (tsig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

Bits codewords(char *attr_value, int m, int k) {
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

// make a tuple signature

Bits makeTupleSig(Reln r, Tuple t) {
    assert(r != NULL && t != NULL);
    //TODO
    Bits tsig = newBits(tsigBits(r));
    char **tupval = tupleVals(r, t);
    for (int i = 0; i < nAttrs(r); i++) {

        // skip when we scan '?'
        if (tupval[i][0] == '?') {
            continue;
        }
        // codewords for each attribute
        Bits cw = codewords(tupval[i], tsigBits(r), codeBits(r));
        orBits(tsig, cw);
    }
    return tsig;
}

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q) {
    assert(q != NULL);
    //TODO

    Reln r = q->rel;
    // query signature
    Bits qsig = makeTupleSig(r, q->qstring);
    // tuple signatrue
    Bits tsig = newBits(tsigBits(r));
    // Pages = AllZeroBits
    unsetAllBits(q->pages);
    int pid;

    // iterate all the pages
    for (int i = 0; i < nTsigPages(r); i++) {
        pid = i;
        Page p = getPage(r->tsigf, pid);
        // for each tuple
        for (int j = 0; j < pageNitems(p); j++) {

            // get the signature for each tuple store in the tsig
            getBits(p, j, tsig);
            if (isSubset(qsig, tsig)) {
                // means it matches
                int located_page = q->nsigs / maxTupsPP(r);
                // include PID in Pages
                setBit(q->pages, located_page);
            }
            // the number of signature we scanned
            q->nsigs++;
        }
        q->nsigpages++;
    }

    // The printf below is primarily for debugging
    // Remove it before submitting this function


    //printf("Matched Pages :"); showBits(q->pages); putchar('\n');

}
