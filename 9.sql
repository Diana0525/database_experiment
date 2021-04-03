select de.dept_no,
		sum(sa.salary)
from employee.dept_emp de,
    employee.salaries sa
where de.to_date > now()
    and sa.to_date > now()
    and de.emp_no = sa.emp_no
group by de.dept_no
