create view que14 as
	(select e.emp_no,
			e.birth_date,
            e.first_name,
            e.last_name,
            e.hire_date,
            de.from_date,
            de.to_date
	from employee.employees e,
		employee.dept_emp de,
        employee.departments d
	where d.dept_name = 'Finance'
		and de.from_date > '1990-01-01'
        and e.gender = 'M'
        and de.dept_no = d.dept_no
        and e.emp_no = de.emp_no);