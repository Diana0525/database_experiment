select de.emp_no
from employee.dept_emp de,
	employee.departments dep
where dep.dept_name = 'Production'
	and  de.emp_no in(
		select de.emp_no
        from employee.dept_emp de,
			employee.departments dep
		where dep.dept_name = 'Quality Management'
        and dep.dept_no = de.dept_no
    )
	and dep.dept_no = de.dept_no
