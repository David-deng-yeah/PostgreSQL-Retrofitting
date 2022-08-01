# Aims
This assignment aims to give you an understanding of
* how database files are structured and accessed
* how superimposed codeword (SIMC) signatures are implemented
* how partial-match retrieval searching is implemented using SIMC signatures
The goal is to build a simple implementation of a SIMC signature file, including applications to create SIMC files, insert
tuples into them, and search for tuples based on partial-match retrieval queries
# Introduction
Signatures are a style of indexing where (in its simplest form) each tuple is associated with a compact representation of its
values (i.e. its signature). Signatures are used in the context of partial-match retrieval queries, and are particularly
effective for large tuples. Selection is performed by first forming a query signature, based on the values of the known
attributes, and then scanning the stored signatures, matching them against the query signature, to identify potentially
matching tuples. Only these tuples are read from the data pages and compared against the query to check whether they
are true matching tuples. Signature matching allows for "false matches", where the query and tuple signatures match, but
the tuple is not a valid result for the query. Note that signature matching can be quite efficient if the signatures are small,
and efficient bit-wise operations are used to check for signature matches.
The kind of signature matching described above uses one signature for each tuple (as in the diagram below). Other kinds
of signatures exist, and one goal is to implement them and compare their performance to that of tuple signatures.
# Commands
* makes a relation called abc where each tuple has 4 attributes and the indexing has a false match probability of 1/100
```sql
$ ./create abc 10000 4 100
```

* select data from relation
```sql
 $ ./select R '1000001,?,?,?' t
```
![image](https://user-images.githubusercontent.com/65102150/182082741-69a054a2-7a6e-4144-a01a-e7992ea36d17.png)

* Generates a specified number of n-attribute tuples in the appropriate format to insert into a created relation
```sql
$ ./gendata 5 4
```
![image](https://user-images.githubusercontent.com/65102150/182082864-292cab07-7f4b-47d9-bdc1-58c9432eff4f.png)

* Prints information about the sizes of various aspects of the relation
```sql
$ ./stats R
```
![image](https://user-images.githubusercontent.com/65102150/182082952-97332d7b-c0c2-4310-8675-7e25648feda8.png)

* Writes all tuples from the relation RelName, one per line, to standard output.
```sql
$ ./dump R
```
![image](https://user-images.githubusercontent.com/65102150/182083021-f6de156a-b786-41a0-8760-e9c1bb038797.png)

# Data Types
## Relations
Relations are defined by three data types: Reln, RelnRep, RelnParams. Reln is just a pointer to a RelnRep object;
this is useful for passing to functions that need to modify some aspect of the relation structure. RelnRep is a
representation of an open relation and contains the parameters, plus file descriptors for all of the open files.
RelnParams is a list of various properties of the database. See reln.h for details.
## Queries
Queries are defined via a QueryRep structure which contains fields to represent the current state of the scan for the
query, plus a collection of statistics counters. It is essentially like the query iteration structures described in lectures,
and is used to control and monitor the query evaluation. The QueryRep structure also contains a reference to the
relation being queried, and a copy of the query string. The Query data type is simply a pointer to a QueryRep
structure. See query.h for details. The following diagram might also help:
![image](https://user-images.githubusercontent.com/65102150/182083359-8cd26304-6676-4fd1-a2fc-792b65e397dd.png)

## Pages
Pages are defined via a PageRep structure which contains a counter for the number of items, and then an array of
bytes containing the actual items, whether they are tuples or signatures or slices. The size of each type of item is
held in the RelnParams structure, and so Pages are typically considered in conjunction with Relns. The Page data
type is simply a pointer to a PageRep structure. See page.h for details. The following diagram might also help:
![image](https://user-images.githubusercontent.com/65102150/182083392-24fb3c1a-39dc-4c45-bb78-bc63e26466c9.png)

## Bit-strings
Bit-strings are defined via a BitsRep structure which contains two counters (one for the number of bits, and the
other for the number of bytes used to represent the bit-string). The BitsRep structure also contains an array of
bytes which hold the bits in the string; the array is created when and instance of a Bits data type is created. Note
that Bits is an ADT, so the concrete data structure is hidden from its clients; the Bits data type is simply a pointer
to a BitsRep structure. See bits.c for details of the data structure, and bits.h for the function interface. The
following diagram might help:
![image](https://user-images.githubusercontent.com/65102150/182083414-39aaefd5-e634-47f5-a8c5-5de8659afe2d.png)


## Tuple
Tuples are just character sequences (like C strings). See tuple.h for details.
There are also a range of (hopefully) self-explanatory data types defined in defs.h. The various signature types are
represented as bit-strings (Bits).

# Task
## Task 1 A Bit-string Type
Implement all of the incomplete functions in the bits.c file, to produce a working bit-string data type. The functions to
complete are flagged with TODO, and the purpose of each should be clear from the comment at the start of the function
and its name. The x1.c file contains some simple test cases for the Bits type.
## Task 2 Scanning for Results
After you have Bits working, you can start to implement query evaluation, although without indexing. The startQuery()
function parses the query string and then uses the appropriate type of signature to generate a list of pages which
potentially contain matching tuples. This list is implemented as a bit-string where a 1 indicates a page which needs to be
checked for matches. At this stage, all of the signature types mark all pages as potential matches, so all pages need to be
checked.
Note that the startQuery() function can return NULL. It should do so only if the query string contains the wrong number
of attributes for the relation.
Before this will work, you need to implement the scanAndDisplayMatchingTuples() function, which performs the check
for matching tuples in each of the marked pages. This function, as well as finding and displaying result tuples, maintains
the query statistics for number of data pages read, and number of pages that were read but contained no matching tuples.
For this task, you need to complete the scanAndDisplayMatchingTuples() function from the query.c file. This function
behaves roughly as follows:
```c
foreach PID in 0 .. npages-1 {
  if (PID is not set in MatchingPages)
    ignore this page
  for each tuple T in page PID {
    if (T matches the query string)
      display it as a query result
  }
  if (no tuples in page PID are results)
    count it as a false match page
}

```
## Task 3 Tuple Signatures
Implement indexing by using tuple-based signatures (i.e. each tuple has its own signature, stored in the Rel.tsig file).
You will need to complete the makeTupleSig() and findPagesUsingTupSigs() functions in the tsig.c file, and add
some code to the addToRelation() function in reln.c.
The addToRelation() function inserts a tuple into the next available slot in the data file, but currently does nothing with
signatures. You should add code here which generates a tuple signature for the new tuple and inserts it in the next available slot in the Rel.tsig file.
The makeTupleSig() function takes a tuple and returns a bit-string which contains a superimposed codeword signature for that tuple. It behaves roughly as follows:
```c
Tsig = AllZeroBits
for each attribute A in tuple T {
  CW = codeword for A
  Tsig = Tsig OR CW
}
```
A method for computing codewords is given in the lecture notes.
The findPagesUsingTupSigs() take a tuple signature and scans the Rel.tsig file, comparing that signature to the stored
tuple signatures. It builds a bit-string showing which pages contain at least one "matching" tuple. It behaves roughly as
follows:
```c
QuerySig = makeTupleSig(Query)
Pages = AllZeroBits
foreach Tsig in tsigFile {
  if (Tsig matches QuerySig) {
    PID = data page for tuple corresponding to Tsig
    include PID in Pages
  }
}
```
Note that the ith tuple in the data file has its correpsonding signature as the ith signature in the Rel.tsig file. However,
since tuples and tuple signatures are different sizes, the page that the signature appears on will not necessarily have the
same page ID as the page in which the corresponding tuple is located.

## Task 4 Page Signatures
Implement indexing using page-level signatures (psigs).
This is similar to how tuple-level signature indexing is done, except that the sigantures are larger. The functions that you
need to complete are makePageSig() and findPagesUsingPageSigs() in the psig.c file. You will also need to add more
code to the addToRelation() function to maintain page signatures when new tuples are inserted.
One major difference between tuple signatures and page signatures is that page signatures are not a one-off insertion. When a new tuple is added, its page-level signature needs to be included page signature for the page where it it is
inserted. The process can be described roguhly as follows:
```c
new Tuple is inserted into page PID
Psig = makePageSig(Tuple)
PPsig = fetch page signature for data page PID from psigFile
merge Psig and PPsig giving a new PPsig
update page signature for data page PID in psigFile
```
The makePageSig() function be used to generate a page-level signature for the query, and then used to generate a bit string of matching pages roughly as follows:
```c
QuerySig = makePageSig(Query)
Pages = AllZeroBits
foreach Psig in psigFile {
  if (Psig matches QuerySig) {
    PID = data page corresponding to Psig
    include PID in Pages
  }
}
```
## Task 5 Bit-sliced Signatures
Implement indexing using bit-sliced page signatures.
Each bit-slice is effectively a list of pages that have a specific bit from the page-signature set to 1 (e.g. if a page-level
signature has bit 5 set to one, the bit-slice 5 has a 1 bit for every page with a page signature where bit 5 is set). This
drives both the updating of bit-slices and their use in indexing.
You will need to modify the functions: newRelation() in reln.c, addToRelation() in reln.c, and
findPagesUsingBitSlices() in bsig.c. The modifications to newRelation() are relatively straightforward, but remember to update the relation parameters appropriately.
The addToRelation() should take a tuple, produce a page signature for it, then update all of the bit-slices corresponding
to 1-bits in the page signature. This can be described roughly as follows:
```c
PID = data page where new Tuple inserted
Psig = makePageSig(Tuple)
for each i in 0..pm-1 {
  if (Psig bit[i] is 1) {
    Slice = get i'th bit slice from bsigFile
    set the PID'th bit in Slice
    write up
```


