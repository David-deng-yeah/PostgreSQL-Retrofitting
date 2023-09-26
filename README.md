# ass2 
```bash
hewen=# select * from employee
left join salary
on employee.id = salary.id;
INFO:  state: HJ_BUILD_HASHTABLE
INFO:  Hash outer got 1
INFO:  inserting hash value 0x8e731746 into bucket 1862
INFO:  state: HJ_NEED_NEW_INNER
INFO:  Hash inner got 1
INFO:  inserting hash value 0x8e731746 into bucket 838
INFO:  trying 0x8e731746 == 0x8e731746
INFO:  potential 0x56089398bdd8 and 0x560893983d78 as match
INFO:  marked 0x56089398bdd8 and 0x560893983d78 as match
INFO:  **** single match, end
INFO:  state: HJ_PROBE_OUTER
INFO:  out of match
INFO:  state: HJ_NEED_NEW_OUTER
INFO:  Hash outer got 2
INFO:  inserting hash value 0x439edcf6 into bucket 1270
INFO:  state: HJ_NEED_NEW_INNER
INFO:  Hash inner got 2
INFO:  inserting hash value 0x439edcf6 into bucket 246
INFO:  trying 0x439edcf6 == 0x439edcf6
INFO:  potential 0x56089398be08 and 0x560893983da8 as match
INFO:  marked 0x56089398be08 and 0x560893983da8 as match
INFO:  **** single match, end
INFO:  state: HJ_PROBE_OUTER
INFO:  out of match
INFO:  state: HJ_NEED_NEW_OUTER
INFO:  Hash outer got 3
INFO:  inserting hash value 0xfe534f97 into bucket 1943
INFO:  state: HJ_NEED_NEW_INNER
INFO:  Hash inner got 3
INFO:  inserting hash value 0xfe534f97 into bucket 919
INFO:  trying 0xfe534f97 == 0xfe534f97
INFO:  potential 0x56089398be48 and 0x560893983dd8 as match
INFO:  marked 0x56089398be48 and 0x560893983dd8 as match
INFO:  **** single match, end
INFO:  state: HJ_PROBE_OUTER
INFO:  out of match
INFO:  state: HJ_NEED_NEW_OUTER
INFO:  Hash outer got 4
INFO:  inserting hash value 0xc3bc2f2b into bucket 1835
INFO:  state: HJ_NEED_NEW_INNER
INFO:  Hash inner got 4
INFO:  inserting hash value 0xc3bc2f2b into bucket 811
INFO:  trying 0xc3bc2f2b == 0xc3bc2f2b
INFO:  potential 0x56089398be88 and 0x560893983e08 as match
INFO:  marked 0x56089398be88 and 0x560893983e08 as match
INFO:  **** single match, end
INFO:  state: HJ_PROBE_OUTER
INFO:  out of match
INFO:  state: HJ_NEED_NEW_OUTER
INFO:  Hash outer got null
INFO:  state: HJ_NEED_NEW_INNER
INFO:  Hash inner got null
INFO:  state: HJ_FILL_TUPLES
 id | first_name | last_name | department  | id |  salary  
----+------------+-----------+-------------+----+----------
  1 | John       | Doe       | HR          |  1 | 55000.00
  2 | Jane       | Smith     | Engineering |  2 | 60000.00
  3 | Alice      | Johnson   | Finance     |  3 | 50000.00
  4 | david      | ray       | student     |  4 | 48000.00
(4 rows)

hewen=# 
```

