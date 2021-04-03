select count(distinct de.emp_no)
from employee.dept_emp de
where de.emp_no in(
		select de.emp_no
        from employee.dept_emp de
        group by de.emp_no 
        having count(*) >=2)
