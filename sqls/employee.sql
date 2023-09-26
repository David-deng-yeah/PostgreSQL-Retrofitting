CREATE TABLE employee (
    id serial PRIMARY KEY,
    first_name VARCHAR(50),
    last_name VARCHAR(50),
    department VARCHAR(50)
);

INSERT INTO employee (first_name, last_name, department)
VALUES
    ('John', 'Doe', 'HR'),
    ('Jane', 'Smith', 'Engineering'),
    ('Alice', 'Johnson', 'Finance'),
    ('david', 'ray', 'student');

CREATE TABLE salary (
    id INT,
    salary DECIMAL(10, 2)
);

INSERT INTO salary (id, salary)
VALUES
    (1, 55000.00),
    (2, 60000.00),
    (3, 50000.00),
    (4, 48000.00);
