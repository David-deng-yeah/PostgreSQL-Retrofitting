-- Create the "employee" table
/* CREATE TABLE employee (
    id serial PRIMARY KEY,
    first_name VARCHAR(50),
    last_name VARCHAR(50),
    department VARCHAR(50)
); */

-- Insert 1000 records into the "employee" table
INSERT INTO employee (first_name, last_name, department)
SELECT
    'First Name ' || generate_series(1, 1000) AS first_name,
    'Last Name ' || generate_series(1, 1000) AS last_name,
    'Department ' || (generate_series(1, 1000) % 5 + 1)::text AS department;

-- Create the "salary" table
/* CREATE TABLE salary (
    id INT,
    salary DECIMAL(10, 2)
); */

-- Insert 1000 records into the "salary" table
INSERT INTO salary (id, salary)
SELECT
    generate_series(1, 1000) AS id,
    (50000.00 + (generate_series(1, 1000) % 5001)::numeric)::numeric(10, 2) AS salary;
