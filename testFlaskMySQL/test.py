from flask import Flask,render_template
from flask import request
import mysql.connector
conn=mysql.connector.connect(user="root",password="mysql",database="testdb")#数据库连接
cursor = conn.cursor()
app=Flask(__name__)#引入Flask
# 查询数据库
def read_sql(conn,sentence):
    cursor = conn.cursor()
    cursor.execute(sentence) #sentence为sql指令
    result = cursor.fetchall()
    conn.commit()
    cursor.close()
    return result  #返回为远元组

@app.route('/',methods=['GET'])#跳转至login.html，请求方式GET
def show():
    users = read_sql(conn,'select username,password from userinfo')
    print(users)
    return render_template('index.html',users=users)

@app.route('/login',methods=['POST', 'GET'])
def login():
    if request.method == 'GET':
        return render_template('login.html')
    else:
        username = request.form['username'] #界面传值
        password = request.form['password'] #界面传值
        if(len(username) == 0 | len(password) == 0):
            return render_template('login.html')

        # cursor.execute('select username from userinfo')#查询test表查询用户名
        usernames=read_sql(conn,'select username from userinfo')
        for user in usernames:
            if request.form['username']==user[0]:
                cursor.execute('select password from userinfo where username=%s',(user[0],))
                pw=cursor.fetchall()#从test表中获取密码

                if request.form['password']==pw[0][0]:#如果页面输入的password匹配test表返回的密码
                    return '<h>欢迎回来,%s！</h>'%user[0]

                return '<h>账号、密码错误！</h>'

        # cursor.close()#关闭游标
        # conn.close()#关闭连接

@app.route('/regist',methods=['POST', 'GET'])#表单提交
def regist():
    if request.method == 'GET':
        return render_template('regist.html')
    else:
        user = request.form.get('username')
        pw1 = request.form.get('password1')
        pw2 = request.form.get('password2')
        if pw1 == pw2:
            cursor.execute('insert into userinfo(username, password) values (%s, %s)', (user, pw1))#把注册信息加入test表
            conn.commit()
            return '<h>注册成功！请登录。</h><form action="/login" method="get"><p><button type="submit">返回登录</button></p></form>'

        # cursor.close()#关闭游标
        # conn.close()#关闭连接

if __name__=='__main__':
    app.run()
