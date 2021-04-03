select de.dept_no,
		d.dept_name,
		avg(sa.salary)
from employee.dept_emp de,
    employee.salaries sa,
    employee.departments d
where 
	de.to_date > now()
	and sa.to_date > now()
	and de.emp_no = sa.emp_no
    and d.dept_no = de.dept_no
group by de.dept_no
having avg(sa.salary) >= 70000 and avg(sa.salary) < 80000
    
    