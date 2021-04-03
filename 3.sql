select distinct e.emp_no,
	e.birth_date,
    e.first_name,
    e.last_name,
    e.gender,
    e.hire_date
from employee.employees e,
	employee.departments d,
	employee.dept_emp de
where e.first_name like 'Ge%'
	and e.emp_no not in(
		select e.emp_no
		from employee.employees e,
			employee.departments d,
			employee.dept_emp de
		where d.dept_name = 'Production'
			and d.dept_no = de.dept_no
            and de.emp_no = e.emp_no)