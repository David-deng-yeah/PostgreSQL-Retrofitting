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
# compare of performance
symmetric hash join: 1000 tuple hash join
```bash
992 | First Name 988  | Last Name 988  | Department 4 |  992 | 50992.00
  993 | First Name 989  | Last Name 989  | Department 5 |  993 | 50993.00
  994 | First Name 990  | Last Name 990  | Department 1 |  994 | 50994.00
  995 | First Name 991  | Last Name 991  | Department 2 |  995 | 50995.00
  996 | First Name 992  | Last Name 992  | Department 3 |  996 | 50996.00
  997 | First Name 993  | Last Name 993  | Department 4 |  997 | 50997.00
  998 | First Name 994  | Last Name 994  | Department 5 |  998 | 50998.00
  999 | First Name 995  | Last Name 995  | Department 1 |  999 | 50999.00
 1000 | First Name 996  | Last Name 996  | Department 2 | 1000 | 51000.00
 1003 | First Name 999  | Last Name 999  | Department 5 |      |         
 1001 | First Name 997  | Last Name 997  | Department 3 |      |         
 1002 | First Name 998  | Last Name 998  | Department 4 |      |         
 1004 | First Name 1000 | Last Name 1000 | Department 1 |      |         
(1008 rows)

Time: 176.506 ms
hewen=# 
```
