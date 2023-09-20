-- ======================================
-- my implementation of the intset type.
-- Author: Miigon 2022-03-17
-- https://github.com/Miigon
-- ======================================

-- this script is used to test the correctness of `intset` type and it's operators
-- NOTE: it assumes the intset is implemented as a sorted array.


do $$ begin
    RAISE WARNING ' *********** THE FOLLOWING QUERIES SHOULD ALL SUCCEED (aka. show nothing) *********** ';
    RAISE NOTICE '';

    -- test input/output
    ASSERT text(intset(' {  } ')) = '{}', 'i/o test 1 failed';
    ASSERT text(intset('{0}')) <> '{}', 'i/o test 2 failed';
    ASSERT text(intset('{}')) <> '{0}', 'i/o test 3 failed';
    ASSERT text(intset('   {  2, 3 ,1, 4,5,0 }  ')) = '{0,1,2,3,4,5}', 'i/o test 4 failed';
    ASSERT text(intset('{2,3,1,15,1,3,2,3,15,3}')) = '{1,2,3,15}', 'i/o test 5 failed';
    ASSERT text(intset('{10,9,8,7,666666,5,4,4,3,2,11,1,111}')) = '{1,2,3,4,5,7,8,9,10,11,111,666666}', 'i/o test 6 failed';

    RAISE NOTICE '      passed input/output tests.';

    -- test INT_MAX
    ASSERT text(intset('{2147483647,2147483647,2147483646}')) = '{2147483646,2147483647}', 'INT_MAX test failed';
    RAISE NOTICE '      passed INT_MAX test.';

    -- test equal
    ASSERT intset('{2,1,3,5,5555,5,5,4,777,1,1,6}') = intset('{1,2,3,5555,4,5,6,777}'), 'equal test 1 failed';
    ASSERT NOT intset('{2,1,3,5,5555,5,5,4,777,1,1,6}') <> intset('{1,2,3,4,5555,5,6,777}'), 'equal test 2 failed';
    ASSERT intset('{}') = intset('{}'), 'equal test 3 failed';
    ASSERT intset('{}') <> intset('{0}'), 'equal test 4 failed';
    ASSERT intset('{0}') <> intset('{}'), 'equal test 5 failed';
    ASSERT intset('{1}') <> intset('{3}'), 'equal test 6 failed';
    ASSERT intset('{1}') = intset('{1}'), 'equal test 7 failed';
    RAISE NOTICE '      passed equal test.';

    -- test contain
    ASSERT 5555 ? intset('{2,1,3,5,5555,5,5,4,777,1,1,6}') , 'contain test 1 failed';
    ASSERT NOT 3333 ? intset('{2,1,3,5,5555,5,5,4,777,1,1,6}') , 'contain test 2 failed';
    ASSERT NOT 0 ? intset('{2,1,3,5,5555,5,5,4,777,1,1,6}') , 'contain test 3 failed';
    ASSERT NOT 99999999 ? intset('{2,1,3,5,5555,5,5,4,777,1,1,6,88888,10,98765,13}') , 'contain test 4 failed';
    ASSERT 88888 ? intset('{2,1,3,5,5555,5,5,4,777,1,1,6,88888,10,98765,13}') , 'contain test 5 failed';
    ASSERT 98765 ? intset('{2,1,3,5,5555,5,5,4,777,1,1,6,88888,10,98765,13}') , 'contain test 6 failed';
    RAISE NOTICE '      passed contain test.';
end; $$ LANGUAGE plpgsql;

do $$ begin
    RAISE WARNING ' *********** THE QUERIES ABOVE SHOULD ALL SUCCEED (aka. show nothing) *********** ';
end; $$ LANGUAGE plpgsql;