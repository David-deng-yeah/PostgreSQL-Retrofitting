// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"
#include "bits.h"

void findPagesUsingBitSlices(Query q) {
    assert(q != NULL);
    //TODO

    Reln r = q->rel;
    // Qsig = makePageSig(Query)
    Bits qsig = makePageSig(r, q->qstring);

    Bits bsig = newBits(bsigBits(r));

    // Pages = AllOneBits
    setAllBits(q->pages);
    int flag = -1;
    // iterate all the pages(bits page size)
    for (int i = 0; i < psigBits(r); i++) {
        if (bitIsSet(qsig, i)) {
            // find the pageId,which page it should be in, one page have maxBsigsPP tuple
            // so using the index i then we can find the pageId
            PageID pid = i / maxBsigsPP(r);
            Page p = getPage(r->bsigf, pid);
            // get the signature  then store in the bsig
            getBits(p, i % maxBsigsPP(r), bsig);

            // check whether we are in the same page, do not need to add it twice
            if (pid != flag) {
                flag = pid;
                q->nsigpages++;
            }
            // zero bits in Pages which are zero in Slice
            for (int j = 0; j < nPsigs(r); j++) {
                if (!bitIsSet(bsig, j)) {
                    unsetBit(q->pages, j);
                }
            }
            // the number of signature we scanned
            q->nsigs++;
        }
    }
}

