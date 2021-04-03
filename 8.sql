select first_name,
		last_name
from employee.dept_manager ma,
	employee.employees e,
    employee.dept_emp de
where de.dept_no = 'd002'
	and de.emp_no = e.emp_no
    and ma.emp_no = e.emp_no
    and ma.to_date = '9999-01-01'
