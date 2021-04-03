select e.emp_no,
		e.birth_date,
        e.first_name,
        e.last_name,
        e.gender,
        e.hire_date
from employee.employees e
where e.first_name in(
	select e.first_name
    from employee.employees e
    group by e.first_name 
		having count(*)>3
        )
order by e.first_name ASC;
