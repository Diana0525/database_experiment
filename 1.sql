select e.emp_no,
	e.birth_date,
    e.first_name,
    e.last_name,
    e.gender,
    e.hire_date,
    ti.title
from employee.employees e,
	employee.titles ti
where e.emp_no = '10029'
	and ti.emp_no = '10029';

