select distinct e.first_name,
	e.last_name
from employee.employees e,
	employee.departments d,
    employee.dept_emp de
where e.hire_date >= '1990-01-01'
	and e.gender = 'M'
    and d.dept_name = 'Finance'
    and de.emp_no = e.emp_no
    and d.dept_no = de.dept_no
    