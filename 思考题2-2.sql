select distinct concat(e.first_name,'',e.last_name) Name
from employees e
where e.emp_no in (
	select distinct s.emp_no
    from salaries s
    where s.salary > 100000)
    