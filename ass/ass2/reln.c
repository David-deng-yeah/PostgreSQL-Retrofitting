// reln.c ... functions on Relations
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "tsig.h"
#include "psig.h"
#include "bits.h"
#include "hash.h"
// open a file with a specified suffix
// - always open for both reading and writing

File openFile(char *name, char *suffix) {
    char fname[MAXFILENAME];
    sprintf(fname, "%s.%s", name, suffix);
    File f = open(fname, O_RDWR | O_CREAT, 0644);
    assert(f >= 0);
    return f;
}

// create a new relation (five files)
// data file has one empty data page

Status newRelation(char *name, Count nattrs, float pF,
                   Count tk, Count tm, Count pm, Count bm) {
    Reln r = malloc(sizeof(RelnRep));
    RelnParams *p = &(r->params);
    assert(r != NULL);
    p->nattrs = nattrs;
    p->pF = pF,
            p->tupsize = 28 + 7 * (nattrs - 2);
    Count available = (PAGESIZE - sizeof(Count));
    p->tupPP = available / p->tupsize;
    p->tk = tk;
    if (tm % 8 > 0) tm += 8 - (tm % 8); // round up to byte size
    p->tm = tm;
    p->tsigSize = tm / 8;
    p->tsigPP = available / (tm / 8);
    if (pm % 8 > 0) pm += 8 - (pm % 8); // round up to byte size
    p->pm = pm;
    p->psigSize = pm / 8;
    p->psigPP = available / (pm / 8);
    if (p->psigPP < 2) {
        free(r);
        return -1;
    }
    if (bm % 8 > 0) bm += 8 - (bm % 8); // round up to byte size
    p->bm = bm;
    p->bsigSize = bm / 8;
    p->bsigPP = available / (bm / 8);
    if (p->bsigPP < 2) {
        free(r);
        return -1;
    }
    r->infof = openFile(name, "info");
    r->dataf = openFile(name, "data");
    r->tsigf = openFile(name, "tsig");
    r->psigf = openFile(name, "psig");
    r->bsigf = openFile(name, "bsig");
    addPage(r->dataf);
    p->npages = 1;
    p->ntups = 0;
    addPage(r->tsigf);
    p->tsigNpages = 1;
    p->ntsigs = 0;
    addPage(r->psigf);
    p->psigNpages = 1;
    p->npsigs = 0;
    //addPage(r->bsigf); p->bsigNpages = 1; p->nbsigs = 0; // replace this
    // Create a file containing "pm" all-zeroes bit-strings,
    // each of which has length "bm" bits
    //TODO

    // we know the bits-sliced pages, as we know the length of page signature
    // how many tuples we can store
    Count nbitslicepage = psigBits(r) / maxBsigsPP(r);
    Count nbitsleft = psigBits(r) % maxBsigsPP(r);
    PageID pid;
    Page page;
    if (nbitsleft != 0) {
        nbitslicepage = nbitslicepage + 1;
    }
    p->bsigNpages = 0;
    p->nbsigs = 0;
    for (int i = 0; i < nbitslicepage; i++) {
        //
        addPage(r->bsigf);
        pid = i;
        page = getPage(r->bsigf, pid);
        // current page is full,then create new page
        if (i != nbitslicepage - 1) {
            for (int j = 0; j < maxBsigsPP(r); j++) {
                Bits slice = newBits(bsigBits(r));
                putBits(page, j, slice);
                addOneItem(page);
                p->nbsigs++;
            }
        } // the page is not full
        else {
            if (nbitsleft != 0) {
                for (int j = 0; j < nbitsleft; j++) {
                    Bits slice = newBits(bsigBits(r));
                    putBits(page, j, slice);
                    addOneItem(page);
                    p->nbsigs++;
                }
            } else {
                for (int j = 0; j < maxBsigsPP(r); j++) {
                    Bits slice = newBits(bsigBits(r));
                    putBits(page, j, slice);
                    addOneItem(page);
                    p->nbsigs++;
                }
            }
        }
        // update data to the file
        putPage(r->bsigf, pid, page);
        p->bsigNpages++;
    }
    closeRelation(r);
    return 0;
}

// check whether a relation already exists

Bool existsRelation(char *name) {
    char fname[MAXFILENAME];
    sprintf(fname, "%s.info", name);
    File f = open(fname, O_RDONLY);
    if (f < 0)
        return FALSE;
    else {
        close(f);
        return TRUE;
    }
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name) {
    Reln r = malloc(sizeof(RelnRep));
    assert(r != NULL);
    r->infof = openFile(name, "info");
    r->dataf = openFile(name, "data");
    r->tsigf = openFile(name, "tsig");
    r->psigf = openFile(name, "psig");
    r->bsigf = openFile(name, "bsig");
    read(r->infof, &(r->params), sizeof(RelnParams));
    return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file
// note: we don't write ChoiceVector since it doesn't change

void closeRelation(Reln r) {
    // make sure updated global data is put in info file
    lseek(r->infof, 0, SEEK_SET);
    int n = write(r->infof, &(r->params), sizeof(RelnParams));
    assert(n == sizeof(RelnParams));
    close(r->infof);
    close(r->dataf);
    close(r->tsigf);
    close(r->psigf);
    close(r->bsigf);
    free(r);
}

// insert a new tuple into a relation
// returns page where inserted
// returns NO_PAGE if insert fails completely

PageID addToRelation(Reln r, Tuple t) {
    assert(r != NULL && t != NULL && strlen(t) == tupSize(r));
    Page p;
    PageID pid;
    RelnParams *rp = &(r->params);

    // add tuple to last page
    pid = rp->npages - 1;
    p = getPage(r->dataf, pid);
    // check if room on last page; if not add new page
    if (pageNitems(p) == rp->tupPP) {
        addPage(r->dataf);
        rp->npages++;
        pid++;
        free(p);
        p = newPage();
        if (p == NULL) return NO_PAGE;
    }
    addTupleToPage(r, p, t);
    rp->ntups++;  //written to disk in closeRelation()
    putPage(r->dataf, pid, p);

    // compute tuple signature and add to tsigf

    //TODO

    // make a tuple signature
    Bits tsig = makeTupleSig(r, t);
    PageID tsigpid = rp->tsigNpages - 1;
    Page tsigpage = getPage(r->tsigf, tsigpid);
    // if the page is full
    if (pageNitems(tsigpage) == maxTsigsPP(r)) {
        addPage(r->tsigf);
        rp->tsigNpages++;
        tsigpid++;
        free(tsigpage);
        tsigpage = newPage();
        if (tsigpage == NULL) return NO_PAGE;
    }
    putBits(tsigpage, pageNitems(tsigpage), tsig);
    // add one item
    addOneItem(tsigpage);
    rp->ntsigs++;
    // write into file
    putPage(r->tsigf, tsigpid, tsigpage);


    // compute page signature and add to psigf
    //TODO

    // make page signature
    Bits psig = makePageSig(r, t);
    PageID psigpid = rp->psigNpages - 1;
    Page psigpage = getPage(r->psigf, psigpid);
    if (nPsigs(r) != nPages(r)) {
        // if the page is full
        if (pageNitems(psigpage) == maxPsigsPP(r)) {
            addPage(r->psigf);
            rp->psigNpages++;
            psigpid++;
            free(psigpage);
            psigpage = newPage();
            if (psigpage == NULL) return NO_PAGE;
        }
        // create new page
        putBits(psigpage, pageNitems(psigpage), psig);
        addOneItem(psigpage);
        rp->npsigs++;
        putPage(r->psigf, psigpid, psigpage);
    } else {
        // already have the page
        Bits ppsig = newBits(psigBits(r));
        getBits(psigpage, pageNitems(psigpage) - 1, ppsig);
        orBits(psig, ppsig);
        putBits(psigpage, pageNitems(psigpage) - 1, psig);
        // write the page into file
        putPage(r->psigf, psigpid, psigpage);
    }


    // use page signature to update bit-slices
    //TODO
    int bpage_index = nPsigs(r) - 1;
    // length of bits signature
    for (int i = 0; i < psigBits(r); i++) {
        if (bitIsSet(psig, i)) {
            Bits bslice = newBits(bsigBits(r));
            int bpage = i / maxBsigsPP(r);
            int bindex = i % maxBsigsPP(r);
            Page bsigp = getPage(r->bsigf, bpage);
            getBits(bsigp, bindex, bslice);
            // get the bits and then set 1
            setBit(bslice, bpage_index);
            putBits(bsigp, bindex, bslice);
            // write the page into file
            putPage(r->bsigf, bpage, bsigp);
        }
    }

    return nPages(r) - 1;
}

// displays info about open Reln (for debugging)

void relationStats(Reln r) {
    RelnParams *p = &(r->params);
    printf("Global Info:\n");
    printf("Dynamic:\n");
    printf("  #items:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
           p->ntups, p->ntsigs, p->npsigs, p->nbsigs);
    printf("  #pages:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
           p->npages, p->tsigNpages, p->psigNpages, p->bsigNpages);
    printf("Static:\n");
    printf("  tups   #attrs: %d  size: %d bytes  max/page: %d\n",
           p->nattrs, p->tupsize, p->tupPP);
    printf("  sigs   bits/attr: %d\n", p->tk);
    printf("  tsigs  size: %d bits (%d bytes)  max/page: %d\n",
           p->tm, p->tsigSize, p->tsigPP);
    printf("  psigs  size: %d bits (%d bytes)  max/page: %d\n",
           p->pm, p->psigSize, p->psigPP);
    printf("  bsigs  size: %d bits (%d bytes)  max/page: %d\n",
           p->bm, p->bsigSize, p->bsigPP);
}
