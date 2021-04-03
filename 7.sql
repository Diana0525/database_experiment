select  sa.emp_no,
		max(sa.salary)
from employee.dept_emp de,
	employee.salaries sa
where de.dept_no = 'd003'
	and de.emp_no = sa.emp_no

	