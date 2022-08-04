CREATE FUNCTION pname_in(cstring)
   RETURNS PersonName
   AS '/home/database/postgresql-12.5/src/tutorial/pname'
   LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION pname_out(PersonName)
   RETURNS cstring
   AS '/home/database/postgresql-12.5/src/tutorial/pname'
   LANGUAGE C IMMUTABLE STRICT;




CREATE TYPE PersonName (
   input = pname_in,
   output = pname_out
);

CREATE FUNCTION show(PersonName)
   RETURNS text
   AS '/home/database/postgresql-12.5/src/tutorial/pname'
   LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION family(PersonName)
   RETURNS text
   AS '/home/database/postgresql-12.5/src/tutorial/pname'
   LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION given(PersonName)
   RETURNS text
   AS '/home/database/postgresql-12.5/src/tutorial/pname'
   LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION pname_bigger(PersonName, PersonName) RETURNS bool
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_bigger_equal(PersonName, PersonName) RETURNS bool
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_less(PersonName, PersonName) RETURNS bool
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_less_equal(PersonName, PersonName) RETURNS bool
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_equal(PersonName, PersonName) RETURNS bool
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_not_equal(PersonName, PersonName) RETURNS bool
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;               


CREATE OPERATOR < (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_less,
   commutator = > , negator = >= ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_less_equal,
   commutator = >= , negator = > ,
   restrict = scalarlesel, join = scalarlejoinsel
);
CREATE OPERATOR = (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_equal,
   commutator = = ,
   -- leave out negator since we didn't create <> operator
   negator = <> ,
   restrict = eqsel, join = eqjoinsel
);
CREATE OPERATOR >= (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_bigger_equal,
   commutator = <= , negator = < ,
   restrict = scalargesel, join = scalargejoinsel
);
CREATE OPERATOR > (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_bigger,
   commutator = < , negator = <= ,
   restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR <> (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_not_equal,
   commutator = <> ,
   -- leave out negator since we didn't create <> operator
   negator = = ,
   restrict = neqsel, join = neqjoinsel
);

CREATE FUNCTION pname_abs_cmp(PersonName, PersonName) RETURNS int4
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;   
CREATE FUNCTION pname_own_hash(PersonName) RETURNS int4
   AS '/home/database/postgresql-12.5/src/tutorial/pname' LANGUAGE C IMMUTABLE STRICT;      

CREATE OPERATOR CLASS PersonName_btree_ops
    DEFAULT FOR TYPE PersonName USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       pname_abs_cmp(PersonName, PersonName);

CREATE OPERATOR CLASS PersonName_hash_ops
    DEFAULT FOR TYPE PersonName USING hash AS
        OPERATOR        1       =,
        FUNCTION        1       pname_own_hash(PersonName);