select distinct concat(e.first_name,'',e.last_name)Name 
from employees e,
	salaries s
where e.emp_no = s.emp_no and s.salary > 100000;